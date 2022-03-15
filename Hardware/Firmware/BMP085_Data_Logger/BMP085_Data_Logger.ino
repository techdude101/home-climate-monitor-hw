/*
 * Project: Temperature and Pressure data logger 
 * Description: Read data from a bmp085 sensor and store it in a database
*/ 
#include <Arduino.h>

#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP085.h>

#include <WiFi.h>
#include <HTTPClient.h>

#include <WiFiClientSecure.h>

#include "WiFi_Info.h"
#include "rootCA.h"

#define DEBUG 0
#define TRUE 1
#define FALSE 0
#define USE_STATIC_IP FALSE // Change to TRUE for static IP & DNS. Update WiFi_Info.h

#define BUF_SIZE 150

//#### BMP085 Pins ####
#define BMP_SCK 22
#define BMP_MISO 21
#define BMP_MOSI 11

#define SEALEVELPRESSURE_HPA (1013.25)

//#### Server Config ####
#define URL "http://<yourherokuapp>.com/data/"
#define URL_SECURE "https://<yourherokuapp>.com/data/"

const char* API_KEY = "<device API key>";

Adafruit_BMP085 bmp; // I2C

//#### Device serial number ####
uint16_t serial_num = 1; // CHANGE ME

/*******************************************************************************
* Function Name: printValues
********************************************************************************
*
* Summary:
*  This routine reads the BMP085 sensor and prints the values to the serial console
*
* Parameters:
*  None
*
* Return:
*  None
*
*******************************************************************************/
void printValues() {
    Serial.print("Temperature = ");
    Serial.print(bmp.readTemperature());
    Serial.println(" *C");

    Serial.print("Pressure = ");

    Serial.print(bmp.readPressure() / 100.0F);
    Serial.println(" hPa");

   Serial.print("Approx. Altitude = ");
   Serial.print(bmp.readAltitude(SEALEVELPRESSURE_HPA));
   Serial.println(" m");

    Serial.println();
}

/*******************************************************************************
* Function Name: setClock
********************************************************************************
*
* Summary:
*  This routine sets the RTC from an NTP server
*
* Parameters:
*  None
*
* Return:
*  None
*
*******************************************************************************/
void setClock() {
  configTime(0, 0, "pool.ntp.org", "time.nist.gov");

  Serial.print(F("Waiting for NTP time sync: "));
  time_t nowSecs = time(nullptr);
  while (nowSecs < 8 * 3600 * 2) {
    delay(500);
    Serial.print(F("."));
    yield();
    nowSecs = time(nullptr);
  }

  Serial.println();
  struct tm timeinfo;
  gmtime_r(&nowSecs, &timeinfo);
  Serial.print(F("Current time: "));
  Serial.print(asctime(&timeinfo));
}

/*******************************************************************************
* Function Name: setup
********************************************************************************
*
* Summary:
*  This routine is the builtin Arduino setup routine. Initialize hardware in this routine
*
* Parameters:
*  None
*
* Return:
*  None
*
*******************************************************************************/
void setup() {
  // Start the serial port
  Serial.begin(115200);

  unsigned status;
  
  status = bmp.begin();  
  if (!status) {
      Serial.println("Could not find a valid bmp085 sensor, check wiring, address, sensor ID!");
      while (1);
  }
  
  // Configure WiFi and connect to the network
  #if USE_STATIC_IP == TRUE
  if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)) {
    Serial.println("STA Failed to configure with static IP");
  }
  #endif

  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.print("ESP Mac Address: ");
  Serial.println(WiFi.macAddress());
  Serial.print("Subnet Mask: ");
  Serial.println(WiFi.subnetMask());
  Serial.print("Gateway IP: ");
  Serial.println(WiFi.gatewayIP());
  Serial.print("DNS: ");
  Serial.println(WiFi.dnsIP());

  setClock();
}

