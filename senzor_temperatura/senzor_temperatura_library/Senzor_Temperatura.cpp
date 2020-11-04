//---------------------------------------------------------------------------------------
// Proiect     : Minisera automata RTOS.
// Autor       : Șuiu-Cristea Andreea-Cristina
// Data        : 04.11.2020
//---------------------------------------------------------------------------------------
// Descriere   : Source File. Cuprinde comportamentul functiilor declarate in fisierul Header.
//---------------------------------------------------------------------------------------

#include "Senzor_Temperatura.h"
#include <Arduino.h>

#define DHT11_PIN 6

// Constructor. Specificarea pinului dorit pentru utilizare.
Senzor_Temperatura:: Senzor_Temperatura(int pin)
{
  _pin = pin;
}

// Functia care cere senzorului datele
void Senzor_Temperatura:: Request() 
{
  DDRD  |= (1<<DHT11_PIN);  // DDRD = registru care seteaza ca pinul 6 sa fie de iesire
  PORTD |= (1<<DHT11_PIN);  // pinul 6 de pe PORTD = activ
  _delay_ms(100);           // delay pentru ca nu poate sa faca request non stop
  PORTD &= ~(1<<DHT11_PIN);  
  _delay_ms(20); 
  PORTD |= (1<<DHT11_PIN); 
}

// Functia de raspuns a senzorului
void Senzor_Temperatura:: Raspuns()      
{
  DDRD &= ~(1<<DHT11_PIN);             // pinul 6 trecut in 0 => pin de intrare
  // cat timp pinul 6 e activ, senzorul raspunde
  while(PIND & (1<<DHT11_PIN));        // senzorul se pregateste pentru trimiterea datelor (a detectat semnalul de start de la uC)       
  while((PIND & (1<<DHT11_PIN))==0);   // nu se trimite nimic de la senzor; senzorul pregateste transmisia datelor si trimite semnalul raspuns catre uC
  while(PIND & (1<<DHT11_PIN));        // au fost trimise toate datele si revine la free status
}

// Functia de primire a datelor
// Cat timp pinul 6 este pin de intrare, se primesc 8 biti ce sunt stocati intr-un registru
uint8_t Senzor_Temperatura:: Primire_date()     
{ 
  uint8_t c = 0; // data de tip byte
  for (int i = 0; i < 8; i++)
  {
    while((PIND & (1<<DHT11_PIN)) == 0);  
    _delay_us(30);
    if(PIND & (1<<DHT11_PIN))
    c = (c<<1)|(0x01);  
    else      
    c = (c<<1);
    while(PIND & (1<<DHT11_PIN));
  }
  return c;
}
