/*
  WiFi-Settings Header File
  Place SSID and passwords here
  Created on 19-June-2020
*/

#ifndef WIFI_SETTINGS_H
#define WIFI_SETTINGS_H

#define NISHANTH_MOBILE_HOTSPOT 0
#define NISHNATH_HOUSE_ROUTER   1

//change below line to change WiFi connections
#define WiFi_CREDENTIALS NISHANTH_MOBILE_HOTSPOT

#if (WiFi_CREDENTIALS == NISHANTH_MOBILE_HOTSPOT)

  const char* ssid = "TP-Link_2FFE";
  const char* password = "69215872";
#elif (WiFi_CREDENTIALS == NISHANTH_HOUSE_ROUTER) 
  const char* ssid = "Lenovo Z2 Plus";
  const char* password = "012345678";
#endif

#endif
