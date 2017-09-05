#include "JsonStringBuilder.h"

String JsonStringBuilder::jsonString(const Jsonable * jsonable) {
    // Convert to json
    DynamicJsonBuffer jsonBuffer;
    JsonObject & json = jsonBuffer.createObject();
    jsonable->convertToJson(json);

    // Convert to string
    String jsonString;
    json.printTo(jsonString);
    
    return jsonString;
}


String JsonStringBuilder::jsonString(const std::vector<Jsonable *> & jsonableArray) {
    DynamicJsonBuffer jsonBuffer;
    JsonArray & json = jsonBuffer.createArray();

    for (std::vector<Jsonable *>::const_iterator it = jsonableArray.begin(); it != jsonableArray.end(); it++) {
        const Jsonable * jsonable = *it;
        jsonable->convertToJson(json.createNestedObject());
    }

    String jsonString;
    json.printTo(jsonString);

    return jsonString;
}
