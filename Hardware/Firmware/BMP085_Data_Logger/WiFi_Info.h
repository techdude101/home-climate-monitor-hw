// WiFi information
const char* ssid     = "<ssid>";
const char* password = "<password>";

#define SERVER_IP_OR_HOSTNAME "192.168.1.x"

IPAddress local_IP(192, 168, 1, 0); // Static IP of ESP32
IPAddress gateway(192, 168, 1, 255); // Router IP
IPAddress subnet(255, 255, 255, 0);
IPAddress primaryDNS(8, 8, 8, 8); // optional
IPAddress secondaryDNS(8, 8, 4, 4); // optional
