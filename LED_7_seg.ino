#define MILLISEC_IN_MIN 600

// HC595 Pins
byte latchPin = 7;
byte clockPin = 6;
byte dataPin = 5;

// Button pins
byte stopPin = 2;
byte resetPin = 3;
byte selectPin = 12;
byte increasePin = 13;

// LED pins
byte digitPin[4] = {11, 10, 9, 8};

// Button state
byte stopButtonState = 0;
byte resetButtonState = 0;

// Running state
byte stopped = 0;

byte digitNum[4] = {0, 0, 0, 0};
byte minute = 0;
// Bit decode for 7 segments LED
byte numbers[10] = {
  //abcdefg
  0b11111100, // 0
  0b01100000, // 1
  0b11011010, // 2
  0b11110010, // 3
  0b01100110, // 4
  0b10110110, // 5
  0b10111110, // 6
  0b11100000, // 7
  0b11111110, // 8
  0b11110110, // 9
};

unsigned long buttonTime;
unsigned long increaseTime;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  pinMode(stopPin, INPUT);
  pinMode(resetPin, INPUT);
  pinMode(selectPin, INPUT);
  pinMode(increasePin, INPUT);

  for (int i = 0; i < 4; i++) {
    pinMode(digitPin[i], OUTPUT);
    digitalWrite(digitPin[i], HIGH);
  }

  attachInterrupt(0, stop, RISING);
  buttonTime = millis();
  increaseTime = millis();
}

void display() {
    int hour = minute / 60;
    int mins = minute % 60;
    digitNum[0] = mins % 10;
    digitNum[1] = mins / 10;
    digitNum[2] = hour % 10;
    digitNum[3] = hour / 10;

  for (int i = 0; i < 4; i++) {
    digitalWrite(latchPin, LOW);
    if (i == 2) {
        shiftOut(dataPin, clockPin, LSBFIRST, numbers[digitNum[i]] | 0b1);
    } else {
        shiftOut(dataPin, clockPin, LSBFIRST, numbers[digitNum[i]]);
    }
    digitalWrite(latchPin, HIGH);
    digitalWrite(digitPin[i], LOW);
    delay(1);
    digitalWrite(digitPin[i], HIGH);
  }
}

void stop(){
    stopped = ~stopped;
    /* byte buttonState = digitalRead(stopPin);
    if (buttonState != stopButtonState) {
        if (buttonState == 1) {
            stopped = ~stopped;
        }
        stopButtonState = stopButtonState;
    } */
}

void reset() {
    minute = 0;
}

void increase() {
    minute++;
    if (minute == 1440) {
        minute = 0;
    }
}

void loop() {
  // put your main code here, to run repeatedly:
  long currTime = millis();
  if (currTime - buttonTime > 50) {
      int buttonState = digitalRead(resetPin);
      Serial.println(buttonState);
      if (buttonState != resetButtonState) {
          if (buttonState == 1) {
              minute = 0;
              increaseTime = millis();
          }
          resetButtonState = buttonState;
      }
      buttonTime = millis();
  }

  if (currTime - increaseTime > MILLISEC_IN_MIN && !stopped) {
    increase();
    increaseTime = millis();
  }
  display();
}
