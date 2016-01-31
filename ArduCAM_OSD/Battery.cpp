// 
// 
// 

#include "Battery.h"

void BatteryClass::init(ParameterManagerClass* pParameterManager)
{
    m_pParameterManager = pParameterManager;
    m_voltage = 0;
    m_dischargeRate = 6; // 6 mAh/second
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

unsigned long BatteryClass::GetRemainingTime() const
{
    if (m_dischargeRate <= 1) // clearly an error
    {
        return 0;
    }
    return (unsigned long)(float(GetRemainingCapacity()) / m_dischargeRate * 1000);
}

uint16_t BatteryClass::GetTotalCapacity() const
{
    return m_pParameterManager->getParameter(ParameterManagerClass::BATT_CAPACITY);
}

uint16_t BatteryClass::GetRemainingCapacity() const
{
    return uint16_t(float(m_batteryPercentage) / 100 * GetTotalCapacity());
}

uint16_t BatteryClass::GetFailsafeCapacity() const
{
    return m_pParameterManager->getParameter(ParameterManagerClass::FS_BATT_MAH);
}



BatteryClass Battery;

