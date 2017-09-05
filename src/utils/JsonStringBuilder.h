#ifndef JSONSTRINGUILDER_H
#define JSONSTRINGUILDER_H

#include <model/Jsonable.h>
#include <vector>

class JsonStringBuilder {
public:
    static String jsonString(const Jsonable * jsonable);
    static String jsonString(const std::vector<Jsonable *> & jsonableArray);
};

#endif /*JSONSTRINGUILDER_H*/