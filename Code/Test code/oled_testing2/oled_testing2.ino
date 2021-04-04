#include <Wire.h>
#include "SH1106Wire.h"
SH1106Wire display(0x3c, SDA, SCL);

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println();


  // Initialising the UI will init the display too.
  display.init();

  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_10);

}

void loop() {
  // clear the display
  display.clear();
  // draw the current demo method
  draw_ui("105.433");

  display.setTextAlignment(TEXT_ALIGN_RIGHT);
  display.drawString(10, 128, String(millis()));
  // write the buffer to the display
  display.display();

  delay(10);
}

void draw_ui(String dato) {
    // Font Demo1
    // create more fonts at http://oleddisplay.squix.ch/
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.setFont(ArialMT_Plain_10);
    display.drawString(0, 0, "Hello world");
    display.setFont(ArialMT_Plain_16);
    display.drawString(0, 10, "Heading:");
    display.setFont(ArialMT_Plain_24);
    display.drawString(0, 26, dato+"°");
}
