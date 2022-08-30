#include <Adafruit_Fingerprint.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h> 
#include <NTPClient.h>
#include <WiFiUdp.h>

#define mySerial Serial2
#define BOTtoken "5598820891:AAHRB4uU4JXf82uVMNariBXgm1CRwaSOKRQ"
#define CHAT_ID "1409472617"

int numberAttempted = 0;
unsigned long myTime;

const char* ssid = "OnePlus 7Pro";
const char* password = "gh-#$Ay@";

Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

String formattedDate;
String dayStamp;
String timeStamp;

// Fuction to get WIFI status
String get_wifi_status(int status){
    switch(status){
        case WL_IDLE_STATUS:
        return "WL_IDLE_STATUS";
        case WL_SCAN_COMPLETED:
        return "WL_SCAN_COMPLETED";
        case WL_NO_SSID_AVAIL:
        return "WL_NO_SSID_AVAIL";
        case WL_CONNECT_FAILED:
        return "WL_CONNECT_FAILED";
        case WL_CONNECTION_LOST:
        return "WL_CONNECTION_LOST";
        case WL_CONNECTED:
        return "WL_CONNECTED";
        case WL_DISCONNECTED:
        return "WL_DISCONNECTED";
    }
}

// Function to get date and time from internet server
String get_date_time()
{
  while(!timeClient.update()) {
    timeClient.forceUpdate();
  }
  // The formattedDate comes with the following format:
  // 2018-05-28T16:00:13Z
  formattedDate = timeClient.getFormattedDate();
  return formattedDate;
}

void setup() {
    /************************* Initializing Serial Ports ************************/
      pinMode(32,OUTPUT);
      pinMode(21,OUTPUT);
      pinMode(22,OUTPUT);

      digitalWrite(32,LOW);
      digitalWrite(21,LOW);
      digitalWrite(22,LOW);
      
      //pinMode(10,INPUT_PULLUP);
      Serial.begin(9600);
    //Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);
    
    delay(1000);

    
    /*****************Connecting to WIFI*********************/
    int status = WL_IDLE_STATUS;
    Serial.println("\nConnecting");
    Serial.println(get_wifi_status(status));
    WiFi.begin(ssid, password);
    while(status != WL_CONNECTED){
        delay(500);
        status = WiFi.status();
        Serial.println(get_wifi_status(status));
    }

    Serial.println("\nConnected to the WiFi network");
    Serial.print("Local ESP32 IP: ");
    Serial.println(WiFi.localIP());
   /***************************** Initializing Telegram ****************************/ 

   client.setCACert(TELEGRAM_CERTIFICATE_ROOT);

   /************************ Initializing Time *********************************/
   timeClient.begin();
   timeClient.setTimeOffset(21600);

     /************************************ Fingerprint Sensor Initialization **********************/   
  finger.begin(57600);
  delay(5);
  // Check if fingerprint Sensor is connected and working properly
  
  if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor!");
  } else {
    Serial.println("Did not find fingerprint sensor :(");
  }

  // Check if the fingerprint sensor has any valid fingerprint enrolled
   finger.getTemplateCount();

  if (finger.templateCount == 0) {
    Serial.print("Sensor doesn't contain any fingerprint data. Please run the 'enroll' example.");
  }
  else {
    Serial.println("Waiting for valid finger...");
      Serial.print("Sensor contains "); Serial.print(finger.templateCount); Serial.println(" templates");
  } 
}

void loop() {

          int fingerID = getFingerprintIDez();
          String mystr = String(fingerID);
          if (fingerID != -1){
            if(fingerID != -2){
                numberAttempted = 0;
                Serial.print("fingerprint attempted by ID :");
                Serial.println(fingerID);
                dataSend(fingerID);
                bot.sendMessage(CHAT_ID,get_date_time());
                bot.sendMessage(CHAT_ID, "Door opened by user ID: " + mystr); 
            }
            else{
                numberAttempted = numberAttempted + 1;
                if (numberAttempted>=10){
                  numberAttempted = 0;
                  Serial.println("fingerprint attempted by unknown user");
                  dataSend(fingerID);
                  bot.sendMessage(CHAT_ID,get_date_time());
                  bot.sendMessage(CHAT_ID, "Finger print attempted by unknown person");
                }
            
            }
          }
          else{
              Serial.println("fingerprint error");
              dataSend(fingerID);
          }

}


int getFingerprintIDez() {
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK)  return -2;

  // found a match!
  //Serial.print("Found ID #"); Serial.print(finger.fingerID);
  //Serial.print(" with confidence of "); Serial.println(finger.confidence);
  
  return finger.fingerID;
}

void dataSend(int ID)
{
  switch(ID){
    case -1:
      digitalWrite(32,LOW);
      digitalWrite(21,LOW);
      digitalWrite(22,LOW);
      break;
    case -2:
      digitalWrite(32,HIGH);
      digitalWrite(21,HIGH);
      digitalWrite(22,HIGH);
      break;
      case 1:
      digitalWrite(32,HIGH);
      digitalWrite(21,LOW);
      digitalWrite(22,LOW);
      break;
      case 2:
      digitalWrite(32,LOW);
      digitalWrite(21,HIGH);
      digitalWrite(22,LOW);
      break;
      case 3:
      digitalWrite(32,HIGH);
      digitalWrite(21,HIGH);
      digitalWrite(22,LOW);
      break;
      case 4:
      digitalWrite(32,LOW);
      digitalWrite(21,LOW);
      digitalWrite(22,HIGH);
      break;
      case 5:
      digitalWrite(32,HIGH);
      digitalWrite(21,LOW);
      digitalWrite(21,HIGH);
      break;
      case 6:
      digitalWrite(32,LOW);
      digitalWrite(21,HIGH);
      digitalWrite(22,HIGH);
      break;
  }
}
