/* ******************************************************************/
/* *********************** GENERAL FUNCTIONS ********************** */

boolean getBit(byte Reg, byte whichBit) {
    boolean State;
    State = Reg & (1 << whichBit);
    return State;
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

    osd.setPanel(4,9);
    osd.openPanel();
    osd.printf_P(PSTR("OSD Initialized, reboot")); 
    osd.closePanel();

    // run for ever so user resets 
    for(;;) {}

}


int checkPAL(int line){
    if(line >= osd.getCenter() && osd.getMode() == 0){
        line -= 3;//Cutting lines offset after center if NTSC
    }
    return line;
}
