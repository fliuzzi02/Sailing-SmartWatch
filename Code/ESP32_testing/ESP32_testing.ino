#include "WiFi.h"
#include "AsyncUDP.h"

struct pack {
  String orario;
  String info;
};

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
        while(1) {
            delay(1000);
        }
    }
    if(udp.listen(1234)) {
        Serial.print("UDP Listening on IP: ");
        Serial.println(WiFi.localIP());
        udp.onPacket([](AsyncUDPPacket packet) {
            /*Serial.print("UDP Packet Type: ");
            Serial.print(packet.isBroadcast()?"Broadcast":packet.isMulticast()?"Multicast":"Unicast");
            Serial.print(", From: ");
            Serial.print(packet.remoteIP());
            Serial.print(":");
            Serial.print(packet.remotePort());
            Serial.print(", To: ");
            Serial.print(packet.localIP());
            Serial.print(":");
            Serial.print(packet.localPort());
            Serial.print(", Length: ");
            Serial.print(packet.length());
            Serial.print(", Data: ");
            Serial.write(packet.data(), packet.length());
            Serial.println();*/
            payload = String( (char*) packet.data());
            unwrap(payload);
            //reply to the client
            packet.printf("ACK", packet.length());
        });
    }
}

void loop()
{
    delay(1000);
    //Send broadcast
    udp.broadcast("Broadcasting from: ");
}

//function that divides UDP's packet payload
void unwrap(String dati){
  String campo1 = dati.substring(dati.indexOf("Time:")+5, dati.indexOf(';'));
  String campo2 = dati.substring(dati.indexOf("Data:")+5, dati.indexOf(';', dati.indexOf("Data:")));
  Serial.println(campo1);
  Serial.println(campo2);
}
