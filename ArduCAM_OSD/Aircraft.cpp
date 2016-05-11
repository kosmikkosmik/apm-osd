#include "Aircraft.h"

Aircraft::Aircraft()
{
    m_customMode = (CustomMode)-1; // just to see that we didn't get value
    m_armed = false;
    m_state = MAV_STATE_UNINIT;
    m_gotHeartbeat = false;
    m_fixType = 0;
    m_sensorsHealth = 0;
    m_sensorsPresence = -1;
    m_mavType = MAV_TYPE_ENUM_END;
    m_satellites = 0;
    m_hdop = -1;
    m_vdop = -1;
    m_magCalStatus = MAG_CAL_NOT_STARTED;
    m_magCalPercentage = 0;
}

void Aircraft::init()
{
    ParameterManager.init();
    Battery.init(&ParameterManager);
    DistanceAlert.init(&Battery);
}

void Aircraft::processMessage(const mavlink_message_t& msg)
{
    //handle msg
    switch (msg.msgid)
    {
    case MAVLINK_MSG_ID_HEARTBEAT:
    {
        MAV_TYPE source = (MAV_TYPE)mavlink_msg_heartbeat_get_type(&msg);
        if (source == MAV_TYPE_GCS)
        {
            if (!connected())
            {
                m_mavType = source;
            }
            return;
        }

        m_mavSystem = msg.sysid;
        m_mavComponent = msg.compid;

        m_customMode = (CustomMode)mavlink_msg_heartbeat_get_custom_mode(&msg);
        m_armed = ((mavlink_msg_heartbeat_get_base_mode(&msg) & MAV_MODE_FLAG_SAFETY_ARMED) != 0);
        m_state = (MAV_STATE)mavlink_msg_heartbeat_get_system_status(&msg);

        if (!m_gotHeartbeat)
        {
            m_gotHeartbeat = 1;
            requestMavlinkStreams();

            // Make modules request their stuff
            DistanceAlert.requestData(m_mavSystem, m_mavComponent);
            ParameterManager.requestData(m_mavSystem, m_mavComponent);
        }
    }
    break;
    
    case MAVLINK_MSG_ID_SYS_STATUS:
    {
        if (!m_gotHeartbeat)
        {
            return;
        }

        uint32_t enabledSensors = mavlink_msg_sys_status_get_onboard_control_sensors_enabled(&msg);
        uint32_t presentSensors = mavlink_msg_sys_status_get_onboard_control_sensors_present(&msg);
        m_sensorsPresence = presentSensors & enabledSensors;

        m_sensorsHealth = mavlink_msg_sys_status_get_onboard_control_sensors_health(&msg);
    }
    break;

    case MAVLINK_MSG_ID_GPS_RAW_INT:
    {
        if (!m_gotHeartbeat)
        {
            return;
        }

        m_fixType = mavlink_msg_gps_raw_int_get_fix_type(&msg);
        m_satellites = mavlink_msg_gps_raw_int_get_satellites_visible(&msg);
        m_hdop = mavlink_msg_gps_raw_int_get_eph(&msg);
        m_vdop = mavlink_msg_gps_raw_int_get_epv(&msg);
    }
    break;

    case MAVLINK_MSG_ID_VFR_HUD:
    {
        if (!m_gotHeartbeat)
        {
            return;
        }

        m_airSpeed = mavlink_msg_vfr_hud_get_airspeed(&msg);
        m_groundSpeed = mavlink_msg_vfr_hud_get_groundspeed(&msg);
        m_heading = mavlink_msg_vfr_hud_get_heading(&msg); // 0..360 deg, 0=north
        m_throttle = (uint8_t)mavlink_msg_vfr_hud_get_throttle(&msg);
        m_altitude = mavlink_msg_vfr_hud_get_alt(&msg);
        m_climbRate = mavlink_msg_vfr_hud_get_climb(&msg);
    }
    break;


    case MAVLINK_MSG_ID_STATUSTEXT:
    {
        if (!m_gotHeartbeat)
        {
            return;
        }

        memset(m_lastWarning, 0, WARNING_TEXT_TOTAL + 1);
        mavlink_msg_statustext_get_text(&msg, m_lastWarning);
        int length = strlen(m_lastWarning);
        for (int i = 0; i < length; i++)
        {
            m_lastWarning[i] = tolower(m_lastWarning[i]);
        }

        m_lastWarningTimestamp = millis();
    }
    break;

    case MAVLINK_MSG_ID_EKF_STATUS_REPORT:
    {
        if (!m_gotHeartbeat)
        {
            return;
        }

        const float ekf_warn_level = 0.5;
        const float ekf_bad_level = 0.8;

        mavlink_ekf_status_report_t ekf_status;
        mavlink_msg_ekf_status_report_decode(&msg, &ekf_status);

        if ((ekf_status.velocity_variance >= ekf_bad_level) ||
            (ekf_status.pos_horiz_variance >= ekf_bad_level) ||
            (ekf_status.pos_vert_variance >= ekf_bad_level) ||
            (ekf_status.compass_variance >= ekf_bad_level) ||
            (ekf_status.terrain_alt_variance >= ekf_bad_level))
        {
            m_ekfStatus = EKFStatus_BAD;
        }
        else if ((ekf_status.velocity_variance >= ekf_warn_level) ||
            (ekf_status.pos_horiz_variance >= ekf_warn_level) ||
            (ekf_status.pos_vert_variance >= ekf_warn_level) ||
            (ekf_status.compass_variance >= ekf_warn_level) ||
            (ekf_status.terrain_alt_variance >= ekf_warn_level))
        {
            m_ekfStatus = EKFStatus_WARN;
        }
        else
        {
            m_ekfStatus = EKFStatus_OK;
        }
    }
    break;

    case MAVLINK_MSG_ID_MAG_CAL_PROGRESS:
    {
        m_magCalStatus = (MAG_CAL_STATUS)mavlink_msg_mag_cal_progress_get_cal_status(&msg);
        m_magCalPercentage = mavlink_msg_mag_cal_progress_get_completion_pct(&msg);

/*        if (m_magCalStatus == MAG_CAL_SUCCESS)
        {
            cancelCompassCalibration();
        } */
    }
    break;

    case MAVLINK_MSG_ID_MAG_CAL_REPORT:
    {
        m_magCalStatus = (MAG_CAL_STATUS)mavlink_msg_mag_cal_report_get_cal_status(&msg);
    }
    break;

    }
    
    if (m_gotHeartbeat)
    {
        ParameterManager.handleMessage(&msg);
        DistanceAlert.handleMessage(&msg);
    } 
}

