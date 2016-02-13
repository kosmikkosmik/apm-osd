enum Panel
{
    Panel_Main,
    Panel_Debug
};

static Panel        activePanel = Panel_Main;

static bool         osd_clear = 0;

static boolean      takeofftime = 0;
static boolean      haltset = 0;

static uint8_t      spe = 0;
static uint8_t      high = 0;

static float        start_Time = -1.0;
static unsigned long landed_at_time = 4294967295;   // Time landing was declared (4294967295 means airborn)
static unsigned long FTime = 0;

static unsigned long not_moving_since = 0;          // get time since uav not moving

byte modeScreen = 0; //NTSC:0, PAL:1
