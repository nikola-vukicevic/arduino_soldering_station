/* -------------------------------------------------------------------------- */
#include <Adafruit_SH1106.h>
/* -------------------------------------------------------------------------- */
#define OLED_DRAW
// #define IRON_DEBUG
#define IRON_LIVE
// #define DRAW_COUNTER
#define DRAW_CLOCK
#define SERIAL_MESSAGES_FPS
// #define SAMPLE_SMOOTHING
// #define SERIAL_MESSAGES_DEBUG
// #define SERIAL_MESSAGES_INFO
/* -------------------------------------------------------------------------- */
#define i2C_ADDRESS         0x3C
#define OLED_RESET          -1   // QT-PY / XIAO
#define SCREEN_WIDTH        128
#define SCREEN_HEIGHT       64
#define SERIAL_RATE         115200
#define PWUP_DELAY          500
#define PIN_A               2   // Encoder A
#define PIN_B               3   // Encoder B
#define PIN_K               4   // Encoder button
#define PIN_LED             10  // LED
#define PIN_T               A0  // Temp sensor
#define MASK_PIN_K          0b00010000  // set according to value of PIN_K
#define MASK_PINS_AB        0b00001100  // set according to values of PIN_A and PIN_B
#define MASK_PIN_LED        0b00000100  // set according to value of PIN_LED
/* Modes */
#define MODE_DEFAULT        1
#define MODE_SET_TEMP       2
#define MODE_BTN_HOLD       3
#define MODE_SET_CAL        4
#define MODE_SEL_SENS       5
#define MODE_INACTIVE       6
/* Temperatures */
#define TEMP_SET_MIN        200
#define TEMP_SET_MAX        420
#define TEMP_DEFAULT        250
#define TEMP_INACTIVE       200
#define TEMP_NO_TIP         435
#define TEMP_DISPLAY_LIMIT  999
#define TEMP_TOLERANCE_UP   5
#define TEMP_TOLERANCE_DOWN 5
#define TEMP_READ_RATE      100 // milliseconds
#define SENSOR_TEMP_REF     330
/* Timers */
#define TIMER_SET_T_DEF     600    // Default time (ms) for exiting set temp mode
#define TIMER_SET_C_DEF     2400   // Default time (ms) for exiting calibration mode
#define TIMER_SEL_S_DEF     2400   // Default time (ms) for exiting select sensor mode
#define TIMER_SENS_DEF      1200   // Default time (ms) for entering select sensor mode
#define TIMER_INACTIVE_DEF  300000 // Default time (ms) for entering inactive mode
/* Display */
#define COORD_X_TEMP        48
#define COORD_Y_TEMP        31
#define COORD_X_LED         15
#define COORD_Y_LED         45
/* Smoothing */
#ifdef SAMPLE_SMOOTHING
    #define SENSOR_COUNT   2
    #define TEMP_SAMP_AVG  6
# else
    #define SENSOR_COUNT   3
#endif
/* -------------------------------------------------------------------------- */
struct Timer {
    unsigned long time;
    unsigned long currentMillis, previousMillis;
    boolean trigger;
};
/* -------------------------------------------------------------------------- */
struct SensorInfo {
    float   tempRef;
    float   slopeLow;
    float   slopeHigh;
    int16_t adcRef;
    int16_t calibration;
    char    name[6];
};
/* -------------------------------------------------------------------------- */
struct StationClock {
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
};
/* -------------------------------------------------------------------------- */
#ifdef SAMPLE_SMOOTHING
struct TempSampleNode {
	struct TempSampleNode *prev;
	struct TempSampleNode *next;
	uint16_t val;
};
#endif
/* -------------------------------------------------------------------------- */
#ifdef SAMPLE_SMOOTHING
struct StationQueue {
	struct TempSampleNode *nodes;
	struct TempSampleNode *first;
	struct TempSampleNode *last;
	int size;
	unsigned long long sum;
};
#endif
/* -------------------------------------------------------------------------- */
struct SolderingStation {
    uint8_t  turnReadout;
    uint16_t tempSet;
    uint16_t tempSetSave;
    uint16_t tempSensor;
    uint8_t  acc;
    uint8_t  click;
    uint8_t  previousClick;
    uint8_t  isHeaterOn;
    uint8_t  isLedOn;
    uint16_t tempMeasured;
    uint16_t previousTempMeasured;
    uint16_t tempDisplay;
    uint16_t previousTempDisplay;
    int8_t   tempTrend;
    uint8_t  tempToleranceUp;
    uint8_t  tempToleranceDown;
    uint8_t  sensorIndex;
    uint8_t  sensorCount;
    int8_t   userCalibration;
    uint8_t  userCalibrationStep;
    uint8_t  mode; // 1 - default
                   // 2 - set temperature
                   // 3 - calibration
                   // 4 - select sensor
                   // 5 - inactive
    uint8_t previousMode;
    unsigned long initMillis;
    struct StationClock clock;
    struct SensorInfo *currentSensor;
    struct SensorInfo *sensors[SENSOR_COUNT];
    #ifdef SAMPLE_SMOOTHING
    struct StationQueue queue;
    #endif
    struct Timer timerTemp, timerCalSens, timerCal, timerSens, timerInactive, timerRegulate; // , timerClock;
};
/* -------------------------------------------------------------------------- */
