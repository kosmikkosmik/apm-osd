void startPanels()
{
  do_converts(); // load the unit conversion preferences
}

boolean shouldEraseFlashText()
{  
  return ((millis() / 500) % 2 == 1);
}


void displayCentered(int row, int width, const char* str)
{
    if (width > MAX_PANEL_WIDTH)
    {
        width = MAX_PANEL_WIDTH;
    }

    memset(osd_formatting_buffer, 0x20, width);
    osd_formatting_buffer[width] = '\0';

    int len = strlen(str);
    if (len > MAX_PANEL_WIDTH)
    {
        len = MAX_PANEL_WIDTH;
    }

    int offset = (width - len) / 2;

    memcpy(osd_formatting_buffer + offset, str, len);
//    osd_formatting_buffer[offset + len] = '\0';  

    osd.setPanel((MAX_PANEL_WIDTH - width) / 2 + 1, row);
    osd.openPanel();
    osd.printf("%s", osd_formatting_buffer);
    osd.closePanel();
}


/******* PANELS - POSITION *******/

const int TOP_ROW = 0;
const int LEFT_COLUMN = 1;
const int BOTTOM_ROW = 12;
const int RIGHT_COLUMN = 25;
const int GPS_COLUMN = 26;
const int BATTERY_COLUMN = 1;
const int TIME_COLUMN = 12;
const int MIDDLE_ROW = 5;
const int ARROW_COLUMN = 14;
const int WARNING_ROW = 4;


