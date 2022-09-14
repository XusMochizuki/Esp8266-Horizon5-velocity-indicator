#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiUDP.h>
#include "SSD1306Wire.h"  //0.96inch screen

#define ssid        "wifissid"   // You need change it to you wifi ssid and password
#define password    "password"
#define port        22543       // The port is customizable, same as data output port in horizon5 game

union charnumber{     // A union to convert received data
  unsigned char c[332];
  float f[83];
  int i[83];
};

IPAddress ip(192,168,0,3);
IPAddress gateway(192,168,0,1);
IPAddress netmask(255,255,255,0);
const int I2C_DISPLAY_ADDRESS = 0x3c;  // I2c address default
const int SDA_PIN = 0;  // Pin for ESP8266-01S
const int SDC_PIN = 2;  //
const char gearicon[]={0x8F,0x03,0xCF,0x03,0xDE,0x3B,0xDE,0x7B,0xDC,0x7D,0xE8,0x3E,0xC8,0xDE,0x00,0xEE,0x00,0xF6,0x00,0x78,0x00,0x38,0x00,0x58,0x00,0x60,0x00,0x78,0x00,0x38,0x00,0x10};
int sleepcount=0;
bool screenon=true;
WiFiUDP Udp;
SSD1306Wire display(I2C_DISPLAY_ADDRESS, SDA_PIN, SDC_PIN);   // 0.96inch
OLEDDisplay* dp=&display;
union charnumber cn;

void drawLabel(OLEDDisplay *display,String label,bool clear);
void drawPanel(OLEDDisplay *display, int Lengine, int Lspeed, int Lgear,int Lbrake,int Lhand);

void setup() {
  //Serial.begin(115200);
  //Serial.println();
  // init screen
  display.init();
  drawLabel(dp,"Connecting...",true);
  display.flipScreenVertically(); // Flip Screen
  display.setContrast(255);       // Screen brightness
  
  WiFi.config(ip,gateway,netmask);
  WiFi.begin(ssid,password);
  //Serial.printf("Trying to connect %s\n",ssid);
  while (WiFi.status() != WL_CONNECTED)   // waitting for connect
  {
    delay(500);
    sleepcount++;
    //Serial.print(".");
    if(sleepcount>20){
      drawLabel(dp,"Wifi connect timeout",true);
    }
  }
  //Serial.printf("\nConnect Successful!\n");
  delay(1000);
  if(Udp.begin(port)){
    //Serial.printf("Listen at %s : %d\n", WiFi.localIP().toString().c_str(), port);
  }
  drawLabel(dp,"Waiting for data...",true);
}

void loop() {
  cn.c[0]=0;
  int packetSize=Udp.parsePacket();
  if(packetSize){
    int len= Udp.read(cn.c,332);
  }
  if(cn.c[0]){
    if(!screenon){
      display.displayOn();
      //Serial.println("running mode");
      screenon=true;
    }
    sleepcount=0;
    drawPanel(dp,(int)((cn.f[4]/cn.f[2])*100),(int)(cn.f[64]*3.6),cn.c[319],cn.c[316],cn.c[318]);
    //Serial.printf("%f",(cn.f[4]/cn.f[2])*100);
  }else{
    sleepcount++;
//    if(sleepcount==5){    // clean data when data transform stop,if you need
//      drawPanel(dp,0,0,0,0,0);
//    }
    if(sleepcount == 2900){
      drawLabel(dp,"Power Save Mode",true);
    }
    if(sleepcount==3000){
      display.displayOff();
      //Serial.println("low performance mode");
      screenon=false;
     }
     if(sleepcount>3000){
      delay(1000);    // powersave mode
     }
  }
  delay(33);
  Udp.endPacket();
}

void drawLabel(OLEDDisplay *display,String label,bool clear){
  if(clear){
    display->clear();
  }
  display->setFont(ArialMT_Plain_16);
  display->setTextAlignment(TEXT_ALIGN_CENTER);
  display->drawString(64,16,label);
  display->display();
}
void drawPanel(OLEDDisplay *display, int Lengine, int Lspeed, int Lgear,int Lbrake,int Lhand) {    //绘制进度
  display->clear();
  
  display->drawRect(3,2,122,15);  // left top as 0,0 right bottom as 128,64
  map(Lengine,1,95,0,118);
  display->fillRect(5,4,Lengine,11);
  display->setFont(ArialMT_Plain_24);
  display->setTextAlignment(TEXT_ALIGN_CENTER);
  display->drawString(64,32,(String)Lspeed);
  display->setFont(ArialMT_Plain_10);
  if(Lhand){
    display->drawString(15,47,"H");
  }
  if(Lbrake){
    display->drawString(30,47,"B");
  }
  display->setFont(ArialMT_Plain_16);
  
  display->drawIco16x16(101,30,gearicon,0);
  display->setTextAlignment(TEXT_ALIGN_RIGHT);
  if(Lgear==0){
    display->drawString(110,38,"R");
  }else{
    display->drawString(110,38,(String)Lgear);
  }
  display->display();
}
