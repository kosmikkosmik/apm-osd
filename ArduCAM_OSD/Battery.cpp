#include "Battery.h"

void BatteryClass::init(ParameterManagerClass* pParameterManager)
{
    m_cellCount = 0; // undefined
    m_pParameterManager = pParameterManager;
    m_voltage = 0;
    m_dischargeRate = 4; // 4 mAh/second
    m_reserveCapacityPercentage = 15; // 15% reserve capacity

    m_estimatedDischargeRate = m_dischargeRate;
    m_measuredCapacity = m_prevMeasuredCapacity = 0;
    m_measuredTime = m_prevMeasuredTime = 0;
}

void BatteryClass::SetBatteryPercentage(int8_t pc)
{
    m_batteryPercentage = pc;
}


void BatteryClass::SetDischargeRate(float rate)
{
    m_dischargeRate = rate;
}

void BatteryClass::SetVoltage(float voltage)
{
    m_voltage = voltage;
}

void BatteryClass::SetFailsafeVoltage(float voltage)
{
    if (m_cellCount == 0)
    {
        m_cellCount = floor(voltage / 3.2);
    }
}

void BatteryClass::UpdateMeasurements()
{
    if (m_prevMeasuredCapacity > 0)
    {
        unsigned long interval = m_measuredTime - m_prevMeasuredTime;
        uint16_t usedCapacity = m_measuredCapacity - m_prevMeasuredCapacity;

        m_estimatedDischargeRate = float(usedCapacity) * 1000 / interval;
    }

    m_prevMeasuredCapacity = m_measuredCapacity;
    m_prevMeasuredTime = m_prevMeasuredTime;
}

void BatteryClass::SetUsedCapacity(uint16_t usedCapacity)
{
    m_measuredCapacity = usedCapacity;
    m_measuredTime = millis();
}

uint16_t BatteryClass::GetRemainingTimeInSeconds() const
{
    if (m_dischargeRate <= 1) // clearly an error
    {
        return 0;
    }
    return (uint16_t)(float(GetRemainingCapacity()) / m_dischargeRate);
}

uint16_t BatteryClass::GetTotalCapacity() const
{
    return m_pParameterManager->getParameter(ParameterManagerClass::BATT_CAPACITY);
}

uint16_t BatteryClass::GetRemainingCapacity() const
{
    return uint16_t(float(m_batteryPercentage) * (100 - m_reserveCapacityPercentage) / 10000 * GetTotalCapacity());
}

uint16_t BatteryClass::GetFailsafeCapacity() const
{
    return m_pParameterManager->getParameter(ParameterManagerClass::FS_BATT_MAH);
}



BatteryClass Battery;

