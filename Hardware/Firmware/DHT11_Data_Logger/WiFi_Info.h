// WiFi information
const char* ssid     = "<ssid / wifi network name>";
const char* password = "<wifi password>";

IPAddress local_IP(192, 168, 1, 3); // Static IP
IPAddress gateway(192, 168, 1, 1); // Router IP
IPAddress subnet(255, 255, 255, 0);
IPAddress primaryDNS(8, 8, 8, 8); //optional
IPAddress secondaryDNS(8, 8, 4, 4); //optional
