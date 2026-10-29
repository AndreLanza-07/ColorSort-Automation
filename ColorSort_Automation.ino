
#include <Wire.h>
#include <Adafruit_TCS34725.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>

// ===== SENSOR DE COR =====
Adafruit_TCS34725 tcs(
  TCS34725_INTEGRATIONTIME_154MS,
  TCS34725_GAIN_1X
);

// ===== DISPLAY LCD =====
LiquidCrystal_I2C lcd(0x27, 16, 2);

// ===== SERVOS =====
Servo servo1;
Servo servo2;

const int servoPin1 = 12;
const int servoPin2 = 13;

// ===== MOTOR DA ESTEIRA =====
const int motorPin = 6;
const int velocidade = 255;

// ===== CONTADORES =====
int total = 0;
int amarelo = 0;
int azul = 0;

// ===== CONTROLE DO SENSOR =====
bool sensorDisponivel = false;

void setup() {
  Serial.begin(9600);
  Wire.begin();

  lcd.init();
  lcd.backlight();

  lcd.setCursor(0, 0);
  lcd.print("Separador de");
  lcd.setCursor(0, 1);
  lcd.print("Cores");

  servo1.attach(servoPin1);
  servo2.attach(servoPin2);

  servo1.write(90);
  servo2.write(90);

  delay(1000);

  pinMode(motorPin, OUTPUT);

  sensorDisponivel = tcs.begin();

  if (!sensorDisponivel) {
    analogWrite(motorPin, 0);
    lcd.clear();
    lcd.print("Erro no sensor");
    Serial.println("Erro ao iniciar sensor TCS34725!");
    return;
  }

  analogWrite(motorPin, velocidade);

  lcd.clear();
  lcd.print("Sistema pronto");
  Serial.println("Sensor de cor iniciado!");

  delay(1500);
  lcd.clear();
}

void loop() {
  if (!sensorDisponivel) {
    analogWrite(motorPin, 0);
    return;
  }

  uint16_t r, g, b, c;
  tcs.getRawData(&r, &g, &b, &c);

  Serial.print("R: ");
  Serial.print(r);
  Serial.print(" G: ");
  Serial.print(g);
  Serial.print(" B: ");
  Serial.print(b);
  Serial.print(" C: ");
  Serial.println(c);

  // ===== IDENTIFICACAO DA COR AMARELA =====
  if (c > 150 &&
      r > b * 1.5 &&
      g > b * 1.2 &&
      r > g * 1.05) {

    amarelo++;
    total++;

    Serial.println("AMARELO DETECTADO");

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Cor: Amarelo");
    lcd.setCursor(0, 1);
    lcd.print("Total: ");
    lcd.print(total);

    // Servo 1: valores solicitados
    servo1.write(90);
    delay(20);
    servo1.write(3600);
    delay(5000);
    servo1.write(90);
    delay(2000);
  }

  // ===== IDENTIFICACAO DA COR AZUL =====
  else if (c > 150 &&
           b > r &&
           b > g) {

    azul++;
    total++;

    Serial.println("AZUL DETECTADO");

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Cor: Azul");
    lcd.setCursor(0, 1);
    lcd.print("Total: ");
    lcd.print(total);

    // Servo 2: mesma sequencia solicitada
    servo2.write(90);
    delay(20);
    servo2.write(10);
    delay(5000);
    servo2.write(90);
    delay(2000);
  }

  // ===== EXIBE OS CONTADORES =====
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Total: ");
  lcd.print(total);

  lcd.setCursor(0, 1);
  lcd.print("A:");
  lcd.print(amarelo);
  lcd.print(" Az:");
  lcd.print(azul);

  delay(300);
}
