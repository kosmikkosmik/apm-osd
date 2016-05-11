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

byte modeScreen = 0; //NTSC:0, PAL:1
