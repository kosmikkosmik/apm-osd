#ifdef DEBUG

#include "Test.h"

const uint8_t SYSTEM_ID = 1;
const uint8_t COMPONENT_ID = 2;

Test::Test(Aircraft& aircraft, GUI& gui):
m_aircraft(aircraft),
m_gui(gui)
{
}

void Test::test()
{
//    sendHearbeat(false, MAV_STATE_STANDBY);

    // battery
    updateUI();

    sendSystemStatus(100, 12.4);

    updateUI();

    sendHomePosition(
        getRadians(59, 57, 11.44), // 59�57'11.44"N
        getRadians(30, 18, 51.69), // 30�18'51.69"E
        0);

    updateUI();

    sendCurrentPosition(
        getRadians(59, 57, 11.44), // 59�57'11.44"N
        getRadians(30, 18, 51.69), // 30�18'51.69"E
        200);

    updateUI();

    sendVfrHud(
        10,  // ground speed 10 m/s
        180,   // heading south (180 deg)
        70,  // 70% throttle
        2);  // climb rate 2 m/s down

    updateUI();

    sendEkfStatusReport(0.9);

    updateUI();

    // Base parameters
    sendParameter("WPNAV_SPEED", 500);
    sendParameter("WPNAV_SPEED_UP", 250);
    sendParameter("WPNAV_SPEED_DN", 150);
    sendParameter("RTL_ALT", 1500);
    sendParameter("RTL_ALT_FINAL", 0);
    sendParameter("LAND_SPEED", 50);
    sendParameter("BATT_CAPACITY", 5000);
    sendParameter("FS_BATT_MAH", 1000);
    sendParameter("FS_BATT_VOLTAGE", 13);

    updateUI();

    //sendHearbeat(true);

/*    sendCurrentPosition(
        getRadians(59, 57, 37.56), // 59�57'37.56"N
        getRadians(30, 18, 47.24), // 30�18'47.24"E
        10); */

//    sendHearbeat(true, MAV_STATE_CRITICAL);
    sendStatusText(MAV_SEVERITY_CRITICAL, "Some very very very long status text.");

    updateUI();

    sendStatusText(MAV_SEVERITY_CRITICAL, "TEST");

    updateUI();

    sendChannels(1100, 1500);

    updateUI();

    sendHearbeat(true, MAV_STATE_ACTIVE);
}

void Test::updateUI()
{
    Battery.UpdateMeasurements();
    m_gui.refresh();
    delay(500);
}

void Test::testMessage(const mavlink_message_t& msg)
{
    m_aircraft.processMessage(msg);
    m_gui.processMessage(msg);
}

void Test::sendChannels(uint16_t channel1, uint16_t channel2)
{
    m_channels.chan1_raw = channel1;
    m_channels.chan2_raw = channel2;

    mavlink_message_t buf;
    mavlink_msg_rc_channels_encode(SYSTEM_ID, COMPONENT_ID, &buf, &m_channels);
    testMessage(buf);
}

void Test::sendHearbeat(bool armed, MAV_STATE state)
{
    mavlink_heartbeat_t hearbeat;
    hearbeat.autopilot = MAV_AUTOPILOT_ARDUPILOTMEGA;
    hearbeat.base_mode = 0;
    if (armed)
    {
        hearbeat.base_mode |= MAV_MODE_FLAG_SAFETY_ARMED;
    }

    hearbeat.custom_mode = Aircraft::CustomMode_PositionHold;
    hearbeat.mavlink_version = 1;
    hearbeat.system_status = state;
    hearbeat.type = MAV_TYPE_QUADROTOR;
//    hearbeat.type = MAV_TYPE_GCS;

    mavlink_message_t buf;
    mavlink_msg_heartbeat_encode(SYSTEM_ID, COMPONENT_ID, &buf, &hearbeat);
    testMessage(buf);
}

