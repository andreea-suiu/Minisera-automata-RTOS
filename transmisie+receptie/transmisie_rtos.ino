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
  uint8_t date_transmise_mediu[4];
  unsigned int sensorValue_Sol;
};

struct date_transmise date_transmise;

void setup()
{
  Serial.begin(9600);
  // crearea task-urilor
  xTaskCreate (Task_senzor_temp, "Task_senzor_temp", 100, NULL, 1, &Task_senzor_temp_handle);
  xTaskCreate (Task_senzor_umiditate, "Task_senzor_umiditate", 100, NULL, 1, &Task_senzor_umiditate_handle);
  xTaskCreate (Task_transmisie, "Task_transmisie", 100, NULL, 2, &Task_transmisie_handle);
  Queue_Handle = xQueueCreate(10, sizeof(struct date_transmise));
}

// task pentru masurarea temperaturii
// folosirea librariei create pentru senzorul de temperatura DHT11 
void Task_senzor_temp(void *param)
{

   (void) param;
   Senzor_Temperatura senzor_temp(6); // crearea unui obiect de tip Senzor_Temperatura
   //char data[5];
   uint8_t SumaBiti;
   //uint8_t date_mediu[4];
   TickType_t xLastWakeTime = xTaskGetTickCount();
  while(1)
  {
    senzor_temp.Request();    
    senzor_temp.Raspuns();   
    date_transmise.date_transmise_mediu[0] = senzor_temp.Primire_date();
    date_transmise.date_transmise_mediu[1] = senzor_temp.Primire_date();
    date_transmise.date_transmise_mediu[2] = senzor_temp.Primire_date();
    date_transmise.date_transmise_mediu[3] = senzor_temp.Primire_date();
    /*date_mediu[0]  = senzor_temp.Primire_date();  // primii 8 biti din valoarea umiditatii(partea intreaga)
    date_mediu[1]  = senzor_temp.Primire_date();  // urmatorii 8 biti din valoarea umiditatii(partea zecimala)
    date_mediu[2]  = senzor_temp.Primire_date();  // primii 8 biti din valoarea temperaturii
    date_mediu[3]  = senzor_temp.Primire_date();  // urmatorii 8 biti din valoarea temperaturii*/
    SumaBiti   = senzor_temp.Primire_date();  // variabila ce aduna toti bitii(check sum)
    
    if ((date_transmise.date_transmise_mediu[0] + date_transmise.date_transmise_mediu[1] + date_transmise.date_transmise_mediu[2] + date_transmise.date_transmise_mediu[3]) != SumaBiti) // verifica numar de biti; daca suma lor este diferita de valoarea din SumaBiti => eroare
    {
      Serial.print("Error");
    }
    else
    { 
      /*date_transmise.date_transmise_mediu[0]  = date_mediu[0];
      date_transmise.date_transmise_mediu[1]  = date_mediu[1];
      date_transmise.date_transmise_mediu[2]  = date_mediu[2];
      date_transmise.date_transmise_mediu[3]  = date_mediu[3];*/
      if(!xQueueSend(Queue_Handle, &date_transmise.date_transmise_mediu ,1000))
      {
         Serial.println("Coada nu a putut trimite nicio data!");    
      }
      /*if(!xQueueSend(Queue_Handle, &date_transmise.Val2_Hum ,1000))
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
      }*/
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
  struct date_transmise_local
  {
    uint8_t date_transmise_mediu_local[4];
    unsigned int sensorValue_Sol;
  };
struct date_transmise_local date_transmise_local;
  TickType_t xLastWakeTime = xTaskGetTickCount();
  while(1)
  {
    if(xQueueReceive(Queue_Handle,&date_transmise_local,1000))
    {
            Serial.println("Date primite!");  
            Serial.println(date_transmise_local.date_transmise_mediu_local[0]);  
            Serial.println(date_transmise_local.date_transmise_mediu_local[1]);  
            Serial.println(date_transmise_local.date_transmise_mediu_local[2]);
            Serial.println(date_transmise_local.date_transmise_mediu_local[3]);
            Serial.println(date_transmise_local.sensorValue_Sol);
            radio.write(&date_transmise_local, sizeof(date_transmise_local));
      }
      else
      {
        Serial.println("Datele nu au fost receptionate!");   
      }
    vTaskDelayUntil(&xLastWakeTime, (1000/portTICK_PERIOD_MS));
  }  
}

void loop(){}
