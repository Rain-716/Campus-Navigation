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
        LocationInfo(const std::string& n,const std::string& t,int v) noexcept : name(std::move(n)),type(std::move(t)),visitTime(v) {}
    };
}

#endif // CAMPUSNAVIGATION_LOCATIONINFO_H