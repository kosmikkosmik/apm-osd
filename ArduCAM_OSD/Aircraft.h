#ifndef __AIRCRAFT_H__
#define __AIRCRAFT_H__

#include <FastSerial.h>
#include "GCS_MAVLink.h"
#include "Battery.h"
#include "ParameterManager.h"
#include "DistanceAlert.h"

const int WARNING_TEXT_TOTAL = 50;

class Aircraft
{
public:
    enum EkfStatus
    {
        EKFStatus_OK = 0,
        EKFStatus_WARN = 1,
        EKFStatus_BAD = 2
    };

    enum CustomMode
    {
        CustomMode_Stabilize    = 0,
        CustomMode_Acro         = 1,
        CustomMode_AltitudeHold = 2,
        CustomMode_Auto         = 3,
        CustomMode_Guided       = 4,
        CustomMode_Loiter       = 5,
        CustomMode_RTL          = 6,
        CustomMode_Circle       = 7,
        CustomMode_Land         = 9,
        CustomMode_Drift        = 11,
        CustomMode_Sport        = 13,
        CustomMode_Flip         = 14,
        CustomMode_AutoTune     = 15,
        CustomMode_PositionHold = 16,
        CustomMode_Break        = 17
    };

    Aircraft();

    void init();

    void processMessage(const mavlink_message_t& msg);
    const BatteryClass& getBattery() const { return Battery; }
    const DistanceAlertClass& getDistanceAlert() const { return DistanceAlert; }

    const char* getLastWarning() const { return m_lastWarning;  }
    unsigned long getLastWarningTimestamp() const { return m_lastWarningTimestamp; }
    uint8_t getFixType() const { return m_fixType; }
    EkfStatus getEkfStatus() const { return m_ekfStatus; }
    MAV_STATE getState() const { return m_state; }
    uint8_t getThrottle() const { return m_throttle; }
    float getGroundSpeed() const { return m_groundSpeed; }
    float getClimbRate() const { return m_climbRate; }
    uint16_t getDirectionToHome() const;
    CustomMode getCustomMode() const { return m_customMode; }
    bool armed() const { return m_armed; }

    bool connected() const { return m_gotHeartbeat; }
    MAV_TYPE getType() const { return m_mavType; }

    uint32_t getSensorsHealth() const { return m_sensorsHealth; }
    uint32_t getSensorsPresence() const { return m_sensorsPresence; }

    uint8_t getSatellites() const { return m_satellites; }
    uint16_t getHdop() const { return m_hdop; }
    uint16_t getVdop() const { return m_vdop; }

    void beginCompassCalibration();
    void acceptCompassCalibration();
    void cancelCompassCalibration();
    MAG_CAL_STATUS getCompassCalibrationStatus() const { return m_magCalStatus; }
    uint8_t getCompassCalibrationPercentage() const { return m_magCalPercentage; }

private:
    void requestMavlinkStreams();

private:
    uint8_t     m_mavSystem;
    uint8_t     m_mavComponent;
    
    CustomMode  m_customMode;
    bool        m_armed;
    MAV_STATE   m_state;

    bool        m_gotHeartbeat;

    uint8_t     m_fixType;

    float       m_airSpeed;
    float       m_groundSpeed;
    int16_t     m_heading;
    uint8_t     m_throttle;
    float       m_altitude;
    float       m_climbRate;

    char        m_lastWarning[WARNING_TEXT_TOTAL + 1];
    unsigned long   m_lastWarningTimestamp;

    EkfStatus   m_ekfStatus;
    uint32_t    m_sensorsHealth;
    uint32_t    m_sensorsPresence;
    MAV_TYPE    m_mavType;
    uint8_t     m_satellites;
    uint16_t    m_hdop;
    uint16_t    m_vdop;

    MAG_CAL_STATUS  m_magCalStatus;
    uint8_t         m_magCalPercentage;
};

#endif // __AIRCRAFT_H__

