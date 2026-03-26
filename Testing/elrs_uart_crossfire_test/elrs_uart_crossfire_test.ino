#include <AlfredoCRSF.h>
#include <HardwareSerial.h>
#include <ESP32Servo.h>

// ELRS UART Communication Pins
#define PIN_RX 16
#define PIN_TX 17

// Define system input pins
const int buttonPin1 = 23;
const int buttonPin2 = 22;

// Define system output pins
const int connectionLossIndicatorLedPin = 21;
const int buzzerPin = 19;

// ===========================================
// Define actuator output pins
const int actuatorPin_power1 = 18;
const int actuatorPin_power2 = 15;


Servo servo1;
const int actuatorPin_servo1 = 5;
Servo servo2;
const int actuatorPin_servo2 = 4;
Servo servo3;
const int actuatorPin_servo3 = 2;
// ===========================================
// Define crossfire communication serial channel;
HardwareSerial crsfSerial(2); 
AlfredoCRSF crsf;

// Number of channels
const int NUM_CHANNELS = 16;

// Array to hold all channel values
uint16_t elrsChannelValues[NUM_CHANNELS];

// Array to define free channels, 
// so channels used for flight aren't accidentally assigned expansion roles.
// 0 means flight critical, 1 means free channel
uint16_t freeELRSChannels[NUM_CHANNELS] = {
  0, // [0] CH1 #POWER
  0, // [1] CH2 #ROLL
  0, // [2] CH3 #PITCH
  0, // [3] CH4 #YAW
  0, // [4] CH5 #ARM
  0, // [5] CH6 #FLIGHT MODE
  1, // [6] CH7 - 3WAY Switch (1000, 1500, 2000)
  0, // [7] CH8
  1, // [8] CH9 - Push Button Switch (1000, 2000)
  1, // [9] CH10 - Toggle Switch (1000-2000)
  1, // [10] CH11 - Scroll Wheel (1000 ... 2000)
  0, // [11] CH12
  0, // [12] CH13
  0, // [13] CH14
  0, // [14] CH15
  0  // [15] CH16
};


/*
 ==== Beyond this point, the actuation functions for the expansion features are written ====
*/
void actuator_scroll_moveSingleServo(int rc_channel) {
  if (verifyCriticalFlightChannelsFree(rc_channel)) {
    uint16_t servo_channel_value = elrsChannelValues[rc_channel];

    // Map 1000–2000 -> 0–180 degrees
    int angle = map(servo_channel_value, 1000, 2000, 0, 180);

    // Constrain just in case (safety)
    angle = constrain(angle, 0, 180);

    Serial.println(servo_channel_value);
    Serial.println(angle);
    servo2.write(angle);
  }
}

void actuator_3Way_servoToggler(int servo_selector_rc_channel, int actuator_rc_channel) {
  if (verifyCriticalFlightChannelsFree(servo_selector_rc_channel) &&
      verifyCriticalFlightChannelsFree(actuator_rc_channel)) {

    uint16_t selector_val = elrsChannelValues[servo_selector_rc_channel];
    uint16_t trigger_val  = elrsChannelValues[actuator_rc_channel];

    // Only run if trigger channel is HIGH
    if (trigger_val > 1800) {

      if (selector_val <= 1200) {
        servo1.write(180);
        delay(500);
        servo1.write(0);
        delay(500);
      }
      else if (selector_val >= 1300 && selector_val <= 1700) {
        servo2.write(180);
        delay(500);
        servo2.write(0);
        delay(500);
      }
      else if (selector_val >= 1800 && selector_val <= 2200) {
        servo3.write(180);
        delay(500);
        servo3.write(0);
        delay(500);
      }

    }
  }
}

void actuator_turnPowerOn(int rc_channel, int actuator_pin) {
  
  if (verifyCriticalFlightChannelsFree(rc_channel)) {
    uint16_t led_channel_value = elrsChannelValues[rc_channel];
    if (led_channel_value > 1900) {
      digitalWrite(actuator_pin, HIGH);
    } else {
      digitalWrite(actuator_pin, LOW);
    }
  }

}



int verifyCriticalFlightChannelsFree(int rc_channel) {
  // Make sure the channel index is valid
  if (rc_channel < 0 || rc_channel >= NUM_CHANNELS) {
      Serial.println("Invalid channel index!");
      return 1;
  }

  // Check if the channel is critical (0)
  if (freeELRSChannels[rc_channel] == 0) {
    // Channel is critical – put your code here
    Serial.print("Channel ");
    Serial.print(rc_channel + 1);
    Serial.println(" is flight-critical! Action required.");
    
    digitalWrite(buzzerPin, HIGH);
    delay(100);
    digitalWrite(buzzerPin, LOW);
    delay(100);
    return 0;
  }

  return 1;
}


