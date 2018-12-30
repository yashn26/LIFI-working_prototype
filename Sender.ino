int ledState = LOW;// ledState used to set the LED
const int clkPin =  13;// the number of the LED pin for clock
const int dataPin = 11;//The number of the LED pin for data
const long interval = 100;// interval at which to blink (milliseconds)
unsigned long previousMillis = 0;        // will store last time LED was updated
volatile char dataword; //The single character
volatile int i = 7; //index for the dataword 8bits
volatile int j = 0;

bool readserial() {
  if (Serial.available()) { //constructing the data frame only if serial is ready
    dataword = Serial.read();
     
    return true;
  }
  else {
    return false;
  }
}

void moddata() { //modulate the data HIGH = 1 , LOW = 0
  if (bitRead(dataword, i) == 1) {
    digitalWrite(dataPin, HIGH);
    i--;
  }
  else if (bitRead(dataword, i) == 0) {
    digitalWrite(dataPin, LOW);
    i--;
  }
  if (i == -1) {
    i = 7;
  }
}

void setup() {
  // set the digital pin as output:
  pinMode(clkPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  Serial.begin (115200);
  Serial.write("Sender Ready For Input...\n");
}



void loop() {
  // check to see if it's time to blink the LED; that is, if the difference
  // between the current time and last time you blinked the LED is bigger than
  // the interval at which you want to blink the LED.
  j = 0;
  bool serialready = readserial();
  if (serialready) { //start the clock and hence transmission only if data is ready
    while (j < 8) { //send the byte(8bits) then reset for next byte

      unsigned long currentMillis = millis();
      if (currentMillis - previousMillis >= interval) {
        previousMillis = currentMillis; // save the last time you blinked the LED
        if (ledState == LOW) {
          ledState = HIGH;
          digitalWrite(clkPin, HIGH);
          moddata();
          j++;
          
        }
        else {
          digitalWrite(clkPin, LOW);
          ledState = LOW;
          moddata();
          j++;
        }
        
      }
    }
    Serial.write("Sending - ");
        Serial.println(dataword);
  }
}
