#include "CompassCalibrationPanel.h"


CompassCalibrationPanel::CompassCalibrationPanel(OSD& osd, Aircraft& aircraft):
OSDPanel(osd, aircraft)
{
}


CompassCalibrationPanel::~CompassCalibrationPanel()
{
}

void CompassCalibrationPanel::write()
{
    int row = 1;
    getOSD().setPanel(2, row++);
    getOSD().openPanel();
    getOSD().print(F("compass calibration"));
    getOSD().closePanel();

    getOSD().setPanel(3, row++);
    getOSD().openPanel();
    switch (getAircraft().getCompassCalibrationStatus())
    {
    case MAG_CAL_FAILED:
        getOSD().print(F("failed          "));
        break;

    case MAG_CAL_NOT_STARTED:
        getOSD().print(F("not started"    ));
        break;

    case MAG_CAL_SUCCESS:
        getOSD().print(F("completed       "));
        break;

    case MAG_CAL_WAITING_TO_START:
        getOSD().print(F("ready           "));
        break;

    default:
        getOSD().printf_P(PSTR("calibrating: %i%%"), (uint16_t)getAircraft().getCompassCalibrationPercentage());
        break;
    };
    getOSD().closePanel();

}
