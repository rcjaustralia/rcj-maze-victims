// RCJA RESCUE MAZE VICTIM FIRMWARE
// Version: $VERSION$ (2017-08-11)
// Author: Tristan Roberts <tristan_roberts [at] icloud.com>

#define VERSION 1

/*** START CONFIG ***/

/* The number of devices connected to this Arduino (two Arduinos per unit) */
#define TOTAL_DEVICES 6

/* The number of data points to average over */
#define AVERAGE_POINTS 10

/* The desired set point temperature (in degrees C) */
#define TEMPERATURE 42

/* The LED pin to use to indicate overall status */
#define LED 13

/* The frequency, as an integer that divides 1000, in Hz */
#define FREQUENCY 4

/* Uncomment this line to enable debug, or comment out to disable debug */
#define DEBUG 1

/* The baud rate to use for debug Serial */
#define BAUD_RATE 38400

/* Control pin OFF state, either HIGH or LOW depending on if inverted */
#define OFF LOW

/* Control pin ON state, opposite of OFF, either LOW or HIGH depending on if inverted */
#define ON HIGH

/* The pins used for temperature sensors, in order of device */
byte TEMP_PINS[TOTAL_DEVICES] = {A7, A6, A5, A4, A3, A2};

/* The digital pins used to control the devices, in order of device */
byte CONTROL_PINS[TOTAL_DEVICES] = {2, 3, 4, 5, 6, 7};

/*** END CONFIG ***/


#ifdef DEBUG
boolean has_waited;
#endif

byte index;
byte temperatures[TOTAL_DEVICES][AVERAGE_POINTS];
byte heating_devices;
float reading;
unsigned long loop_started;
unsigned long current_time;

void setup() {
    index = 0;
  
    #ifdef DEBUG
    Serial.begin(BAUD_RATE);

    Serial.println(" ");
    Serial.println(" ");

    Serial.print("HEATPAD FIRMWARE VERSION ");
    Serial.println(VERSION, DEC);

    Serial.print("TOTAL_DEVICES = ");
    Serial.println(TOTAL_DEVICES, DEC);

    Serial.print("AVERAGE_POINTS = ");
    Serial.println(AVERAGE_POINTS, DEC);

    Serial.print("TEMPERATURE = ");
    Serial.println(TEMPERATURE, DEC);

    Serial.print("LED = ");
    Serial.println(LED, DEC);

    Serial.print("FREQUENCY = ");
    Serial.println(FREQUENCY, DEC);

    Serial.print("ON = ");
    Serial.println(ON == LOW ? "LOW" : "HIGH");

    Serial.print("OFF = ");
    Serial.println(OFF == LOW ? "LOW" : "HIGH");
    #endif

    if (ON == OFF) {
        for(;;) {
            #ifdef DEBUG
            Serial.println("ERROR: OFF == ON state");
            #endif
            delay(2000);
        }
    }

    #ifdef DEBUG
    Serial.println(" ");
    #endif
    
    for (byte i = 0;i < TOTAL_DEVICES;i++) {
        #ifdef DEBUG
        Serial.print("Configuring device #");
        Serial.println(i, DEC);
        #endif

        pinMode(TEMP_PINS[i], INPUT);
        pinMode(CONTROL_PINS[i], OUTPUT);
        digitalWrite(CONTROL_PINS[i], OFF);
        for (byte j = 0;j < AVERAGE_POINTS;j++) {
            temperatures[i][j] = 0;
        }
    }
}

void loop() {
    heating_devices = 0;
    loop_started = millis();

    #ifdef DEBUG
    Serial.println(" ");
    Serial.print("Index = ");
    Serial.println(index, DEC);
    #endif
    
    for (byte device = 0;device < TOTAL_DEVICES;device++) {
        reading = analogRead(TEMP_PINS[device]) * 500.0 / 1024.0;
        temperatures[device][index] = (byte) reading;

        #ifdef DEBUG
        Serial.print("Device #");
        Serial.print(device, DEC);
        Serial.print(" temperature = ");
        Serial.println(temperatures[device][index], DEC);
        #endif

        reading = 0;
        for (byte i = 0;i < AVERAGE_POINTS;i++) {
            reading += temperatures[device][i];
        }
        reading = reading / AVERAGE_POINTS;

        #ifdef DEBUG
        Serial.print("    Average = ");
        Serial.println(reading, DEC);
        #endif

        if (reading <= TEMPERATURE) {
            digitalWrite(CONTROL_PINS[device], ON);
            heating_devices++;

            #ifdef DEBUG
            Serial.println("    State = ON");
            #endif
        } else {
            digitalWrite(CONTROL_PINS[device], OFF);

            #ifdef DEBUG
            Serial.println("    State = OFF");
            #endif
        }
    }

    if (heating_devices > 0) { //(TOTAL_DEVICES / 2)) {
        digitalWrite(LED, HIGH);

        #ifdef DEBUG
        Serial.println("LED = ON");
        #endif
    } else {
        digitalWrite(LED, LOW);

        #ifdef DEBUG
        Serial.println("LED = OFF");
        #endif
    }

    index = (index + 1) % AVERAGE_POINTS;

    #ifdef DEBUG
    has_waited = false;
    #endif
    
    for(;;) {
        current_time = millis();

        if (current_time < loop_started) {
            return;
        } else if (current_time - loop_started >= (1000 / FREQUENCY)) {
            return;
        }

        #ifdef DEBUG
        if (!has_waited) {
            Serial.println("Waiting...");
            has_waited = true;
        }
        #endif
    }
}

