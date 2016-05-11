#include "GUI.h"
#include "ArduCam_Max7456.h"

const uint16_t RC_MAX = 1900;
const uint16_t RC_MIN = 1100;

// Up/down - channel 2. Up: 1100, Down: 1900
// Left/right - channel 1. Left: 1100, Right: 1900

GUI::GUI(OSD& osd, Aircraft& aircraft):
m_osd(osd),
m_aircraft(aircraft),
m_currentPanel(Panel_Settings),
m_clearPanel(false),
m_mainPanel(osd, aircraft),
m_settingsPanel(osd, aircraft),
m_ccPanel(osd, aircraft),
m_wasConnected(false),
m_wasInSync(false),
m_lastCommand(Command_None)
{
}

void GUI::processMessage(const mavlink_message_t& msg)
{
    switch (msg.msgid)
    {
    case MAVLINK_MSG_ID_MAG_CAL_PROGRESS:
    case MAVLINK_MSG_ID_MAG_CAL_REPORT:
    {
        if (m_aircraft.getCompassCalibrationStatus() != MAG_CAL_NOT_STARTED)
        {
            setCurrentPanel(Panel_CompassCalibration);
        }
    }
    break;

    case MAVLINK_MSG_ID_RC_CHANNELS:
    {
        if (!m_aircraft.armed() && ParameterManager.isInSync())
        {
            Command newCommand = getCommand(msg);
            if (newCommand != m_lastCommand)
            {
                if (newCommand != Command_None)
                {
                    performCommand(newCommand);
                }

                m_lastCommand = newCommand;
            }
        }
    }
    break;
    }
}

GUI::Command GUI::getCommand(const mavlink_message_t& msg) const
{
    uint16_t rcX = mavlink_msg_rc_channels_get_chan1_raw(&msg);
    uint16_t rcY = mavlink_msg_rc_channels_get_chan2_raw(&msg);

    if ((rcX >= RC_MAX) && (rcY >= RC_MAX))
    {
        return GUI::Command_CompassCalibration;
    }

    if ((rcX >= RC_MAX) || (rcX <= RC_MIN))
    {
        return GUI::Command_NextPanel;
    }

    return GUI::Command_None;
}

void GUI::performCommand(Command command)
{
    switch (command)
    {
    case Command_NextPanel:
        if (m_currentPanel == Panel_Main)
        {
            setCurrentPanel(Panel_Settings);
        }
        else
        {
            setCurrentPanel(Panel_Main);
        }
        break;

    case Command_CompassCalibration:
        {
            if (m_aircraft.getCompassCalibrationStatus() == MAG_CAL_NOT_STARTED)
            {
                m_aircraft.beginCompassCalibration();
            }
            else
            {
                m_aircraft.cancelCompassCalibration();
            }
        }
        break;
    }
}

void GUI::setCurrentPanel(Panel panel)
{
    if (panel != m_currentPanel)
    {
        m_currentPanel = panel;
        m_clearPanel = true;
    }
}

void GUI::refresh()
{
    if (!m_wasConnected && m_aircraft.connected())
    {
        m_wasConnected = true;
        m_clearPanel = true;
    }

    if (m_aircraft.armed() && (m_currentPanel != Panel_Main))
    {
        setCurrentPanel(Panel_Main);
    }

    if (m_clearPanel)
    {
        m_osd.clear();
        m_clearPanel = false;
    }

    if (!m_aircraft.connected())
    {
        m_osd.setPanel(3, 8);
        m_osd.openPanel();
        m_osd.printf_P(PSTR("video mode: %s"), (m_osd.getMode() == OSD::Mode_PAL) ? "pal" : "ntsc");
        m_osd.closePanel();


        m_osd.setPanel(3, 10);
        m_osd.openPanel();
        m_osd.printf_P(PSTR("waiting for heartbeat..."));
        m_osd.closePanel();
        return;
    }

    if (!ParameterManager.isInSync())
    {
        ParameterManager.showDiagnostics(m_osd);
        return;
    }
    else if (!m_wasInSync)
    {
        m_wasInSync = true;
        m_clearPanel = true;
    }

    switch (m_currentPanel)
    {
    case Panel_Main:
        m_mainPanel.write();
        break;

    case Panel_Settings:
        m_settingsPanel.write();
        break;

    case Panel_CompassCalibration:
        m_ccPanel.write();
        break;
    }
}

bool GUI::shouldEraseText()
{
    return ((millis() / 500) % 2 == 1);
}

void GUI::displayCentered(int row, int width, const char* str)
{    
    if (width > MAX_PANEL_WIDTH)
    {
        width = MAX_PANEL_WIDTH;
    }

    int len = strlen(str);
    int remainder = 0;
    if (len > MAX_PANEL_WIDTH)
    {
        remainder = len - MAX_PANEL_WIDTH;
        len = MAX_PANEL_WIDTH;
    }

    int offset = (width - len) / 2;

    memset(m_formattingBuffer, 0x20, width);
    m_formattingBuffer[width] = '\0';
    memcpy(m_formattingBuffer + offset, str, len);

    m_osd.setPanel((MAX_PANEL_WIDTH - width) / 2 + 1, row);
    m_osd.openPanel();
    m_osd.print(m_formattingBuffer);
    m_osd.closePanel();

    memset(m_formattingBuffer, 0x20, width);
    m_formattingBuffer[width] = '\0';
    if (remainder > 0)
    {
        memcpy(m_formattingBuffer, str + len, remainder);
    }

    m_osd.setPanel((MAX_PANEL_WIDTH - width) / 2 + 1, row + 1);
    m_osd.openPanel();
    m_osd.print(m_formattingBuffer);
    m_osd.closePanel(); 
}
