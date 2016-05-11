#include "SettingsPanel.h"

SettingsPanel::SettingsPanel(OSD& osd, const Aircraft& aircraft):
m_osd(osd),
m_aircraft(aircraft)
{
}

void SettingsPanel::write()
{
    int row = 1;
    m_osd.setPanel(2, row++);
    m_osd.openPanel();
    m_osd.print(F("battery"));
    m_osd.closePanel();

    uint16_t capacity = m_aircraft.getBattery().GetRemainingCapacity() + m_aircraft.getBattery().GetFailsafeCapacity();
    m_osd.setPanel(3, row++);
    m_osd.openPanel();
    m_osd.printf_P(PSTR("capacity: %i (%i)"), capacity, m_aircraft.getBattery().GetNominalCapacity());
    m_osd.closePanel();

    m_osd.setPanel(3, row++);
    m_osd.openPanel();
    m_osd.printf_P(PSTR("discharge rate: %.1f"), m_aircraft.getBattery().GetDischargeRate());
    m_osd.closePanel();

    m_osd.setPanel(3, row++);
    m_osd.openPanel();
    m_osd.printf_P(PSTR("voltage: %.2f"), m_aircraft.getBattery().GetVoltage());
    m_osd.closePanel();

    m_osd.setPanel(3, row++);
    m_osd.openPanel();
    m_osd.printf_P(PSTR("initial: %.2fv"), m_aircraft.getBattery().GetPowerOnVoltage());
    m_osd.closePanel();

    m_osd.setPanel(3, row++);
    m_osd.openPanel();
    m_osd.printf_P(PSTR("cell count: %i"), m_aircraft.getBattery().GetCellCount());
    m_osd.closePanel();

    m_osd.setPanel(3, row++);
    m_osd.openPanel();
    m_osd.printf_P(PSTR("estimated (apm): %3.i%%"), m_aircraft.getBattery().GetApmBatteryPercentage());
    m_osd.closePanel();


    m_osd.setPanel(3, row++);
    m_osd.openPanel();
    m_osd.printf_P(PSTR("estimated: %3.i%%"), m_aircraft.getBattery().GetBatteryPercentage());
    m_osd.closePanel();

    m_osd.setPanel(3, row++);
    m_osd.openPanel();
    float vCell = m_aircraft.getBattery().GetVoltage() / m_aircraft.getBattery().GetCellCount();    
    m_osd.printf_P(PSTR("estimated (v): %3.i%%"), m_aircraft.getBattery().GetBatteryPercentage(vCell));
    m_osd.closePanel();

    m_osd.setPanel(2, row++);
    m_osd.openPanel();
    m_osd.print_P(PSTR("sensors"));
    m_osd.closePanel();

    // sensors
    uint32_t sensors = m_aircraft.getSensorsPresence();
    uint32_t health = m_aircraft.getSensorsHealth();

    uint32_t status = sensors ^ health;

    char buffer[33];
    itoa(status, buffer, 2);

    m_osd.setPanel(3, row++);
    m_osd.openPanel();
    m_osd.printf_P(PSTR("health: %s"), buffer);
    m_osd.closePanel();

    m_osd.setPanel(3, row++);
    m_osd.openPanel();
    m_osd.printf_P(PSTR("satellites : %i"), m_aircraft.getSatellites());
    m_osd.closePanel();

    m_osd.setPanel(3, row++);
    m_osd.openPanel();
    m_osd.printf_P(PSTR("hdop/vdop: %4.i/%4.i"), m_aircraft.getHdop(), m_aircraft.getVdop());
    m_osd.closePanel();
}

