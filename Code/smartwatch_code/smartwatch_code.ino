#include "WiFi.h"
#include "AsyncUDP.h"
#include <Wire.h>
#include "SH1106Wire.h"
#include "images.h"
SH1106Wire display(0x3c, SDA, SCL);

typedef void (*Info)(void);

struct pack {
  bool wifi_stat;
  String orario;
  String heading;
  String gspeed;
  String dspeed;
  String ddistance;
};

pack attuale;

int selected_info = 0;
int startup = 0;

const char * ssid = "openplotter";
const char * password = "koxaqidl";

AsyncUDP udp;
String payload;

void setup()
{
    //Initializing struct
    attuale.wifi_stat = 0;
    attuale.orario = "00:00:00";
    attuale.heading = "0°";
    attuale.gspeed = "0kt";
    attuale.dspeed = "0kt";
    attuale.ddistance = "0nm";

    //init button
    pinMode(2, INPUT);

    //Initializing display
    display.init();    
    display.flipScreenVertically();
    display.setFont(ArialMT_Plain_10);
    drawError("Connecting...");
    delay(2000);

    //Initializing wifi
    Serial.begin(115200);
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    if (WiFi.waitForConnectResult() != WL_CONNECTED) {
        Serial.println("WiFi Failed");
        while(1) {
            drawError("Connection Error!!!");
            delay(1000);
            drawError("Rebooting");
            delay(500);
            ESP.restart();
        }
    }
    
    if(udp.listen(1234)) {
        Serial.print("UDP Listening on IP: ");
        Serial.println(WiFi.localIP());
        udp.onPacket([](AsyncUDPPacket packet) {
            char buf[packet.length()];

            for (int i=0;i<packet.length();i++){
                buf[i]= (char)*(packet.data()+i);
            }

            if(String(buf).startsWith("$ECRMB")) rmb(buf);
            if(String(buf).startsWith("$ECRMC")) rmc(buf);
            if(String(buf).startsWith("$ECAPB")) apb(buf);
        });
    }
}

//Makes RMB phrase understandable
void rmb(char dati[]){
  
  String info[14];
  int i,j = 0;
  
  for(i = 0; i<14; i++){
    info[i] = "";
    do{
      info[i].concat(dati[j]);
      j++;
    }while(dati[j] != ',');
    j++;
  }

  //passing RMB info to global struct
  attuale.ddistance = info[10];
  attuale.dspeed = info[12];
  //Serial.println("Recieved RMB");
}

//Makes RMC phrase understandable
void rmc(char dati[]){
  
  String info[12];
  int i,j = 0;
  
  for(i = 0; i<12; i++){
    info[i] = "";
    do{
      info[i].concat(dati[j]);
      j++;
    }while(dati[j] != ',');
    j++;
  }

  //passing RMC info to global struct
  timeset(info[1]);
  attuale.gspeed = info[7];
  //Serial.println("Recieved RMC");
}

//Makes APB phrase understandable
void apb(char dati[]){
  
  String info[14];
  int i,j = 0;
  
  for(i = 0; i<14; i++){
    info[i] = "";
    do{
      info[i].concat(dati[j]);
      j++;
    }while(dati[j] != ',');
    j++;
  }

  //passing APB info to global struct
  attuale.heading = info[13];
}

void timeset(String str){
  int h = str.substring(0,2).toInt();
  String m = str.substring(2,4);
  String s = str.substring(4);

  h += 2;
  
  attuale.orario = String(h)+":"+m+":"+s;
}

//function that draws the time
void draw_orario() {
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.setFont(ArialMT_Plain_10);
    display.drawString(0, 0, "by desmofede42");
    
    display.setTextAlignment(TEXT_ALIGN_CENTER);
    display.setFont(ArialMT_Plain_24);
    display.drawString(64, 22, attuale.orario);
}

//function that draws heading info
void draw_heading() {
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.setFont(ArialMT_Plain_10);
    display.drawString(0, 0, attuale.orario);
    display.setFont(ArialMT_Plain_16);
    display.drawString(0, 10, "Head to:");
    display.setFont(ArialMT_Plain_24);
    display.drawString(0, 26, attuale.heading+"°");
}

//function that draws ground speed info
void draw_gspeed() {
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.setFont(ArialMT_Plain_10);
    display.drawString(0, 0, attuale.orario);
    display.setFont(ArialMT_Plain_16);
    display.drawString(0, 10, "Ground speed:");
    display.setFont(ArialMT_Plain_24);
    display.drawString(0, 26, attuale.gspeed+"kt");
}

//function that draws vmg info
void draw_dspeed() {
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.setFont(ArialMT_Plain_10);
    display.drawString(0, 0, attuale.orario);
    display.setFont(ArialMT_Plain_16);
    display.drawString(0, 10, "VMG:");
    display.setFont(ArialMT_Plain_24);
    display.drawString(0, 26, attuale.dspeed+"kt");
}

//function that draws distance info
void draw_ddistance() {
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.setFont(ArialMT_Plain_10);
    display.drawString(0, 0, attuale.orario);
    display.setFont(ArialMT_Plain_16);
    display.drawString(0, 10, "Distance:");
    display.setFont(ArialMT_Plain_24);
    display.drawString(0, 26, attuale.ddistance+"nm");
}

//Initial show
void drawError(String message) {
    display.clear();
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.setFont(ArialMT_Plain_10);
    display.drawString(0, 0, message);
    display.drawXbm(34, 14, WiFi_Logo_width, WiFi_Logo_height, WiFi_Logo_bits);
    display.display();
}

Info info[] = {draw_orario, draw_heading, draw_gspeed, draw_dspeed, draw_ddistance};

void loop()
{    
    delay(500);

    Serial.println("Orario: "+attuale.orario);
    Serial.println("Heading: "+attuale.heading);
    Serial.println("Ground Speed: "+attuale.gspeed);
    Serial.println("VMG: "+attuale.dspeed);
    Serial.println("Distance: "+attuale.ddistance);

    display.clear();
    // draw the current demo method
    info[selected_info]();
  
    display.setTextAlignment(TEXT_ALIGN_RIGHT);
    display.drawString(10, 128, String(millis()));
    // write the buffer to the display
    display.display();

    //checks if button is pressed
    if(digitalRead(4) == HIGH && selected_info != 4) selected_info++;
    else if(selected_info == 4) selected_info = 0;
}
