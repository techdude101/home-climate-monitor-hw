# home-climate-monitor-hw
Home Climate Monitor - Hardware and Firmware

## Prerequisites

DB setup  
API setup  
Frontend setup  
Root / Admin API key

## Firmware
1. Checkout / download the project
2. Add a new device via the web page e.g. youraccount.github.io/home-climate-monitor/
3. Copy the API key generated AND the serial number
4. Open DHT11_Data_Logger.ino with the Arduino IDE
4. On line const char* API_KEY = "device API key"; replace <device API key> with the API key previously copied
5. In the file WiFi_Info.h add your WiFi network username and password
6. On line uint16_t serial_num = 5; // CHANGE ME - change the serial number to match the serial number copied after adding the device
7. Upload the firmware to the device
