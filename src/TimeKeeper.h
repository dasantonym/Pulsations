//
// Created by anton on 05/06/17.
//

#ifndef PULSATIONS_TIME_KEEPER_H
#define PULSATIONS_TIME_KEEPER_H

#include <cstdint>
#include <cmath>
#include <chrono>

using namespace std::chrono;

class TimeKeeper {
public:
    uint64_t getTimeMillis() {
        milliseconds ms = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
        return (uint64_t) ms.count();
    }
};


#endif //PULSATIONS_TIME_KEEPER_H
