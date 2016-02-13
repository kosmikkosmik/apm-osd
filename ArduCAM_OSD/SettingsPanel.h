#ifndef __SettingsPanel_H__
#define __SettingsPanel_H__

#include <FastSerial.h>
#include "arduino.h"
#include "Aircraft.h"

class SettingsPanel
{
public:
    SettingsPanel(OSD& osd, const Aircraft& aircraft);

    void write();

private:
    OSD&              m_osd;
    const Aircraft&   m_aircraft;
};

#endif // __SettingsPanel_H__


