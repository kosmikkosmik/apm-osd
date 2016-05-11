#ifndef __GUI_H__
#define __GUI_H__

#include <FastSerial.h>
#include "GCS_MAVLink.h"
#include "ArduCam_Max7456.h"
#include "MainPanel.h"
#include "SettingsPanel.h"
#include "CompassCalibrationPanel.h"

const int MAX_PANEL_WIDTH = 28;

class GUI
{
public:
    enum Panel
    {
        Panel_Main,
        Panel_Settings,
        Panel_CompassCalibration
    };

    enum Command
    {
        Command_None,
        Command_NextPanel,
        Command_CompassCalibration
    };

    GUI(OSD& osd, Aircraft& aircraft);

    void processMessage(const mavlink_message_t& msg);
    void refresh();

    bool shouldEraseText();
    void displayCentered(int row, int width, const char* str);
    OSD& getOSD() const { return m_osd; }

private:
    Command getCommand(const mavlink_message_t& msg) const;
    void performCommand(Command command);
    void setCurrentPanel(Panel panel);

private:
    OSD&        m_osd;
    Aircraft&   m_aircraft;
    char    m_formattingBuffer[MAX_PANEL_WIDTH + 1];
    Panel   m_currentPanel;
    bool    m_clearPanel;
    bool    m_wasConnected;
    bool    m_wasInSync;
    Command m_lastCommand;

    MainPanel       m_mainPanel;
    SettingsPanel   m_settingsPanel;
    CompassCalibrationPanel m_ccPanel;
};

#endif // __GUI_H__
