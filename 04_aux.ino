/* -------------------------------------------------------------------------- */
void getFps(unsigned long &counter, unsigned long &previousMilliseconds, unsigned g) {
    #ifdef SERIAL_MESSAGES_FPS
    unsigned milliseconds = millis();
    counter++;
    
    if (milliseconds - previousMilliseconds >= g) {
        // Serial.print("(");
        // Serial.print(station.mode);
        // Serial.print(") ");
        Serial.print(counter);
        Serial.println(" fps");
        previousMilliseconds = milliseconds;
        counter              = 0;
    }
    #endif
}
/* -------------------------------------------------------------------------- */
void regulateClock(struct SolderingStation &station) {
    int t  = (millis() - station.initMillis) / 1000;
    
    station.clock.hour   = t / 3600;
    station.clock.minute = (t % 3600) / 60;
    station.clock.second = t % 60;
}
/* -------------------------------------------------------------------------- */
