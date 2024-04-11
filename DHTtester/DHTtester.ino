#include <DHT.h>
#include <Adafruit_Sensor.h>
#include <WiFiNINA.h>
#include "secrets.h"
#include <ThingSpeak.h>

#define DHTPIN 2
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);

char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;   
int keyIndex = 0;            
WiFiClient  client;

unsigned long myChannelNumber = SECRET_CH_ID;
const char * myWriteAPIKey = SECRET_WRITE_APIKEY;

void setup() {
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect.
  }
  
  if(WiFi.status() != WL_CONNECTED){
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(SECRET_SSID);
    while(WiFi.status() != WL_CONNECTED){
      WiFi.begin(ssid, pass);
      delay(5000);     
    } 
    Serial.println("\nConnected.");
  }
  Serial.println("\nWiFi connected");

  dht.begin();
  ThingSpeak.begin(client);
}

void loop() {
  // Reading temperature or humidity
  float h = dht.readHumidity();
  // Read temperature as Celsius
  float t = dht.readTemperature();

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  // Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(t, h, false);

  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.print("%\t");
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.print("°C ");

  // Update ThingSpeak fields with temperature and humidity
  ThingSpeak.setField(1, t); // Field 1 for temperature
  ThingSpeak.setField(2, h); // Field 2 for humidity

  // Write the fields that you've set all at once.
  if (ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey) == 200) {
    Serial.println("Channel update successful.");
  } else {
    Serial.println("Problem updating channel. HTTP error code " + String(ThingSpeak.getLastReadStatus()));
  }

  // Wait a few seconds between measurements.
  delay(15000);
}
