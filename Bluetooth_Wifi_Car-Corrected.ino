#include <SoftwareSerial.h>
#include <WiFiEsp.h>
#include <WiFiEspClient.h>
#include <PubSubClient.h>

#define motorA1 7
#define motorA2 6
#define motorB1 5
#define motorB2 4
#define ENA 9
#define ENB 3

char ssid[] = "YOUR_WIFI_NAME";
char pass[] = "YOUR_WIFI_PASSWORD";
const char* mqtt_server = "YOUR_MQTT_BROKER_IP";

SoftwareSerial espSerial(10, 11);
WiFiEspClient espClient;
PubSubClient client(espClient);

void setup() {
  Serial.begin(9600);
  espSerial.begin(9600);

  pinMode(motorA1, OUTPUT);
  pinMode(motorA2, OUTPUT);
  pinMode(motorB1, OUTPUT);
  pinMode(motorB2, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);

  stopMotors();

  WiFi.init(&espSerial);

  if (WiFi.status() == WL_NO_SHIELD) {
    while (true) {
      delay(1000);
    }
  }

  WiFi.begin(ssid, pass);

  unsigned long startTime = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startTime < 20000) {
    delay(500);
  }

  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  client.setKeepAlive(45);
  client.setSocketTimeout(15);
}

void callback(char* topic, byte* payload, unsigned int length) {
  if (length == 0) {
    return;
  }

  char command = (char)payload[0];
  controlMotors(command);
}

void reconnect() {
  while (!client.connected()) {
    if (client.connect("ArduinoRobotCarClient")) {
      client.subscribe("car/control");
    } else {
      delay(2000);
    }
  }
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }

  client.loop();
}

void controlMotors(char command) {
  switch (command) {
    case 'F':
      digitalWrite(motorA1, LOW);
      digitalWrite(motorA2, HIGH);
      digitalWrite(motorB1, HIGH);
      digitalWrite(motorB2, LOW);
      analogWrite(ENA, 255);
      analogWrite(ENB, 255);
      break;

    case 'B':
      digitalWrite(motorA1, HIGH);
      digitalWrite(motorA2, LOW);
      digitalWrite(motorB1, LOW);
      digitalWrite(motorB2, HIGH);
      analogWrite(ENA, 255);
      analogWrite(ENB, 255);
      break;

    case 'L':
      digitalWrite(motorA1, LOW);
      digitalWrite(motorA2, HIGH);
      digitalWrite(motorB1, LOW);
      digitalWrite(motorB2, HIGH);
      analogWrite(ENA, 255);
      analogWrite(ENB, 255);
      break;

    case 'R':
      digitalWrite(motorA1, HIGH);
      digitalWrite(motorA2, LOW);
      digitalWrite(motorB1, HIGH);
      digitalWrite(motorB2, LOW);
      analogWrite(ENA, 255);
      analogWrite(ENB, 255);
      break;

    case 'S':
      stopMotors();
      break;
  }
}

void stopMotors() {
  digitalWrite(motorA1, LOW);
  digitalWrite(motorA2, LOW);
  digitalWrite(motorB1, LOW);
  digitalWrite(motorB2, LOW);
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
}
