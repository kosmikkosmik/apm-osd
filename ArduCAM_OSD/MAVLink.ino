#include "../GCS_MAVLink/include/mavlink/v1.0/mavlink_types.h"
#include "../GCS_MAVLink/include/mavlink/v1.0/ardupilotmega/mavlink.h"

static uint8_t crlf_count = 0;

static int packet_drops = 0;
static int parse_error = 0;

void read_mavlink()
{
    mavlink_message_t msg; 
    mavlink_status_t status;
    memset(&status, 0, sizeof(mavlink_status_t));

    while (Serial.available() > 0) 
    { 
        uint8_t c = Serial.read();

        /* allow CLI to be started by hitting enter 3 times, if no
        heartbeat packets have been received */
        if (!aircraft.connected() && (millis() < 20000) && (millis() > 5000)) {
            if (c == '\n' || c == '\r') {
                crlf_count++;
            } else {
                crlf_count = 0;
            }
            if (crlf_count == 3) {
                uploadFont();
            }
        }

        //trying to grab msg  
        if(mavlink_parse_char(MAVLINK_COMM_0, c, &msg, &status)) 
        {
            aircraft.processMessage(msg);
            gui.processMessage(msg);
        }

        delayMicroseconds(138);
    }

    // Update global packet drops counter
    packet_drops += status.packet_rx_drop_count;
    parse_error += status.parse_error;

}

