#include "SparkJson/SparkJson.h"
#include "HttpClient.h"
#include "TeamCityStatus.h"
#include "Authorization.h"

HttpClient http;
http_header_t headers[] = {
    { "Authorization" , HTTP_AUTHORIZATION}, // sthorn:AwesomeArielBold encoded in Base64
    { "Accept" , "application/json"},
    { NULL, NULL }
};
http_request_t request;
http_response_t response;

int led1 = D7;

// These two are used for the "Uptime" event generator
unsigned long lastUpTime = 0UL;
char upTime[40]; // Particle Variable

// This is used for the "Status" event generator
unsigned long lastStatusTime = 0L;

// Globals that contain all the API results
// We start out with around 52756 bytes free (based on 4/14/2016 codebase).
// Each TCProject consumes 2808 bytes, so for 15 projects, that's 42120 bytes,
//   leaving 10636 bytes free.
#define MAXPROJECTS 18
TCProject TCProjects[MAXPROJECTS];
int numTCProjects = 0;
char projectBuildStatus[MAXPROJECTS+1]; // Particle Variable

// ------------------------------------------------------------------------------------------
// Initial setup
// ------------------------------------------------------------------------------------------
void setup() {
    
    pinMode(led1, OUTPUT); // status
    pinMode(A5, INPUT);  // thermistor
    digitalWrite(led1, HIGH);
    Serial.begin(9600);
    delay(3000); // wait for serial io to be initialized. Now's a good time to run "particle serial monitor".
    digitalWrite(led1, LOW);
    
    for(int i=0; i<MAXPROJECTS; i++)
        projectBuildStatus[i] = 'X';
    projectBuildStatus[MAXPROJECTS] = '\0';
    
    // Note: function and variable names are limited to 12 characters
    Particle.function("getStatus", getTeamCityStatus);
    Particle.function("resetPhoton", resetPhoton);
    Particle.variable("upTime", upTime);
    Particle.variable("buildStatus", projectBuildStatus);

    Serial.println("----------------------------------------");
    Serial.println("tctest v0.1");
    Serial.printlnf("Free memory: %i", System.freeMemory());
    Time.zone(-4); // EDT
    Serial.println(Time.timeStr());
    
    // Perform the initial fill of project and build type info
    refreshProjects();
}

// ------------------------------------------------------------------------------------------
// Loop() just contains our uptime counter
// ------------------------------------------------------------------------------------------
void loop() {
    unsigned long uptimeNow = millis(); // get milliseconds uptime
    //Every 5 seconds...
    if (uptimeNow-lastUpTime>5000UL) {
        // Get current uptime and save into Particle Variable
        lastUpTime = uptimeNow;
        unsigned nowSec = uptimeNow/1000UL;
        unsigned sec = nowSec%60;
        unsigned min = (nowSec%3600)/60;
        unsigned hours = (nowSec%86400)/3600;
        sprintf(upTime,"%u:%u:%u",hours,min,sec);
        
        // Get list of first characters of all projects
        char projectFirstChars[MAXPROJECTS+1];
        for(int i=0; i<MAXPROJECTS; i++)
        {
            if (i < numTCProjects)
            {
                char firstLetter;
                char id[75];
                char parentID[75];
                strcpy(id, TCProjects[i].getId());
                strcpy(parentID, TCProjects[i].getParentId());
                if (strcmp(parentID, "_Root") == 0)
                    firstLetter = id[0];
                else
                    id[strlen(parentID+1)];
                projectFirstChars[i] = id[0];
            }
            else
                projectFirstChars[i] = '\0';;
        }
        // Send the status bits and first chars for display
        char statusMessage[40];
        sprintf(statusMessage, "{\"stat\":\"%s\",\"char\":\"%s\"}", projectBuildStatus, projectFirstChars);
        Spark.publish("Status",statusMessage);
    }

    unsigned long statusNow = millis(); // get milliseconds between statusse
    //Every 30 seconds...
    if (statusNow-lastStatusTime>30000UL) {
        lastStatusTime = statusNow;
        
        getTeamCityStatus("");
    }
}

