#include "SettingsPanel.h"

SettingsPanel::SettingsPanel(OSD& osd, const Aircraft& aircraft):
m_osd(osd),
m_aircraft(aircraft)
{
}

void SettingsPanel::write()
{
    m_osd.setPanel(2, 1);
    m_osd.openPanel();
    m_osd.print(F("battery"));
    m_osd.closePanel();

    m_osd.setPanel(3, 2);
    m_osd.openPanel();
    m_osd.printf("total capacity: %i", m_aircraft.getBattery().GetTotalCapacity());
    m_osd.closePanel();

    m_osd.setPanel(3, 3);
    m_osd.openPanel();
    m_osd.printf("reserve capacity: %i%%", m_aircraft.getBattery().GetReserveCapacityPercentage());
    m_osd.closePanel();

    m_osd.setPanel(3, 4);
    m_osd.openPanel();
    m_osd.printf("discharge rate: %.1f", m_aircraft.getBattery().GetDischargeRate());
    m_osd.closePanel();

    m_osd.setPanel(3, 5);
    m_osd.openPanel();
    m_osd.printf("voltage: %.2f", m_aircraft.getBattery().GetVoltage());
    m_osd.closePanel();

    m_osd.setPanel(3, 6);
    m_osd.openPanel();
    m_osd.printf("cell count: %i", m_aircraft.getBattery().GetCellCount());
    m_osd.closePanel();

    m_osd.setPanel(2, 7);
    m_osd.openPanel();
    m_osd.print_P(PSTR("sensors"));
    m_osd.closePanel();

    // sensors
    uint32_t sensors = m_aircraft.getSensorsPresence();
    uint32_t health = m_aircraft.getSensorsHealth();

    uint32_t status = sensors ^ health;

    char buffer[33];
    itoa(status, buffer, 2);

    m_osd.setPanel(3, 8);
    m_osd.openPanel();
    m_osd.printf("health: %s", buffer);
    m_osd.closePanel();

    m_osd.setPanel(3, 9);
    m_osd.openPanel();
    m_osd.printf("satellites : %i", m_aircraft.getSatellites());
    m_osd.closePanel();

    m_osd.setPanel(3, 10);
    m_osd.openPanel();
    m_osd.printf("hdop : %i", m_aircraft.getHdop());
    m_osd.closePanel();

    m_osd.setPanel(3, 11);
    m_osd.openPanel();
    m_osd.printf("vdop : %i", m_aircraft.getVdop());
    m_osd.closePanel();
}

