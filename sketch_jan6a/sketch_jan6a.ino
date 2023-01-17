//#include <Arduino.h>
#include <WiFi.h>//include the wifi library
#include <PubSubClient.h>//a client library for mqtt messaging


const char ssid[] ="Reinhard";//ssid is the name of the wifi network
const char pass[] ="rey.kiptoo.";//password of the wifi network
//include mqtt client

#define mqtt_server "192.168.62.96" // ip address of the server
WiFiClient espClient;
PubSubClient client(espClient);
//defining the topics that are gonna be published
#define mqttdistance "esp1/distance"//define a topic called esp1/distance ... our messages will be published under a topic
#define mqttduration "esp1/duration"//define a topic called esp1/duration
//MQTT setup end
//Incoming messages
#define mqttpumpcontrol "esp1/runmotor"
//define input and output pin
int trigPin = 5;//trigger pin
int echoPin =18;//echo pin
int highPin =26;
int lowPin = 27;
int enablePin=14;
float duration,distance;

String payload_received;
bool newData = false;
void callback(const MQTT::Publish& pub) {
  payload_received= pub.payload_string();
  newData = true;
}

//this function attempts to reconnect the microcontroller to mqtt in case connection is lost,also handles initial connection to mqtt
void reconnect(){
  //loop till we are reconnected
  int counter =0;
  while(!client.connected()){
    if(counter == 5){
      ESP.restart();//reboots the SDK ... SDK stands for software development kit,it is a set of libraries that allow aour code to interact with the hardware
    }
    counter += 1;
    Serial.print("Attempting mqtt connection...");
    //attempting mqtt connection
    if(client.connect("My esp")){//if the esp gets connected to mqtt,print 'connected' ... this will be the last iteration of the loop since the esp is now connected
      Serial.println("connected");
    }else{
      Serial.print("failed, ");
     // Serial.print(client.state());
      Serial.println("Try again in 5 seconds");
      //try again after 3s
      delay(3000);
    }
  }
  

}

void setup() {
  Serial.begin(9600);
  // put your setup code here, to run once:
  pinMode(trigPin,OUTPUT);//sets the trigger pin as an output from the esp32
  pinMode(echoPin,INPUT);//the echo pin will receive the echo pulse,it inputs the pulse into the esp32
  pinMode(highPin,OUTPUT);
  pinMode(lowPin,OUTPUT);
  pinMode(enablePin,OUTPUT);
  //begin WiFi connect
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);//Our esp32 acts as a wifi station,hence WIFI_STA read more on what a wifi station is
  WiFi.disconnect();//disconnect from any previously connected wifi network
  delay(2000);
  WiFi.begin(ssid,pass);//This statement connects our esp32 to the provided wifi name(ssid) using the password provided(pass)
  while(WiFi.status() != WL_CONNECTED){//check if the esp32 is connected to wifi ... if not,delay for 0.5s and print '.'
    delay(500);
    Serial.print("**  ");
  }
  Serial.println(" ");//if the esp32 is connected print the following to the serial monitor
  Serial.println("WiFi connected");
  Serial.println("Ip address");
  Serial.println(WiFi.localIP());
  //end wifi connect
  client.set_server(mqtt_server);//set the server to the defined IP address ,check line 14
  client.set_callback(callback);
  //client.subscribe(mqttpumpcontrol);


}

void loop() {
  if(!client.connected()){//if not connected to mqtt call the reconnect function
    reconnect();//call the reconnect function.
  }
  //client.loop();
  client.subscribe(mqttpumpcontrol);
  if (newData) {
    Serial.print("Payload received: ");
    Serial.println(payload_received);
    newData = false;
  }
if(payload_received.equalsIgnoreCase("on") == 0){
  digitalWrite(highPin,HIGH);
  digitalWrite(lowPin,LOW);
  analogWrite(enablePin,100);

}else{
  // put your main code here, to run repeatedly:
  //supply a 10 microsecond pulse to the trigpin ... the trigpin is high for 10 microseconds then low for 2 microseconds,this creates a 10us pulse.
  digitalWrite(trigPin,LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin,HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin,LOW);

  //getting data from the echo pin
  duration =pulseIn(echoPin,HIGH);//the duration is the period when the echoPin stays high in one complete cycle

  // calculating the distance 
  distance = (duration/2) * 0.0343;//distance in cm ... the speed has to be converted to cm/us
  if(distance >10.0){
    //TURN ON MOTOR
    digitalWrite(highPin,HIGH);
    digitalWrite(lowPin,LOW);
    analogWrite(enablePin,100);
  }else{//turn off motor
    analogWrite(enablePin,0);
  }

  //publishing to the server
  if(distance <=400 || distance >=2){
  client.publish(mqttdistance,String(distance).c_str());//client.publish(topic,message-as-a-character-array)
  client.publish(mqttduration,String(duration).c_str());
  }
  
  //Serial.println(String(message.payload));
  

  //display the result 
  Serial.print("Distance =");
  if(distance >=400 || distance <=2){//the ultrasonic sensor is inaccurate for distances >=400 cm and those <= 2 cm hence out of range
    Serial.print("Out of range");
    Serial.println();
  }else{
    Serial.print(distance);//if distance lies within the defined range then print the following:
    Serial.print("cm");
    delay(500);
    Serial.println(payload_received);
  }
  delay(500);//wait for .5 secs before the next iteration

}
  
}