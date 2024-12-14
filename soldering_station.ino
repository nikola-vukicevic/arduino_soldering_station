/* -------------------------------------------------------------------------- */
#include "config.h"
/* -------------------------------------------------------------------------- */
Adafruit_SH1106 Display(OLED_RESET);
struct SolderingStation Station;
struct SensorInfo SENSOR_01;
struct SensorInfo SENSOR_02;
#ifndef SAMPLE_SMOOTHING
struct SensorInfo SENSOR_03;
struct SensorInfo SENSOR_04;
struct SensorInfo SENSOR_05;
#endif
unsigned long Counter, CounterFps, PreviousMilliseconds;
/* -------------------------------------------------------------------------- */
void setup() {
    Serial.begin(SERIAL_RATE);
    Serial.println("INIT");

    pinMode(PIN_A,   INPUT);
    pinMode(PIN_B,   INPUT);
    pinMode(PIN_K,   INPUT);
    pinMode(PIN_LED, OUTPUT);
    pinMode(PIN_T,   INPUT);

    attachInterrupt(digitalPinToInterrupt(PIN_A), readPin_A, CHANGE);
    attachInterrupt(digitalPinToInterrupt(PIN_B), readPin_B, CHANGE);

    initTimersInfo(Station);
    initSensorsInfo(Station);
    Station.currentSensor = Station.sensors[0];
    #ifdef SAMPLE_SMOOTHING
        initQueue(&Station.queue, TEMP_SAMP_AVG);
    #endif

    delay(PWUP_DELAY);
    Display.begin();

    Counter    = 0;
    CounterFps = 0;

    Station.mode                 = MODE_DEFAULT;
    Station.turnReadout          = 3; // 11 bin
    Station.previousClick        = 0;
    Station.acc                  = 5;
    Station.tempSet              = 250;
    Station.userCalibration      = 0;
    Station.userCalibrationStep  = 10;
    Station.sensorIndex          = 1;
    Station.sensorCount          = SENSOR_COUNT;
    Station.tempTrend            = 0;
    Station.tempToleranceUp      = TEMP_TOLERANCE_UP;
    Station.tempToleranceDown    = TEMP_TOLERANCE_DOWN;
    Station.initMillis           = millis();
}
/* -------------------------------------------------------------------------- */
void loop() {
    getFps(CounterFps, PreviousMilliseconds, 1000);

    correctTimerInactive(Station);
        
    Station.click = !(PIND & MASK_PIN_K); // !digitalRead(PIN_K);

    regulateTemperature(Station); // iron ON/OFF regulated here, as well as sample smoothing
    regulateClock(Station);

    if (Station.click == 1 || Station.previousClick == 1) {
        readPins(Station); // All button events handled here
    }

    switch (Station.mode) {
        case MODE_DEFAULT:  regulateLoop_Default(Station, Display);  break;
        case MODE_SET_TEMP: regulateLoop_SetTemp(Station, Display);  break;
        case MODE_BTN_HOLD: regulateLoop_BtnHold(Station, Display);  break;
        case MODE_SET_CAL:  regulateLoop_SetCal(Station, Display);   break;
        case MODE_SEL_SENS: regulateLoop_SelSens(Station, Display);  break;
        case MODE_INACTIVE: regulateLoop_Inactive(Station, Display); break;
        default: regulateLoop_Default(Station, Display); break;
    }

    Counter++;
}
/* -------------------------------------------------------------------------- */
