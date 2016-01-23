// 
// 
// 

#include "DistanceAlert.h"
#include "GCS_MAVLink.h"

void DistanceAlertClass::init()
{
    m_WPNAV_SPEED = 0;
    m_WPNAV_SPEED_DN = 0;
    m_WPNAV_SPEED_UP = 0;
    m_RTL_ALT = 0;
    m_RTL_ALT_FINAL = 0;
    m_LAND_SPEED = 0;

    // home position
    m_hasHomePosition = false;
    m_homeLongtitude = 0;
    m_homeLattitude = 0;
    m_homeAltitude = 0;

    // current position
    m_longtitude = 0;
    m_lattitude = 0;
    m_altitude = 0;

    m_takeoffTime = 0;
    m_batteryAtTakeoff = 0;
    m_batteryRemaining = 0;

}

void DistanceAlertClass::requestData(uint8_t system, uint8_t component)
{
    m_system = system;
    m_component = component;

    requestParameter("WPNAV_SPEED");
    requestParameter("WPNAV_SPEED_DN");
    requestParameter("WPNAV_SPEED_UP");
    requestParameter("RTL_ALT");
    requestParameter("RTL_ALT_FINAL");
    requestParameter("LAND_SPEED");
}

void DistanceAlertClass::requestParameter(const char* pParamId)
{
    mavlink_msg_param_request_read_send(MAVLINK_COMM_0, m_system, m_component, pParamId, -1);
}

#define TAKE_PARAMETER(p)   if (strcmp_P(paramId, PSTR(#p))) \
                            { \
                                m_##p = v; \
                            }

void DistanceAlertClass::handleMessage(const mavlink_message_t* pMsg)
{
    switch (pMsg->msgid)
    {
        case MAVLINK_MSG_ID_PARAM_VALUE:
        {
            char paramId[17];
            paramId[16] = 0;
            mavlink_msg_param_value_get_param_id(pMsg, paramId);

            float v = mavlink_msg_param_value_get_param_value(pMsg);

            TAKE_PARAMETER(WPNAV_SPEED)
            TAKE_PARAMETER(WPNAV_SPEED_DN)
            TAKE_PARAMETER(WPNAV_SPEED_UP)
            TAKE_PARAMETER(RTL_ALT)
            TAKE_PARAMETER(RTL_ALT_FINAL)
            TAKE_PARAMETER(LAND_SPEED)

        }
        break;

        case MAVLINK_MSG_ID_GPS_RAW_INT:
        {
            m_altitude = mavlink_msg_gps_raw_int_get_alt(&msg) / 1000.0f;
            m_lattitude = mavlink_msg_gps_raw_int_get_lat(&msg) / 10000000.0f;
            m_longtitude = mavlink_msg_gps_raw_int_get_lon(&msg) / 10000000.0f;
        }
        break;

        case MAVLINK_MSG_ID_HOME_POSITION:
        {
            m_altitude = mavlink_msg_home_position_get_altitude(&msg) / 1000.0f;
            m_lattitude = mavlink_msg_home_position_get_latitude(&msg) / 10000000.0f;
            m_longtitude = mavlink_msg_home_position_get_longitude(&msg) / 10000000.0f;
            m_hasHomePosition = true;
        }
        break;




    }
}




DistanceAlertClass DistanceAlert;

