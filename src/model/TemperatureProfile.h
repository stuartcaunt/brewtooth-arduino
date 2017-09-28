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
        toleranceC(0.0),
        durationS(0),
        timerS(0.0),
        startTimeS(0),
        state(ProfileState::Inactive) {}

    TemperatureLevel(const JsonObject & json) :
        setpointC(json["setpointC"]),
        toleranceC(0.0),
        durationS(json["durationS"]),
        timerS(0.0),
        startTimeS(0),
        state(ProfileState::Inactive) {}

    TemperatureLevel & operator=(const TemperatureLevel & rhs) {
        setpointC = rhs.setpointC;
        toleranceC = rhs.toleranceC;
        durationS = rhs.durationS;
        timerS = rhs.timerS;
        startTimeS = rhs.startTimeS;
        state = rhs.state;
    }

    void init(float toleranceC) {
        toleranceC = toleranceC;
        timerS = 0.0;
        startTimeS = 0;
        state = ProfileState::Inactive;
    }

    void start(float timeS, float temperatureC) {
        if (state == ProfileState::Inactive) {
            if (temperatureC < (setpointC - toleranceC)) {
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
            if (temperatureC >= (setpointC - toleranceC)) {
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
    float toleranceC;
    float durationS;
    float timerS;
    float startTimeS;
    ProfileState state;
};
    

struct TemperatureProfile : public Jsonable {
    TemperatureProfile() :
        startTimeS(0.0),
        currentTimeS(0.0),
        toleranceC(0.0),
        state(ProfileState::Inactive),
        activeLevel(-1) {}

    TemperatureProfile(const JsonObject & json) :
        startTimeS(0.0),
        currentTimeS(0.0),
        toleranceC(json["toleranceC"]),
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
        toleranceC = rhs.toleranceC;
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
            (*it).init(toleranceC);
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
        json["toleranceC"] = toleranceC;
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
    float toleranceC;
    ProfileState state;
    std::vector<TemperatureLevel> levels;
    int activeLevel;
};

#endif /*TEMPERATUREPROFILE_H*/