#include "MainPanel.h"
#include "Aircraft.h"
#include "ArduCam_Max7456.h"
#include "GUI.h"

// Layout
const int TOP_ROW = 1;
const int LEFT_COLUMN = 1;
const int RIGHT_COLUMN = 29;
const int GPS_COLUMN = 26;
const int BATTERY_COLUMN = 1;
const int TIME_COLUMN = 12;
const int ARROW_COLUMN = 14;
const int WARNING_ROW = 8;

extern GUI gui;

MainPanel::MainPanel(OSD& osd, const Aircraft& aircraft):
m_osd(osd),
m_aircraft(aircraft),
m_lastState(MAV_STATE_UNINIT),
m_lastStateTime(0),
m_lastArmed(false)
{
}

void MainPanel::write()
{
    int bottomRow = m_osd.getBottomRow();

    panBatteryVoltage(LEFT_COLUMN, TOP_ROW);
    panRemainingTime(LEFT_COLUMN + 7, TOP_ROW);
    panHomeDis(LEFT_COLUMN + 14, TOP_ROW);
    panAlt(LEFT_COLUMN + 22, TOP_ROW);

    panEKF(6, TOP_ROW + 4);
    panGPSStatus(20, TOP_ROW + 4);
    panStatus(11, TOP_ROW + 4);

    panWarn(LEFT_COLUMN, WARNING_ROW);

    panFlightMode(RIGHT_COLUMN - 5, bottomRow);

    panThr(BATTERY_COLUMN + 7, bottomRow);

    panBatteryPercent(BATTERY_COLUMN, bottomRow);

    if (DistanceAlert.hasHomePosition())
    {
        panHomeDir(ARROW_COLUMN, bottomRow);
    }

//    panVelocity(RIGHT_COLUMN - 6, bottomRow);
}

void MainPanel::panAlt(int col, int row) const
{
    m_osd.setPanel(col, row);
    m_osd.openPanel();
    if (m_aircraft.getDistanceAlert().hasHomePosition())
    {
        m_osd.printf("a:%-4.0f ", round(DistanceAlert.getAltitudeToHome()));
    }
    else
    {
        m_osd.print_P(PSTR("a:----"));
    }
    m_osd.closePanel();

    const char CLIMB_UP = 191;
    const char CLIMB_DOWN = 207;
    const char CLIMB_NO = 32;

    // climb rate up
    float climbRate = m_aircraft.getClimbRate();
    m_osd.setPanel(col + 1, row + 1);
    m_osd.openPanel();
    if (fabs(climbRate) > 0.1) // 5cm/sec
    {
        m_osd.printf_P(PSTR("%+3.1f "), climbRate);
    }
    else
    {
        m_osd.print_P(PSTR("    "));
    }
    m_osd.closePanel();
    
/*    m_osd.setPanel(col, row - 1);
    m_osd.openPanel();
    if (climbRate > 0)
    {
        m_osd.print(CLIMB_UP);
    }
    else
    {
        m_osd.print(CLIMB_NO);
    }
    m_osd.closePanel();

    m_osd.setPanel(col, row + 1);
    m_osd.openPanel();
    if (climbRate < 0)
    {
        m_osd.print(CLIMB_DOWN);
    }
    else
    {
        m_osd.print(CLIMB_NO);
    }
    m_osd.closePanel();
    */
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
    m_osd.printf_P(PSTR("th:%-3i"), m_aircraft.getThrottle());
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
    m_osd.setPanel(first_col, first_line);
    m_osd.openPanel();

//    const char HOME_CHAR = 31;
    const char HOME_CHAR = 'd';

    if (m_aircraft.getDistanceAlert().hasHomePosition())
    {
        m_osd.printf("%c:%-4.0f  ", HOME_CHAR, DistanceAlert.getDistanceToHome());
    }
    else
    {
        m_osd.printf("%c:----", HOME_CHAR);
    }
    m_osd.closePanel();

    float speed = round(m_aircraft.getGroundSpeed());
    m_osd.setPanel(first_col + 1, first_line + 1);
    m_osd.openPanel();
    if (fabs(speed) > 1) // 1m/sec
    {
        m_osd.printf_P(PSTR("%+3.0f "), speed);
    }
    else
    {
        m_osd.print_P(PSTR("   "));
    }
    m_osd.closePanel();
}


