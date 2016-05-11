#include "ValueFilter.h"


ValueFilter::ValueFilter():
m_size(0),
m_headIndex(0),
m_valueCount(0),
m_average(0)
{
}

void ValueFilter::init(uint8_t size)
{
    m_pBuffer = new float[size];

    m_size = size;
    for (uint8_t i = 0; i < size; i++)
    {
        m_pBuffer[i] = 0;
    }
}


ValueFilter::~ValueFilter()
{
    delete m_pBuffer;
}


void ValueFilter::addValue(float v)
{
    m_pBuffer[m_headIndex++] = v;
    if (m_headIndex == m_size)
    {
        m_headIndex = 0;
    }

    if (m_valueCount < m_size)
    {
        m_valueCount++;
    }

    m_average = 0;
    for (uint8_t i = 0; i < m_size; i++)
    {
        m_average += m_pBuffer[i];
    }

    m_average /= m_valueCount;
}