// 
// 
// 

#include "Battery.h"

void BatteryClass::init(ParameterManagerClass* pParameterManager)
{
    m_pParameterManager = pParameterManager;
    m_voltage = 0;
    m_dischargeRate = 6; // 6 mAh/second
    m_reserveCapacityPercentage = 15; // 15% reserve capacity
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

uint16_t BatteryClass::GetRemainingTimeInSeconds() const
{
    if (m_dischargeRate <= 1) // clearly an error
    {
        return 0;
    }
    return (uint16_t)(float(GetRemainingCapacity()) / float(m_dischargeRate));
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

