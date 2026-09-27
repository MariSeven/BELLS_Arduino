const int btn1Pin = 3;
const int btn2Pin = 4;
const int relayPin = 2;

const int BAUD_RATE = 9600;

unsigned long relayOnTime = 0;
bool relayActive = false;
const unsigned long BELL_DURATION = 3000;
String inputString = "";

int mode = 0;
unsigned long lastToggleTime = 0;
bool pulseState = false;
//защита от дребезга физической модели
int lastBtn1State = HIGH;
int lastBtn2State = HIGH;
void setup() {
    pinMode(relayPin, OUTPUT); 
    pinMode(btn1Pin, INPUT_PULLUP);
    pinMode(btn2Pin, INPUT_PULLUP);
    digitalWrite(relayPin, LOW); 
    Serial.begin(BAUD_RATE);
    //Сообщение, выводимое в терминал на этапе тестирования  
    Serial.println("Send '1' to activate or 'PING' to connection check\nBLACK BUTTON - barricade\nRED BUTTON - evacuation"); 
    }

void loop() {
    int currentBtn1 = digitalRead(btn1Pin);
    if (currentBtn1 == LOW && lastBtn1State == HIGH) {
        delay(50); //защита от дребезга контактов физической модели
        if (digitalRead(btn1Pin) == LOW) {
            if (mode == 1) {
                mode = 0;
                digitalWrite(relayPin, LOW);  
                } else {
                mode = 1;
            }
        }
    }
    lastBtn1State = currentBtn1;

    int currentBtn2 = digitalRead(btn2Pin);
    if (currentBtn2 == LOW && lastBtn2State == HIGH) {
        delay(50); //защита от дребезга контактов физической модели
        if (digitalRead(btn2Pin) == LOW) {
            if (mode == 2) {
                mode = 0;
                digitalWrite(relayPin, LOW);
                } else {
                mode = 2;
            }
        }
    }
    lastBtn2State = currentBtn2;
    //Режим чтения сигналов на COM-порт
    if (Serial.available() > 0) {
        char inChar = (char)Serial.read();
        if (inChar == '\n') {
            inputString.trim();
            if (inputString == "PING") {
                Serial.println("PONG");
            } 
            else if (inputString == "1" && mode == 0) {
                digitalWrite(relayPin, HIGH);
                relayOnTime = millis();
                relayActive = true;
                Serial.println("Relay ON");
            }
            
            inputString = "";
        } else {
            inputString += inChar;
        }
    }
    if (mode == 1) {
        // ТРЕВОГА 1 (баррикадироваться)
        if (millis() - lastToggleTime >= 700) {
            pulseState = !pulseState;
            digitalWrite(relayPin, pulseState ? HIGH : LOW);
            lastToggleTime = millis();
        }
    } 
    else if (mode == 2) {
        // ТРЕВОГА 2 (эвакуироваться)
        digitalWrite(relayPin, HIGH);
    } 
    else {
        if (relayActive && (millis() - relayOnTime >= BELL_DURATION)) {
            digitalWrite(relayPin, LOW);
            relayActive = false;
            Serial.println("Relay OFF");
        }
    }
}
