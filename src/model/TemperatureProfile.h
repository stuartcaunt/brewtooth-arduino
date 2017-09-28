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
        startTimeS(0),
        state(ProfileState::Inactive) {}

    TemperatureLevel(const JsonObject & json) :
        setpointC(json["setpointC"]),
        durationS(json["durationS"]),
        timerS(0.0),
        startTimeS(0),
        state(ProfileState::Inactive) {}

    TemperatureLevel & operator=(const TemperatureLevel & rhs) {
        setpointC = rhs.setpointC;
        durationS = rhs.durationS;
        timerS = rhs.timerS;
        startTimeS = rhs.startTimeS;
        state = rhs.state;
    }

    void init() {
        timerS = 0.0;
        startTimeS = 0;
        state = ProfileState::Inactive;
    }

    void start(float timeS, float temperatureC) {
        if (state == ProfileState::Inactive) {
            if (temperatureC < setpointC) {
                state = ProfileState::Pending;

            } else {
                startTimeS = timeS;
                state = ProfileState::Active;
            }
        }
    }

    void stop() {
        state = ProfileState::Inactive;
    }

    void update(float timeS, float temperatureC) {
        if (state == ProfileState::Pending) {
            if (temperatureC >= setpointC) {
                state = ProfileState::Active;
                startTimeS = timeS;
            }

        } else if (state == ProfileState::Active) {
            timerS = timeS - startTimeS;
            if (timerS >= durationS) {
                state = ProfileState::Terminated;
            }
        }
    }

    virtual void convertToJson(JsonObject & json) const {
        json["setpointC"] = setpointC;
        json["durationS"] =durationS;
        json["timerS"] = timerS;
        json["startTimeS"] = startTimeS;
        json["state"] = toString(state);
    }

    float setpointC;
    float durationS;
    float timerS;
    float startTimeS;
    ProfileState state;
};
    

struct TemperatureProfile : public Jsonable {
    TemperatureProfile() :
        startTimeS(0),
        currentTimeS(0),
        state(ProfileState::Inactive),
        activeLevel(-1) {}

    TemperatureProfile(const JsonObject & json) :
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
        startTimeS = rhs.startTimeS;
        currentTimeS = rhs.currentTimeS;
        state = rhs.state;
        levels = rhs.levels;
    }

    float start(float timeS, float temperatureC) {
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
        startTimeS = timeS;

        TemperatureLevel & level = levels[0];
        activeLevel = 0;
        level.start(timeS, temperatureC);
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

    float update(float timeS, float temperatureC) {
        if (activeLevel >= 0 && state != ProfileState::Terminated) {
            currentTimeS = timeS;

            TemperatureLevel * level = &levels[activeLevel];
            level->update(timeS, temperatureC);

            // Go to next state
            if (level->state == ProfileState::Terminated) {
                activeLevel++;

                if (activeLevel >= levels.size()) {
                    state = ProfileState::Terminated;
                    activeLevel = levels.size() - 1;

                } else {
                    level = &levels[activeLevel];
                    level->start(timeS, temperatureC);
                }
            }

            return level->setpointC;

        } else if (activeLevel >= 0 && state == ProfileState::Terminated) {
            // Return setpoint of last active level
            TemperatureLevel * level = &levels[activeLevel];
            return level->setpointC;
        }
        
        return 0.0;
    }

    virtual void convertToJson(JsonObject & json) const {
        json["startTimeS"] = startTimeS;
        json["currentTimeS"] = currentTimeS;
        json["state"] = toString(state);
        
        JsonArray & array = json.createNestedArray("levels");
        for (std::vector<TemperatureLevel>::const_iterator it = levels.begin(); it != levels.end(); it++) {
            const TemperatureLevel & level = *it;
            JsonObject & levelJson = array.createNestedObject();
            
            level.convertToJson(levelJson);
        }
    }

    float startTimeS;
    float currentTimeS;
    ProfileState state;
    std::vector<TemperatureLevel> levels;
    int activeLevel;
};

#endif /*TEMPERATUREPROFILE_H*/