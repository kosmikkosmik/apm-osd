// DistanceAlert.h

#ifndef _DISTANCEALERT_h
#define _DISTANCEALERT_h

#include <FastSerial.h>

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "../GCS_MAVLink/include/mavlink/v1.0/mavlink_types.h"

class DistanceAlertClass
{
 protected:
    // parameters
    float m_WPNAV_SPEED;
    float m_WPNAV_SPEED_DN;
    float m_WPNAV_SPEED_UP;
    float m_RTL_ALT;
    float m_RTL_ALT_FINAL;
    float m_LAND_SPEED;

    // home position
    bool  m_hasHomePosition;
    float m_homeLongtitude;
    float m_homeLattitude;
    float m_homeAltitude;

    // current position
    float m_longtitude;
    float m_lattitude;
    float m_altitude;

    float m_horizontalDistance;

    unsigned long m_takeoffTime;
    uint8_t m_batteryAtTakeoff;
    uint8_t m_batteryRemaining;

    uint8_t m_system;
    uint8_t m_component;

    void requestParameter(const char* pParamId);

 public:
	void init();

    void requestData(uint8_t system, uint8_t component);
    void handleMessage(const mavlink_message_t* pMsg);

    bool hasHomePosition() const { return m_hasHomePosition; }
    float getMaxFlightTime() const;
    float getMaxHeight() const;
    float getMaxDistance() const;
};

extern DistanceAlertClass DistanceAlert;

#endif

