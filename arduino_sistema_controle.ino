// Sistema Inteligente de Controle com Arduino UNO - DIO Challenge
// Versão 1.0 - Janeiro 2026
// Projeto: Controle de LEDs com PWM, sensor de temperatura e botões

// HARDWARE:
// - 3x LEDs com resistores 220 ohm nos pinos 3, 5, 6 (PWM)
// - 2x Botões: Pino 7 (Modo), Pino 8 (Controle)
// - Sensor LM35: Pino A0

// Constantes
const int LED_PIN_1 = 3, LED_PIN_2 = 5, LED_PIN_3 = 6;
const int BUTTON_MODE = 7, BUTTON_CONTROL = 8;
const int TEMP_SENSOR = A0;
const unsigned long DEBOUNCE_DELAY = 50;
const float TEMP_MIN = 15.0, TEMP_MAX = 35.0;

// Variáveis globais
boolean isAutoMode = true;
float currentTemperature = 0.0;
int manualBrightness = 128;
unsigned long lastButtonModeTime = 0, lastButtonControlTime = 0, lastSerialTime = 0;
boolean isButtonModePressed = false, isButtonControlPressed = false;

void setup() {
  Serial.begin(9600);
  pinMode(LED_PIN_1, OUTPUT);
  pinMode(LED_PIN_2, OUTPUT);
  pinMode(LED_PIN_3, OUTPUT);
  pinMode(BUTTON_MODE, INPUT_PULLUP);
  pinMode(BUTTON_CONTROL, INPUT_PULLUP);
  pinMode(TEMP_SENSOR, INPUT);
  
  Serial.println("=== SISTEMA INTELIGENTE - INICIALIZADO ===");
  testLEDs();
}

void loop() {
  readButtons();
  readTemperature();
  
  if (isAutoMode) {
    controlLEDsAutomatic();
  } else {
    controlLEDsManual();
  }
  
  if (millis() - lastSerialTime >= 1000) {
    sendSerialData();
    lastSerialTime = millis();
  }
  
  delay(100);
}

void readButtons() {
  unsigned long currentTime = millis();
  
  if (digitalRead(BUTTON_MODE) == LOW) {
    if (!isButtonModePressed && (currentTime - lastButtonModeTime) >= DEBOUNCE_DELAY) {
      isButtonModePressed = true;
      lastButtonModeTime = currentTime;
      toggleMode();
    }
  } else {
    isButtonModePressed = false;
  }
  
  if (digitalRead(BUTTON_CONTROL) == LOW) {
    if (!isButtonControlPressed && (currentTime - lastButtonControlTime) >= DEBOUNCE_DELAY) {
      isButtonControlPressed = true;
      lastButtonControlTime = currentTime;
      if (!isAutoMode) incrementBrightness();
    }
  } else {
    isButtonControlPressed = false;
  }
}

void readTemperature() {
  int rawValue = analogRead(TEMP_SENSOR);
  float voltage = (rawValue / 1024.0) * 5.0;
  currentTemperature = voltage / 0.01;
}

void controlLEDsAutomatic() {
  int brightness = 0;
  
  if (currentTemperature < TEMP_MIN) {
    brightness = map(currentTemperature, 0, TEMP_MIN, 0, 85);
    analogWrite(LED_PIN_1, brightness);
    analogWrite(LED_PIN_2, 0);
    analogWrite(LED_PIN_3, 255 - brightness);
  } 
  else if (currentTemperature >= TEMP_MIN && currentTemperature <= TEMP_MAX) {
    brightness = map(currentTemperature, TEMP_MIN, TEMP_MAX, 100, 255);
    analogWrite(LED_PIN_1, 0);
    analogWrite(LED_PIN_2, brightness);
    analogWrite(LED_PIN_3, 0);
  } 
  else if (currentTemperature > TEMP_MAX) {
    brightness = map(currentTemperature, TEMP_MAX, 50, 100, 255);
    analogWrite(LED_PIN_1, brightness);
    analogWrite(LED_PIN_2, 0);
    analogWrite(LED_PIN_3, 0);
  }
}

void controlLEDsManual() {
  analogWrite(LED_PIN_1, manualBrightness);
  analogWrite(LED_PIN_2, manualBrightness);
  analogWrite(LED_PIN_3, manualBrightness);
}

void toggleMode() {
  isAutoMode = !isAutoMode;
  Serial.print("Modo: ");
  Serial.println(isAutoMode ? "AUTOMATICO" : "MANUAL");
}

void incrementBrightness() {
  manualBrightness += 25;
  if (manualBrightness > 255) manualBrightness = 0;
  Serial.print("Brilho: ");
  Serial.println(manualBrightness);
}

void testLEDs() {
  Serial.println("Testando LEDs...");
  for (int i = 0; i < 3; i++) {
    analogWrite(LED_PIN_1, 255);
    delay(200);
    analogWrite(LED_PIN_1, 0);
    analogWrite(LED_PIN_2, 255);
    delay(200);
    analogWrite(LED_PIN_2, 0);
    analogWrite(LED_PIN_3, 255);
    delay(200);
    analogWrite(LED_PIN_3, 0);
  }
  Serial.println("Teste completo!");
}

void sendSerialData() {
  Serial.print("[TELEMETRIA] Temp: ");
  Serial.print(currentTemperature, 1);
  Serial.print("C | Modo: ");
  Serial.println(isAutoMode ? "AUTO" : "MANUAL");
}

// FIM DO CODIGO
// Github: https://github.com/Hbini/arduino-embedded-systems-dio
// Pastebin: https://pastebin.com/q8CPCmQ6
// README: https://pastebin.com/iqxZcExz
