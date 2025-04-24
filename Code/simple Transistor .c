const int buttonPin = 6;
const int ledPin = 4;
int ledState = LOW;
int lastButtonState = HIGH;

void setup() {
  pinMode(ledPin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);
  digitalWrite(ledPin, ledState);
}

void loop() {
  int currentButtonState = digitalRead(buttonPin);
  
  if (lastButtonState == HIGH && currentButtonState == LOW) {
    // Toggle using explicit if-else
    if (ledState == LOW) {
      ledState = HIGH;
    } else {
      ledState = LOW;
    }
    
    digitalWrite(ledPin, ledState);
    delay(200); // Debounce delay
  }
  
}
