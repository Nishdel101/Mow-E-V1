/*
  MOW-E Cpp File
  Created on 19-June-2020
*/

#include "MOW-E.h"

char timeRTC[10];
const char* mqtt_server = "broker.mqttdashboard.com";

//Messages to UI
char msg[MSG_BUFFER_SIZE];


//Password
char masterPassword[PASSWORD_LENGTH] = {'1','2','1','8','1'};
char inputPassword[PASSWORD_LENGTH];

//Counters and flags
int z=0;
int c=0;
int interuptMove=0;

//Keypad definitions
//Use this 2D array to map the keys as you desire
char keys[ROWS][COLS] = {
{'1','2','3','3'},
{'4','5','6','B'},
{'7','8','9','C'},
{'*','0','#','D'}};

byte rowPins[ROWS] = {16, 0, 2, 14};  //connect to the row pinouts of the keypad
byte colPins[COLS] = {12, 13, 15, 10}; //connect to the column pinouts of the keypad

//for battery level check
float battValue = 0;
float battVoltage;
int   battPerc;

//initialize objects for class Keypad,Serial communication, I2C communication, and PUBSUB+ WiFi client
WiFiClient espClient;
PubSubClient client(espClient);
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);
SoftwareSerial s(3,1);
LiquidCrystal_I2C lcd(0x27, 16, 2);


void setup() 
{  
  // pinMode(D0, OUTPUT);
  enableDebugPrint(False);
      
  //Wifi setup
  setupWiFi();

  //Server Setup
  setupMQTTServer();

  MQTTServer_SendMessage("MOW-E says hi!");
 
  s.begin(9600);
  
  LCD_Initialize();

}

void loop() 
{
  if (!client.connected()) 
  {
    MQTT_Connect();
  }
  
  c=0;
  
  client.loop();
  
  char key = keypad.getKey();

  if (key)
  {
    Keypad_InputPassword(key);
  }
  else if(z==0)
  {
    readBatteryLevel();
    printOnLCD();
  }
    
  while(c==1)
  {
      //DEBUG("Mowing");   
      s.write('1');
      readBatteryLevel();
      printOnLCD();
      client.loop();
      delay(50);
      
  }   
  while(c==2)//not mowing
  { 
    
      //DEBUG("Not Mowing");
      MQTTServer_SendMessage("Stop");
      s.write('2');
      readBatteryLevel();
      printOnLCD();
      client.loop();
      delay(50);
      MQTTServer_SendMessage("Stop");
      break;
  } 
   while(c==3 && interuptMove==1)//low battery mode
  {   
      
      //DEBUG("Low Battery");
      //MQTTServer_SendMessage("Stop");
      s.write('3');
      readBatteryLevel();
      printOnLCD();
      //interuptMove=1;
      client.loop();
      //MQTTServer_SendMessage("Stop");
      delay(50);
    
  } 
}

//Setting up the wifi connection
void enableDebugPrint(bool enableDebug)
{
  if(enableDebug)
  {
    Serial.begin(DEBUG_BAUD_RATE);
  }
  else
  {
    //Do Nothing
  }  
}

void setupWiFi(void)
{
  delay(10);
  DEBUG("Connecting to ");
  DEBUG(ssid);
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
    DEBUG(".");
  }
  
  DEBUG("WiFi connected");
  DEBUG("IP address: ");
  DEBUG(WiFi.localIP());
}

  //Server Setup
void setupMQTTServer()
{
  client.setServer(mqtt_server, MQTT_PORT);
  client.setCallback(callback);
}

void MQTTServer_SendMessage(char* message)
{
 snprintf (msg, MSG_BUFFER_SIZE,message);
 client.publish("fromESP", msg);
  //DEBUG(message);
}

void LCD_Initialize()
{
  Wire.begin(4,5); 
  // initializing the LCD
  lcd.init();
  lcd.backlight(); // Enable or Turn On the backlight
  lcd.setCursor(0,0);
  lcd.print(LCD_WELCOME_TEXT);
  delay(1500);
  lcd.clear();
}

void MQTT_Connect(void)
{
  while (!client.connected()) 
  {
    DEBUG("Attempting MQTT connection...");
    
    // Create a random client ID
    String clientId = "ESP32Client-";               
    clientId += String(random(0xffff), HEX);
    
    // Attempt to connect
    if (client.connect(clientId.c_str()))     
    {       
      // Once connected, publish an announcement
      client.publish("fromESP", "Reconnected");   
      // Resubscribe  
      client.subscribe("toESP");                       
      DEBUG("connected");
    } 
    else 
    {
      DEBUG("failed, rc=");
      DEBUG(client.state());
      DEBUG(" try again");
      //Wait before trying again
      delay(500);
    }
  }
}

