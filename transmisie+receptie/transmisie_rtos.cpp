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
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include "semphr.h"
#include "queue.h"


TaskHandle_t  Task_senzor_temp_handle;
TaskHandle_t  Task_senzor_umiditate_handle;
TaskHandle_t  Task_transmisie_handle;
QueueHandle_t Queue_Handle = NULL;

struct date_transmise
{
  uint8_t Val1_Hum ;
  uint8_t Val2_Hum ;
  uint8_t Val1_Temp;
  uint8_t Val2_Temp;
  unsigned int sensorValue_Sol;
};

struct date_transmise date_transmise;

void setup()
{
  Serial.begin(9600);
  // crearea task-urilor
  xTaskCreate (Task_senzor_temp, "Task_senzor_temp", 100, NULL, 1, &Task_senzor_temp_handle);
  xTaskCreate (Task_senzor_umiditate, "Task_senzor_umiditate", 100, NULL, 1, &Task_senzor_umiditate_handle);
  xTaskCreate (Task_transmisie, "Task_transmisie", 100, NULL, 3, &Task_transmisie_handle);
  Queue_Handle = xQueueCreate(30, sizeof(uint8_t));
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
      date_transmise.Val1_Hum  = Val1_Hum;
      date_transmise.Val2_Hum  = Val2_Hum;
      date_transmise.Val1_Temp = Val1_Temp;
      date_transmise.Val2_Temp = Val2_Temp;
      if(!xQueueSend(Queue_Handle, &date_transmise.Val1_Hum ,1000))
      {
         Serial.println("Coada nu a putut trimite nicio data!");    
      }
      if(!xQueueSend(Queue_Handle, &date_transmise.Val2_Hum ,1000))
      {
         Serial.println("Coada nu a putut trimite nicio data!");    
      }
      if(!xQueueSend(Queue_Handle, &date_transmise.Val1_Temp ,1000))
      {
         Serial.println("Coada nu a putut trimite nicio data!");    
      }
      if(!xQueueSend(Queue_Handle, &date_transmise.Val2_Temp ,1000))
      {
         Serial.println("Coada nu a putut trimite nicio data!");    
      }
     /* itoa(Val1_Hum,data,10);  // functie ce transforma int in string (8 biti in binar transformati in zecimal)
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
      Serial.print("C ");    */
    }        
  vTaskDelayUntil(&xLastWakeTime, (1000/portTICK_PERIOD_MS));
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
    unsigned int sensorValue = senzor_umiditate.CitesteADC(0);
    //maparea rezultatului
    sensorValue = map(sensorValue, 0, 1023, 100, 0);
    date_transmise.sensorValue_Sol = sensorValue;
    if(!xQueueSend(Queue_Handle, &date_transmise.sensorValue_Sol ,1000))
    {
      Serial.println("Coada nu a putut trimite nicio data!");    
    }
    //itoa(sensorValue,buffer_analog,10); 
    //Serial.println(buffer_analog);
    vTaskDelayUntil(&xLastWakeTime, (1000/portTICK_PERIOD_MS));
   }
}

void Task_transmisie(void *param)
{
  (void) param;
  RF24 radio(9, 10); // CE, CSN         
  const byte address[6] = "00002";
  radio.begin();                  
  radio.openWritingPipe(address); 
  radio.setPALevel(RF24_PA_MIN);  
  radio.stopListening();
  TickType_t xLastWakeTime = xTaskGetTickCount();
  while(1)
  {
    if(xQueueReceive(Queue_Handle,&date_transmise,6000))
    {
            Serial.println("Date primite!");  
            Serial.println(date_transmise.Val1_Hum);  
            Serial.println(date_transmise.Val2_Hum);  
            Serial.println(date_transmise.Val1_Temp);
            Serial.println(date_transmise.Val2_Temp);
            Serial.println(date_transmise.sensorValue_Sol);
            radio.write(&date_transmise, sizeof(date_transmise));
      }
      else
      {
        Serial.println("Datele nu au fost receptionate!");   
      }
    vTaskDelayUntil(&xLastWakeTime, (1000/portTICK_PERIOD_MS));
  }  
}

void loop(){}
