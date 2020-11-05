//---------------------------------------------------------------------------------------
// Proiect     : Minisera automata RTOS.
// Autor       : Șuiu-Cristea Andreea-Cristina
// Data        : 05.11.2020
//---------------------------------------------------------------------------------------
// Descriere   : Source File. Cuprinde comportamentul functiilor declarate in fisierul Header.
//---------------------------------------------------------------------------------------

#include <Senzor_Umiditate_Sol.h>
#include <Arduino.h>

/* Nu se selecteaza nicio tensiune de referinta prin dezactivarea bitilor REFS1 si REFS2
   Ajustam valoarea la dreapta prin bitul ADLAR*/
#define ADC_VREF_TYPE ((0<<REFS1)|(0<<REFS0)|(0<<ADLAR))
//rezultatul unei singure conversii

void Senzor_Umiditate_Sol:: InitializareADC()
{
  //dezactivare a intrarilor digitale
  DIDR0=(0<<ADC5D) | (0<<ADC4D) | (0<<ADC3D) | (ADC2D) | (0<<ADC1D) | (0<<ADC0D);
  //selectarea canalului pe care se primeste semnalul analogic
  ADMUX=ADC_VREF_TYPE;
  //configurarea registrului de control si date
  //bitul ADEN activeaza modulul ADC
  //bitul ADATE conversia porneste pe frontul pozitiv  
  ADCSRA=(1<<ADEN) | (0<<ADSC) | (1<<ADATE) | (0<<ADIF) | (0<<ADIE) | (1<<ADPS2) | (0<<ADPS1) | (0<<ADPS0); 
}

unsigned int Senzor_Umiditate_Sol:: CitesteADC(unsigned char adc_input)
{
  //prin setarea bitului ADCS din registrul ADCSRA se porneste o conversie
  ADCSRA|=(1<<ADSC);
  //se asteapta finalizarea conversiei 
  while((ADCSRA&(1<<ADIF))==0)
  {
  /*prin setarea bitului 1 a registrului ADIF se sterge manual flagul
  care genereaza intrerupereaa cum ca s-a sfarsit procesul de conversie*/  
  ADCSRA |= (1<<ADIF); 
  }
  //returneaza o valoare in registrul ADCW 
  return ADCW;
}