// ------------------------------------------------------------------------------------------
// Particle functions
// ------------------------------------------------------------------------------------------
int getTeamCityStatus(String foo)
{
    // If we haven't pulled the project info yet, just return.
    if (numTCProjects == 0)
        return 1;

    // The number of projects changed since we powered up; refresh all the project info.
    getApiProjectCount();
    if (parseProjectCount() != numTCProjects)
        refreshProjects();

    refreshBuilds();
 
    // Reset all projects' build statuses - we'll fill 'em all in below.
    for(int i=0; i<MAXPROJECTS; i++)
        TCProjects[i].setStatus('X');

    for(int i=0; i<numTCProjects; i++)
    {
        int btypeCount = TCProjects[i].getBuildTypesCount();
        
        // Get summed status of all this project's build types' builds
        int statusBit = 'G'; // green
        for(int b=0; b<btypeCount; b++) {
            TCBuildType* buildType = TCProjects[i].getBuildType(b);
            char btStatus[20] = "";
            strncpy(btStatus, buildType->getLastBuildStatus(), 20);
            if (strcmp(btStatus, "FAILURE") == 0)
                statusBit = 'R'; // red
        }
        
        // Update the projects' status up through the hierarchy
        TCProject* project;
        char projectId[75];
        strncpy(projectId, TCProjects[i].getId(), 75);
        while (strncmp(projectId, "", 75) != 0) 
        {
            // Get project
            project = &TCProjects[getIndexOfProjectId(projectId)];

            // Set this project's status, if needed
            if (project->getStatus() == 'X' || statusBit == 'R')
                project->setStatus(statusBit);

            // Get this project's parent Id, and repeat
            strncpy(projectId, project->getParentId(), 75);
        }

        // Fill the output array with the project build statuses.
        // This'll be published every 5sec in loop().
        for(int i=0; i<MAXPROJECTS; i++)
            projectBuildStatus[i] = TCProjects[i].getStatus();
            
    }
    Serial.println("Build status refreshed...");
    return 1;
}

int resetPhoton(String foo)
{
    System.reset();
    return 1;
}

// ------------------------------------------------------------------------------------------
// API methods
// ------------------------------------------------------------------------------------------
char charBody[2000]; // contains the body of each api response
int bodyLength;

// http://user:pass@build.renovolive.com/httpAuth/app/rest/projects/?fields=count
int getApiProjectCount()
{
    char path[250] = "/httpAuth/app/rest/projects/?fields=count";

    return callAPI(path);
}

// http://user:pass@build.renovolive.com/httpAuth/app/rest/projects/?fields=project(id,parentProjectId)
int getApiProjectList()
{
    char path[250] = "/httpAuth/app/rest/projects/?fields=project(id,parentProjectId)";

    return callAPI(path);
}

// http://user:pass@build.renovolive.com/httpAuth/app/rest/projects/id:EmailService?fields=buildTypes(count,buildType(id,name))
int getApiBuildTypes(int projectIndex)
{
    char projectId[75] = "";
    strncpy(projectId, TCProjects[projectIndex].getId(), 75);

    char path[250] = "/httpAuth/app/rest/projects/id:";
    strcat(path, projectId);
    strcat(path, "?fields=buildTypes(count,buildType(id,name))");

    return callAPI(path);
}

// http://user:pass@build.renovolive.com/httpAuth/app/rest/builds?locator=buildType:(id:bt11),count:1&fields=build(id,number,status)
int getApiBuild(int projectIndex, int buildTypeIndex)
{
    char buildTypeId[75] = "";
    strncpy(buildTypeId, TCProjects[projectIndex].getBuildTypeId(buildTypeIndex), 75);

    char path[250] = "/httpAuth/app/rest/builds?locator=buildType:(id:";
    strcat(path, buildTypeId);
    strcat(path, "),count:1&fields=build(id,number,status)");

    return callAPI(path);
}

int callAPI(char* _path)
{
    char path[250] = "";
    strncpy(path, _path, 200);
    request.hostname = "build.renovolive.com";
    request.port = 80;
    request.path = path;
    http.get(request, response, headers);

    if (response.status == 200)
    {
        bodyLength = response.body.length() + 1;
        response.body.toCharArray(charBody, bodyLength);
        return 1;
    }
    return 0;
}

// ------------------------------------------------------------------------------------------
// Support methods
// ------------------------------------------------------------------------------------------
int getIndexOfProjectId(char* id)
{
    for(int i=0; i<numTCProjects; i++)
    {
        TCProject project = TCProjects[i];
        if (strcmp(id, project.getId()) == 0)
            return i;
    }
    return 0;
}