//storing the keys and also reseting the password entry
void Keypad_InputPassword(char key)
{
  lcd.setCursor(0,0);
  lcd.clear();
  lcd.print("Insert Password");  
  
  switch(key)
  {
    case '*':
        z=1;
        break;
        
    case 'C':
        delay(100);                
        Keypad_CheckIfPasswordCorrect();
        break;
        
      default:
        if (z>=1)
        {
          lcd.setCursor(z-1,1);
          lcd.print("*");
          inputPassword[z-1]=key;
         }
         z++;
         DEBUG(z);
         break;
     }
} 

//Password entry checker
void Keypad_CheckIfPasswordCorrect()
{
   int correct=0;
   int i;
   
   for (i=0; i < PASSWORD_LENGTH; i++)
   {
    if (inputPassword[i]==masterPassword[i]) 
    {
      correct++;
    }
   }
    
   if (correct == PASSWORD_LENGTH && (z-1) == PASSWORD_LENGTH)
   {
      lcd.clear();
      lcd.setCursor(0,1);
      lcd.print("Correct Key");
      delay(3000);
      
      z=0;
      
      LCD_Menu();
      delay(5000);
      lcd.clear();
    }
    
    else
    {
      lcd.clear();
      lcd.setCursor(0,1);
      lcd.print("Incorrect Key");
      delay(2000);
      z=0;
      lcd.clear();
      lcd.setCursor(0,1);
      lcd.print("Try Again");
      delay(2000);
      lcd.clear();    
   }
}

//on board hmi of the bot
void LCD_Menu()
{
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Press1-StartMow");
    lcd.setCursor(0,1);
    lcd.print("Press2-Cancel");
    while (c==0)
    {
      char akey = keypad.getKey();
    
      if(akey=='1'&& interuptMove==0)
      {
        c=1;
        interuptMove=1;
        MQTTServer_SendMessage("Start");
      }
      else if (akey=='2') //&& interuptMove==1
      {
        //delay(100);
        c=2;
        //interuptMove=0;
      }
    }
    lcd.clear();
}

//battery level check and print

void readBatteryLevel()
{
  battValue = analogRead(A0);
  
  //DEBUG(battValue);

  
  battPerc = map(battValue, 100, 900, 0, 100);
  
  //DEBUG("Voltage= ");
  //DEBUG(battVoltage);
  //DEBUG("Battery level= ");
  //DEBUG(battPerc);
  //DEBUG(" %");
  
  char batteryLevelString[7];         //the ASCII of the integer will be stored in this char array
  itoa(battPerc,batteryLevelString,10);
  MQTTServer_SendMessage(batteryLevelString);
    
    //snprintf (msg, MSG_BUFFER_SIZE, "%ld", battPerc.);
    //client.publish("fromESP", msg);  

  if ( battPerc <= 20 && interuptMove==1)
  {
    c=3;
  }
  else if (battPerc <=20 && interuptMove == 0)
  {
    c=2;
  }
}


//printing the battery and time continuously
void printOnLCD(void)
{ 
  lcd.setCursor(0,0);
  lcd.print("Battery   ");
  lcd.setCursor(12,0);
  lcd.print(battPerc);
  lcd.setCursor(15,0);
  lcd.print("%");
  lcd.setCursor(0,1);
  lcd.print("Time");
  lcd.setCursor(11,1);
  lcd.print(timeRTC);
}


// Handles all the topics from the UI and input being sent
void callback(char* topic, byte* payload, unsigned int length) 
{

  int i;
  char payloadTemp[50];
  DEBUG(topic);
  DEBUG(": ");
  for (i = 0; i < length; i++) 
  {

    payloadTemp[i]= (char)payload[i];
    DEBUG((char)payload[i]);
  }
  payloadTemp[i]={'\0'};
  DEBUG(payloadTemp);
  DEBUG(interuptMove);
  if ((strncmp(payloadTemp,"Start",5))==0 && interuptMove==0 )
  {
      DEBUG("start");
      MQTTServer_SendMessage("Start");  
      c=1;                                          // Starts the mowing
      interuptMove=1;
  }
  else if ((strncmp(payloadTemp,"Stop",4))==0 && interuptMove==1)
  {
    DEBUG("stop");
    c=2;                                          // Stops the mowing
    interuptMove=0;
  }
  else if ((strncmp(payloadTemp,"RTC",3))==0)
  {
   int q;
   DEBUG("time");
   for(q = 3; q < length; q++)
   {
   timeRTC[q-3] = payloadTemp[q];
   }
   timeRTC[q]={'\0'};
   DEBUG(timeRTC);
  } 
}
