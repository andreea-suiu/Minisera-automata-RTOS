//---------------------------------------------------------------------------------------
// Proiect     : Sera automata.
// Autor       : Șuiu-Cristea Andreea-Cristina
// Data        : 04.11.2020
//---------------------------------------------------------------------------------------
// Descriere   : Main File. Cuprinde task-urile pentru citirea datelor de la senzorul de temperatura si cel de umiditate.
//---------------------------------------------------------------------------------------

#include <Arduino_FreeRTOS.h>
#include <Senzor_Temperatura.h>
#include <Senzor_Umiditate_Sol.h>
//#define INCLUDE_vTaskDelayUntil    1


TaskHandle_t Task_senzor_temp_handle;
TaskHandle_t Task_senzor_umiditate_handle;

void setup()
{
  Serial.begin(9600);
  // crearea task-urilor
  xTaskCreate(Task_senzor_temp, "Task_senzor_temp", 100, NULL, 1, &Task_senzor_temp_handle);
  xTaskCreate(Task_senzor_umiditate, "Task_senzor_umiditate", 100, NULL, 1, &Task_senzor_umiditate_handle);
}

// task pentru masurarea temperaturii
// folosirea librariei create pentru senzorul de temperatura DHT11 
void Task_senzor_temp(void *param)
{
   (void) param;
   Senzor_Temperatura senzor_temp(6); // crearea unui obiect de tip Senzor_Temperatura
   char data[5];
   uint8_t Val1_Hum, Val2_Hum, Val1_Temp, Val2_Temp, SumaBiti; // date de tip byte
   TickType_t xLastWakeTime = xTaskGetTickCount();
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
  vTaskDelayUntil(&xLastWakeTime, (2000/portTICK_PERIOD_MS));
  }
}

// task pentru masurarea umiditatii
// folosirea librariei create pentru senzorul de umiditate
void Task_senzor_umiditate(void *param)
{
   (void) param;
   Senzor_Umiditate_Sol senzor_umiditate; // crearea unui obiect de tip Senzor_Umiditate_Sol
   char buffer_analog[5];
   TickType_t xLastWakeTime = xTaskGetTickCount();
   while(1)
   {
    senzor_umiditate.InitializareADC();
    //citire intrare analogica
    unsigned int sensorValue= senzor_umiditate.CitesteADC(0);
    //maparea rezultatului
    sensorValue = map(sensorValue, 0, 1023, 100, 0);
    itoa(sensorValue,buffer_analog,10); 
    Serial.println(buffer_analog);
    vTaskDelayUntil(&xLastWakeTime, (2000/portTICK_PERIOD_MS));
   }
}



void loop(){}