void Test::sendParameter(const char* parameterId, float value)
{
    mavlink_param_value_t msg;
    msg.param_count = 1;
    strcpy(msg.param_id, parameterId);
    msg.param_index = -1;
    msg.param_value = value;

    mavlink_message_t buf;
    mavlink_msg_param_value_encode(SYSTEM_ID, COMPONENT_ID, &buf, &msg);
    testMessage(buf);
}

void Test::sendSystemStatus(uint8_t batteryPercentage, float batteryVoltage)
{
    mavlink_sys_status_t msg;
    msg.battery_remaining = batteryPercentage;
    msg.voltage_battery = (uint16_t)(batteryVoltage * 1000);
    msg.onboard_control_sensors_present = MAV_SYS_STATUS_SENSOR_3D_MAG | MAV_SYS_STATUS_SENSOR_3D_GYRO;
    msg.onboard_control_sensors_enabled = MAV_SYS_STATUS_SENSOR_3D_MAG | MAV_SYS_STATUS_SENSOR_3D_GYRO;
    msg.onboard_control_sensors_health = MAV_SYS_STATUS_SENSOR_3D_MAG;

    mavlink_message_t buf;
    mavlink_msg_sys_status_encode(SYSTEM_ID, COMPONENT_ID, &buf, &msg);
    testMessage(buf);
}

void Test::sendHomePosition(float lattitude, float longtitude, float altitude)
{
    mavlink_home_position_t msg;
    msg.latitude = (int32_t)(lattitude * 1e7);
    msg.longitude = (int32_t)(longtitude * 1e7);
    msg.altitude = (int32_t)(altitude * 1000);

    mavlink_message_t buf;
    mavlink_msg_home_position_encode(SYSTEM_ID, COMPONENT_ID, &buf, &msg);
    testMessage(buf);
}

void Test::sendCurrentPosition(float lattitude, float longtitude, float altitude)
{
    mavlink_gps_raw_int_t msg;
    msg.lat = (int32_t)(lattitude * 1e7);
    msg.lon = (int32_t)(longtitude * 1e7);
    msg.alt = (int32_t)(altitude * 1000);

    mavlink_message_t buf;
    mavlink_msg_gps_raw_int_encode(SYSTEM_ID, COMPONENT_ID, &buf, &msg);
    testMessage(buf);
}

void Test::sendVfrHud(float groundSpeed, int16_t heading, int16_t throttle, float climbRate)
{
    mavlink_vfr_hud_t msg;
    msg.airspeed = 0;
    msg.alt = 0;
    msg.climb = climbRate;
    msg.groundspeed = groundSpeed;
    msg.heading = heading;
    msg.throttle = throttle;

    mavlink_message_t buf;
    mavlink_msg_vfr_hud_encode(SYSTEM_ID, COMPONENT_ID, &buf, &msg);
    testMessage(buf);
}

void Test::sendEkfStatusReport(float ekfVariance)
{
    mavlink_ekf_status_report_t msg;
    msg.compass_variance = ekfVariance;
    msg.pos_horiz_variance = ekfVariance;
    msg.pos_vert_variance = ekfVariance;
    msg.terrain_alt_variance = ekfVariance;
    msg.velocity_variance = ekfVariance;

    mavlink_message_t buf;
    mavlink_msg_ekf_status_report_encode(SYSTEM_ID, COMPONENT_ID, &buf, &msg);
    testMessage(buf);
}

void Test::sendStatusText(MAV_SEVERITY severity, const char* text)
{
    mavlink_statustext_t msg;
    memcpy(msg.text, text, 50);
    msg.severity = severity;

    mavlink_message_t buf;
    mavlink_msg_statustext_encode(SYSTEM_ID, COMPONENT_ID, &buf, &msg);
    testMessage(buf);
}

float Test::getRadians(int deg, int min, float sec) const
{
    return deg + (float)(min) / 60 + sec / 3600;
}

#endif // DEBUG
