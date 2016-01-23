/*Panels variables*/
//Will come from APM telem port

static float         max_home_distance = 0;
static float        max_osd_airspeed = 0;
static float        max_osd_groundspeed = 0; 
static float        max_osd_home_alt = 0;
static float        max_osd_windspeed = 0;
static float        nor_osd_windspeed = 0;
static float        vs = 0;

static unsigned long dt = 0;
static float tdistance = 0;


static int16_t	    wp_target_bearing = 0; // Bearing to current MISSION/target in degrees
static uint16_t     wp_dist = 0; // Distance to active MISSION in meters
static uint8_t      wp_number = 0; // Current waypoint number

static uint8_t      base_mode=0;
static bool         motor_armed = 0;
static bool         osd_clear = 0;

static uint8_t      ch_toggle = 0;
static boolean      switch_mode = 0;
static boolean      takeofftime = 0;
static boolean      haltset = 0;

static float        converts = 0;
static float        converth = 0;
static uint8_t      overspeed = 0;
static uint8_t      stall = 0;
static uint8_t      battv = 0; //Battery warning voltage - units Volt *10 
static uint16_t      distconv = 0;

static uint8_t      spe = 0;
static uint8_t      high = 0;
static uint8_t      temps = 0;
static float        osd_vbat_A = 0;                 // Battery A voltage in milivolt
static int16_t      osd_curr_A = 0;                 // Battery A current
static uint8_t      osd_battery_remaining_A = 0;    // 0 to 100 <=> 0 to 1000
static int8_t       start_battery_reading = 0;    // 0 to 100 <=> 0 to 1000
static int8_t       last_battery_reading = 0;    // 0 to 100 <=> 0 to 1000
static uint8_t      batt_warn_level = 0;
static float     mah_used = 0;         // Battery Consumed mAh

static float        start_Time = -1.0;
static unsigned long landed_at_time = 4294967295;   // Time landing was declared (4294967295 means airborn)
static uint8_t      osd_mode = 0;                   // Navigation mode from RC AC2 = CH5, APM = CH8
static uint8_t      osd_nav_mode = 0;               // Navigation mode from RC AC2 = CH5, APM = CH8
static unsigned long warning_timestamp = 0;
static unsigned long FTime = 0;

static float        osd_climb = 0;

static float        osd_lat = 0;                    // latidude
static float        osd_lon = 0;                    // longitude
static unsigned long not_moving_since = 0;          // get time since uav not moving
static uint8_t      gps_fix_type = 0;               // GPS lock 0-1=no fix, 2=2D, 3=3D
static uint16_t     osd_cog;                        // Course over ground
static int16_t      off_course;

static bool         osd_got_home = false;               // tels if got home position or not
static float        osd_alt_to_home = 0; 
static long         osd_home_distance = 0;          // distance from home
static uint8_t      osd_home_direction;             // Arrow direction pointing to home (1-16 to CW loop)

static float        osd_heading = 0;                // ground course heading from GPS

static float        osd_alt = 0;                    // altitude
static int32_t      osd_gps_alt = 0;                    // altitude
static float        osd_airspeed = -1;              // airspeed
static float        osd_windspeed = 0;

static float        osd_groundspeed = 0;            // ground speed
static uint8_t     osd_throttle = 0;               // throtle
static uint8_t      tempconv = 1;
static uint16_t     tempconvAdd = 0;
static uint16_t     distchar = 0;
static uint16_t     climbchar = 0;

static uint8_t      apm_mav_system; 
static uint8_t      apm_mav_component;

static const uint8_t npanels = 2;
static uint8_t panel = 0; 

byte modeScreen = 0; //NTSC:0, PAL:1


// warnings
#define WARNING_TEXT_TOTAL   50
static char osd_warning[WARNING_TEXT_TOTAL+1];

#define MAX_PANEL_WIDTH 28
static char osd_formatting_buffer[MAX_PANEL_WIDTH + 1];

// EKF flags
const byte EKF_STATUS_OK    = 0;
const byte EKF_STATUS_WARN  = 1;
const byte EKF_STATUS_BAD   = 2;

static byte osd_ekf_status = EKF_STATUS_BAD;

// System status
static uint8_t osd_mav_status = MAV_STATE_UNINIT;



