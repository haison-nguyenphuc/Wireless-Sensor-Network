#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>

// Update these with values suitable for your network.
const char* mqtt_server = "192.168.1.21";
const char* mqtt_user = "haison-nguyenphuc";
const char* mqtt_password = "soncua2001";
uint16_t mqtt_port = 1883;
const char* mqtt_clientId = "ClientEsp32";
const char* topic1 = "led1";
const char* topic2 = "led2";
const char* topic3 = "flameSensor";
#define led1Pin 2
#define led2Pin 22
#define flameSensorPin 32

WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE	(50)
char msg[MSG_BUFFER_SIZE];
int value = 0;

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  WiFi.mode(WIFI_AP_STA);
  /* start SmartConfig */
  WiFi.beginSmartConfig();
 
  /* Wait for SmartConfig packet from mobile */
  Serial.println("Waiting for SmartConfig.");
  while (!WiFi.smartConfigDone()) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("SmartConfig done.");
 
  /* Wait for WiFi to connect to AP */
  Serial.println("Waiting for WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi Connected.");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  
  if (strcmp(topic, topic1) == 0) {
    Serial.print("Message arrived [");
    Serial.print(topic1);
    Serial.print("] ");
    for (int i = 0; i < length; i++) {
      Serial.print((char)payload[i]);
    }
    if (payload[0] == '1') {
      digitalWrite(led1Pin, HIGH);
    }
    else {
      digitalWrite(led1Pin, LOW);
    }
    Serial.println();
  }

  if (strcmp(topic, topic2) == 0) {
    Serial.print("Message arrived [");
    Serial.print(topic2);
    Serial.print("] ");
    for (int i = 0; i < length; i++) {
      Serial.print((char)payload[i]);
    }
    if (payload[0] == '1') {
      digitalWrite(led2Pin, HIGH);
    }
    else {
      digitalWrite(led2Pin, LOW);
    }
    Serial.println();
  }
  
  
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect(mqtt_clientId, mqtt_user, mqtt_password)) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("connected", "hello world");
      // ... and resubscribe
      client.subscribe("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
  client.subscribe(topic1);
  client.subscribe(topic2);
}

void setup() {
  pinMode(led1Pin, OUTPUT);     // Initialize the led1Pin pin as an output
  pinMode(led2Pin, OUTPUT);
  Serial.begin(9600);
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  unsigned long now = millis();
  if (now - lastMsg > 10000) {
    lastMsg = now;
    strcpy(msg, ((String)analogRead(flameSensorPin)).c_str());
    Serial.print("Publish message: ");
    Serial.println(msg);
    client.publish(topic3, msg);
  }
}
