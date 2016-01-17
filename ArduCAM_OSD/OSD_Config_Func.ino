/* ******************************************************************/
/* *********************** GENERAL FUNCTIONS ********************** */

//Extract functions (get bits from the positioning bytes
#define ISa(panel,whichBit) getBit(panA_REG[panel], whichBit)
#define ISb(panel,whichBit) getBit(panB_REG[panel], whichBit)
#define ISc(panel,whichBit) getBit(panC_REG[panel], whichBit)
#define ISd(panel,whichBit) getBit(panD_REG[panel], whichBit)
#define ISe(panel,whichBit) getBit(panE_REG[panel], whichBit)

boolean getBit(byte Reg, byte whichBit) {
    boolean State;
    State = Reg & (1 << whichBit);
    return State;
}

byte setBit(byte &Reg, byte whichBit, boolean stat) {
    if (stat) {
        Reg = Reg | (1 << whichBit);
    } 
    else {
        Reg = Reg & ~(1 << whichBit);
    }
    return Reg;
}

// EEPROM reader/writers
// Utilities for writing and reading from the EEPROM
byte readEEPROM(int address) {

    return EEPROM.read(address);
}

void writeEEPROM(byte value, int address) {
    EEPROM.write(address, value);
}


void InitializeOSD() {

    //loadBar();
    //delay(500);

    writeEEPROM(42, CHK1);
    writeEEPROM(VER-42,CHK2);
    for(panel = 0; panel < npanels; panel++) writeSettings();

    osd.setPanel(4,9);
    osd.openPanel();
    osd.printf_P(PSTR("OSD Initialized, reboot")); 
    osd.closePanel();

    // run for ever so user resets 
    for(;;) {}

}

// Write our latest FACTORY settings to EEPROM
void writeSettings() 
{
    uint16_t offset = OffsetBITpanel * panel;

 
    writeEEPROM(30,overspeed_ADDR);
    writeEEPROM(0,stall_ADDR);
    writeEEPROM(100,battv_ADDR); //10Volts
    writeEEPROM(6,ch_toggle_ADDR);

}
void readSettings() {
    overspeed = EEPROM.read(overspeed_ADDR);
    stall = EEPROM.read(stall_ADDR);
    battv = EEPROM.read(battv_ADDR);
    switch_mode = EEPROM.read(switch_mode_ADDR);
    ch_toggle = EEPROM.read(ch_toggle_ADDR);

    batt_warn_level = EEPROM.read(OSD_BATT_WARN_ADDR);

}


int checkPAL(int line){
    if(line >= osd.getCenter() && osd.getMode() == 0){
        line -= 3;//Cutting lines offset after center if NTSC
    }
    return line;
}

void updateSettings(byte panelu, byte panel_x, byte panel_y, byte panel_s ) 
{
    if(panel >= 1 && panel <= 32) {

        writeEEPROM(panel_s, (6 * panelu) - 6 + 0);
        if(panel_s != 0) {
            writeEEPROM(panel_x, (6 * panelu) - 6 + 2);
            writeEEPROM(panel_y, (6 * panelu) - 6 + 4);
        }
        osd.clear();
        readSettings();
    } 
}


