// ParameterManager.h

#ifndef _PARAMETERMANAGER_h
#define _PARAMETERMANAGER_h

#include <FastSerial.h>

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

#include "../GCS_MAVLink/include/mavlink/v1.0/mavlink_types.h"
#include <avr/pgmspace.h>
#include "ArduCam_Max7456.h"

#define PARAMETER_INTERFACE(p) protected: \
                                float m_paramValue_##p; \
                                bool m_paramReceived_##p; \
                             public: \
                                float get_##p() const { return m_paramValue_##p; } \
                                bool isReceived_##p() const { return m_paramReceived_##p; }


const int PARAMETER_COUNT = 9;

class ParameterManagerClass
{
public:
    enum ParameterId
    {
        WPNAV_SPEED = 0,
        WPNAV_SPEED_DN,
        WPNAV_SPEED_UP,
        RTL_ALT,
        RTL_ALT_FINAL,
        LAND_SPEED,
        BATT_CAPACITY,
        FS_BATT_MAH,
        FS_BATT_VOLTAGE
    };

protected:

     struct Param
     {
         float  m_value;
         bool   m_received;
     };

     Param          m_parameters[PARAMETER_COUNT];

     uint8_t        m_system;
     uint8_t        m_component;
     int            m_pendingParameters;
     unsigned long  m_lastReceivedTime;
     int            m_retryCount;

     void requestMissingParameters();
     void setParameter(ParameterId id, float value);
public:
	void init();

    void requestData(uint8_t system, uint8_t component);
    void handleMessage(const mavlink_message_t* pMsg);

    bool isInSync() const;
    int getPendingCount() const { return m_pendingParameters;  }

    float getParameter(ParameterId id) { return m_parameters[(int)id].m_value; }

    void showDiagnostics(OSD& osd) const;
};

extern ParameterManagerClass ParameterManager;

#endif

