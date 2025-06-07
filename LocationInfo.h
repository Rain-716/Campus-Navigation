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
        LocationInfo(std::string& n,std::string& t,int v) noexcept : name(n),type(t),visitTime(v) {}
    };
}

#endif // CAMPUSNAVIGATION_LOCATIONINFO_H