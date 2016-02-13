#include "MainPanel.h"
#include "Aircraft.h"
#include "ArduCam_Max7456.h"
#include "GUI.h"

// Layout
const int TOP_ROW = 0;
const int LEFT_COLUMN = 1;
const int BOTTOM_ROW = 12;
const int RIGHT_COLUMN = 25;
const int GPS_COLUMN = 26;
const int BATTERY_COLUMN = 1;
const int TIME_COLUMN = 12;
const int MIDDLE_ROW = 5;
const int ARROW_COLUMN = 14;
const int WARNING_ROW = 4;

extern GUI gui;

MainPanel::MainPanel(OSD& osd, const Aircraft& aircraft):
m_osd(osd),
m_aircraft(aircraft)
{
}

void MainPanel::write()
{
    panWarn(LEFT_COLUMN, WARNING_ROW);
    panFlightMode(GPS_COLUMN - 5, TOP_ROW);
    panStatus(GPS_COLUMN - 5, TOP_ROW + 1);

    panGPSStatus(GPS_COLUMN, TOP_ROW);
    panEKF(GPS_COLUMN, TOP_ROW + 1);

    //        panTime(TIME_COLUMN, TOP_ROW);
    panRemainingTime(TIME_COLUMN, TOP_ROW);

    panBatteryPercent(BATTERY_COLUMN, TOP_ROW);
    panBatteryVoltage(BATTERY_COLUMN, TOP_ROW + 1);

    panAlt(LEFT_COLUMN + 6, BOTTOM_ROW);
    panHomeDis(LEFT_COLUMN, BOTTOM_ROW);

    if (DistanceAlert.hasHomePosition())
    {
        panHomeDir(ARROW_COLUMN, TOP_ROW + 2);
    }

    panVelocity(RIGHT_COLUMN - 6, BOTTOM_ROW);
    panClimb(RIGHT_COLUMN, BOTTOM_ROW);
}

void MainPanel::panAlt(int first_col, int first_line) const
{
    if (m_aircraft.getDistanceAlert().hasHomePosition())
    {
        m_osd.setPanel(first_col, first_line);
        m_osd.openPanel();
        m_osd.printf("h %.0f ", round(DistanceAlert.getAltitudeToHome()));
        m_osd.closePanel();
    }
}


void MainPanel::panClimb(int first_col, int first_line) const
{
    m_osd.setPanel(first_col, first_line);
    m_osd.openPanel();

    float climbRate = m_aircraft.getClimbRate();

    char climbChar;
    if (climbRate > 0)
    {
        climbChar = 191;
    }
    else if (climbRate < 0)
    {
        climbChar = 207;
    }
    else
    {
        climbChar = 32;
    }

    if (climbRate < 0)
    {
        climbRate = -climbRate;
    }

    m_osd.printf("c%c%2.0f", climbChar, climbRate);
    m_osd.closePanel();
}

void MainPanel::panVelocity(int first_col, int first_line) const
{
    m_osd.setPanel(first_col, first_line);
    m_osd.openPanel();

    m_osd.printf("v %.0f ", m_aircraft.getGroundSpeed());
    m_osd.closePanel();
}

void MainPanel::panWarn(int first_col, int first_line) const
{
    //5 seconds to show a warning
    if (millis() - m_aircraft.getLastWarningTimestamp() > 5000)
    {
        gui.displayCentered(first_line, MAX_PANEL_WIDTH, " ");
    }
    else
    {
        gui.displayCentered(first_line, MAX_PANEL_WIDTH, m_aircraft.getLastWarning());
    }
}

void MainPanel::panThr(int first_col, int first_line) const
{
    m_osd.setPanel(first_col, first_line);
    m_osd.openPanel();
    m_osd.printf("%c%3.0i%c", 0x02, m_aircraft.getThrottle(), 0x25);
    m_osd.closePanel();
}

void MainPanel::panRemainingTime(int first_col, int first_line) const
{
    m_osd.setPanel(first_col, first_line);
    m_osd.openPanel();

    uint16_t maxFlightTime = m_aircraft.getDistanceAlert().getMaxFlightTimeInSeconds();
    m_osd.printf("%2i%c%02i", (maxFlightTime / 60) % 60, 0x3A, maxFlightTime % 60);

    m_osd.closePanel();
}


void MainPanel::panHomeDis(int first_col, int first_line) const
{
    if (m_aircraft.getDistanceAlert().hasHomePosition())
    {
        m_osd.setPanel(first_col, first_line);
        m_osd.openPanel();
        m_osd.printf("d %.0f ", DistanceAlert.getDistanceToHome());
        m_osd.closePanel();
    }
}


void MainPanel::panBatteryVoltage(int first_col, int first_line) const
{
    char batteryChar;

    float batteryPercentage = m_aircraft.getBattery().GetBatteryPercentage();
    if (batteryPercentage <= 10)
    {
        batteryChar = 180;
    }
    else if (batteryPercentage <= 20)
    {
        batteryChar = 181;
    }
    else if (batteryPercentage <= 30)
    {
        batteryChar = 182;
    }
    else if (batteryPercentage <= 50)
    {
        batteryChar = 183;
    }
    else
    {
        batteryChar = 184;
    }

    m_osd.setPanel(first_col, first_line);
    m_osd.openPanel();
    m_osd.printf("%c %.1f", batteryChar, Battery.GetVoltage());
    m_osd.closePanel();
}

