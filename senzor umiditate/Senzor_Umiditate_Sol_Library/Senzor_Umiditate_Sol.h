//---------------------------------------------------------------------------------------
// Proiect     : Minisera automata RTOS.
// Autor       : Șuiu-Cristea Andreea-Cristina
// Data        : 05.11.2020
//---------------------------------------------------------------------------------------
// Descriere   : Header File. Cuprinde clasa ce contine declaratiile functiilor si variabilele utilizate.
//---------------------------------------------------------------------------------------


#ifndef Senzor_Umiditate_Sol_h
#define Senzor_Umiditate_Sol_h
#include <Arduino.h>

class Senzor_Umiditate_Sol
{
  public:
    void InitializareADC();
    unsigned int CitesteADC(unsigned char adc_input);   
};

#endif
