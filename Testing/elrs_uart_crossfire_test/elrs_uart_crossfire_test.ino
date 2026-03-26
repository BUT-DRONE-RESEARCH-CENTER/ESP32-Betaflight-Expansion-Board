#include <AlfredoCRSF.h>
#include <HardwareSerial.h>


// ELRS UART Communication Pins
#define PIN_RX 16
#define PIN_TX 17

// Define input pins
const int buttonPin1 = 23;
const int buttonPin2 = 22;

// Define output pins
const int connectionLossIndicatorLedPin = 21;
const int buzzerPin = 19;

// State Variables

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
  0, // CH1 #POWER
  0, // CH2 #ROLL
  0, // CH3 #PITCH
  0, // CH4 #YAW
  0, // CH5 #ARM
  0, // CH6 #FLIGHT MODE
  1, // CH7 - 3WAY Switch (1000, 1500, 2000)
  0, // CH8
  1, // CH9 - Push Button Switch (1000, 2000)
  1, // CH10 - Toggle Switch (1000-2000)
  1, // CH11 - Scroll Wheel (1000 ... 2000)
  0, // CH12
  0, // CH13
  0, // CH14
  0, // CH15
  0 // CH16
};


/*
 ==== Beyond this point, the actuation functions for the expansion features are written ====
*/
void moveSingleServo(int rc_channel, int servo_pin) {
  uint16_t ch1 = elrsChannelValues[0];
  // Map or actuate your servo here
}

// int verifyChannelIsFree(channelIndex: uint16_t) {
//   if (freeELRSChannels[channels])
// }



void verifyCriticalFlightChannelsFree(int rc_channel) {
  // Make sure the channel index is valid
  if (rc_channel < 0 || rc_channel >= NUM_CHANNELS) {
      Serial.println("Invalid channel index!");
      return;
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
  }
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
    ===== User Input Block =====
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
  }


  /*
    ===== Output Utility Functions Block ====
  */
    // UNCOMMENT the one you want to use:
  outputSerialMonitor(elrsChannelValues); 
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