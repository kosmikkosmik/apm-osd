#ifndef __MainPanel_H__
#define __MainPanel_H__

#include <FastSerial.h>
#include "arduino.h"
#include "Aircraft.h"

class MainPanel
{
public:
    MainPanel(OSD& osd, const Aircraft& aircraft);

    void write();

private:
    void panAlt(int col, int row) const;
    void panVelocity(int first_col, int first_line) const;
    void panWarn(int first_col, int first_line) const;
    void panThr(int first_col, int first_line) const;
    void panRemainingTime(int first_col, int first_line) const;
    void panHomeDis(int first_col, int first_line) const;
    void panBatteryVoltage(int first_col, int first_line) const;
    void panBatteryPercent(int first_col, int first_line) const;
    void panGPSStatus(int first_col, int first_line) const;
    void panHomeDir(int first_col, int first_line) const;
    void panEKF(int first_col, int first_line) const;
    void panStatus(int first_col, int first_line);
    void panFlightMode(int first_col, int first_line) const;
    void showArrow(uint8_t rotate_arrow, uint8_t method) const;

private:
    OSD&              m_osd;
    const Aircraft&   m_aircraft;

    MAV_STATE         m_lastState;
    bool              m_lastArmed;
    unsigned long     m_lastStateTime;
};

#endif // __MainPanel_H__
