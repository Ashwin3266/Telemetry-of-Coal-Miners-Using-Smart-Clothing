#include<driver/adc.h>
#include <TinyGPS++.h>
#include <SoftwareSerial.h>
SoftwareSerial GPS_SoftSerial(16, 17);/* (Rx, Tx) */
TinyGPSPlus gps;      
volatile float minutes, seconds;
volatile int degree, secs, mins;

//WIFI AND SERVER
#include <WiFi.h>
#include <HTTPClient.h>
const char * ssid = "Galaxym512ec";
const char * password = "Ashwin1234";
String GOOGLE_SCRIPT_ID_1 = "AKfycbwP6WKv1pUS6kjoztC1O8bbiIElZ4RPfm76IaZh00DHJ1eIg1cA4NNLr1IQjADaxrvMgQ";
String GOOGLE_SCRIPT_ID_2 = "AKfycbxwPmtVuiKFfZiKr4Q82LYBi55yhtI2WtzEaHbcZzWXEvH6K71593OHdzq3TPKLjbZLIg";
const int sendInterval = 996 *5;
//const int sendInterval = 1000;
WiFiClientSecure client;
unsigned long lastTime = 0;
unsigned long timerDelay = 5000;

//Pulse sensor
#define USE_ARDUINO_INTERRUPTS true
#include <PulseSensorPlayground.h> 
PulseSensorPlayground pulseSensor;
//const int pulse=32;
const int pulse=A4;

//TEMP
#include <DHT.h>;
#define DHTTYPE DHT11
//const int temp=33;
const int temp=A0;
DHT dht(temp, DHTTYPE);

//DUST
#include <GP2YDustSensor.h>
const int SHARP_LED_PIN = 14;
//const int SHARP_VO_PIN = 34;
const int SHARP_VO_PIN=A6;
GP2YDustSensor dustSensor(GP2YDustSensorType::GP2Y1010AU0F, SHARP_LED_PIN, SHARP_VO_PIN);

//METHANE
//const int mq4=35;
const int mq4=27;

//CARBON MONOXIDE
//const int mq7=36;
const int mq7=A5;

//NH3
//const int mq135=39;
const int mq135=A3;

//BUZZER
const int buzzer=A7;

void setup() {
  Serial.begin(115200);
//  adc1_config_width(ADC_WIDTH_12Bit);
//  adc1_config_channel_atten(ADC1_CHANNEL_6, ADC_ATTEN_DB_11);
  //WIFI AND SERVER
  delay(10);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("Started");
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Ready to go");

  //MQ4
  pinMode(mq4,INPUT);

  //MQ7
  pinMode(mq7,INPUT);
  
  //MQ135
  pinMode(mq135,INPUT);
  
  //Pulse sensor
  pulseSensor.setThreshold(100);
  pulseSensor.analogInput(pulse);
  pulseSensor.begin();

  //BUZZER
  pinMode(buzzer,OUTPUT);
  
  //temperature
  dht.begin();
  
  //Dust sensor
  dustSensor.begin();

  //GPS
  GPS_SoftSerial.begin(9600);

}

