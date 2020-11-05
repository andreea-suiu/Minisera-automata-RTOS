#include <Senzor_Umiditate_Sol.h>
#include <Arduino.h>

Senzor_Umiditate_Sol senzor_umiditate_sol;

void setup()
 {
 senzor_umiditate_sol.InitializareADC();
 Serial.begin(9600); 
 }
 
void loop()
{  
  char buffer_analog[5];
  //citire intrare analogica
  unsigned int sensorValue= senzor_umiditate_sol.CitesteADC(0);
  //maparea rezultatului
  sensorValue = map(sensorValue, 0, 1023, 100, 0);
  itoa(sensorValue,buffer_analog,10); 
  Serial.println(buffer_analog);
  delay(100);
}
