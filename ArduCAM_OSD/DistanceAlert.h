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

class BatteryClass;

class DistanceAlertClass
{
 protected:
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
    uint16_t m_directionToHome;

    uint8_t m_system;
    uint8_t m_component;
    BatteryClass*    m_pBattery;


 public:
	void init(BatteryClass* pBattery);

    void requestData(uint8_t system, uint8_t component);
    void handleMessage(const mavlink_message_t* pMsg);

    bool hasHomePosition() const { return m_hasHomePosition; }
    float getDistanceToHome() const { return m_horizontalDistance; }
    uint16_t getDirectionToHome() const { return m_directionToHome; }
    float getAltitudeToHome() const { return m_altitude - m_homeAltitude; }

    uint16_t getMaxFlightTimeInSeconds() const;

private:
    void recalculate();
    float getDistance(float lat1, float long1, float lat2, float long2) const;
};

extern DistanceAlertClass DistanceAlert;

#endif

