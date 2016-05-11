#ifndef __OSDPanel_H__
#define __OSDPanel_H__

#include <FastSerial.h>
#include "arduino.h"
#include "Aircraft.h"

class OSDPanel
{
public:
    OSDPanel(OSD& osd, Aircraft& aircraft);
    ~OSDPanel();

    virtual void write() = 0;

protected:
    Aircraft& getAircraft() { return m_aircraft; }
    OSD& getOSD() { return m_osd; }

private:
    OSD&        m_osd;
    Aircraft&   m_aircraft;
};

#endif 
