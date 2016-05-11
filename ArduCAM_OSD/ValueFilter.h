#ifndef __ValueFilter_H__
#define __ValueFilter_H__

#include <FastSerial.h>

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

class ValueFilter
{
public:
    ValueFilter();
    ~ValueFilter();

    void init(uint8_t size);
    void addValue(float f);
    float getAverage() const { return m_average; }
private:
    float*  m_pBuffer;
    uint8_t m_size;
    uint8_t m_headIndex;
    uint8_t m_valueCount;
    float   m_average;
};

#endif