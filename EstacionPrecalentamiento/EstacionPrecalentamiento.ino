#include <Wire.h>
#include <PID_v1.h>             // https://github.com/br3ttb/Arduino-PID-Library/tree/master
#include "sonda3950.h"          // https://github.com/MeloCuentan/termistor3950/tree/main
#include <LiquidCrystal_I2C.h>  // https://github.com/johnrickman/LiquidCrystal_I2C
#include <RotaryEncoder.h>      // https://github.com/mathertel/RotaryEncoder

// Valores de los pines utilizados
const uint8_t pinTermistor = A3;
const uint8_t pinSSR = 9;
const uint8_t pinEN1 = 3;
const uint8_t pinEN2 = 2;
const uint8_t pinBoton = 4;

// Valores del PID
double Setpoint = 0.0, Input = 0.0, Output = 0.0;
const double Kp = 4.0;
const double Ki = 0.03;
const double Kd = 3.0;

// Valores del display OLED
const uint8_t SCREEN_WIDTH = 20;
const uint8_t SCREEN_HEIGHT = 4;
const uint8_t SCREEN_ADDRESS = 0x3F;

// Valores generales de funcionamiento
const uint8_t temperaturaMinima = 50;
const uint16_t temperaturaMaxima = 260;
const uint16_t adcResolucion = 1024;
const uint16_t rPullUp = 4700;
const uint32_t rTermistor = 100000;
float temperaturaInicio = 180.0;

// Variables generales
const uint8_t bufferSize = 20;
char buffer[bufferSize + 1];
const char mensaje[] = "PLACA CALENTADORA   BY LUITI      ";
bool estadoCalentador = false;
float valorSonda = 0;
uint16_t valorEncoder = 1;
uint16_t valorEncoderAnterior = 0;
bool lecturaBoton = false;
volatile uint8_t moveEnc = 0;
volatile bool lecturaEnc = false;

// Creamos objetos
LiquidCrystal_I2C lcd(SCREEN_ADDRESS, SCREEN_WIDTH, SCREEN_HEIGHT);
sonda3950 termistor(pinTermistor, adcResolucion, rPullUp, rTermistor);
PID myPID(&Input, &Output, &Setpoint, Kp, Ki, Kd, DIRECT);
RotaryEncoder encoder(pinEN1, pinEN2, RotaryEncoder::LatchMode::FOUR3);

void setup() {
  Setpoint = temperaturaInicio;
  valorEncoder = Setpoint;

  encoder.setPosition(temperaturaInicio);
  myPID.SetMode(AUTOMATIC);
  myPID.SetOutputLimits(0, 20);

  termistor.begin();

  lcd.init();
  lcd.backlight();

  buffer[0] = '\0';

  pinMode(pinBoton, INPUT_PULLUP);

  pantallaInicio();
}

void loop() {
  termistor.actualizar();
  comprobarEncoder();
  medirTemperatura();
  controlLimitesPID();
  PID();
  mostrarDisplay();
}

void PID() {
  if (estadoCalentador == true) {
    Input = valorSonda;
    myPID.Compute();
    analogWrite(pinSSR, Output);
  } else {
    Output = 0;
    analogWrite(pinSSR, 0);
  }
}

void controlLimitesPID() {
  uint8_t diferencia = abs(int(Setpoint) - int(valorSonda));
  if (diferencia < 50) {
    myPID.SetOutputLimits(0, 10);
  } else if (diferencia < 80) {
    myPID.SetOutputLimits(0, 50);
  } else {
    myPID.SetOutputLimits(0, 255);
  }
}

void pantallaInicio() {
  lcd.setCursor(0, 0);
  lcd.print(mensaje);
  lcd.setCursor(0, 1);
  lcd.print(F("POWER -->"));
  lcd.setCursor(0, 2);
  lcd.print(F("SET TEMP:"));
  lcd.setCursor(0, 3);
  lcd.print(F("REAL TEMP:"));
}

void medirTemperatura() {
  valorSonda = termistor.temperaturaLeida();
}

void comprobarEncoder() {
  static uint32_t tiempoDebounce, intervaloDebounce = 250;
  encoder.tick();

  valorEncoder = encoder.getPosition();
  if (valorEncoder < temperaturaMinima) {
    encoder.setPosition(temperaturaMinima);
    valorEncoder = temperaturaMinima;
  } else if (valorEncoder > temperaturaMaxima) {
    encoder.setPosition(temperaturaMaxima);
    valorEncoder = temperaturaMaxima;
  }

  if (valorEncoder != valorEncoderAnterior) {
    valorEncoderAnterior = valorEncoder;
    Setpoint = float(valorEncoder);
  }

  if (digitalRead(pinBoton) == LOW && lecturaBoton == true) {
    lecturaBoton = false;
    estadoCalentador = !estadoCalentador;
    tiempoDebounce = millis();
  }

  if (millis() - tiempoDebounce >= intervaloDebounce && digitalRead(pinBoton) == HIGH) {
    lecturaBoton = true;
  }
}

void updateBuffer(char *newChar) {
  memmove(buffer, buffer + 1, bufferSize - 1);
  buffer[bufferSize - 1] = *newChar;
  buffer[bufferSize] = '\0';
}

void scrollMensaje() {
  static uint32_t tiempoAnterior, intervalo = 500;
  static uint8_t posicionMensaje = SCREEN_WIDTH;
  static uint8_t numeroChar = 0;
  uint8_t tamanioMensaje = strlen(mensaje);

  if (millis() - tiempoAnterior >= intervalo) {
    tiempoAnterior = millis();
    numeroChar++;
    if (numeroChar == tamanioMensaje) numeroChar = 0;

    updateBuffer(&mensaje[numeroChar]);
    lcd.setCursor(0, 0);
    lcd.print(buffer);
  }
}

void mostrarDisplay() {
  static uint32_t tiempoAnterior, intervalo = 200;
  static uint8_t posicionMensaje = SCREEN_WIDTH;
  if (millis() - tiempoAnterior >= intervalo) {
    tiempoAnterior = millis();

    scrollMensaje();

    lcd.setCursor(10, 1);
    lcd.print(estadoCalentador ? "ON " : "OFF");

    lcd.setCursor(10, 2);
    if (Setpoint < 100) lcd.print(" ");
    lcd.print(Setpoint, 0);
    lcd.print(" ");

    lcd.setCursor(10, 3);
    if (int(valorSonda) >= 100) {
      lcd.print(valorSonda, 1);
    } else {
      lcd.print(" ");
      lcd.print(valorSonda, 2);
    }
    lcd.print(" ");
  }
}
