#ifndef PATHPART_H
#define PATHPART_H

#include <string>

struct PathPart {
    PathPart(const std::string & text, bool isParam = false) :
        text(text),
        isParam(isParam) {}
    std::string text;
    boolean isParam;
    std::string lastParamValue;
};

#endif /*PATHPART*/