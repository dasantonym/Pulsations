//
// Created by anton on 15/06/17.
//

#ifndef PULSATIONS_PACKETS_H
#define PULSATIONS_PACKETS_H

#define USE_SHORT_INT_AS_FLOAT

#include "Types.h"

class Packets {
public:
    static uint32_t unpackFloat(float &target, const char *data, int i) {
#ifdef USE_SHORT_INT_AS_FLOAT
        target = (float)(*((int16_t *) &data[i])) / 80.f;
#else
        target = *((float *) &data[i]);
#endif
        return i + getFloatSize();
    }

    static uint32_t getFloatSize() {
#ifdef USE_SHORT_INT_AS_FLOAT
        return sizeof(int16_t);
#else
        return sizeof(float);
#endif
    }

    static uint32_t getPacketSize() {
        uint32_t _packetSize = 3;
#ifdef RECEIVE_QUATERNION
        _packetSize += 4 * getFloatSize();
#endif
#ifdef RECEIVE_EULER
        _packetSize += 3 * getFloatSize();
#endif
#ifdef RECEIVE_LINEAR_ACCELERATION
        _packetSize += 3 * getFloatSize();
#endif
        return _packetSize;
    }
};


#endif //PULSATIONS_PACKETS_H
