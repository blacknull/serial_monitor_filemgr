#include <Arduino.h>
#include "sm_filemgr.h"

void setup() {
    // Serial port initialization
    Serial.begin(115200);
    delay(10);
    
    sm_filemgr_init();
}

void loop() {

  sm_filemgr_process();

  delay(10);
}
