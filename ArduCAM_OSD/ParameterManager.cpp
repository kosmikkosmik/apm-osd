// 
// 
// 

#include "ParameterManager.h"
#include "GCS_MAVLink.h"
#include <avr/pgmspace.h>

#define DECLARE_PARAMETER(p) const char paramId_##p[] PROGMEM=#p; 

#define PARAMETER(p) paramId_##p

DECLARE_PARAMETER(WPNAV_SPEED)
DECLARE_PARAMETER(WPNAV_SPEED_DN)
DECLARE_PARAMETER(WPNAV_SPEED_UP)
DECLARE_PARAMETER(RTL_ALT)
DECLARE_PARAMETER(RTL_ALT_FINAL)
DECLARE_PARAMETER(LAND_SPEED)
DECLARE_PARAMETER(BATT_CAPACITY)
DECLARE_PARAMETER(FS_BATT_MAH)

const char* const parameterIds[] PROGMEM = 
{ 
    PARAMETER(WPNAV_SPEED),
    PARAMETER(WPNAV_SPEED_DN),
    PARAMETER(WPNAV_SPEED_UP),
    PARAMETER(RTL_ALT),
    PARAMETER(RTL_ALT_FINAL),
    PARAMETER(LAND_SPEED),
    PARAMETER(BATT_CAPACITY),
    PARAMETER(FS_BATT_MAH)
};


void ParameterManagerClass::init()
{
    m_system = 0;
    m_component = 0;
    m_pendingParameters = PARAMETER_COUNT;
    m_lastReceivedTime = 0;
    m_retryCount = 0;

    for (int i = 0; i < PARAMETER_COUNT; i++)
    {
        m_parameters[i].m_received = false;
        m_parameters[i].m_value = false;
    }
}

void ParameterManagerClass::requestData(uint8_t system, uint8_t component)
{
    m_system = system;
    m_component = component;

    requestMissingParameters();
}


void ParameterManagerClass::handleMessage(const mavlink_message_t* pMsg)
{
    unsigned int timestamp = millis();

    switch (pMsg->msgid)
    {
        case MAVLINK_MSG_ID_PARAM_VALUE:
        {
            char paramId[17];
            paramId[16] = 0;
            mavlink_msg_param_value_get_param_id(pMsg, paramId);

            for (int i = 0; i < PARAMETER_COUNT; i++)
            {
                if (strcmp_P(paramId, (char*)pgm_read_word(&(parameterIds[i]))) == 0)
                {
                    m_parameters[i].m_value = mavlink_msg_param_value_get_param_value(pMsg);
                    if (!m_parameters[i].m_received)
                    {
                        m_parameters[i].m_received = true;
                        m_pendingParameters--;
                        m_lastReceivedTime = timestamp;
                    }
                }
            }

        }
        break;
    }

    // Re-request paramerters on timeout
    if (m_pendingParameters > 0 && ((timestamp - m_lastReceivedTime) > 1000))
    {
        m_lastReceivedTime = timestamp;
        m_retryCount++;
        requestMissingParameters();
    }
}

bool ParameterManagerClass::isInSync() const
{
    return (m_pendingParameters == 0);
}

void ParameterManagerClass::requestMissingParameters()
{
    char buffer[17];

    for (int i = 0; i < PARAMETER_COUNT; i++)
    {
        if (!m_parameters[i].m_received)
        {
            strcpy_P(buffer, (char*)pgm_read_word(&(parameterIds[i])));
            mavlink_msg_param_request_read_send(MAVLINK_COMM_0, m_system, m_component, buffer, -1);
        }
    }
}

void ParameterManagerClass::showDiagnostics(OSD& osd) const
{
    int row = 1;
    osd.clear();

    char buffer[17];

    for (int i = 0; i < PARAMETER_COUNT; i++)
    {
        strcpy_P(buffer, (char*)pgm_read_word(&(parameterIds[i])));

        osd.setPanel(1, row++);
        osd.openPanel();
        if (m_parameters[i].m_received)
        {
            osd.printf("%s: %.2f", buffer, m_parameters[i].m_value);
        }
        else
        {
            osd.printf("%s: pending", buffer);
        }
        osd.closePanel();
    }

    osd.setPanel(1, row++);
    osd.openPanel();
    osd.printf("retries: %i", m_retryCount);
    osd.closePanel();
}


ParameterManagerClass ParameterManager;

