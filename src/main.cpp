/*********
  Rui Santos
  Complete project details at https://randomnerdtutorials.com  
*********/

#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#include <esp_task_wdt.h>

#define USERNAME "fakecc"
#define TOPIC0 "fakecc/feeds/tmp0"
#define TOPIC1 "fakecc/feeds/tmp1"

#define ONE_WIRE_BUS_0 15
#define ONE_WIRE_BUS_1 4

#define MQTT_INTERVAL_PUBLISH 60000

OneWire oneWire0(ONE_WIRE_BUS_0);
DallasTemperature sensor0(&oneWire0);
OneWire oneWire1(ONE_WIRE_BUS_1);
DallasTemperature sensor1(&oneWire1);

// Replace the next variables with your SSID/Password combination
const char *ssid = "StarNet";
const char *password = "022844444";
const char *apiKey = "";
const char *userName = USERNAME;


// Add your MQTT Broker IP address, example:
//const char* mqtt_server = "192.168.1.144";
const char* mqtt_server = "io.adafruit.com";

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;



void setup_wifi();
void readDSTemperatureC();
void reconnect();

#define WDT_TIMEOUT 10

void setup() {
  pinMode(BUILTIN_LED, OUTPUT);
  digitalWrite(BUILTIN_LED, 1);

  Serial.begin(115200);
  Serial.println("Configuring WDT...");
  esp_task_wdt_init(WDT_TIMEOUT, true); //enable panic so ESP32 restarts
  esp_task_wdt_add(NULL); //add current thread to WDT watch

  setup_wifi();
  digitalWrite(BUILTIN_LED, 1);
  client.setServer(mqtt_server, 1883);
  digitalWrite(BUILTIN_LED, 0);
 
}

void setup_wifi() {
  digitalWrite(BUILTIN_LED, 1);
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  digitalWrite(BUILTIN_LED, 0);
}

void reconnect() {
  digitalWrite(BUILTIN_LED, 1);
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP32Client", userName, apiKey)) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
  digitalWrite(BUILTIN_LED, 0);
}

void loop() {
  esp_task_wdt_reset();
  if (WiFi.status() != WL_CONNECTED){
    setup_wifi();
  }
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  digitalWrite(BUILTIN_LED, 0);
  long now = millis();
  if (now - lastMsg > MQTT_INTERVAL_PUBLISH) {
    lastMsg = now;
    readDSTemperatureC();
  }
}

void readDSTemperatureC()
{
  // Call sensors.requestTemperatures() to issue a global temperature and Requests to all devices on the bus
  sensor0.requestTemperatures();
  sensor1.requestTemperatures();
  delay(1000);
  float tempC = sensor0.getTempCByIndex(0);
  String temp = "";
  char buff[10];
  temp.concat(sensor0.getTempCByIndex(0));
  temp.toCharArray(buff, 10);
  if (-126.5 < tempC)
  {
    Serial.print("Temperature Celsius: ");
    Serial.println(buff);
    client.publish(TOPIC0, buff);
  }else{
    Serial.println(buff);
  }
  temp = "";
  temp.concat(sensor1.getTempCByIndex(0));
  temp.toCharArray(buff, 10);
  if (-126.5 < tempC)
  {
    Serial.print("Temperature Celsius: ");
    Serial.println(buff);
    client.publish(TOPIC1, buff);
  }else{
    Serial.println(buff);
  }
}