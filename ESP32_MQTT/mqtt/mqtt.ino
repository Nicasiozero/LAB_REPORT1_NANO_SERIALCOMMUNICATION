#include <WiFi.h>
#include <PubSubClient.h>

const char* ssid = "Frank";       
const char* password = "12345678";

const char* mqtt_server = "192.168.43.114"; 
const int mqtt_port = 1883;        
// const char* mqtt_username = "frank";
// const char* mqtt_password = "frank"; 

const char* topic = "data";  
const char* btnTopic = "esp32/btn"; 

WiFiClient espClient;
PubSubClient client(espClient);

char status;
int ledLV = 0;
String lvAsString;

void setup() {
  Serial.begin(9600);
  Serial2.begin(9600);


  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");


  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);

  while (!client.connected()) {
    if (client.connect("ESP32Client")) {
      Serial.println("Connected to MQTT Broker");
      client.subscribe(topic);
    } else {
      Serial.println("Failed to connect to MQTT Broker");
      delay(5000);
    }
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  for (int i = 0; i < length; i++) {
    Serial2.print((char)payload[i]);
  }
  Serial2.println();
}

void serialEvent2() {
  String buffer = Serial2.readStringUntil('\n');
  client.publish(btnTopic, buffer.c_str());
  //Serial.println("resive : " + buffer);
}
void loop() {
 if (!client.connected()) {
   reconnect();
 }
 client.loop(); 
 
}

void reconnect() {
  while (!client.connected()) {
    if (client.connect("ESP32Client")) {
      Serial.println("Connected to MQTT Broker");
      client.subscribe(topic);
    } else {
      Serial.println("Failed to connect to MQTT Broker");
      delay(5000);
    }
  }
}