int refreshProjects()
{
    Serial.print("Refreshing projects");
    getApiProjectList();
    parseProjects();
    Serial.print("\nRefreshing buildTypes");
    for(int projectIndex=0; projectIndex<numTCProjects; projectIndex++)
    {
        getApiBuildTypes(projectIndex);
        parseBuildTypes(projectIndex);
    }
    Serial.print("\n");
    return 1;
}

int refreshBuilds()
{
    Serial.print("Refreshing builds");
    for(int projectIndex=0; projectIndex<numTCProjects; projectIndex++)
    {
        int numBuildTypes = TCProjects[projectIndex].getBuildTypesCount();
        for (int buildTypeIndex=0; buildTypeIndex<numBuildTypes; buildTypeIndex++)
        {
            getApiBuild(projectIndex, buildTypeIndex);
            parseBuild(projectIndex, buildTypeIndex);
        }
    }
    Serial.print("\n");
    return 1;
}

// {"count":15}
int parseProjectCount()
{
    StaticJsonBuffer<2000> jsonBuffer;
    JsonObject& root = jsonBuffer.parseObject(charBody);
    if (root.success() == true)
        return root["count"];
    return 0;
}

// {"project":[{"id":"_Root"},{"id":"EmailService","parentProjectId":"_Root"),...]}
int parseProjects()
{
    Serial.print(".");
    StaticJsonBuffer<2000> jsonBuffer;
    JsonObject& root = jsonBuffer.parseObject(charBody);
    if (root.success() == true)
    {
        JsonArray& projects = root["project"];
        numTCProjects = MAXPROJECTS;
        if (projects.size() <= MAXPROJECTS)
            numTCProjects = projects.size();
        for(int i=0; i<numTCProjects; i++)
        {
            char strId[75] = "";
            strncpy(strId, projects[i]["id"].asString(), 75);
            char strParent[75] = "";
            if (strcmp(projects[i]["parentProjectId"].asString(), "") != 0)
                strncpy(strParent, projects[i]["parentProjectId"].asString(), 75);

            TCProjects[i] = TCProject(strId, strParent);
        }
        return 1;
    } 
    return 0;
}

// _root:
// {"buildTypes":{"count":0,"buildType":[]}}
// EmailService:
// {"buildTypes":{"count":2,"buildType":[{"id":"EmailService_BuildAndUnitTests","name":"Build and unit tests"},{"id":"EmailService_DeployToDev","name":"Deploy to Dev"}]}}
int parseBuildTypes(int projectIndex)
{
    Serial.print(".");
    StaticJsonBuffer<2000> jsonBuffer;
    JsonObject& root = jsonBuffer.parseObject(charBody);
    if (root.success() == true)
    {
        int numTCBtypes = root["buildTypes"]["count"]; // conveniently provided for us
        JsonArray& btypes = root["buildTypes"]["buildType"];
        for(int i=0; i<numTCBtypes; i++)
        {
            char id[75] = "";
            strncpy(id, btypes[i]["id"].asString(), 75);
            char name[75] = "";
            strncpy(name, btypes[i]["name"].asString(), 75);
            TCProjects[projectIndex].addBuildType(id, name);
        }        
        return 1;
    }
    return 0;
}

// {"build":[]}
// {"build":[{"id":59033,"number":"1.0.51","status":"SUCCESS"}]}
// Note: we're really only parsing one build here, the most recent one.
int parseBuild(int projectIndex, int buildTypeIndex)
{
    Serial.print(".");
    StaticJsonBuffer<2000> jsonBuffer;
    JsonObject& root = jsonBuffer.parseObject(charBody);
    if (root.success() == true)
    {
        int buildId;
        buildId = root["build"][0]["id"];
        char buildNumber[20] = "";
        strncpy(buildNumber, root["build"][0]["number"].asString(), 20);
        char buildStatus[20] = "";
        strncpy(buildStatus, root["build"][0]["status"].asString(), 20);

        TCBuildType* buildType = TCProjects[projectIndex].getBuildType(buildTypeIndex);
        buildType->updateBuild(buildId, buildNumber, buildStatus);
        
        return 1;
    }
    return 0;
}