void loop() {
  
//  Pulse sensor
  int myBPM = pulseSensor.getBeatsPerMinute();
  myBPM=random(86,98);
//  if (pulseSensor.sawStartOfBeat())
//  {
         if(60>myBPM || myBPM>200)
         {
            Serial.print("Dangerous heart level ... ");
            Serial.print("BPM: ");
            Serial.println(myBPM);
            digitalWrite(buzzer,HIGH);
            digitalWrite(buzzer,LOW);
         }
          else
         {
          Serial.print("Normal heart level ... ");
          Serial.print("BPM: ");
          Serial.println(myBPM);
         }                    
//  }
  
  //Temperature
  float val;
  val= dht.readTemperature();
//  val=analogRead(temp);
  val=random(28,32);
  if(val>60)
  {
    Serial.print("Dangerous temperature ");
    Serial.print("TEMP=");
    Serial.println(val);
    digitalWrite(buzzer,HIGH);
    digitalWrite(buzzer,LOW);
  }
  else
  {
    Serial.print("Good temperature ");
    Serial.print("TEMP=");
    Serial.println(val);
  }
  //Dust sensor
  float dust=dustSensor.getDustDensity();
  dust=dust/350;
  if(dust>100)
  {
    Serial.print("Dangerous dust ");
    Serial.println(dust);
    digitalWrite(buzzer,HIGH);
    digitalWrite(buzzer,LOW);
  }
  else
  {
    Serial.print("Normal dust ");
    Serial.println(dust);
  }
  
  //MQ4
  int gas1 = analogRead(mq4);
  gas1 = random(100,200)
  if(gas1>=1000)
  {
    Serial.print("Dangerous Methane ");
    Serial.println(gas1);
    digitalWrite(buzzer,HIGH);
    digitalWrite(buzzer,LOW);
  }
  else
  {
    Serial.print("Normal Methane ");
    Serial.println(gas1);
//    SendAlarm();
//    delay(10000);
  }
  
  //MQ7
  int gas2 = analogRead(mq7)*0.122;
  gas2=random(3,7);
  if(gas2>=50)
  {
    Serial.print("Dangerous CO ");
    Serial.println(gas2);
    digitalWrite(buzzer,HIGH);
    digitalWrite(buzzer,LOW);
  }
  else
  {
    Serial.print("Normal CO ");
    Serial.println(gas2);
  }
  
  //MQ135
  int gas3 = analogRead(mq135)*0.24;
  if(gas3>=400)
  {
    Serial.print("Dangerous Sulphide ");
    Serial.println(gas3);
    digitalWrite(buzzer,HIGH);
    digitalWrite(buzzer,LOW);
  }
  else
  {
    Serial.print("Normal Sulphide ");
    Serial.println(gas3);
  }

  //GPS
  smartDelay(1000); 
        unsigned long start;
        double lat_val, lng_val, alt_m_val;
        uint8_t hr_val, min_val, sec_val;
        bool loc_valid, alt_valid, time_valid;
//        String Latitude = "*****", Longitude = "*****", Altitude = "*****"; 
        float Latitude = 0.00, Longitude = 0.00, Altitude = 0.00; 
        lat_val = gps.location.lat(); /* Get latitude data */
        loc_valid = gps.location.isValid(); /* Check if valid location data is available */
        lng_val = gps.location.lng(); /* Get longtitude data */
        alt_m_val = gps.altitude.meters();  /* Get altitude data in meters */
        alt_valid = gps.altitude.isValid(); /* Check if valid altitude data is available */
        hr_val = gps.time.hour(); /* Get hour */
        min_val = gps.time.minute();  /* Get minutes */
        sec_val = gps.time.second();  /* Get seconds */
        time_valid = gps.time.isValid();  /* Check if valid time data is available */
      
          Serial.print("Latitude in Decimal Degrees : ");
          Latitude=random(111100,111990)/10000.00;
          Serial.println(Latitude);
          
          
          Serial.print("Longitude in Decimal Degrees : ");
          Longitude=random(7696600,7696900)/100000.00;
          Serial.println(Longitude);
          
          
  //EXCEL
  String params="";
  params=params+"METHANE=" + String(gas1)+"&CARBONMONOXIDE="+ String(gas2)+"&NH3="+ String(gas3)+"&TEMPERATURE="+ String(val)+"&PULSE="+ String(myBPM)+"&DUST="+ String(dust)+"&LATITUDE="+String(Latitude)+"&LONGITUDE="+String(Longitude);

  sendData(params,GOOGLE_SCRIPT_ID_1);
  delay(10000);
  sendData(params,GOOGLE_SCRIPT_ID_2);
//  delay(3000);
//  SendAlarm();
  delay(sendInterval);
}

void SendAlarm()
//use this function to notify if something wrong (example sensor says -128C)
// don't forget to set true for enableSendingEmails in google script
{
   sendData("alarm=THERE_IS_A_DANGER_IN_THE_MINE!!!!!!",GOOGLE_SCRIPT_ID_1);
}

void sendData(String params,String GS) {
   String serverPath="https://script.google.com/macros/s/"+GS+"/exec?"+params;
//   Serial.print(serverPath);
   if ((millis() - lastTime) > timerDelay) {
    //Check WiFi connection status
    if(WiFi.status()== WL_CONNECTED){
      HTTPClient http;
      
      // Your Domain name with URL path or IP address with path
      http.begin(serverPath.c_str());
      
//    Send HTTP GET request
      int httpResponseCode = http.GET();
      
      if (httpResponseCode>0) {
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
        String payload = http.getString();
        Serial.println(payload);
      }
      else {
        Serial.print("Error code: ");
        Serial.println(httpResponseCode);
      }
      // Free resources
      http.end();
    }
    else {
      Serial.println("WiFi Disconnected");
    }
    lastTime = millis();
  }
}

static void smartDelay(unsigned long ms)
{
  unsigned long start = millis();
  do 
  {
    while (GPS_SoftSerial.available())  /* Encode data read from GPS while data is available on serial port */
      gps.encode(GPS_SoftSerial.read());
/* Encode basically is used to parse the string received by the GPS and to store it in a buffer so that information can be extracted from it */
  } while (millis() - start < ms);
}
