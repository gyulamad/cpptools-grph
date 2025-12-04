#pragma once

#include "../misc/datetime_defs.hpp"

class TimePoint {
public:
    TimePoint(time_sec time, float value): time(time), value(value) {}
    virtual ~TimePoint() {}

    time_sec getTime() const { return time; }
    float getValue() const { return value; }

    void setTime(time_sec time) { this->time = time; }
    void setValue(float value) { this->value = value; }

protected:
    time_sec time;
    float value;
};
