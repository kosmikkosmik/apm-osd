// Battery.h

#ifndef _BATTERY_h
#define _BATTERY_h

#include <FastSerial.h>

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "ParameterManager.h"
#include "ValueFilter.h"

class BatteryClass
{
protected:
    static const int ENTRIES_COUNT = 9;

    float     m_curveVolts[ENTRIES_COUNT];
    uint8_t   m_curvePercentages[ENTRIES_COUNT];
    int8_t    m_apmBatteryPercentage;
    float     m_dischargeRate;
    ValueFilter     m_voltage;
    float     m_powerOnVoltage;
    int8_t    m_powerOnMeasurementCount;
    uint8_t   m_cellCount;
    uint16_t  m_initialCapacity;

    ParameterManagerClass*  m_pParameterManager;

    // Battery estimation
    float     m_estimatedDischargeRate;
    unsigned long   m_measuredTime;
    uint16_t        m_measuredCapacity;

    unsigned long   m_prevMeasuredTime;
    uint16_t        m_prevMeasuredCapacity;

public:
    void init(ParameterManagerClass* pParameterManager);

    void SetApmBatteryPercentage(int8_t percentage);
    void SetDischargeRate(float rate);
    void SetVoltage(float voltage);
    void SetFailsafeVoltage(float voltage);

    uint16_t GetTotalCapacity() const;
    uint16_t GetNominalCapacity() const;
    uint16_t GetFailsafeCapacity() const;
    uint16_t GetRemainingCapacity() const;
    uint8_t GetCellCount() const { return m_cellCount; }
    float GetPowerOnVoltage() const { return m_powerOnVoltage;  }
    bool IsPowerOnVoltageEstimated() const { return m_powerOnMeasurementCount > 10; }

    void SetUsedCapacity(uint16_t capacityUsed);
    void UpdateMeasurements();

    uint16_t GetRemainingTimeInSeconds() const;
    float GetVoltage() const { return m_voltage.getAverage(); }
    uint16_t GetBatteryPercentage() const;
    float GetDischargeRate() const { return m_dischargeRate; }
    void RecalculateInitialCapacity();
    uint8_t GetBatteryPercentage(float cellV) const;
    uint8_t GetApmBatteryPercentage() const { return m_apmBatteryPercentage; }
};

extern BatteryClass Battery;

#endif

