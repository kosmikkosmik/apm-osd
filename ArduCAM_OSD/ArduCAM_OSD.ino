/*

Copyright (c) 2011.  All rights reserved.
An Open Source Arduino based OSD and Camera Control project.

Program  : ArduCAM-OSD (Supports the variant: minimOSD)
Version  : V2.1, 24 September 2012
Author(s): Sandro Benigno
Coauthor(s):
Jani Hirvinen   (All the EEPROM routines)
Michael Oborne  (OSD Configutator)
Mike Smith      (BetterStream and Fast Serial libraries)
Gábor Zoltán
Pedro Santos
Special Contribuitor:
Andrew Tridgell by all the support on MAVLink
Doug Weibel by his great orientation since the start of this project
Contributors: James Goppert, Max Levine, Burt Green, Eddie Furey
and all other members of DIY Drones Dev team
Thanks to: Chris Anderson, Jordi Munoz


This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <http://www.gnu.org/licenses/>

*/

/* ************************************************************ */
/* **************** MAIN PROGRAM - MODULES ******************** */
/* ************************************************************ */

#undef PROGMEM 
#include "ParameterManager.h"
#include "Battery.h"
#include "DistanceAlert.h"
#define PROGMEM __attribute__(( section(".progmem.data") )) 

#undef PSTR 
#define PSTR(s) (__extension__({static prog_char __c[] PROGMEM = (s); &__c[0];})) 

#define isPAL 1

/* **********************************************/
/* ***************** INCLUDES *******************/

//#define membug 
//#define FORCEINIT  // You should never use this unless you know what you are doing 


// AVR Includes
#include <FastSerial.h>
#include <AP_Common.h>
#include <AP_Math.h>
#include <math.h>
#include <inttypes.h>
#include <avr/pgmspace.h>
// Get the common arduino functions
#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "wiring.h"
#endif
#include <EEPROM.h>
#include <SimpleTimer.h>
#include <GCS_MAVLink.h>

#ifdef membug
#include <MemoryFree.h>
#endif

// Configurations
#include "OSD_Config.h"
#include "ArduCam_Max7456.h"
#include "OSD_Vars.h"
#include "OSD_Func.h"

/* *************************************************/
/* ***************** DEFINITIONS *******************/

//OSD Hardware 
//#define ArduCAM328
#define MinimOSD

#define TELEMETRY_SPEED  57600  // How fast our MAVLink telemetry is coming to Serial port
#define BOOTTIME         2000   // Time in milliseconds that we show boot loading bar and wait user input

// Objects and Serial definitions
FastSerialPort0(Serial);
OSD osd; //OSD object 

SimpleTimer  mavlinkTimer;


/* **********************************************/
/* ***************** SETUP() *******************/

void setup() 
{
#ifdef ArduCAM328
    pinMode(10, OUTPUT); // USB ArduCam Only
#endif
    pinMode(MAX7456_SELECT,  OUTPUT); // OSD CS

    Serial.begin(TELEMETRY_SPEED);
    // setup mavlink port
    mavlink_comm_0_port = &Serial;

    // Prepare OSD for displaying 
    unplugSlaves();
    osd.init();
    ParameterManager.init();
    Battery.init(&ParameterManager);
    DistanceAlert.init(&Battery);

    // Start 
    startPanels();
    delay(500);


    // Just to easy up development things
#ifdef FORCEINIT
    InitializeOSD();
#endif


    //loadBar();
    delay(2000);
    Serial.flush();

    // Startup MAVLink timers  
    mavlinkTimer.Set(&OnMavlinkTimer, 120);

    // House cleaning, clear display and enable timers
    osd.clear();
    mavlinkTimer.Enable();

} // END of setup();


void loop() 
{
    // debug
    //read_mavlink();
    
    DistanceAlert.test();
    ParameterManager.test();

    mavlinkTimer.Run();
}

void OnMavlinkTimer()
{
    setHeadingPatern();  // generate the heading patern

    //osd_battery_pic_A = setBatteryPic(osd_battery_remaining_A);     // battery A remmaning picture
    //osd_battery_pic_B = setBatteryPic(osd_battery_remaining_B);     // battery B remmaning picture

    setHomeVars(osd);   // calculate and set Distance from home and Direction to home
    
    writePanels();       // writing enabled panels (check OSD_Panels Tab)    
    setFdataVars();
}


void unplugSlaves()
{
    //Unplug list of SPI
#ifdef ArduCAM328
    digitalWrite(10,  HIGH); // unplug USB HOST: ArduCam Only
#endif
    digitalWrite(MAX7456_SELECT,  HIGH); // unplug OSD
}
