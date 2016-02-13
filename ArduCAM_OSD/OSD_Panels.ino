void writeCharset()
{
    char start = 10;
    for (char y = start; y < 16; y++)
    {
        for (char x = 0; x < 16; x++)
        {
            osd.setPanel(x + 1, y - start + 1);
            osd.openPanel();
            osd.printf("%c", x + y * 16);
            osd.closePanel();
        }
    }
}

void writeDebugPanel()
{
    osd.setPanel(1, 1);
    osd.openPanel();
    osd.printf_P(PSTR("batt total: %i  "), Battery.GetTotalCapacity());
    osd.closePanel();

    osd.setPanel(1, 2);
    osd.openPanel();
    osd.printf_P(PSTR("batt remaining: %i   "), Battery.GetRemainingCapacity());
    osd.closePanel();

    osd.setPanel(1, 3);
    osd.openPanel();
    osd.printf_P(PSTR("got home: %i   "), DistanceAlert.hasHomePosition());
    osd.closePanel();

    osd.setPanel(1, 4);
    osd.openPanel();
    osd.printf_P(PSTR("max flight time (sec): %i   "), DistanceAlert.getMaxFlightTimeInSeconds());
    osd.closePanel();

    osd.setPanel(1, 5);
    osd.openPanel();
    osd.printf_P(PSTR("f/s capacity: %i   "), Battery.GetFailsafeCapacity());
    osd.closePanel();

    osd.setPanel(1, 6);
    osd.openPanel();
    osd.printf_P(PSTR("dischange rate: %i   "), Battery.GetDischargeRate());
    osd.closePanel();

    osd.setPanel(1, 7);
    osd.openPanel();
    osd.printf_P(PSTR("WPNAV_SPEED (m/sec): %.1f   "), ParameterManager.getParameter(ParameterManagerClass::WPNAV_SPEED));
    osd.closePanel();

    osd.setPanel(1, 8);
    osd.openPanel();
    osd.printf_P(PSTR("WPNAV_SPEED_DN (m/sec): %.1f   "), ParameterManager.getParameter(ParameterManagerClass::WPNAV_SPEED_DN));
    osd.closePanel();

    osd.setPanel(1, 9);
    osd.openPanel();
    osd.printf_P(PSTR("packet drops: %i"), packet_drops);
    osd.closePanel();

    osd.setPanel(1, 10);
    osd.openPanel();
    osd.printf_P(PSTR("parse errors: %i"), parse_error);
    osd.closePanel();
}

