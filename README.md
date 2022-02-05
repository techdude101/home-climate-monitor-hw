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
5. Install the DHT sensor library
6. Change the board type to ESP32 (Tools -> Board -> ESP32 Arduino -> ESP32 Dev Module)
7. Set the serial port (Tools -> Port -> COMx)
8. On line const char* API_KEY = "device API key"; replace <device API key> with the API key previously copied
9. On line uint16_t serial_num = 5; // CHANGE ME - change the serial number to match the serial number copied after adding the device 
10. In the file WiFi_Info.h add your WiFi network username and password  
11. Change URL_SECURE "https://<your.herokuapp>.com/data/" to match your Heroku app
12. Upload the firmware to the device
13. Once uploaded press the reset button on the ESP32 module
14. Open the serial monitor to verify the device successfully connects to the WiFi network, reads the sensor & transmits the data to the server
