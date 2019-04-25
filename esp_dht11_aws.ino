// Andrei Kaplun
#include <ESP8266WiFi.h>
#include <DHT11.h>
#include <AmazonIOTClient.h>
#include <ESP8266AWSImplementations.h>

Esp8266HttpClient httpClient;
Esp8266DateTimeProvider dateTimeProvider;

AmazonIOTClient iotClient;
ActionError actionError;

// Credentials for Wi-Fi connection
const char* ssid = "XXXXX";
const char* password = "xxxxxxxxx";

// Pin 2 will be used to read sensor data
int pin = 2;

// Initializing the sensor
DHT11 dht11(pin);

// Function to convert Celsius to Fahrenheit
double Fahrenheit(double celsius) {
  return ((celsius * 9 / 5) + 32);
}

// One time setup
void setup() {
  Serial.begin(115200);
  delay(10);
  Serial.println();
  
  // Connecting to Wi-Fi
  WiFi.mode(WIFI_STA);
  Serial.println();
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
  
  // Set up AWS IoT
  iotClient.setAWSRegion("us-east-1");
  iotClient.setAWSEndpoint("amazonaws.com");
  iotClient.setAWSDomain("a1me2v15ff6ds5.iot.us-east-1.amazonaws.com");
  iotClient.setAWSPath("/things/ESP/shadow");
  iotClient.setAWSKeyID("xxxxxxxxx");
  iotClient.setAWSSecretKey("xxxxxxxxx");
  iotClient.setHttpClient(&httpClient);
  iotClient.setDateTimeProvider(&dateTimeProvider);
}

// This code will be running in a loop for as long as the device is operational
void loop() {

  int err;
  float temp, humi;
  
  // Reading the sensor data
  if ((err = dht11.read(humi, temp)) == 0) {
    Serial.print("temperature:");
    Serial.print(temp);
    Serial.print(" humidity:");
    Serial.print(humi);
    Serial.println();
  }
  else {
    Serial.println();
    Serial.print("Error No :");
    Serial.print(err);
    Serial.println();
  }

  // Converting the float data to char arrays
  char sTemp[10], sHumi[10];
  dtostrf(humi, 6, 2, sHumi);
  dtostrf(temp, 6, 2, sTemp);

  // Creating the Thing Shadow to be sent to AWS IoT
  char shadow[100];
  strcpy(shadow, "{\"state\":{\"reported\": {\"Humidity\":");
  strcat(shadow, sHumi);
  strcat(shadow, ", \"Temperature\":");
  strcat(shadow, sTemp);
  strcat(shadow, "}}}");
  Serial.print(shadow);

  // Send the generated shadow to AWS IoT
  char* result = iotClient.update_shadow(shadow, actionError);
  Serial.print(result);

  delay(60000);
}

