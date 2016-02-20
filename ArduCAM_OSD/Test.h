#ifdef DEBUG

#include <Aircraft.h>
#include <GUI.h>

class Test
{
public:
    Test(Aircraft& aircraft, GUI& gui);

    void test();
    
private:
    void sendHearbeat(bool armed, MAV_STATE state);
    void sendParameter(const char* parameterId, float value);
    void sendSystemStatus(uint8_t batteryPercentage, float batteryVoltage);
    void sendHomePosition(float lattitude, float longtitude, float altitude);
    void sendCurrentPosition(float lattitude, float longtitude, float altitude);
    void sendVfrHud(float groundSpeed, int16_t heading, int16_t throttle, float climbRate);
    void sendChannels(uint16_t channel1, uint16_t channel2);
    void sendEkfStatusReport(float ekfVariance);
    void sendStatusText(MAV_SEVERITY severity, const char* text);

    float getRadians(int deg, int min, float sec) const;
    void testMessage(const mavlink_message_t& msg);
private:
    Aircraft&   m_aircraft;
    GUI&        m_gui;

    mavlink_rc_channels_t   m_channels;
};

#endif // DEBUG

