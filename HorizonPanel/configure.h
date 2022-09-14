#define ssid        "wifissid"   // You need change it to you wifi ssid and password
#define password    "password"
#define port        22543       // The port is customizable, same as data output port in horizon5 game

IPAddress ip(192,168,0,3);
IPAddress gateway(192,168,0,1);
IPAddress netmask(255,255,255,0);


const int SDA_PIN = 0;  // Pin for ESP8266-01S
const int SDC_PIN = 2;  //
