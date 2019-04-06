/*
 * Project ledPanel
 * Description:  control an LED panel controlled by MQTT
 * Author: Kirk Carlson
 * Date: 8 Aug 2018
 */

// This file contains the addressed used by OLED

#define NODE_NAME "panel1"

/**
 * if want to use IP address:
 *   byte server[] = { XXX,XXX,XXX,XXX };
 *   MQTT client(server, 1882, callback);
 * want to use domain name:
 *   MQTT client("iot.eclipse.org", 1882, callback);
 **/
byte mqttServer[] = { 192,168,4,1 };
