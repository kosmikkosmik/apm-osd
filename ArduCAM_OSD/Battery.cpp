#include "Battery.h"

void BatteryClass::init(ParameterManagerClass* pParameterManager)
{
    m_cellCount = 0; // undefined
    m_pParameterManager = pParameterManager;
    m_voltage.init(10); // 10 slots for averaging
    m_powerOnVoltage = 0;
    m_dischargeRate = 4; // 4 mAh/second
    m_initialCapacity = 0;

    m_estimatedDischargeRate = m_dischargeRate;
    m_measuredCapacity = m_prevMeasuredCapacity = 0;
    m_measuredTime = m_prevMeasuredTime = 0;

    m_powerOnMeasurementCount = 0;

    m_curveVolts[0] = 4.2;
    m_curveVolts[1] = 4.1;
    m_curveVolts[2] = 4.0;
    m_curveVolts[3] = 3.95;
    m_curveVolts[4] = 3.81;
    m_curveVolts[5] = 3.75;
    m_curveVolts[6] = 3.7;
    m_curveVolts[7] = 3.65;
    m_curveVolts[8] = 3.43;
    
    m_curvePercentages[0] = 100;
    m_curvePercentages[1] = 95;
    m_curvePercentages[2] = 85;
    m_curvePercentages[3] = 77;
    m_curvePercentages[4] = 56;
    m_curvePercentages[5] = 50;
    m_curvePercentages[6] = 25;
    m_curvePercentages[7] = 17;
    m_curvePercentages[8] = 8;
}

void BatteryClass::SetApmBatteryPercentage(int8_t pc)
{
    m_apmBatteryPercentage = pc;
}


void BatteryClass::SetDischargeRate(float rate)
{
    m_dischargeRate = rate;
}

void BatteryClass::SetVoltage(float voltage)
{
    m_voltage.addValue(voltage);

    // Measuring initial voltage
    if (!IsPowerOnVoltageEstimated())
    {
        if (voltage > m_powerOnVoltage)
        {
            m_powerOnVoltage = voltage;
        }
        m_powerOnMeasurementCount++;
    }
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
    RecalculateInitialCapacity();

/*    if (m_prevMeasuredCapacity > 0)
    {
        unsigned long interval = m_measuredTime - m_prevMeasuredTime;
        uint16_t usedCapacity = m_measuredCapacity - m_prevMeasuredCapacity;

        m_estimatedDischargeRate = float(usedCapacity) * 1000 / interval;
    }

    m_prevMeasuredCapacity = m_measuredCapacity;
    m_prevMeasuredTime = m_prevMeasuredTime; */
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
    return m_initialCapacity;
}

uint16_t BatteryClass::GetRemainingCapacity() const
{
    uint16_t totalRemainingCapacity = uint16_t(float(m_apmBatteryPercentage) * GetTotalCapacity() / 100);
    uint16_t failsafeCapacity = GetFailsafeCapacity();

    if (totalRemainingCapacity <= failsafeCapacity)
    {
        return 0;
    }

    return totalRemainingCapacity - failsafeCapacity;
}

uint16_t BatteryClass::GetFailsafeCapacity() const
{
    return m_pParameterManager->getParameter(ParameterManagerClass::FS_BATT_MAH);
}

uint16_t BatteryClass::GetNominalCapacity() const
{
    return m_pParameterManager->getParameter(ParameterManagerClass::BATT_CAPACITY);
}

void BatteryClass::RecalculateInitialCapacity()
{
    if (!IsPowerOnVoltageEstimated())
    {
        return;
    }

    if (m_initialCapacity != 0)
    {
        // already initialized
        return;
    }

    uint16_t nominal = GetNominalCapacity();

    if ((m_cellCount == 0) || (m_powerOnVoltage == 0) || (nominal == 0))
    {
        // not enough data yet
        return;
    }

    m_initialCapacity = float(nominal) * GetBatteryPercentage(m_powerOnVoltage / m_cellCount) / 100;
}

uint16_t BatteryClass::GetBatteryPercentage() const
{
    if (GetNominalCapacity() < GetFailsafeCapacity())
    {
        return 0;
    }

    uint16_t capacity = GetRemainingCapacity();
    uint16_t total = GetNominalCapacity() - GetFailsafeCapacity();


    return float(capacity) * 100 / total ;
}

uint8_t BatteryClass::GetBatteryPercentage(float cellV) const
{
    uint8_t prevPercentage = 0;
    float prevVoltage = 3.2;

    if (cellV <= prevVoltage)
    {
        return 0;
    }

    for (int i = ENTRIES_COUNT - 1; i >= 0; i--)
    {
        if (cellV <= m_curveVolts[i])
        {
            float r = (cellV - prevVoltage) / (m_curveVolts[i] - prevVoltage);
            return uint8_t(r * (m_curvePercentages[i] - prevPercentage)) + prevPercentage;
        }

        prevVoltage = m_curveVolts[i];
        prevPercentage = m_curvePercentages[i];
    }

    return 100;
}


BatteryClass Battery;