void Aircraft::requestMavlinkStreams()
{
    const int  maxStreams = 6;
    const uint8_t MAVStreams[maxStreams] = 
    { 
        MAV_DATA_STREAM_RAW_SENSORS,
        MAV_DATA_STREAM_EXTENDED_STATUS,
        MAV_DATA_STREAM_POSITION,
        MAV_DATA_STREAM_EXTRA1,
        MAV_DATA_STREAM_EXTRA2,
        MAV_DATA_STREAM_EXTRA3 
    };

    const uint16_t MAVRates[maxStreams] = 
    { 
        1,      // MAV_DATA_STREAM_RAW_SENSORS
        2,      // MAV_DATA_STREAM_EXTENDED_STATUS
        2,      // MAV_DATA_STREAM_POSITION
        5,      // MAV_DATA_STREAM_EXTRA1
        2,      // MAV_DATA_STREAM_EXTRA2
        2       // MAV_DATA_STREAM_EXTRA3
    };

    for (int i = 0; i < maxStreams; i++)
    {
        mavlink_msg_request_data_stream_send(MAVLINK_COMM_0, m_mavSystem, m_mavComponent, MAVStreams[i], MAVRates[i], 1);
    }
}

uint16_t Aircraft::getDirectionToHome() const
{
    uint16_t dir = DistanceAlert.getDirectionToHome() - m_heading;
    if (dir < 0)
    {
        return dir + 360;
    }
    else if (dir >= 360)
    {
        return dir - 360;
    }
}

void Aircraft::beginCompassCalibration()
{
    m_magCalStatus = MAG_CAL_WAITING_TO_START;
    m_magCalPercentage = 0;
    
    mavlink_msg_command_long_send(MAVLINK_COMM_0, 
                                  m_mavSystem, 
                                  m_mavComponent, 
                                  MAV_CMD_DO_START_MAG_CAL, 
                                  0,    //  
                                  1,    // uint8_t bitmask of magnetometers (0 means all)
                                  0,    // Automatically retry on failure (0=no retry, 1=retry).
                                  0,    // Save without user input (0=require input, 1=autosave).
                                  0,    // Delay (seconds)
                                  0,    // Autoreboot (0=user reboot, 1=autoreboot)
                                  0, 
                                  0); 
}

void Aircraft::acceptCompassCalibration()
{
    mavlink_msg_command_long_send(MAVLINK_COMM_0, m_mavSystem, m_mavComponent, MAV_CMD_DO_ACCEPT_MAG_CAL, 0, 0, 0, 0, 0, 0, 0, 0);
}

void Aircraft::cancelCompassCalibration()
{
    m_magCalStatus = MAG_CAL_NOT_STARTED;
    m_magCalPercentage = 0;

    mavlink_msg_command_long_send(MAVLINK_COMM_0, m_mavSystem, m_mavComponent, MAV_CMD_DO_CANCEL_MAG_CAL, 0, 0, 0, 0, 0, 0, 0, 0);
}
