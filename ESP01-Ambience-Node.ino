/*  This is a sketch which uses an ESP8266 ESP-01 to report temperature and humidity
 *  readings from pin 2 to both a local network PC via UDP broadcast and to a
 *  ThingSpeak channel for cloud access.
 *  Variables to configure:
 *    ssid, password, channelKey, reportInterval, sensorID1, sensorID2, localPort, ipAdd
*   Edits by Thomas Friberg
 */

// Import ESP8266 libraries
#include <ESP8266Wifi.h>
#include <WiFiUdp.h>

//DHT sensor libraries
#include <DHT.h>
#define DHTPIN 2 // Pin number - Pin 14 for ESP-12, Pin 2 for ESP-01
//#define DHTTYPE DHT11 //Comment this out if you're using a DHT22
#define DHTTYPE DHT22 // Use DHT11 sensor OR DHT22 where more accuracy required
DHT dht(DHTPIN, DHTTYPE, 15); //Initialize DHT sensor

//Sensor IDs
const char* sensorID1 = "TEM002"; //Name of sensor
const char* sensorID2 = "HUM002"; //Name of sensor
const char* devID = "Temp Sensor"; //Name of sensor

// WiFi parameters
const char* ssid = "TheSubway";
const char* password = "vanillamoon576";

//Server details
unsigned int localPort = 5007;  //UDP send port
const char* ipAdd = "192.168.0.100"; //Server address

// Use WiFiClient class to create TCP connections and UDP sends
WiFiClient client;
WiFiUDP Udp;

//Loop variables
const int loopDelay = 600000; //Delay after each loop


void setup()
{
  // Start Serial port monitoring
  Serial.begin(115200);
  // Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected.");
  // Print the IP address
  Serial.print("IP address is ");
  Serial.print(WiFi.localIP());
  Serial.print(" on port ");
  Serial.println(localPort);

  sendUdpValue("REG",sensorID1,devID);
  sendUdpValue("REG",sensorID2,devID);

  dht.begin(); //Initialize DHT
}


void loop() {
  //Thermometer and Hygrometer sensor code -------------------------
  String humidityValue = String(dht.readHumidity(),1); // Reading humidity, truncate at 1 dp
  String tempValue = String(dht.readTemperature(),1); // Read temperature as Celsius, truncate at 1dp
  sendUdpValue("LOG",sensorID1,tempValue);
  sendUdpValue("LOG",sensorID2,humidityValue);

  delay(loopDelay);  //Update temperature, wait 10 minutes (600000 seconds).
}



void sendUdpValue(String msgType, String sensorID, String value) {
  //Print GPIO state in serial
  Serial.print("Value sent via UDP is: ");
  Serial.println(msgType + "," + sensorID + "," + value);

  // send a message, to the IP address and port
  Udp.beginPacket(ipAdd,localPort);
  Udp.print(msgType);
  Udp.write(",");
  Udp.print(sensorID);
  Udp.write(",");
  Udp.print(value); //This is the value to be sent
  Udp.endPacket();
}

