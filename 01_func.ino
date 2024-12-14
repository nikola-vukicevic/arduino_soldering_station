/* -------------------------------------------------------------------------- */
void initSensorsInfo(struct SolderingStation &station) {

	/* ----- SENSOR #1 (A1321) ----- */

    station.sensors[0] = &SENSOR_01;

    sprintf(SENSOR_01.name, "A1321");
	SENSOR_01.tempRef     = SENSOR_TEMP_REF;
    SENSOR_01.adcRef      = 479.00;
    SENSOR_01.slope       = 2.4561;
    SENSOR_01.calibration = -100.00;

	/* ----- SENSOR #2 (A1322) ----- */	

    station.sensors[1] = &SENSOR_02;

    sprintf(SENSOR_02.name, "A1322");
    SENSOR_02.tempRef     = SENSOR_TEMP_REF;
	SENSOR_02.adcRef      = 415.00;
    SENSOR_02.slope       = 0.7932;
    SENSOR_02.calibration = 0;

    #ifndef SAMPLE_SMOOTHING

    /* ----- SENSOR #3 (TK-12) ----- */

    station.sensors[2] = &SENSOR_03;

    sprintf(SENSOR_03.name, "TK-12");
    SENSOR_03.tempRef = SENSOR_TEMP_REF;
    SENSOR_03.adcRef  = 415;
    SENSOR_03.slope   = 0.7932;
    SENSOR_03.calibration = 0;

    /* ----- SENSOR #4 (TK-16) ----- */

    station.sensors[3] = &SENSOR_04;
    
    sprintf(SENSOR_04.name, "TK-X4");
    SENSOR_04.tempRef = SENSOR_TEMP_REF;
    SENSOR_04.adcRef  = 415;
    SENSOR_04.slope   = 0.7932;
    SENSOR_04.calibration = 0;

    /* ----- SENSOR #5 (TK-18) ----- */

    station.sensors[4] = &SENSOR_05;
    
    sprintf(SENSOR_05.name, "TK-X5");
    SENSOR_05.tempRef = SENSOR_TEMP_REF;
    SENSOR_05.adcRef  = 415;
    SENSOR_05.slope   = 0.7932;
    SENSOR_05.calibration = 0;

    /* ----- SENSOR #6 (TK-20) ----- */

    // station.sensors[5] = &SENSOR_06;
    
    // sprintf(SENSOR_06.name, "TK-X6");
    // SENSOR_06.tempRef = SENSOR_TEMP_REF;
    // SENSOR_06.adcRef  = 415;
    // SENSOR_06.slope   = 0.7932;

    /* ----- SENSOR #7 (TK-22) ----- */

    // station.sensors[6] = &SENSOR_07;
    
    // sprintf(SENSOR_07.name, "TK-X7");
    // SENSOR_07.tempRef = SENSOR_TEMP_REF;
    // SENSOR_07.adcRef  = 415;
    // SENSOR_07.slope   = 0.7932;

    #endif
}
/* -------------------------------------------------------------------------- */
void initTimersInfo(struct SolderingStation &station) {
    station.timerTemp.time           = TIMER_SET_T_DEF;
    station.timerTemp.previousMillis = 0;
    station.timerTemp.trigger        = false;

    station.timerCal.time           = TIMER_SET_C_DEF;
    station.timerCal.previousMillis = 0;
    station.timerCal.trigger        = false;

    station.timerSens.time           = TIMER_SEL_S_DEF;
    station.timerSens.previousMillis = 0;
    station.timerSens.trigger        = false;

    station.timerCalSens.time    = TIMER_SENS_DEF;
    station.timerCalSens.trigger = false;

    station.timerInactive.time           = TIMER_INACTIVE_DEF; // 0
    station.timerInactive.previousMillis = station.timerInactive.currentMillis;
    station.timerInactive.trigger        = true;

    station.timerRegulate.time = TEMP_READ_RATE;
}
/* -------------------------------------------------------------------------- */
void readPin_A() {
    readPins(Station);
}
/* -------------------------------------------------------------------------- */
void readPin_B() {
    readPins(Station);
}
/* -------------------------------------------------------------------------- */
void regulateTimer(struct Timer &timer, struct SolderingStation &station) {
    if (timer.trigger == false) return;

    timer.currentMillis = millis();
    boolean tick = timer.currentMillis - timer.previousMillis >= timer.time;

    if (tick == false) return;

    timer.previousMillis = timer.currentMillis;
    initModeDefault(station);
}
/* -------------------------------------------------------------------------- */
void regulateTimerAux(struct Timer &timer) {
    timer.currentMillis = millis();
    boolean tick        = timer.currentMillis - timer.previousMillis >= timer.time; // 1ms

    if (tick == false) return;

    timer.trigger = true;
}
/* -------------------------------------------------------------------------- */
void correctTimerInactive(struct SolderingStation &station) {
    if (station.timerInactive.trigger == false) return;

    station.timerInactive.currentMillis = millis();
    boolean tick = station.timerInactive.currentMillis -
		           station.timerInactive.previousMillis >=
				   station.timerInactive.time; // 1ms

    if (tick == false) return;

    station.timerInactive.trigger = false;
    station.mode                  = MODE_INACTIVE;
}
/* -------------------------------------------------------------------------- */
void resetTimerInactive(struct SolderingStation &station) {
    // Serial.println("Wakeup");
    station.timerInactive.previousMillis = millis();
    station.timerInactive.trigger        = true;
}
/* -------------------------------------------------------------------------- */
void initModeDefault(struct SolderingStation &station) {
    #ifdef SERIAL_MESSAGES_DEBUG
        if (station.mode != MODE_DEFAULT) Serial.println("INIT Default mode");
    #endif

    station.timerCalSens.trigger = false;
    station.previousMode         = station.mode;
    station.mode                 = MODE_DEFAULT;
}
/* -------------------------------------------------------------------------- */
void initModeSetTemp(struct SolderingStation &station) {
    #ifdef SERIAL_MESSAGES_DEBUG
        if (station.previousMode != MODE_SET_TEMP) Serial.println("INIT Set Temp");
    #endif

    station.previousMode             = station.mode;
    station.mode                     = MODE_SET_TEMP;
    station.timerTemp.trigger        = true;
    station.timerTemp.previousMillis = millis();
}
/* -------------------------------------------------------------------------- */
void initModeBtnHold(struct SolderingStation &station) {
    #ifdef SERIAL_MESSAGES_DEBUG
    //    if (station.previousMode != MODE_BTN_HOLD) Serial.println("INIT Button down");
    #endif

    station.previousMode      = station.mode;
    station.mode              = MODE_BTN_HOLD;
    station.timerCalSens.time = TIMER_SENS_DEF;
}
/* -------------------------------------------------------------------------- */
void initModeCalibration(struct SolderingStation &station) {
    #ifdef SERIAL_MESSAGES_DEBUG
        if (station.previousMode != MODE_SET_CAL) Serial.println("INIT Calibration");
    #endif

    station.previousMode            = station.mode;
    station.mode                    = MODE_SET_CAL;
    station.timerCalSens.time       = 0;
    station.timerCal.trigger        = true;
    station.timerCal.previousMillis = millis();
}
/* -------------------------------------------------------------------------- */
void initModeSelectSensor(struct SolderingStation &station) {
    #ifdef SERIAL_MESSAGES_DEBUG
        if (station.previousMode != MODE_SEL_SENS) Serial.println("INIT Select sensor");
    #endif

    station.previousMode             = station.mode;
    station.mode                     = MODE_SEL_SENS;
    station.timerSens.trigger        = true;
    station.timerSens.previousMillis = millis();
}
/* -------------------------------------------------------------------------- */
void auxSerialWriteInfo(char *name, struct SolderingStation station) {
    Serial.print("(");
    Serial.print(station.mode);
    Serial.print(") ");
    Serial.print(name);
    Serial.print(": ");

    switch (station.mode) {
        case MODE_SET_TEMP: Serial.println(station.tempSet);             break;
        case MODE_SET_CAL:  Serial.println(station.userCalibration);         break;
        case MODE_SEL_SENS: Serial.println(station.currentSensor->name); break;
        default: break;
    }
}
/* -------------------------------------------------------------------------- */
// old (A = 2; B = 3): [ 0, -1, 1, 0, 1, 0, 0, -1, -1, 0, 0, 1, 0, 1, -1, 0 ]
// new (A = 2; B = 3): [ 0, 1, -1, 0, -1, 0, 0, 1, 1, 0, 0, -1, 0, -1, 1, 0 ]
// new (A = 3; B = 2): [ 0, -1, 1, 0, 1, 0, 0, -1, -1, 0, 0, 1, 0, 1, -1, 0 ]
/* -------------------------------------------------------------------------- */
int8_t readTurn(struct SolderingStation &station) {
    static const int8_t turnDirectionTable[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, -1, 0 };
    uint8_t pinsReadout;
    
    pinsReadout = PIND & MASK_PINS_AB;

    station.turnReadout >>= 2;
    station.turnReadout  |= pinsReadout;

    return turnDirectionTable[station.turnReadout];
}
/* -------------------------------------------------------------------------- */
void readPins(struct SolderingStation &station) {
    int8_t tick = readTurn(station);
    // station.acc =  correct_acc(station.tempSet, station.acc, station.accGranica, ACC_1, ACC_2);

    boolean press   = station.click == 1 && station.previousClick == 0;
    boolean hold    = station.click == 1 && station.previousClick == 1;
    boolean release = station.click == 0 && station.previousClick == 1;

    if (press == true) {
        // Serial.println("Button push");

        station.timerCalSens.trigger        = false;
        station.timerCalSens.previousMillis = millis();

        if (station.mode == MODE_DEFAULT) {
            initModeBtnHold(station);
        }
        else {
            initModeDefault(station);
        }
    }

    // Holding down the encoder button:

    if (station.mode == MODE_BTN_HOLD && hold == true && station.timerCalSens.trigger == true) {
        initModeSelectSensor(station);
    }

    // Quick release to enter calibration mode:
    
    if (station.mode == MODE_BTN_HOLD && release == true && station.timerCalSens.trigger == false) {
        // Serial.println("Button release");
        initModeCalibration(station);
    }

    station.previousClick = station.click;
    
    if (tick != 0) {
        encoderTurn(tick, station);
    }
}
/* -------------------------------------------------------------------------- */
void encoderTurn(int8_t tick, struct SolderingStation &station) {
    resetTimerInactive(station);

    if (station.mode < MODE_SET_CAL) {
        initModeSetTemp(station);
        station.tempSet += tick * station.acc;
        station.tempSet  = limitSetTemp(station.tempSet, TEMP_SET_MIN, TEMP_SET_MAX);
        
        #ifdef SERIAL_MESSAGES_INFO
            auxSerialWriteInfo("Temperature", station);
        #endif
        
        return;
    }

    if (station.mode == MODE_SET_CAL || station.mode == MODE_BTN_HOLD) { // TODO !!!!
        initModeCalibration(station);
        station.userCalibration += tick;
        
        #ifdef SERIAL_MESSAGES_INFO
            auxSerialWriteInfo("Calibration", station);
        #endif
        
        return;
    }

    if (station.mode == MODE_SEL_SENS) {
        initModeSelectSensor(station);
        selectSensor(tick, station);

        #ifdef SERIAL_MESSAGES_INFO
            auxSerialWriteInfo("Sensor", station);
        #endif

        return;
    }

    if (station.mode == MODE_INACTIVE) {
        initModeDefault(station);
        return;
    }
}
/* -------------------------------------------------------------------------- */
void selectSensor(int8_t tick, struct SolderingStation &station) {
    station.sensorIndex += tick;

    if (station.sensorIndex < 1) { // not < 0 !!
        station.sensorIndex = station.sensorCount;
    }

    if (station.sensorIndex > station.sensorCount) {
        station.sensorIndex = 1;
    }

    station.currentSensor  = station.sensors[station.sensorIndex - 1]; // -1 (it's not 0 any more)
}
/* -------------------------------------------------------------------------- */
void regulateLoop_Default(struct SolderingStation &station, Adafruit_SH1106 &display) {
    #ifdef OLED_DRAW
        // Serial.println("Default draw");
        drawDefault(station, display);
    #endif
}
/* -------------------------------------------------------------------------- */
void regulateLoop_SetTemp(struct SolderingStation &station, Adafruit_SH1106 &display) {
    regulateTimer(Station.timerTemp, station);
    #ifdef OLED_DRAW
        drawSetTemp(station, display);
    #endif
}
/* -------------------------------------------------------------------------- */
void regulateLoop_BtnHold(struct SolderingStation &station, Adafruit_SH1106 &display) {
    regulateTimerAux(Station.timerCalSens);
    #ifdef OLED_DRAW
        drawDefault(station, display);
    #endif
}
/* -------------------------------------------------------------------------- */
void regulateLoop_SetCal(struct SolderingStation &station, Adafruit_SH1106 &display) {
    regulateTimer(Station.timerCal, station);
    #ifdef OLED_DRAW
        drawCalibrateSensor(station, display);
    #endif
}
/* -------------------------------------------------------------------------- */
void regulateLoop_SelSens(struct SolderingStation &station, Adafruit_SH1106 &display) {
    regulateTimer(Station.timerSens, station);
    #ifdef OLED_DRAW
        drawSelectSensor(station, display);
    #endif
}
/* -------------------------------------------------------------------------- */
void regulateLoop_Inactive(struct SolderingStation &station, Adafruit_SH1106 &display) {
    #ifdef OLED_DRAW
        drawInactive(station, display);
    #endif
}
/* -------------------------------------------------------------------------- */

