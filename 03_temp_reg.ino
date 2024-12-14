/* -------------------------------------------------------------------------- */
void regulateTemperature(struct SolderingStation &station) {
    if (station.mode < MODE_INACTIVE) {
        regulateTemperatureActiveModes(station);
    }
    else {
        regulateTemperatureInactive(station);
    }

    getTempTrend(station);
    regulateIron(station);
}
/* -------------------------------------------------------------------------- */
void regulateTemperatureActiveModes(struct SolderingStation &station) {
    station.timerRegulate.currentMillis = millis();

    boolean tick = station.timerRegulate.currentMillis -
		           station.timerRegulate.previousMillis >=
				   station.timerRegulate.time;

    if (tick == false) return;

    station.tempSensor    = analogRead(PIN_T);
    station.tempMeasured  = mapSensToTemp(station.tempSensor, station.currentSensor);
    station.tempMeasured += station.userCalibration * station.userCalibrationStep;

    #ifdef SAMPLE_SMOOTHING
        sampleSmoothingQueue(station);
    #else
        sampleSmoothingSimple(station);
    #endif
    
    station.previousTempDisplay          = station.tempDisplay;
    station.previousTempMeasured         = station.tempMeasured;
    station.timerRegulate.previousMillis = station.timerRegulate.currentMillis;
}
/* -------------------------------------------------------------------------- */
void regulateTemperatureInactive(struct SolderingStation &station) {
    station.tempMeasured = TEMP_INACTIVE;
    station.tempDisplay  = TEMP_INACTIVE;
}
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
void regulateIron(struct SolderingStation &station) {
    #ifdef IRON_DEBUG
        if (station.tempMeasured < station.tempSet) {
            // digitalWrite(PIN_LED, HIGH);
            PORTB = PORTB | MASK_PIN_LED;
        }
        else {
            // digitalWrite(PIN_LED, LOW);
            PORTB = PORTB & ~MASK_PIN_LED;
        }
    #endif

    #ifdef IRON_LIVE
    if (station.tempTrend > 0 && station.tempMeasured > station.tempSet + station.tempToleranceUp) {
        // digitalWrite(PIN_LED, HIGH);
        PORTB = PORTB | MASK_PIN_LED;
        return;
    }

    if (station.tempTrend < 0 && station.tempMeasured < station.tempSet - station.tempToleranceDown) {
        // digitalWrite(PIN_LED, LOW);
        PORTB = PORTB & ~MASK_PIN_LED;
        return;
    }
    #endif
}
/* -------------------------------------------------------------------------- */
int limitSetTemp(int position, int limit_1, int limit_2) {
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
    return (int) (sensor->tempRef + (sensVal - sensor->adcRef) * sensor->slope + sensor->calibration);
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