void MainPanel::panBatteryPercent(int first_col, int first_line) const
{
    m_osd.setPanel(first_col, first_line);
    m_osd.openPanel();

    m_osd.printf_P(PSTR("%% %i "), Battery.GetBatteryPercentage());
    m_osd.closePanel();
}


void MainPanel::panGPSStatus(int first_col, int first_line) const
{
    m_osd.setPanel(first_col, first_line);
    m_osd.openPanel();

    const char* gps_str;
    if (m_aircraft.getFixType() == 3)
    {
        gps_str = " 3d";
    }
    else
    {
        if (gui.shouldEraseText())
        {
            gps_str = "   ";
        }
        else
        {
            gps_str = "gps";
        }
    }

    m_osd.printf("%s", gps_str);

    m_osd.closePanel();
}

void MainPanel::panHomeDir(int first_col, int first_line) const
{
    uint16_t dir = m_aircraft.getDirectionToHome();
    int osd_home_direction = ((int)round((float)(dir / 360.0f) * 16.0f) % 16) + 1;//array of arrows =)
    if (osd_home_direction > 16)
    {
        osd_home_direction = 1;
    }

    m_osd.setPanel(first_col, first_line);
    m_osd.openPanel();
    showArrow((uint8_t)osd_home_direction, 0);
    m_osd.closePanel();
}

void MainPanel::panEKF(int first_col, int first_line) const
{
    m_osd.setPanel(first_col, first_line);
    m_osd.openPanel();

    bool should_flash = (m_aircraft.getEkfStatus() == Aircraft::EKFStatus_BAD);
    bool should_warn = (m_aircraft.getEkfStatus() == Aircraft::EKFStatus_WARN);

    if (should_warn || (should_flash && gui.shouldEraseText()))
    {
        m_osd.print("ekf");
    }
    else
    {
        m_osd.print("   ");
    }

    m_osd.closePanel();
}

void MainPanel::panStatus(int first_col, int first_line) const
{
    const char* str = "";
    bool shouldFlash = false;

    switch (m_aircraft.getState())
    {
        /*        case MAV_STATE_ACTIVE:
        str = "active";
        break;

        case MAV_STATE_BOOT:
        str = "powering up";
        break;

        case MAV_STATE_CALIBRATING:
        str = "calibrating";
        break; */

    case MAV_STATE_CRITICAL:
        str = "fs";
        shouldFlash = true;
        break;

    case MAV_STATE_EMERGENCY:
        str = "!!";
        shouldFlash = true;
        break;

        /*        case MAV_STATE_POWEROFF:
        str = "off"; */
        break;

    case MAV_STATE_STANDBY:
        str = "  ";
        break;

        /*        case MAV_STATE_UNINIT:
        str = " ";
        break; */

    default:
        if (m_aircraft.armed())
        {
            str = "  ";
        }
        else
        {
            str = "--";
        }
        break;
    }

    m_osd.setPanel(first_col, first_line);
    m_osd.openPanel();
    m_osd.print(str);
    m_osd.closePanel();
}


void MainPanel::panFlightMode(int first_col, int first_line) const
{
    const char* mode_str = "";
    boolean should_flash = false;

    switch (m_aircraft.getCustomMode())
    {
    case Aircraft::CustomMode_Acro:
        mode_str = "acro";
        break;

    case Aircraft::CustomMode_AltitudeHold:
        mode_str = "alth";
        break;

    case Aircraft::CustomMode_Auto:
        mode_str = "auto";
        break;

    case Aircraft::CustomMode_AutoTune:
        mode_str = "atun";
        break;

    case Aircraft::CustomMode_Break:
        mode_str = "brk ";
        break;

    case Aircraft::CustomMode_Circle:
        mode_str = "circ";
        break;

    case Aircraft::CustomMode_Drift:
        mode_str = "drft";
        break;

    case Aircraft::CustomMode_Flip:
        mode_str = "flip";
        break;

    case Aircraft::CustomMode_Guided:
        mode_str = "guid";
        break;

    case Aircraft::CustomMode_Land:
        mode_str = "land";
        should_flash = true;
        break;

    case Aircraft::CustomMode_Loiter:
        mode_str = "loit";
        break;

    case Aircraft::CustomMode_PositionHold:
        mode_str = "phld";
        break;

    case Aircraft::CustomMode_RTL:
        mode_str = "rtl ";
        should_flash = true;
        break;

    case Aircraft::CustomMode_Sport:
        mode_str = "sprt";
        break;

    case Aircraft::CustomMode_Stabilize:
        mode_str = "stab";
        break;

    default:
        mode_str = "????";
        should_flash = true;
    }

    if (should_flash && (gui.shouldEraseText()))
    {
        mode_str = "    ";
    }

    m_osd.setPanel(first_col, first_line);
    m_osd.openPanel();
    m_osd.print(mode_str);
    m_osd.closePanel();
}


void MainPanel::showArrow(uint8_t rotate_arrow, uint8_t method) const
{
    int arrow_set1 = 0x90;
    //We trust that we receive rotate_arrow [1, 16] so 
    //it's no needed (rotate_arrow <= 16) in the if clause
    arrow_set1 += rotate_arrow * 2 - 2;
    m_osd.printf("%c%c", arrow_set1, arrow_set1 + 1);
}
