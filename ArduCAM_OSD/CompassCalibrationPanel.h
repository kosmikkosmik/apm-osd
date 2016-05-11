#ifndef __CompassCalibrationPanel_H__
#define __CompassCalibrationPanel_H__

#include "OSDPanel.h"

class CompassCalibrationPanel : public OSDPanel
{
public:
    CompassCalibrationPanel(OSD& osd, Aircraft& aircraft);
    ~CompassCalibrationPanel();

    virtual void write();
};

#endif 


