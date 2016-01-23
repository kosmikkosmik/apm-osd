
//------------------ Heading and Compass ----------------------------------------

static char buf_show[12];
const char buf_Rule[36] = {0x82,0x80,0x81,0x80,0x81,0x80,
                           0x84,0x80,0x81,0x80,0x81,0x80,
                           0x83,0x80,0x81,0x80,0x81,0x80,
                           0x85,0x80,0x81,0x80,0x81,0x80};
void setHeadingPatern()
{
  int start;
  start = round((osd_heading * 24)/360);
  start -= 3;
  if(start < 0) start += 24;
  for(int x=0; x <= 10; x++){
    buf_show[x] = buf_Rule[start];
    if(++start > 23) start = 0;
  }
  buf_show[7] = '\0';
}

//------------------ Battery Remaining Picture ----------------------------------

char setBatteryPic(uint16_t bat_level)
{
  if(bat_level <= 100){
    return 0xb4;
  }
  else if(bat_level <= 300){
    return 0xb5;
  }
  else if(bat_level <= 400){
    return 0xb6;
  }
  else if(bat_level <= 500){
    return 0xb7;
  }
  else if(bat_level <= 800){
    return 0xb8;
  }
  else return 0xb9;
}

//------------------ Home Distance and Direction Calculation ----------------------------------

void setHomeVars(OSD &osd)
{
/*  float dstlon, dstlat;
  long bearing;
  
  osd_alt_to_home = (osd_alt - osd_home_alt);
  //Check arm/disarm switching.
  if (motor_armed ^ last_armed){
    //If motors armed, reset home in Arducopter version
    osd_got_home = !motor_armed;
  } 
  last_armed = motor_armed;
  if(!osd_got_home && gps_fix_type > 1){
    osd_home_lat = osd_lat;
    osd_home_lon = osd_lon;
    osd_alt_cnt = 0;
    //osd_home_alt = osd_alt;
    osd_got_home = true;
  }
  else if(osd_got_home){
    // JRChange: osd_home_alt: check for stable osd_alt (must be stable for 25*120ms = 3s)
    if(osd_alt_cnt < 25){
      if(fabs(osd_alt_prev - osd_alt) > 0.5){
        osd_alt_cnt = 0;
        osd_alt_prev = osd_alt;
      }
      else
      {
        if(++osd_alt_cnt >= 25){
          osd_home_alt = osd_alt;  // take this stable osd_alt as osd_home_alt
          haltset = 1;
        }
      }
    }
    // shrinking factor for longitude going to poles direction
    float rads = fabs(osd_home_lat) * 0.0174532925;
    double scaleLongDown = cos(rads);
    double scaleLongUp   = 1.0f/cos(rads);

    //DST to Home
    dstlat = fabs(osd_home_lat - osd_lat) * 111319.5;
    dstlon = fabs(osd_home_lon - osd_lon) * 111319.5 * scaleLongDown;
    osd_home_distance = sqrt(sq(dstlat) + sq(dstlon));

    //DIR to Home
    dstlon = (osd_home_lon - osd_lon); //OffSet_X
    dstlat = (osd_home_lat - osd_lat) * scaleLongUp; //OffSet Y
    bearing = 90 + (atan2(dstlat, -dstlon) * 57.295775); //absolut home direction
    if(bearing < 0) bearing += 360;//normalization
    bearing = bearing - 180;//absolut return direction
    if(bearing < 0) bearing += 360;//normalization
    bearing = bearing - osd_heading;//relative home direction
    if(bearing < 0) bearing += 360; //normalization
    osd_home_direction = ((int)round((float)(bearing/360.0f) * 16.0f) % 16) + 1;//array of arrows =)
    //if(osd_home_direction > 16) osd_home_direction = 1;
  } */
}

void setFdataVars(){
  //
  if (haltset == 1 && takeofftime == 0 && osd_throttle > 15){
    takeofftime = 1;
    tdistance = 0;
    FTime = (millis()/1000);
    start_battery_reading = osd_battery_remaining_A;
    last_battery_reading = osd_battery_remaining_A;
  }

  //Check if is moving (osd_groundspeed > 1Km/h or osd_climb within ]-10, 10[ m/m
  //if((osd_groundspeed > 0.28) || (osd_climb < -0.17) || (osd_climb > 0.17)){
  if(osd_throttle > 15){
    not_moving_since = millis();
    landed_at_time = 4294967295; //Airborn
  }
  //If it is stoped for more than 10 seconds, declare a landing
  else if(((millis() - not_moving_since) > 10000) && (landed_at_time == 4294967295) && (takeofftime == 1)){
    landed_at_time = millis();
  }

  if (osd_groundspeed > 1.0) tdistance += (osd_groundspeed * (millis() - dt) / 1000.0);
  mah_used += (osd_curr_A * 10.0 * (millis() - dt) / 3600000.0);
  //tdistance += (millis() - dt) / 1000.0 * osd_groundspeed;
  //mah_used += (millis() - dt) / 3600000.0 * osd_curr_A * 10.0;
  dt = millis();

  if (takeofftime == 1)
  {
    start_Time = (millis()/1000) - FTime;
    if (osd_home_distance > max_home_distance) max_home_distance = osd_home_distance;
    if (osd_airspeed > max_osd_airspeed) max_osd_airspeed = osd_airspeed;
    if (osd_groundspeed > max_osd_groundspeed) max_osd_groundspeed = osd_groundspeed;
    if (osd_alt_to_home > max_osd_home_alt) max_osd_home_alt = osd_alt_to_home;
    if (osd_windspeed > max_osd_windspeed) max_osd_windspeed = osd_windspeed;
  }
}


