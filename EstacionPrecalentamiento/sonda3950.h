#ifndef SONDA3950_H
#define SONDA3950_H

#include "Arduino.h"

const uint8_t _mediaLecturas = 3;

class sonda3950 {
public:
  sonda3950(uint8_t _pin, uint16_t _adcResolution = 1024, uint32_t _rPULLUP = 98000, uint32_t _rTermistor = 100000);  // Constructor
  void begin();                                                                                                       // Inicializamos la sonda
  void actualizar();                                                                                                  // Actualizamos el valor de la sonda
  float temperaturaLeida();                                                                                           // Devolvemos la media de las lecturas

private:
  const float _VCC = 3.93;                 // Valor del voltaje de trabajo
  float _AnalogResolucion;                 // Resolución del ADC
  uint8_t _pinSonda;                       // Pin analógico al que está conectado el termistor
  float _pullUpSonda;                      // Resistencia de pull-up en ohmios
  float _valorNominal;                     // Valor nominal de resistencia del termistor a 25°C
  const float _temperaturaNominal = 25.0;  // Temperatura nominal en grados Celsius
  const float _coeficienteBeta = 3950.0;   // Coeficiente B del termistor
  float _temperaturaTermistor;             // Valor de temperatura leída
  float _valor[_mediaLecturas];            // Guardamos las últimas temperaturas

  uint32_t _tiempoActual;            // Valor del tiempo actual
  uint32_t _tiempoAnterior;          // Valor del tiempo anterior
  uint32_t _intervaloLecturas = 300;  // Intervalo de tiempo de lecturas

  float temperatura3950();  // Devuelve la temperatura leída por el sensor
};

#endif