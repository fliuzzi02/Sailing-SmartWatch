#include "WiFi.h"
#include "AsyncUDP.h"

struct pack {
  bool wifi_stat;
  String orario;
  String heading;
  String gspeed;
  String dspeed;
  String ddistance;
};

pack attuale;

const char * ssid = "DIGOS-Furgone monitoraggio";
const char * password = "koxaqidl";

AsyncUDP udp;
String payload;

void setup()
{
    Serial.begin(115200);
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    if (WiFi.waitForConnectResult() != WL_CONNECTED) {
        Serial.println("WiFi Failed");
        attuale.wifi_stat = 0;
        while(1) {
            delay(1000);
        }
    } else {
      attuale.wifi_stat = 1;
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
        });
    }
}

void loop()
{
    delay(1000);
    //Send broadcast
    udp.broadcast("Broadcasting");
    Serial.println("Orario: "+attuale.orario);
    Serial.println("Heading: "+attuale.heading);
    Serial.println("Ground Speed: "+attuale.gspeed);
    Serial.println("VMG: "+attuale.dspeed);
    Serial.println("Distance: "+attuale.ddistance);
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
  attuale.heading = info[11];
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

void timeset(String str){
  int h = str.substring(0,2).toInt();
  int m = str.substring(2,4).toInt();
  int s = str.substring(4).toInt();

  h += 2;
  
  attuale.orario = String(h)+":"+String(m)+":"+String(s);
}
