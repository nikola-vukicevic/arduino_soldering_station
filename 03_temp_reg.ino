/* -------------------------------------------------------------------------- */
void regulateTemperature(struct SolderingStation &station) {
    getTempReadout(station);
    getTempTrend(station);
    calculateIronAndLed(station);
    regulateIron(station);
}
/* -------------------------------------------------------------------------- */
void getTempReadout(struct SolderingStation &station) {
    station.timerRegulate.currentMillis = millis();

    boolean tick = station.timerRegulate.currentMillis -
		           station.timerRegulate.previousMillis >=
				   station.timerRegulate.time;

    if (tick == false) return;

    // Serial.println("Read sensor");
    station.previousTempMeasured = station.tempMeasured;
    station.tempSensor           = analogRead(PIN_T);
    station.tempMeasured         = mapSensToTemp(station.tempSensor, station.currentSensor);
    station.tempMeasured        += station.userCalibration; // * station.userCalibrationStep;
	// (int) mapSensToTemp() ?

    if (station.tempMeasured < 0)                  station.tempMeasured = 0;
    if (station.tempMeasured > TEMP_DISPLAY_LIMIT) station.tempMeasured = 0;

    if (abs(station.tempMeasured - station.previousTempMeasured) >= TEMP_DIFF_BEEP) {
        beep(station);
    }

    #ifdef SAMPLE_SMOOTHING
        sampleSmoothingQueue(station);
    #else
        sampleSmoothingSimple(station);
    #endif
    // station.tempDisplay = station.tempMeasured;
    
    station.previousTempDisplay          = station.tempDisplay;
    station.timerRegulate.previousMillis = station.timerRegulate.currentMillis;
}
/* -------------------------------------------------------------------------- */
// boolean regulateTemperatureActiveModes(struct SolderingStation &station) {
    // station.tempMeasured = map(Station.tempSensor, 0, 1023, 200, 420);
    // struct SensorInfo *s = station.currentSensor;
    // station.tempMeasured = map(Station.tempSensor, s->ardVal1, s->ardVal2, s->t1, s->t2);
    // station.tempMeasured = map(Station.tempSensor, station.currentSensor->ardVal1, station.currentSensor->ardVal2, station.currentSensor->t1, station.currentSensor->t2);
// }
/* -------------------------------------------------------------------------- */
#ifdef SAMPLE_SMOOTHING

void sampleSmoothingQueue(struct SolderingStation &station) {
    // station.queue.sum -= station.queue.last->val;
    addQueueNode(&station.queue, station.tempMeasured);
    station.tempDisplay = station.queue.sum / station.queue.size;
}
#else

void sampleSmoothingSimple(struct SolderingStation &station) {
    station.tempDisplay += station.tempMeasured;
    station.tempDisplay /= 2;
}

#endif
/* -------------------------------------------------------------------------- */
void calculateIronAndLed(struct SolderingStation &station) {
    #ifdef IRON_DEBUG

    if (station.tempMeasured <= station.tempSet) {
        station.isHeaterOn = 1;
        station.isLedOn    = 1;
    } else {
        station.isHeaterOn = 0;
        station.isLedOn    = 0;
    }
    
    #endif

    #ifdef IRON_LIVE

    if (station.tempTrend > 0) {
        if (station.tempMeasured <= station.tempSet + station.tempToleranceUp) {
            station.isHeaterOn = 1;
            station.isLedOn    = 1;
        } else {
            station.isHeaterOn = 0;
            station.isLedOn    = 0;
        }
    } else {
        if (station.tempMeasured > station.tempSet - station.tempToleranceDown) {
            station.isHeaterOn = 0;
            station.isLedOn    = 0;
            return;
        } else {
            station.isHeaterOn = 1;
            station.isLedOn    = 1;
            return;
        }
    }

    #endif
}
/* -------------------------------------------------------------------------- */
void regulateIron(struct SolderingStation &station) {
    if (station.isHeaterOn) {
        // digitalWrite(PIN_IRON, HIGH);
        PORTB = PORTB | MASK_PIN_IRON;
    }
    else {
        // digitalWrite(PIN_IRON, LOW);
        PORTB = PORTB & ~MASK_PIN_IRON;
    }
}
/* -------------------------------------------------------------------------- */
int limitTempSet(int position, int limit_1, int limit_2) {
    if (position < limit_1) {
        return limit_1;
    }

    if (position > limit_2) {
        return limit_2;
    }

    return position;
}
/* -------------------------------------------------------------------------- */
int mapSensToTemp(float sensVal, struct SensorInfo *sensor) {
    if (sensVal <= sensor->adcRef) {
        return (int) (sensor->tempRef + (sensVal - sensor->adcRef) * sensor->slopeLow  + sensor->calibration);
    } else {
        return (int) (sensor->tempRef + (sensVal - sensor->adcRef) * sensor->slopeHigh + sensor->calibration);
    }
}
/* -------------------------------------------------------------------------- */
void getTempTrend(struct SolderingStation &station) {
    if (station.tempMeasured > station.previousTempMeasured) {
        station.tempTrend = 1;
        return;
    }

    if (station.tempMeasured < station.previousTempMeasured) {
        station.tempTrend = -1;
        return;
    }

    station.tempTrend = 0;
}
/* -------------------------------------------------------------------------- */
