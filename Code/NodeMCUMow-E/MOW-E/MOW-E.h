/*
  MOW-E Header File
  Created on 19-June-2020
*/

#ifndef MOW_E_H
#define MOW_E_H

//ESP8266 Libraries
#include <ESP8266WiFi.h>
//#include <WiFi.h>
#include <PubSubClient.h>

//Generic Libraries
#include <SoftwareSerial.h>
#include <Keypad.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Wifi-Settings
#include"WiFi_Settings.h"

#define True 1
#define False 0

//To enable serial print for debug
#define DEBUG_PRINT 0

#define DEBUG_BAUD_RATE 115200

#if DEBUG_PRINT
#define DEBUG(X) Serial.println(X)
#else
#define DEBUG(X) 
#endif


//MQTT
#define MQTT_PORT 1883

//LCD 
#define LCD_WELCOME_TEXT "Welcome"


#define MSG_BUFFER_SIZE  (50)

//password
#define PASSWORD_LENGTH 5

//Keypad row and columns
#define ROWS  4 //four rows
#define COLS  4 //four columns


void extern enableDebugPrint(bool enableDebug);

void extern setupWiFi(void);

void extern setupMQTTServer(void);

void extern MQTTServer_SendMessage(char* message);

void extern LCD_Initialize(void);

void extern MQTT_Connect(void);

extern void Keypad_InputPassword(char key);

extern void Keypad_CheckIfPasswordCorrect(void);

extern void LCD_Menu(void);

extern void readBatteryLevel(void);

extern void printOnLCD(void);




#endif
