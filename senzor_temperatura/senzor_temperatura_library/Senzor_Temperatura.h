//---------------------------------------------------------------------------------------
// Proiect     : Minisera automata RTOS.
// Autor       : Șuiu-Cristea Andreea-Cristina
// Data        : 04.11.2020
//---------------------------------------------------------------------------------------
// Descriere   : Header File. Cuprinde clasa ce contine declaratiile functiilor si variabilele utilizate.
//---------------------------------------------------------------------------------------

#ifndef Senzor_Temperatura_h
#define Senzor_Temperatura_h
#include <Arduino.h>

class Senzor_Temperatura
{
  public:
    Senzor_Temperatura(int pin);
    void Request();
    void Raspuns();
    uint8_t Primire_date();
  private:
    int _pin;
};

#endif