void writePanels()
{ 
    //Check for panel toggle
    if (osd_clear == 1)
    {
        osd.clear();
        osd_clear = 0;
    }

    if (!mavlinkActive)
    {
        osd.setPanel(3,10);
        osd.openPanel();
        osd.printf_P(PSTR("waiting for heartbeat...")); 
        osd.closePanel();

        return;
    }

    if (!ParameterManager.isInSync())
    {
        ParameterManager.showDiagnostics(osd);
        return;
    }

    if (activePanel == Panel_Main)
    {
        panWarn(LEFT_COLUMN, WARNING_ROW);
        panFlightMode(GPS_COLUMN - 5, TOP_ROW);
        panStatus(GPS_COLUMN - 5, TOP_ROW + 1);

        panGPSStatus(GPS_COLUMN, TOP_ROW);
        panEKF(GPS_COLUMN, TOP_ROW + 1);

        panTime(TIME_COLUMN, TOP_ROW);

        panBatteryPercent(BATTERY_COLUMN, TOP_ROW);
        panBatteryVoltage(BATTERY_COLUMN, TOP_ROW + 1);

        panAlt(LEFT_COLUMN + 6, BOTTOM_ROW);
        panHomeDis(LEFT_COLUMN, BOTTOM_ROW);

        if (DistanceAlert.hasHomePosition())
        {
            panHomeDir(ARROW_COLUMN, TOP_ROW + 2);
        }

        panVelocity(RIGHT_COLUMN - 6, BOTTOM_ROW);
        panClimb(RIGHT_COLUMN, BOTTOM_ROW);            
    }
    else if (activePanel == Panel_Debug)
    {
//        ParameterManager.showDiagnostics(osd);
        writeDebugPanel();
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
    osd.printf_P(PSTR("max flight time (sec): %i   "), DistanceAlert.getMaxFlightTime());
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

/* **************************************************************** */
// Panel  : panAlt
// Needs  : X, Y locations
// Output : Alt symbol and altitude value in meters from MAVLink
// Size   : 1 x 7Hea  (rows x chars)
// Staus  : done

void panAlt(int first_col, int first_line){
    osd.setPanel(first_col, first_line);
    osd.openPanel();
    osd.printf("h %.0f ", round(DistanceAlert.getAltitudeToHome()));
    osd.closePanel();
}


void panClimb(int first_col, int first_line)
{
    osd.setPanel(first_col, first_line);
    osd.openPanel();
    osd.printf("c %2.0f", (osd_climb * converth * 60) * 0.1);
//    osd.printf("%c%.0f", climbchar, vs);
    osd.closePanel();
}


void panHomeAlt(int first_col, int first_line){
    osd.setPanel(first_col, first_line);
    osd.openPanel();
    osd.printf("%c%5.0f",0x12, DistanceAlert.getAltitudeToHome());
    osd.closePanel();
}

void panVelocity(int first_col, int first_line){
    osd.setPanel(first_col, first_line);
    osd.openPanel();
    
    osd.printf("v %.0f ", (double)(osd_groundspeed * converts));
    osd.closePanel();
}

void panWarn(int first_col, int first_line)
{
    //5 seconds to show a warning
    if (millis() - warning_timestamp > 5000)
    {
        displayCentered(first_line, MAX_PANEL_WIDTH, " ");
    }
    else
    {
        displayCentered(first_line, MAX_PANEL_WIDTH, osd_warning);
    }
}

void panThr(int first_col, int first_line){
    osd.setPanel(first_col, first_line);
    osd.openPanel();
    osd.printf("%c%3.0i%c",0x02,osd_throttle,0x25);
    osd.closePanel();
}


void panTime(int first_col, int first_line)
{
    osd.setPanel(first_col, first_line);
    osd.openPanel();
    
    if (start_Time >= 0)
    {
        osd.printf("%2i%c%02i", ((int)start_Time / 60) % 60, 0x3A, (int)start_Time % 60);
    }
    else
    {
        osd.printf_P(PSTR("00:00"));
    }

    osd.closePanel();
}

void panHomeDis(int first_col, int first_line){
    osd.setPanel(first_col, first_line);
    osd.openPanel();
    osd.printf("d %.0f ", DistanceAlert.getDistanceToHome());
    osd.closePanel();
}


void panBatteryVoltage(int first_col, int first_line)
{
    osd.setPanel(first_col, first_line);
    osd.openPanel();
//    osd.printf("%c%5.1f", 0xBC, (double)osd_vbat_A);
    osd.printf("%c %.1f", 0x17, Battery.GetVoltage());
    osd.closePanel();
}

void panBatteryPercent(int first_col, int first_line)
{
    osd.setPanel(first_col, first_line);
    osd.openPanel();

    osd.printf_P(PSTR("%% %i "), Battery.GetBatteryPercentage());
    osd.closePanel();
}


void panGPSStatus(int first_col, int first_line){
    osd.setPanel(first_col, first_line);
    osd.openPanel();
    
    const char* gps_str;
    if (gps_fix_type == 3)
    {
      gps_str = " 3d";
    }
    else
    {
        if (shouldEraseFlashText())
      {
        gps_str = "   ";
      }
      else
      {
        gps_str = "gps";
      }
    }
    
    osd.printf("%s",gps_str);

    osd.closePanel();
}

/* **************************************************************** */
// Panel  : panHomeDir
// Needs  : X, Y locations
// Output : 2 symbols that are combined as one arrow, shows direction to home
// Size   : 1 x 2  (rows x chars)
// Status : not tested

void panHomeDir(int first_col, int first_line)
{
    osd.setPanel(first_col, first_line);
    osd.openPanel();
    showArrow((uint8_t)osd_home_direction,0);
    osd.closePanel();
}

void panEKF(int first_col, int first_line)
{
    osd.setPanel(first_col, first_line);
    osd.openPanel();
  
    bool should_flash = (osd_ekf_status == EKF_STATUS_BAD);
    bool should_warn = (osd_ekf_status == EKF_STATUS_WARN);

    if (should_warn || (should_flash && shouldEraseFlashText()))
    {
      osd.print("ekf");
    }
    else
    {
      osd.print("   ");
    }
  
    osd.closePanel();
}

void panStatus(int first_col, int first_line)
{
    const char* str = "";
    bool shouldFlash = false;

    switch (osd_mav_status)
    {
/*        case MAV_STATE_ACTIVE:
            str = "active";
            break;

        case MAV_STATE_BOOT:
            str = "powering up";
            break;

        case MAV_STATE_CALIBRATING:
            str = "calibrating";
            break; */

        case MAV_STATE_CRITICAL:
            str = "fs";
            shouldFlash = true;
            break; 

        case MAV_STATE_EMERGENCY:
            str = "!!";
            shouldFlash = true;
            break;

/*        case MAV_STATE_POWEROFF:
            str = "off"; */
            break;

        case MAV_STATE_STANDBY:
            str = "  ";
            break;

/*        case MAV_STATE_UNINIT:
            str = " ";
            break; */

        default:
            str = "  ";
            break;
    }

    osd.setPanel(first_col, first_line);
    osd.openPanel();
    osd.print(str);
    osd.closePanel();
}


void panFlightMode(int first_col, int first_line)
{
    const char* mode_str = "";
    boolean should_flash = false;
    
    if (osd_mode == 0) 
    {
      mode_str = "stab";
    }
    else if (osd_mode == 1)
    {
      mode_str = "acro";
    }
    else if (osd_mode == 2)
    {
      mode_str = "alth";
    }
    else if (osd_mode == 3)
    {
      mode_str = "auto";
    }
    else if (osd_mode == 4)
    {
      mode_str = "guid";
    }
    else if (osd_mode == 5)
    {
      mode_str = "loit";
    }
    else if (osd_mode == 6)
    {
      mode_str = "rtl ";
      should_flash = true;      
    }
    else if (osd_mode == 7)
    {
      mode_str = "circ";
    }
    else if (osd_mode == 8)
    {
      mode_str = "????";
      should_flash = true;
    }
    else if (osd_mode == 9)
    {
      mode_str = "land";
      should_flash = true;      
    }
    else if (osd_mode == 11)
    {
      mode_str = "drft";
    }
    else if (osd_mode == 13)
    {
      mode_str = "sprt";
    }
    else if (osd_mode == 14)
    {
      mode_str = "flip";
    }
    else if (osd_mode == 15)
    {
      mode_str = "tune";
    }
    else if (osd_mode == 16)
    {
      mode_str = "phld";
    }
    else if (osd_mode == 17)
    {
      mode_str = "brk ";
    }
    else 
    {
      mode_str = "????";
      should_flash = true;      
    }
        
    if (should_flash && (shouldEraseFlashText()))
    {
      mode_str = "    ";
    }
    
    osd.setPanel(first_col, first_line);
    osd.openPanel();
    osd.print(mode_str);
    osd.closePanel();
}


// ---------------- EXTRA FUNCTIONS ----------------------
// Show those fancy 2 char arrows
void showArrow(uint8_t rotate_arrow,uint8_t method) {  
    int arrow_set1 = 0x90;
    //We trust that we receive rotate_arrow [1, 16] so 
    //it's no needed (rotate_arrow <= 16) in the if clause
    arrow_set1 += rotate_arrow * 2 - 2;
    //arrow_set2 = arrow_set1 + 1;
//    if(method == 1) osd.printf("%c%3.0f%c|%c%c%2.0f%c",0x1D,(double)(osd_windspeed * converts),spe, (byte)arrow_set1, (byte)(arrow_set1 + 1),(double)(osd_windspeedz * converts),spe);
//    if(method == 1) osd.printf("%c%3.0f%c|%c%c%2.0f%c",0x1d,(double)(osd_windspeed * converts),spe, arrow_set1, arrow_set1 + 1,(double)(nor_osd_windspeed * converts),spe);
    /*else*/ if(method == 2) osd.printf("%c%c%4i%c", arrow_set1, arrow_set1 + 1, off_course, 0x05);   
    else osd.printf("%c%c", arrow_set1, arrow_set1 + 1);
}

// Calculate and shows Artificial Horizon
// For using this, you must load a special mcm file with the new staggered artificial horizon chars!
// e.g. AH_BetterResolutionCharset002.mcm
							// with different factors we can adapt do different cam optics
#define AH_PITCH_FACTOR		0.010471976		// conversion factor for pitch
#define AH_ROLL_FACTOR		0.017453293		// conversion factor for roll
#define AH_COLS			12			// number of artificial horizon columns
#define AH_ROWS			5			// number of artificial horizon rows
#define CHAR_COLS		12			// number of MAX7456 char columns
#define CHAR_ROWS		18			// number of MAX7456 char rows
#define CHAR_SPECIAL		9			// number of MAX7456 special chars for the artificial horizon
#define AH_TOTAL_LINES		AH_ROWS * CHAR_ROWS	// helper define


#define LINE_SET_STRAIGHT__	(0xC7 - 1)		// code of the first MAX7456 straight char -1
#define LINE_SET_STRAIGHT_O	(0xD0 - 3)		// code of the first MAX7456 straight overflow char -3
#define LINE_SET_P___STAG_1	(0xD1 - 1)		// code of the first MAX7456 positive staggered set 1 char -1
#define LINE_SET_P___STAG_2	(0xDA - 1)		// code of the first MAX7456 positive staggered set 2 char -1
#define LINE_SET_N___STAG_1	(0xE3 - 1)		// code of the first MAX7456 negative staggered set 1 char -1
#define LINE_SET_N___STAG_2	(0xEC - 1)		// code of the first MAX7456 negative staggered set 2 char -1
#define LINE_SET_P_O_STAG_1	(0xF5 - 2)		// code of the first MAX7456 positive overflow staggered set 1 char -2
#define LINE_SET_P_O_STAG_2	(0xF9 - 1)		// code of the first MAX7456 positive overflow staggered set 2 char -1
#define LINE_SET_N_O_STAG_1	(0xF7 - 2)		// code of the first MAX7456 negative overflow staggered set 1 char -2
#define LINE_SET_N_O_STAG_2	(0xFC - 1)		// code of the first MAX7456 negative overflow staggered set 2 char -1


#define OVERFLOW_CHAR_OFFSET	6			// offset for the overflow subvals


#define ANGLE_1			9			// angle above we switch to line set 1
#define ANGLE_2			25			// angle above we switch to line set 2


void do_converts()
{
    if (EEPROM.read(measure_ADDR) == 0) {
        converts = 3.6;
        converth = 1.0;
        spe = 0x10;
        high = 0x0C;
        distchar = 0x1B;
        distconv = 1000;
        climbchar = 0x1A;
    } else {
        converts = 2.23;
        converth = 3.28;
        spe = 0x19;
        high = 0x66;
        distchar = 0x1C;
        distconv = 5280;
        climbchar = 0x1E;
    }
}

