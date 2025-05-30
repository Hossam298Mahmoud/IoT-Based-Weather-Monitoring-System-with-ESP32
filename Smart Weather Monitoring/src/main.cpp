#include <Arduino.h>
#include <WiFi.h>
#include "DHT.h"
#include <PubSubClient.h> 

/********************************************************************************************/
#define LED 25
/********************************************************************************************/
// Your WiFi credentials
#define WIFI_SSID "Vodafone555"
#define WIFI_PASSWORD "Vodafone@1999"
/********************************************************************************************/
                        /* ESP32 DHT11 Code */
#define DHTPIN 2     // Digital pin connected to the DHT sensor
//#define DHTTYPE DHT11 
#define DHTTYPE DHT11   // DHT 11 
DHT dht(DHTPIN, DHTTYPE);    //Create DHT instance
float temperature=0.0 , humidity=0.0;  


unsigned long lastReadTime = 0;
const unsigned long readInterval = 1000; // seconds
/**********************************MQTT**********************************************************/
const char* mqttServer = "broker.emqx.io"; 
const char* clientID = "esp32"; 
const char* User = "esp32";
const char* Password = "Hossam1999";
                     /**************************************/
String msgStr = ""; 
WiFiClient espClient; 
PubSubClient client(espClient); 

               /*********************************************/
void mqtt_reconnect() 
{ 
  while (!client.connected()) 
  { 
    Serial.println("Connecting to MQTT...");
    if (client.connect(clientID,User,Password)) 
    {  
      Serial.println("MQTT connected"); 
      client.subscribe("Light"); 
    } 
    else { 
     delay(5000); 
        } 

  } 

} 

                /********************************************/
void callback(char *topic, byte *payload, unsigned int length) 
{
    String data = ""; 
    for (int i = 0; i < length; i++) 
    { 
      data += (char)payload[i]; 
    } 

    Serial.println(data);
    if (String(topic) == "Light") 
    { 
        if (data == "true") 
        { 
          digitalWrite(LED, HIGH); 
        } 
      else if (data == "false") 
        { 
        digitalWrite(LED, LOW); 
        } 
    } 
    Serial.println("-----------------------");
}

/****************************************Wifi_Init*******************************************/
void Wifi_Init() 
{
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();
}
/***********************************----------------------**********************************/



void setup() 
{
  Serial.begin(9600); // Initialise serial communication for Serial Monitor
  // Setup DHT11 Sensor
  dht.begin(); 
  /********************************************************************/
  pinMode(LED, OUTPUT); 
  digitalWrite(LED, LOW); 
  /***************************************************************/
  // Setup wifi Module
  Wifi_Init();
  client.setServer(mqttServer,1883);  //connect to mqtt server
  client.setCallback(callback); 
  /**************************************************************/

}

void loop() 
{
 
  /************************************DHT11***************************************************************/
  if (millis() - lastReadTime >= readInterval)    //Read every 3s
  {
    lastReadTime = millis();
    temperature =dht.readTemperature();  // Read temperature in Celsius
    humidity = dht.readHumidity();     // Read Humidity

  Serial.print(F("Humidity: "));   
  Serial.print(humidity,2);
  Serial.print(F("%  Temperature: "));  
  Serial.print( temperature,2);
  Serial.print(F("°C  /  "));
  Serial.print(dht.readTemperature(true), 2);
  Serial.println(F("°F"));
  Serial.println();

  }
  /**************************************mqtt publish*******************************************************/
  if (!client.connected()) 
  { 
    mqtt_reconnect(); 
  }
  client.loop();
  char msg[6]; 

  msgStr = String(temperature) ; 
  msgStr.toCharArray(msg, 6); 
  client.publish("Temperature", msg); 

  msgStr = String(humidity) ; 
  msgStr.toCharArray(msg, 6); 
  client.publish("Humidity" ,msg); 

  msgStr = ""; 

    /******************************************************************************************************/


#
}

