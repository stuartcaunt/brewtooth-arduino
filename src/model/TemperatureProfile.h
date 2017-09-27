#ifndef TEMPERATUREPROFILE_H
#define TEMPERATUREPROFILE_H

#include "Jsonable.h"

typedef enum {
    Inactive,
    Pending,
    Active,
    Terminated
} ProfileState;

inline String toString(ProfileState state) {
    switch(state) {
        case Inactive: return "Inactive";
        case Pending: return "Pending";
        case Active: return "Active";
        case Terminated: return "Terminated";
    }
}

struct TemperatureLevel : public Jsonable {
    TemperatureLevel() :
        setpointC(0.0),
        durationS(0),
        timerS(0.0),
        startTimeMs(0),
        state(ProfileState::Inactive) {}

    TemperatureLevel(const JsonObject & json) :
        setpointC(json["setpointC"]),
        durationS(json["durationS"]),
        timerS(0.0),
        startTimeMs(0),
        state(ProfileState::Inactive) {}

    TemperatureLevel & operator=(const TemperatureLevel & rhs) {
        setpointC = rhs.setpointC;
        durationS = rhs.durationS;
        timerS = rhs.timerS;
        startTimeMs = rhs.startTimeMs;
        state = rhs.state;
    }

    void init() {
        timerS = 0.0;
        startTimeMs = 0;
        state = ProfileState::Inactive;
    }

    void start(unsigned int timeMs, float temperatureC) {
        if (state == ProfileState::Inactive) {
            if (temperatureC < setpointC) {
                state = ProfileState::Pending;

            } else {
                startTimeMs = timeMs;
                state = ProfileState::Active;
            }
        }
    }

    void stop() {
        state = ProfileState::Inactive;
    }

    void update(unsigned int timeMs, float temperatureC) {
        if (state == ProfileState::Pending) {
            if (temperatureC >= setpointC) {
                state = ProfileState::Active;
                startTimeMs = timeMs;
            }

        } else if (state == ProfileState::Active) {
            timerS = float(timeMs - startTimeMs) * 0.001;
            if (timerS >= durationS) {
                state = ProfileState::Terminated;
            }
        }
    }

    virtual void convertToJson(JsonObject & json) const {
        json["setpointC"] = setpointC;
        json["durationS"] =durationS;
        json["timerS"] = timerS;
        json["startTimeMs"] = startTimeMs;
        json["state"] = toString(state);
    }

    float setpointC;
    float durationS;
    float timerS;
    unsigned int startTimeMs;
    ProfileState state;
};
    

struct TemperatureProfile : public Jsonable {
    TemperatureProfile() :
        startTimeMs(0),
        currentTimeMs(0),
        state(ProfileState::Inactive),
        activeLevel(-1) {}

    TemperatureProfile(const JsonObject & json) :
        startTimeMs(json["startTimeMs"]),
        currentTimeMs(json["currentTimeMs"]),
        state(ProfileState::Inactive),
        activeLevel(-1) {

        // Temperature Levels
        JsonArray & levelsJsonArray = json["levels"];
        for (JsonArray::iterator it = levelsJsonArray.begin(); it != levelsJsonArray.end(); ++it) {
            JsonObject & levelJson = *it;
            TemperatureLevel level = TemperatureLevel(levelJson);
            levels.push_back(level);
        }
    }

    TemperatureProfile & operator=(const TemperatureProfile & rhs) {
        startTimeMs = rhs.startTimeMs;
        currentTimeMs = rhs.currentTimeMs;
        state = rhs.state;
        levels = rhs.levels;
    }

    float start(unsigned int timeMs, float temperatureC) {
        if (state != ProfileState::Inactive) {
            if (activeLevel >=0) {
                TemperatureLevel & level = levels[activeLevel];
                return level.setpointC;                
            }

            return 0.0;
        }

        // Initialise levels
        for (std::vector<TemperatureLevel>::iterator it = levels.begin(); it != levels.end(); it++) {
            (*it).init();
        }

        // Start first level
        state = ProfileState::Active;
        startTimeMs = timeMs;

        TemperatureLevel & level = levels[0];
        activeLevel = 0;
        level.start(timeMs, temperatureC);
        return level.setpointC;
    }

    void stop() {
        // Terminate all levels
        for (std::vector<TemperatureLevel>::iterator it = levels.begin(); it != levels.end(); it++) {
            (*it).stop();
        }

        // Reset active level and state
        activeLevel = -1;
        state = ProfileState::Inactive;
    }

    float update(unsigned int timeMs, float temperatureC) {
        if (activeLevel >= 0 && state != ProfileState::Terminated) {
            currentTimeMs = timeMs;

            TemperatureLevel & level = levels[activeLevel];
            level.update(timeMs, temperatureC);

            // Go to next state
            if (level.state == ProfileState::Terminated) {
                activeLevel++;

                if (activeLevel >= levels.size()) {
                    state = ProfileState::Terminated;
                    activeLevel = levels.size() -1;

                } else {
                    level = levels[activeLevel];
                    level.start(timeMs, temperatureC);
                }
            }

            return level.setpointC;
        } 
        
        return 0.0;
    }

    virtual void convertToJson(JsonObject & json) const {
        json["startTimeMs"] = startTimeMs;
        json["currentTimeMs"] = currentTimeMs;
        json["state"] = state;
        
        JsonArray & array = json.createNestedArray("levels");
        for (std::vector<TemperatureLevel>::const_iterator it = levels.begin(); it != levels.end(); it++) {
            const TemperatureLevel & level = *it;
            JsonObject & levelJson = array.createNestedObject();
            
            level.convertToJson(levelJson);
        }
    }

    unsigned int startTimeMs;
    unsigned int currentTimeMs;
    ProfileState state;
    std::vector<TemperatureLevel> levels;
    int activeLevel;
};

#endif /*TEMPERATUREPROFILE_H*/