void setup() {

  // Initiate serial communication with the programmer computer;
  Serial.begin(115200);

  // Begin listening to the crossfire radio channel values received by the ELRS module;
  crsfSerial.begin(CRSF_BAUDRATE, SERIAL_8N1, PIN_RX, PIN_TX);
  crsf.begin(crsfSerial);

  // Define the pinMode for GPIO pins;
  pinMode(buttonPin1, INPUT_PULLUP);
  pinMode(buttonPin2, INPUT_PULLUP);
  pinMode(connectionLossIndicatorLedPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);

  // Define the pinMode for actuatorPins;
  pinMode(actuatorPin_power1, OUTPUT);
  pinMode(actuatorPin_power2, OUTPUT);

  servo1.setPeriodHertz(50); // standard servo frequency
  servo1.attach(actuatorPin_servo1, 500, 2400); // min/max pulse width in µs
  servo2.setPeriodHertz(50);
  servo2.attach(actuatorPin_servo2, 500, 2400);
  servo3.setPeriodHertz(50);
  servo3.attach(actuatorPin_servo3, 500, 2400);
  
}


void connectionLostLEDAlert() {
  digitalWrite(connectionLossIndicatorLedPin, HIGH);
  delay(50);
  digitalWrite(connectionLossIndicatorLedPin, LOW);
  delay(100);
}


void loop() {


  /*
    ===== Crossfire ELRS Communication Block =====
  */
  crsf.update();
  if (crsf.isLinkUp()) {
    // Fill array with current channel values
    for (int i = 0; i < NUM_CHANNELS; i++) {
      elrsChannelValues[i] = crsf.getChannel(i + 1); // CRSF channels are 1-indexed
    }
    // delay(10);
  } else {
    // if ELRS connection is not initiated, it beeps the buzzer,
    // so the user can know there is a connection problem;
    connectionLostLEDAlert();
  }

  /*
    ===== Actuator Function Calling Block =====
  */
  actuator_turnPowerOn(8, actuatorPin_power1);
  actuator_scroll_moveSingleServo(10);
  actuator_3Way_servoToggler(6, 8);
  actuator_turnPowerOn(9, actuatorPin_power2);

  /*
    ===== Button Input Block =====
  */
  // Read the buttons (LOW when pressed due to PULL-UP)
  bool button1Pressed = digitalRead(buttonPin1) == LOW;
  bool button2Pressed = digitalRead(buttonPin2) == LOW;

  if (button1Pressed) {
    Serial.println("Button 1 pressed!");
    connectionLostLEDAlert();
  }
  if (button2Pressed) {
    Serial.println("Button 2 pressed!");
    digitalWrite(buzzerPin, HIGH);
    delay(100);
    digitalWrite(buzzerPin, LOW);
    delay(100);
  }


  /*
    ===== Output Utility Functions Block ====
  */
  // outputSerialMonitor(elrsChannelValues); 
  // outputSerialPlotter(elrsChannelValues); 
  // runActuatorFunctions(elrsChannelValues);



}
// FORMAT: CH1: 1500 | CH2: 1500 ...
void outputSerialMonitor(uint16_t* channels) {
  for (int i = 0; i < NUM_CHANNELS; i++) {
    Serial.print("CH");
    Serial.print(i + 1);
    Serial.print(": ");
    Serial.print(channels[i]);
    if (i < NUM_CHANNELS - 1) Serial.print(" | ");
  }
  Serial.println();
}

// FORMAT: 1500 1500 1500 ... (Standard for Arduino Plotter)
void outputSerialPlotter(uint16_t* channels) {
  for (int i = 0; i < NUM_CHANNELS; i++) {
    Serial.print(channels[i]);
    if (i < NUM_CHANNELS - 1) Serial.print(" ");
  }
  Serial.println();
}

// Example actuator function using the array
void runActuatorFunctions(uint16_t* channels) {
  // Just printing for now; later replace with actual actuation logic
  for (int i = 0; i < NUM_CHANNELS; i++) {
    Serial.print("CH");
    Serial.print(i + 1);
    Serial.print(": ");
    Serial.print(channels[i]);
    if (i < NUM_CHANNELS - 1) Serial.print(" | ");
  }
  Serial.println();
}