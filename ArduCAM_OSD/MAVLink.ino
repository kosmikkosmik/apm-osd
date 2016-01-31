#include "../GCS_MAVLink/include/mavlink/v1.0/mavlink_types.h"
#include "../GCS_MAVLink/include/mavlink/v1.0/ardupilotmega/mavlink.h"

// true when we have received at least 1 MAVLink packet
static bool mavlinkActive = false;
static uint8_t crlf_count = 0;

static int packet_drops = 0;
static int parse_error = 0;

void request_mavlink_rates()
{
    const int  maxStreams = 6;
    const uint8_t MAVStreams[maxStreams] = {MAV_DATA_STREAM_RAW_SENSORS,
        MAV_DATA_STREAM_EXTENDED_STATUS,
        MAV_DATA_STREAM_POSITION,
        MAV_DATA_STREAM_EXTRA1, 
        MAV_DATA_STREAM_EXTRA2, 
        MAV_DATA_STREAM_EXTRA3};

    const uint16_t MAVRates[maxStreams] = {0x02, 0x02, 0x02, 0x05, 0x02, 0x02};
    
    for (int i=0; i < maxStreams; i++) 
    {
        mavlink_msg_request_data_stream_send(MAVLINK_COMM_0,
            apm_mav_system, apm_mav_component,
            MAVStreams[i], MAVRates[i], 1);
    } 
}

void read_mavlink()
{
    mavlink_message_t msg; 
    mavlink_status_t status;
    memset(&status, 0, sizeof(mavlink_status_t));

    while (Serial.available() > 0) 
    { 
        uint8_t c = Serial.read();

        /* allow CLI to be started by hitting enter 3 times, if no
        heartbeat packets have been received */
        if (!mavlinkActive && (millis() < 20000) && (millis() > 5000)) {
            if (c == '\n' || c == '\r') {
                crlf_count++;
            } else {
                crlf_count = 0;
            }
            if (crlf_count == 3) {
                uploadFont();
            }
        }

        //trying to grab msg  
        if(mavlink_parse_char(MAVLINK_COMM_0, c, &msg, &status)) 
        {
            ParameterManager.handleMessage(&msg);
            DistanceAlert.handleMessage(&msg);

            //handle msg
            switch(msg.msgid) 
            {

            case MAVLINK_MSG_ID_HEARTBEAT:
                {
                    apm_mav_system    = msg.sysid;
                    apm_mav_component = msg.compid;
                    osd_mode = (uint8_t)mavlink_msg_heartbeat_get_custom_mode(&msg);
                    uint8_t baseMode = mavlink_msg_heartbeat_get_base_mode(&msg);
                    armed = getBit(baseMode, 7);

                    osd_mav_status = mavlink_msg_heartbeat_get_system_status(&msg);

                    if (!mavlinkActive)
                    {
                        mavlinkActive = 1;
                        osd_clear = true;
                        request_mavlink_rates();

                        // Make modules request their stuff
                        DistanceAlert.requestData(apm_mav_system, apm_mav_component);
                        ParameterManager.requestData(apm_mav_system, apm_mav_component);
                    }

                }
                break;

            case MAVLINK_MSG_ID_RC_CHANNELS:
            {
                uint16_t rc = mavlink_msg_rc_channels_get_chan9_raw(&msg);
                Panel newPanel = (rc > 1500) ? Panel_Main : Panel_Debug;
                if (newPanel != activePanel)
                {
                    activePanel = newPanel;
                    osd_clear = true;
                }
            }
            break;

            case MAVLINK_MSG_ID_GPS_RAW_INT:
                {
                    gps_fix_type = mavlink_msg_gps_raw_int_get_fix_type(&msg);
                    osd_cog = mavlink_msg_gps_raw_int_get_cog(&msg);
                }
                break;

            case MAVLINK_MSG_ID_VFR_HUD:
                {
                    osd_airspeed = mavlink_msg_vfr_hud_get_airspeed(&msg);
                    osd_groundspeed = mavlink_msg_vfr_hud_get_groundspeed(&msg);
                    osd_heading = mavlink_msg_vfr_hud_get_heading(&msg); // 0..360 deg, 0=north
                    osd_throttle = (uint8_t)mavlink_msg_vfr_hud_get_throttle(&msg);
                    osd_alt = mavlink_msg_vfr_hud_get_alt(&msg);
                    osd_climb = mavlink_msg_vfr_hud_get_climb(&msg);
                }
                break;

                
            case MAVLINK_MSG_ID_STATUSTEXT:
                {
                    memset(osd_warning, 0, WARNING_TEXT_TOTAL);
                    mavlink_msg_statustext_get_text(&msg, osd_warning);
                    warning_timestamp = millis();
                }
                break;
                
            case MAVLINK_MSG_ID_EKF_STATUS_REPORT:
                {
                  const float ekf_warn_level = 0.5;
                  const float ekf_bad_level  = 0.8;
                  
                  mavlink_ekf_status_report_t ekf_status;
                  mavlink_msg_ekf_status_report_decode(&msg, &ekf_status);
                  
                  if ((ekf_status.velocity_variance >= ekf_bad_level) ||
                      (ekf_status.pos_horiz_variance >= ekf_bad_level) ||
                      (ekf_status.pos_vert_variance >= ekf_bad_level) ||
                      (ekf_status.compass_variance >= ekf_bad_level) ||
                      (ekf_status.terrain_alt_variance >= ekf_bad_level))
                      {
                        osd_ekf_status = EKF_STATUS_BAD;
                      }
                  else if ((ekf_status.velocity_variance >= ekf_warn_level) ||
                      (ekf_status.pos_horiz_variance >= ekf_warn_level) ||
                      (ekf_status.pos_vert_variance >= ekf_warn_level) ||
                      (ekf_status.compass_variance >= ekf_warn_level) ||
                      (ekf_status.terrain_alt_variance >= ekf_warn_level))
                      {
                        osd_ekf_status = EKF_STATUS_WARN;
                      }
                  else
                  {
                    osd_ekf_status = EKF_STATUS_OK;
                  }
                }
                
                break;
                

            default:
                //Do nothing
                break;
            }
        }
        delayMicroseconds(138);
        //next one
    }
    // Update global packet drops counter
    packet_drops += status.packet_rx_drop_count;
    parse_error += status.parse_error;

}

