//---------------------------------------------------------------------------------------
// Proiect     : Minisera automata RTOS.
// Autor       : Șuiu-Cristea Andreea-Cristina
// Data        : 04.11.2020
//---------------------------------------------------------------------------------------
// Descriere   : Main File. Cuprinde task-ul pentru citirea valorilor senzorului de temperatura.
//---------------------------------------------------------------------------------------

#include <Arduino_FreeRTOS.h>
#include <Senzor_Temperatura.h>

//Senzor_Temperatura senzor_temp(6);

TaskHandle_t Task_senzor_temp_handle;

void setup()
{
  Serial.begin(9600);
  // crearea task-ului
  xTaskCreate(Task_senzor_temp, "Task_senzor_temp", 100, NULL, 1, &Task_senzor_temp_handle);
}

// task pentru masurarea temperaturii
// folosirea librariei create pentru senzorul de temperatura DHT11 
void Task_senzor_temp(void *param)
{
   (void) param;
   Senzor_Temperatura senzor_temp(6); // crearea unui obiect de tip Senzor_Temperatura
   char data[5];
   uint8_t Val1_Hum, Val2_Hum, Val1_Temp, Val2_Temp, SumaBiti; // date de tip byte
   
  while(1)
  {
    senzor_temp.Request();    
    senzor_temp.Raspuns();   
    Val1_Hum   = senzor_temp.Primire_date();  // primii 8 biti din valoarea umiditatii(partea intreaga)
    Val2_Hum   = senzor_temp.Primire_date();  // urmatorii 8 biti din valoarea umiditatii(partea zecimala)
    Val1_Temp  = senzor_temp.Primire_date();  // primii 8 biti din valoarea temperaturii
    Val2_Temp  = senzor_temp.Primire_date();  // urmatorii 8 biti din valoarea temperaturii
    SumaBiti   = senzor_temp.Primire_date();  // variabila ce aduna toti bitii(check sum)
    
    if ((Val1_Hum + Val2_Hum + Val1_Temp + Val2_Temp) != SumaBiti) // verifica numar de biti; daca suma lor este diferita de valoarea din SumaBiti => eroare
    {
      Serial.print("Error");
    }
    else
    { 
      itoa(Val1_Hum,data,10);  // functie ce transforma int in string (8 biti in binar transformati in zecimal)
      Serial.print(data);
      Serial.print(".");
      
      itoa(Val2_Hum,data,10);
      Serial.print(data);
      Serial.print("%");
      Serial.println(" ");
      itoa(Val1_Temp,data,10);
      Serial.print(data);
      Serial.print(".");
      
      itoa(Val2_Temp,data,10);
      Serial.print(data);
      Serial.print("C ");    
    }        
  _delay_ms(2000);    
  }
}

void loop(){}
