

    tctest.ino
    HttpClient.cpp
    HttpClient.h
    TeamCityStatus.cpp
    TeamCityStatus.h

1
2
3
4
5
6
7
8
9
10
11
12
13
14
15
16
17
18
19
20
21
22
23
24
25
26
27
28
29
30
31
32
33
34
35
36
37
38
39
40
41
42
43
44
45
46
47
48
49
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
        
    private:
        char _id[75];
        char _parentId[75];
        int _numBuildTypes;
        TCBuildType _buildTypes[MAXBUILDTYPES];
        char _status;
};
#endif
Ready.
