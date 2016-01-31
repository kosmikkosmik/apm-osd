#include "DistanceAlert.h"
#include "GCS_MAVLink.h"
#include "Battery.h"
#include "ParameterManager.h"

void DistanceAlertClass::init(BatteryClass* pBattery)
{
    m_pBattery = pBattery;

    // home position
    m_hasHomePosition = false;
    m_homeLongtitude = 0;
    m_homeLattitude = 0;
    m_homeAltitude = 0;

    // current position
    m_longtitude = 0;
    m_lattitude = 0;
    m_altitude = 0;
}

void DistanceAlertClass::requestData(uint8_t system, uint8_t component)
{
    m_system = system;
    m_component = component;

    // Request home position
    mavlink_msg_command_long_send(MAVLINK_COMM_0, m_system, m_component, MAV_CMD_GET_HOME_POSITION, 0, 0, 0, 0, 0, 0, 0, 0);
}

void DistanceAlertClass::handleMessage(const mavlink_message_t* pMsg)
{
    switch (pMsg->msgid)
    {
        case MAVLINK_MSG_ID_GPS_RAW_INT:
        {
            m_altitude = mavlink_msg_gps_raw_int_get_alt(pMsg) / 1000.0f;
            m_lattitude = mavlink_msg_gps_raw_int_get_lat(pMsg) / 10000000.0f;
            m_longtitude = mavlink_msg_gps_raw_int_get_lon(pMsg) / 10000000.0f;
        }
        break;

        case MAVLINK_MSG_ID_HOME_POSITION:
        {
            m_homeAltitude = mavlink_msg_home_position_get_altitude(pMsg) / 1000.0f;
            m_homeLattitude = mavlink_msg_home_position_get_latitude(pMsg) / 10000000.0f;
            m_homeLongtitude = mavlink_msg_home_position_get_longitude(pMsg) / 10000000.0f;
            m_hasHomePosition = true;
        }
        break;

        case MAVLINK_MSG_ID_SYS_STATUS:
        {
            m_pBattery->SetBatteryPercentage(mavlink_msg_sys_status_get_battery_remaining(pMsg));
            m_pBattery->SetVoltage(float(mavlink_msg_sys_status_get_voltage_battery(pMsg)) / 1000.0);
        }
        break;

    }
}

void DistanceAlertClass::recalculate()
{
    if (!m_hasHomePosition)
    {
        return;
    }

    m_horizontalDistance = getDistance(m_homeLattitude, m_homeLattitude, m_lattitude, m_longtitude);

}

uint16_t DistanceAlertClass::getMaxFlightTime() const
{
    if (!m_hasHomePosition)
    {
        return 0;
    }

    float horizontalReturnInSec = m_horizontalDistance / ParameterManager.getParameter(ParameterManagerClass::WPNAV_SPEED) / 100;
    float verticalReturnInSec = getAltitudeToHome() / ParameterManager.getParameter(ParameterManagerClass::WPNAV_SPEED_DN) / 100;
    float returnTime = horizontalReturnInSec + verticalReturnInSec;

    return m_pBattery->GetRemainingTime() - returnTime;
}

#define d2r (M_PI / 180.0)

float DistanceAlertClass::getDistance(float lat1, float long1, float lat2, float long2) const
{
    double dlong = (long2 - long1) * d2r;
    double dlat = (lat2 - lat1) * d2r;
    double a = pow(sin(dlat / 2.0), 2) + cos(lat1*d2r) * cos(lat2*d2r) * pow(sin(dlong / 2.0), 2);
    double c = 2 * atan2(sqrt(a), sqrt(1 - a));
    float d = 6367 * c;
    if (d >= 0)
    {
        return d;
    }
    else
    {
        return -d;
    }
}


DistanceAlertClass DistanceAlert;

