#ifndef __TEAMCITYSTATUS_H_
#define __TEAMCITYSTATUS_H_

#include "spark_wiring_string.h"

#define MAXBUILDTYPES 10

class TCBuildType
{
    public:
        TCBuildType();
        TCBuildType(char* id, char* name);
        char* getId();
        char* getName();
        int getLastBuildId();
        char* getLastBuildNumber();
        char* getLastBuildStatus();
        void updateBuild(int buildId, char* buildNumber, char* buildStatus);
    private:
        char _id[75];
        char _name[75];
        int  _lastBuildId;
        char _lastBuildNumber[20];
        char _lastBuildStatus[20];
};

class TCProject
{
    public:
        TCProject();
        TCProject(char* id, char* parentID);
        char* getId();
        char* getParentId();
        int getBuildTypesCount();
        char* getBuildTypeId(int index);
        TCBuildType* getBuildType(int index);
        int addBuildType(char* id, char* name);
        void setStatus(char status);
        char getStatus();
        
    private:
        char _id[75];
        char _parentId[75];
        int _numBuildTypes;
        TCBuildType _buildTypes[MAXBUILDTYPES];
        char _status;
};


#endif