/*******************************************************************************
* Function Name: sendHTTPPost
********************************************************************************
*
* Summary:
*  This routine sends an HTTP POST request
*
* Parameters:
*  String url:  URL
*  String json: Data / payload in json format
*
* Return:
*  int:         HTTP response code
*
*******************************************************************************/
int sendHTTPPost(String url, String json) {
  HTTPClient http;
  
  http.begin(url); // HTTP
  http.addHeader("Content-Type", "application/json");
  http.addHeader("X-API-Key", API_KEY);
  int httpCode = http.POST(json);
  // httpCode will be negative on error
  if (httpCode > 0) {
    // HTTP header has been send and Server response header has been handled
    Serial.printf("[HTTP] POST... code: %d\n", httpCode);

    // file found at server
    if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
      String payload = http.getString();
      Serial.println(payload);
    }
  } else {
    Serial.printf("[HTTP] POST... failed, error: %s\n", http.errorToString(httpCode).c_str());
  }
  
  http.end();
  return httpCode;
}

/*******************************************************************************
* Function Name: sendHTTPPostSecure
********************************************************************************
*
* Summary:
*  This routine sends an HTTPS POST request
*
* Parameters:
*  String url:  URL
*  String json: Data / payload in json format
*
* Return:
*  int:         HTTP response code
*
*******************************************************************************/
int sendHTTPPostSecure(String url, String json) {
  WiFiClientSecure *client = new WiFiClientSecure;
  client->setInsecure();
  if(client) {
//    client -> setCACert(rootCACertificate); // Causes intermittent hard fault? Use client->setInsecure(); option
    {
      // Add a scoping block for HTTPClient https to make sure it is destroyed before WiFiClientSecure *client is 
      HTTPClient https;
  
      Serial.print("[HTTPS] begin...\n");

      if (https.begin(*client, url)) {  // HTTPS
        Serial.print("[HTTPS] POST...\n");
        // start connection and send HTTP header
        https.addHeader("Content-Type", "application/json");
        https.addHeader("X-API-Key", API_KEY);
        int httpCode = https.POST(json);
  
        // httpCode will be negative on error
        if (httpCode > 0) {
          // HTTP header has been sent and Server response header has been handled
          Serial.printf("[HTTPS] POST... code: %d\n", httpCode);
  
          // file found at server
          if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
            String payload = https.getString();
            Serial.println(payload);
          }
        } else {
          Serial.printf("[HTTPS] POST... failed, error: %s\n", https.errorToString(httpCode).c_str());
        }
  
        https.end();
      } else {
        Serial.printf("[HTTPS] Unable to connect\n");
      }
      // End extra scoping block
    }
  
    delete client;
  } else {
    Serial.println("Unable to create client");
  }
}

/*******************************************************************************
* Function Name: loop
********************************************************************************
*
* Summary:
*  This routine is the builtin Arduino loop routine. Equivalent to while(1) or for(;;)
*
* Parameters:
*  None
*
* Return:
*  None
*
*******************************************************************************/
void loop() {
  // Read sensor
  #ifdef DEBUG
    printValues(); // Debugging
  #endif
  
  float temp = bmp.readTemperature();
  float pressure = bmp.readPressure() / 100.0F;
   
  // TODO: Check sensor readings are sensible

  //  String json = "{\"serial\": 12345, \"temperature\": 123.45, \"humidity\": 59.43}";
  char json[BUF_SIZE];
  snprintf(json, sizeof json, "{\"serial\": %d, \"temperature\": %.2f, \"humidity\":%.2f }", serial_num, temp, (pressure / 100.0f));
  
  // Check if WiFi is connected
  int wifiStatusCode = WiFi.status();
  if (wifiStatusCode != WL_CONNECTED) {
    Serial.print("WiFi not connected!\n Code: ");
    Serial.println(wifiStatusCode);
    
    // Try disconnecting and reconnecting to the wifi network
    WiFi.disconnect();
    WiFi.begin(ssid, password);
  }

  if (wifiStatusCode == WL_CONNECTED) {
    sendHTTPPostSecure(String(URL_SECURE + String(serial_num)), String(json));
  }
  
  Serial.println();
  Serial.println("Waiting 30s before the next round...");
  delay(30 * 1000);
}
