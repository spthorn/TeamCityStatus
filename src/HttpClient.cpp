

    tctest.ino
    HttpClient.cpp
    HttpClient.h
    TeamCityStatus.cpp
    TeamCityStatus.h

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
50
51
52
53
54
55
56
57
58
59
60
61
62
63
64
65
66
67
68
69
70
71
72
73
74
75
76
77
78
79
80
81
82
83
84
85
86
87
88
89
90
91
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
{
}
TCProject::TCProject(char* id, char* parentId)
{
    strncpy(_id, id, 75);
    strncpy(_parentId, parentId, 75);
    _numBuildTypes = 0;
    _status = 'G';
}
char* TCProject::getId()
{
    return _id;
}
char* TCProject::getParentId()
{
    return _parentId;
}
int TCProject::getBuildTypesCount()
{
    return _numBuildTypes;
}
char* TCProject::getBuildTypeId(int index)
{
    if (index < _numBuildTypes)
        return _buildTypes[index].getId();
    return NULL;
}
TCBuildType* TCProject::getBuildType(int index)
{
    if (index < _numBuildTypes)
        return &_buildTypes[index];
    return new TCBuildType();
}
int TCProject::addBuildType(char* id, char* name)
{
    if (_numBuildTypes == MAXBUILDTYPES)
        return 0;
    _buildTypes[_numBuildTypes++] = TCBuildType(id, name);
    return 1;
}
void TCProject::setStatus(char status)
{
    _status = status;
}
Compiling code...