void MainPanel::panBatteryVoltage(int first_col, int first_line) const
{
    float vPerCell = m_aircraft.getBattery().GetVoltage();
    uint8_t cellCount = m_aircraft.getBattery().GetCellCount();
    if (cellCount != 0)
    {
        vPerCell /= cellCount;
    }

    m_osd.setPanel(first_col, first_line);
    m_osd.openPanel();
    m_osd.printf_P(PSTR("%.2fv"), round(vPerCell*100)/100);
    m_osd.closePanel();
}

void MainPanel::panBatteryPercent(int first_col, int first_line) const
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

    m_osd.printf_P(PSTR("%c%i%% "), batteryChar, Battery.GetBatteryPercentage());
    m_osd.closePanel();
}


void MainPanel::panGPSStatus(int first_col, int first_line) const
{
    bool gps3dlock = (m_aircraft.getFixType() >= 3);
    uint16_t hdop = m_aircraft.getHdop();
    uint16_t vdop = m_aircraft.getVdop();

    bool gpsOK = gps3dlock && (hdop < 250) && (vdop < 250);

    char* gpsStr;

    if (!gpsOK && !gui.shouldEraseText())
    {
        gpsStr = "gps";
    }
    else
    {
        gpsStr = "   ";
    }


    m_osd.setPanel(first_col, first_line);
    m_osd.openPanel();
    m_osd.print(gpsStr);
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

    if (should_warn || (should_flash && !gui.shouldEraseText()))
    {
        m_osd.print("ekf");
    }
    else
    {
        m_osd.print("   ");
    }

    m_osd.closePanel();
}

void MainPanel::panStatus(int first_col, int first_line)
{
    const char* str = "         ";
    const char* fsReason = NULL;
    bool armedChanged = false;
    bool shouldFlash = false;
    bool shouldErase = false;

    armedChanged = m_lastArmed != m_aircraft.armed();

    if ((m_lastState != m_aircraft.getState()) || armedChanged)
    {
        m_lastState = m_aircraft.getState();
        m_lastArmed = m_aircraft.armed();
        m_lastStateTime = millis();
    }
    else
    {
        shouldErase = (millis() - m_lastStateTime) > 2000;
    }



    switch (m_aircraft.getState())
    {
    case MAV_STATE_ACTIVE:
        break;

    case MAV_STATE_STANDBY:
        if (m_aircraft.armed())
        {
            str = "  armed  ";
        }
        else
        {
            str = "disarmed ";
            shouldErase = false;
        }

        break;

        /*
    case MAV_STATE_BOOT:
        str = "powering";
        break;

    case MAV_STATE_CALIBRATING:
        str = "calibrate";
        break;
        */

    case MAV_STATE_CRITICAL:
        str = "failsafe";
        shouldFlash = true;
        if (!(m_aircraft.getSensorsHealth() & MAV_SYS_STATUS_SENSOR_RC_RECEIVER))
        {
            fsReason = "radio";
        }
        break;
        /*
    case MAV_STATE_EMERGENCY:
        str = "emergency";
        shouldFlash = true;
        break;

        
    case MAV_STATE_POWEROFF:
        str = "  off  "; 
        break;
        */

    default:
        break;
    }

    m_osd.setPanel(first_col, first_line);
    m_osd.openPanel();
    if (shouldFlash && gui.shouldEraseText() || (shouldErase && !shouldFlash))
    {
        m_osd.print_P(PSTR("        "));
    }
    else
    {
        m_osd.print(str);
    }
    m_osd.closePanel();

    if (!fsReason || gui.shouldEraseText())
    {
        gui.displayCentered(first_line + 1, MAX_PANEL_WIDTH, " ");
    }
    else
    {
        gui.displayCentered(first_line + 1, MAX_PANEL_WIDTH, fsReason);
    }
}


void MainPanel::panFlightMode(int first_col, int first_line) const
{
    const char* mode_str = "";

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
        break;

    case Aircraft::CustomMode_Loiter:
        mode_str = "loit";
        break;

    case Aircraft::CustomMode_PositionHold:
        mode_str = "phld";
        break;

    case Aircraft::CustomMode_RTL:
        mode_str = "rtl ";
        break;

    case Aircraft::CustomMode_Sport:
        mode_str = "sprt";
        break;

    case Aircraft::CustomMode_Stabilize:
        mode_str = "stab";
        break;

    default:
        mode_str = "????";
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
