

    teamcity.ino
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
#include "TeamCityStatus.h"
// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------
TCBuildType::TCBuildType()
{
}
TCBuildType::TCBuildType(char* id, char* name)
{
    strncpy(_id, id, 75);
    strncpy(_name, name, 75);
    _lastBuildId = 0;
    _lastBuildNumber[0] = '\0';
    _lastBuildStatus[0] = '\0';
}
char* TCBuildType::getId()
{
   return _id;
}
char* TCBuildType::getName()
{
    return _name;
}
int TCBuildType::getLastBuildId()
{
    return _lastBuildId;
}
char* TCBuildType::getLastBuildNumber()
{
    return _lastBuildNumber;
}
char* TCBuildType::getLastBuildStatus()
{
    return _lastBuildStatus;
}
void TCBuildType::updateBuild(int buildId, char* buildNumber, char* buildStatus)
{
    _lastBuildId = buildId;
    strncpy(_lastBuildNumber, buildNumber, 20);
    strncpy(_lastBuildStatus, buildStatus, 20);
}
// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------
TCProject::TCProject()
Ready.
