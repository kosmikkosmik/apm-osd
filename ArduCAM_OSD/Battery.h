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

class BatteryClass
{
protected:
    int8_t    m_batteryPercentage;
    uint16_t  m_dischargeRate;
    uint8_t   m_reserveCapacityPercentage;
    float     m_voltage;
    ParameterManagerClass*  m_pParameterManager;

public:
    void init(ParameterManagerClass* pParameterManager);

    void SetBatteryPercentage(int8_t percentage);
    void SetDischargeRate(float rate);
    void SetVoltage(float voltage);
    uint16_t GetTotalCapacity() const;
    uint16_t GetFailsafeCapacity() const;
    uint16_t GetRemainingCapacity() const;

    uint16_t GetRemainingTimeInSeconds() const;
    float GetVoltage() const { return m_voltage; }
    uint16_t GetBatteryPercentage() const { return m_batteryPercentage; }
    uint16_t GetDischargeRate() const { return m_dischargeRate; }
    uint8_t GetReserveCapacityPercentage() const { return m_reserveCapacityPercentage; }
};

extern BatteryClass Battery;

#endif

