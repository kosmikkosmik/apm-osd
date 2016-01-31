static unsigned long dt = 0;
static float tdistance = 0;

enum Panel
{
    Panel_Main,
    Panel_Debug
};

static Panel        activePanel = Panel_Main;

static bool         armed = false;
static bool         osd_clear = 0;

static uint8_t      ch_toggle = 0;
static boolean      takeofftime = 0;
static boolean      haltset = 0;

static float        converts = 0;
static float        converth = 0;
static uint16_t      distconv = 0;

static uint8_t      spe = 0;
static uint8_t      high = 0;

static float        start_Time = -1.0;
static unsigned long landed_at_time = 4294967295;   // Time landing was declared (4294967295 means airborn)
static uint8_t      osd_mode = 0;                   // Navigation mode from RC AC2 = CH5, APM = CH8
static unsigned long warning_timestamp = 0;
static unsigned long FTime = 0;

static float        osd_climb = 0;

static unsigned long not_moving_since = 0;          // get time since uav not moving
static uint8_t      gps_fix_type = 0;               // GPS lock 0-1=no fix, 2=2D, 3=3D
static uint16_t     osd_cog;                        // Course over ground
static int16_t      off_course;

static uint8_t      osd_home_direction;             // Arrow direction pointing to home (1-16 to CW loop)

static float        osd_heading = 0;                // ground course heading from GPS

static float        osd_alt = 0;                    // altitude
static float        osd_airspeed = -1;              // airspeed
static float        osd_windspeed = 0;

static float        osd_groundspeed = 0;            // ground speed
static uint8_t      osd_throttle = 0;               // throtle
static uint16_t     distchar = 0;
static uint16_t     climbchar = 0;

static uint8_t      apm_mav_system; 
static uint8_t      apm_mav_component;

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



