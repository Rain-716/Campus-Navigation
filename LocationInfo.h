#ifndef CAMPUSNAVIGATION_LOCATIONINFO_H
#define CAMPUSNAVIGATION_LOCATIONINFO_H

#include <string>

namespace Graph
{
    struct LocationInfo
    {
        std::string name,type;
        int visitTime=0;
        LocationInfo()=default;
        LocationInfo(const std::string& name_,const std::string& type_,int visitTime_) : name(name_),type(type_),visitTime(visitTime_) {}
    };
}

#endif // CAMPUSNAVIGATION_LOCATIONINFO_H