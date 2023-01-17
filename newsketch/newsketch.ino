#include <WiFi.h>
#include <MQTT.h>
extern "C" {
  #include "freertos/FreeRTOS.h"
  #include "freertos/timers.h"
}
#include <AsyncMqttClient.h>


const char ssid[] = "DESKTOP-991H0VD 4337";
const char pass[] ="6>0982fO";
//include mqtt client
#include <PubSubClient.h>
#define mqtt_server "10.5.5.20" // ip address of the server
WiFiClient espClient;
PubSubClient client(espClient);
//defining the topics that are gonna be sent
#define mqttdistance "esp1/distance"
#define mqttduration "esp1/duration"
//MQTT setup end

void setup() {
  // put your setup code here, to run once:
   Serial.begin(9600);
  // put your setup code here, to run once:
  //pinMode(trigPin,OUTPUT);
 // pinMode(echoPin,INPUT);
  //begin WiFi connect
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(2000);
  WiFi.begin(ssid,pass);
  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }
  Serial.println(" ");
  Serial.println("WiFi connected");
  Serial.println("Ip address");
  Serial.println(WiFi.localIP());
  //end wifi connect
client.set_server(mqtt_server);
client.connect("Soma tank");


}

void loop() {
  // put your main code here, to run repeatedly:
  String distance="Hello world";
  let duration = millis();
  Serial.println(message);
  Serial.println(time);
  client.publish(mqttdistance,distance.c_str());
  client.publish(mqttduration,String(duration).c_str());
  delay(500);


}
