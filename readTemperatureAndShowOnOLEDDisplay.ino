#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "Adafruit_Sensor.h";

// Included Librararye for Temperature
#include <DHT.h>;
DHT dht2(0,DHT11);
#define OLED_RESET LED_BUILTIN  //4
Adafruit_SSD1306 display(OLED_RESET);

const char* ssid = "Valon Hajredini";
const char* password = "hajredini123";
const char* mqtt_server = "138.197.186.115";

WiFiClient espClient;
PubSubClient client(espClient);
String message;
int value = 0;

#if (SSD1306_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif
void setup_wifi(){
  delay(10);
  WiFi.begin(ssid,password);
 
  display.println("\nConnecting");

  display.display();

  while( WiFi.status() != WL_CONNECTED ){
      delay(500);
      display.print("."); 
      display.display();       
  }

  // Clear the buffer.
  display.clearDisplay();
  display.display();
  display.setCursor(0,0);
  display.println("Wifi Connected!");
  display.print("IP:");
  display.println(WiFi.localIP() );
  display.display();
  display.display();  
}
void callback(char* topic, byte* payload, unsigned int length) {
  for (int i = 0; i < length; i++) {
    message = (char)payload[i];
  }
  if(message == "1") {
    digitalWrite(14, HIGH);
    Serial.print("\nOn\n");
  } else if (message == "0"){
    Serial.print("\nOff\n");
    digitalWrite(14, LOW);
  }
}
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("\nconnected");
      client.subscribe("nodeMCU");
      client.publish("nodeMCU", "0");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
void setup() {

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  // Clear the buffer.
  display.clearDisplay();
  display.display();

  Serial.begin(115200);

  display.setTextSize(1);
  display.setTextColor(WHITE);
  
  display.setCursor(0,0);
  display.println("Wifi connecting to ");
  display.println( ssid );

  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  delay(5000);

  display.clearDisplay();
  display.display();
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
 temperature();
 client.publish("nodeMCU", String(dht2.readTemperature( )).c_str());
 delay(1000);
}
void temperature() {
  // Clear the buffer.
  display.clearDisplay();
  display.setCursor(0,0);
  display.setTextSize(1);
  display.print("Temperatura: ");

  display.setTextSize(3);
  display.setCursor(0, 20);
  if(dht2.readTemperature( ) < 10.00 && dht2.readTemperature( ) >= 0.00){
    display.print("0");
  }
  display.print((dht2.readTemperature( )) );
  display.print(" C");
  Serial.println((dht2.readTemperature( )));
  display.setCursor(99,20);
  display.setTextSize(1);
  display.print("O");

  display.setCursor(26,56);
  display.setTextSize(1);
  display.print("Made By: V.H");
  display.display();

  delay(1000); // update every 1 sec
}


