/* -------------------------------------------------------------------------- */
void drawDefault(struct SolderingStation &station, Adafruit_SH1106 &display) {
    
    display.clearDisplay();

    // display.setFont(&FreeMono9pt7b);
    display.setTextColor(WHITE);
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.print("Set:");
    display.print(station.tempSet);

    drawCalibrationInfo(station, display);
    drawSensorInfo(station, display);
    drawBigTemp(display, station.tempDisplay);
    drawClock(station, display);

    display.drawCircle(124, TEMP_COORD_Y + 2, 2, WHITE);
    display.setCursor(122, TEMP_COORD_Y + 8);
    display.setTextSize(1);
    display.print("C");
    
    // display.drawLine(1, 14, 128, 14, WHITE);
        
    drawLed(station, display);

    if (station.mode == MODE_BTN_HOLD) {
        display.setCursor(0, 36);
        display.setTextSize(1);
        display.println("C/S");
        display.println("MODE");
    }

    display.display();
}
/* -------------------------------------------------------------------------- */
void drawSetTemp(struct SolderingStation &station, Adafruit_SH1106 &display) {
    // Serial.println("Draw set temp");

    display.clearDisplay();

    display.setTextColor(WHITE);
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.print("SET TEMP (C):");

    // drawCounter(display, counter);
    // drawClock(station, display);

    drawBigTemp(display, station.tempSet);
    drawLed(station, display);
    
    display.display();
}
/* -------------------------------------------------------------------------- */
void drawCalibrateSensor(struct SolderingStation &station, Adafruit_SH1106 &display) {
    display.clearDisplay(),

    display.setTextColor(WHITE);
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.print("CALIBRATE:");

    // drawCounter(display, counter);
    // drawClock(station, display);

    display.setTextColor(WHITE);
    display.setTextSize(3);
    display.setCursor(8, 34);

    display.print(station.userCalibration);

    display.display();
}
/* -------------------------------------------------------------------------- */
void drawSelectSensor(struct SolderingStation station, Adafruit_SH1106 &display) {
    display.clearDisplay(),

    display.setTextColor(WHITE);
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.print("SENSOR: (#");
    display.print(station.sensorIndex);
    display.print(")");

    // drawCounter(display, counter);
    // drawClock(station, display);

    display.setTextColor(WHITE);
    display.setTextSize(3);
    display.setCursor(8, 34);
    display.print(station.currentSensor->name);

    display.display();
}
/* -------------------------------------------------------------------------- */
void drawInactive(struct SolderingStation &station, Adafruit_SH1106 &display) {
    display.clearDisplay();

    display.setTextColor(WHITE);
    display.setTextSize(1);
    display.setCursor(0, 0);
    
    display.print("Inactive");
    drawSensorInfo(station, display);
    drawClock(station, display);
  
    display.setCursor(TEMP_COORD_X, TEMP_COORD_Y);
    display.setTextColor(WHITE);
    display.setTextSize(4);
    display.print("---");

    display.drawCircle(124, TEMP_COORD_Y + 2, 2, WHITE);
    display.setCursor(122, TEMP_COORD_Y + 8);
    display.setTextSize(1);
    display.print("C");

    display.display();
}
/* -------------------------------------------------------------------------- */
void drawClock(struct SolderingStation &station, Adafruit_SH1106 &display) {
    #ifdef DRAW_CLOCK
        display.setCursor(0, 12);
        display.setTextColor(WHITE);
        display.setTextSize(1);
        
        if (station.clock.hour > 0) {
            if (station.clock.hour < 10) display.print("0");
            display.print(station.clock.hour);
            display.print(":");
        }

        if (station.clock.minute < 10) display.print("0");
        display.print(station.clock.minute);
        display.print(":");

        if (station.clock.second < 10) display.print("0");
        display.println(station.clock.second); 
    #endif
}
/* -------------------------------------------------------------------------- */
void drawLed(struct SolderingStation &station, Adafruit_SH1106 &display) {
    if (station.tempSet > station.tempMeasured && station.mode != MODE_BTN_HOLD) {
        display.drawCircle(LED_COORD_X, LED_COORD_Y, 8, WHITE);
        display.fillCircle(LED_COORD_X, LED_COORD_Y, 4, WHITE);
    }
}
/* -------------------------------------------------------------------------- */
void drawBigTemp(Adafruit_SH1106 &display, int temp) {
    display.setCursor(TEMP_COORD_X, TEMP_COORD_Y);
    display.setTextColor(WHITE);
    display.setTextSize(4);
    display.print(temp);
}
/* -------------------------------------------------------------------------- */
void drawSensorInfo(struct SolderingStation &station, Adafruit_SH1106 &display) {
    display.setCursor(86, 0);
    // display.print("#");
    display.print("(");
    display.print(station.currentSensor->name);
    display.print(")");
}
/* -------------------------------------------------------------------------- */
void drawCalibrationInfo(struct SolderingStation &station, Adafruit_SH1106 &display) {
    display.setCursor(46, 0);
    display.setTextColor(WHITE);
    display.setTextSize(1);

    if (station.userCalibration != 0) {
        display.print("(");
        if (station.userCalibration > 0) {
            display.print("+");
        }
        display.print(station.userCalibration);
        display.print(")");
    }
}
/* -------------------------------------------------------------------------- */

