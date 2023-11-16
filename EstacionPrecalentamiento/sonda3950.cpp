#include "sonda3950.h"

// Constructor donde definimos solo el pin que se va a utilizar
sonda3950::sonda3950(uint8_t _pin, uint16_t _adcResolution, uint32_t _rPULLUP, uint32_t _rTermistor) {
  _pullUpSonda = float(_rPULLUP);
  _AnalogResolucion = float(_adcResolution);
  _valorNominal = float(_rTermistor);
  _pinSonda = _pin;
}

// Inicializamos el pin como entrada y ponemos todos los valores a 0.0
void sonda3950::begin() {
  pinMode(_pinSonda, INPUT);
  for (uint8_t i = 0; i < _mediaLecturas; i++) {
    _valor[i] = temperatura3950();
  }
}

// Leemos el valor del pin y lo convertimos a temperatura según la fórmula del fabricante
float sonda3950::temperatura3950() {
  float vTermistor = float(analogRead(_pinSonda)) * _VCC / _AnalogResolucion;
  float iTermistor = (_VCC - vTermistor) / _pullUpSonda;
  float rTermistor = vTermistor / iTermistor;
  _temperaturaTermistor = 1.0 / ((log(rTermistor / _valorNominal) / _coeficienteBeta) + (1.0 / 298.15)) - 273.15;
  return _temperaturaTermistor;
}

// Actualizamos el valor de las sondas y la almacemamos en un array para hace una media
void sonda3950::actualizar() {
  _tiempoActual = millis();
  if (_tiempoActual - _tiempoAnterior >= _intervaloLecturas) {
    _tiempoAnterior = _tiempoActual;
    for (uint8_t i = _mediaLecturas; i > 0; i--) {
      _valor[i] = _valor[i - 1];
    }
    _valor[0] = temperatura3950();
  }
}

// Devolvemos el valor de la media de todas las lecturas
float sonda3950::temperaturaLeida() {
  float valorTotal = 0.0;
  for (uint8_t i = 0; i < _mediaLecturas; i++) {
    valorTotal += _valor[i];
  }
  valorTotal = valorTotal / float(_mediaLecturas);
  return valorTotal;
}