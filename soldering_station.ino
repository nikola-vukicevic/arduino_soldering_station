/* -------------------------------------------------------------------------- */
#include "config.h"
/* -------------------------------------------------------------------------- */
// Adafruit_SH1106G Display = Adafruit_SH1106G(128, 64, &Wire, -1);
Adafruit_SH1106 Display(OLED_RESET);
struct SolderingStation Station;
struct SensorInfo SENSOR_01;
struct SensorInfo SENSOR_02;
#ifndef SAMPLE_SMOOTHING
struct SensorInfo SENSOR_03;
// struct SensorInfo SENSOR_04;
// struct SensorInfo SENSOR_05;
#endif
// struct StationClock Clock;
unsigned long Counter, CounterFps, PreviousMilliseconds;
/* -------------------------------------------------------------------------- */
void setup() {
    #ifndef IRON_LIVE
    Serial.begin(SERIAL_RATE);
    Serial.println("INIT");
    #endif

    pinMode(PIN_ENCODER_A,   INPUT);
    pinMode(PIN_ENCODER_B,   INPUT);
    pinMode(PIN_ENCODER_SW,  INPUT);
    pinMode(PIN_HEATER,      OUTPUT);
    pinMode(FAKE_GROUND,     OUTPUT);
    pinMode(FAKE_VCC,        OUTPUT);
    pinMode(PIN_BUZZER,      OUTPUT);
    pinMode(PIN_TEMPERATURE, INPUT);

    digitalWrite(FAKE_GROUND, LOW);
    digitalWrite(FAKE_VCC,    HIGH);
    // killBeep();

    // digitalWrite(PIN_BUZZER, LOW);

    attachInterrupt(digitalPinToInterrupt(PIN_ENCODER_A), readPin_A, CHANGE);
    attachInterrupt(digitalPinToInterrupt(PIN_ENCODER_B), readPin_B, CHANGE);

    initTimersInfo(Station);
    initSensorsInfo(Station);
    
    // initSensorsInfoDodatni(Station);
    #ifdef SAMPLE_SMOOTHING
        initQueue(&Station.queue, TEMP_SAMP_AVG);
    #endif

    Counter    = 0;
    CounterFps = 0;

    Station.mode                 = MODE_DEFAULT;
    Station.turnReadout          = 0b11; // 11 bin
    Station.previousClick        = 0;
    Station.turnAcceleration     = TURN_ACCELERATION;
    Station.tempSet              = TEMP_DEFAULT;
    Station.tempSetSave          = TEMP_DEFAULT;
    Station.userCalibration      = 0;
    // Station.userCalibrationStep  = 10;
    Station.sensorCount          = SENSOR_COUNT;
    Station.sensorIndex          = 1; // 1 - A1321, 2 - A1322, 3 - T245
    Station.currentSensor        = Station.sensors[0];
    Station.tempTrend            = 0;
    Station.tempToleranceUp      = TEMP_TOLERANCE_UP;
    Station.tempToleranceDown    = TEMP_TOLERANCE_DOWN;
    Station.initMillis           = millis();

    // delay(PWUP_DELAY);
    // delay(2000);
    Display.begin();
    // Display.begin(0x3C, true);
    // Display.setContrast(0);
}
/* -------------------------------------------------------------------------- */
void loop() {
    getFps(CounterFps, PreviousMilliseconds, 1000);

    correctTimerInactive(Station);
    checkBuzzer(Station);
        
    Station.click = !(PIND & MASK_PIN_ENC_SW); // !digitalRead(PIN_K);

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
