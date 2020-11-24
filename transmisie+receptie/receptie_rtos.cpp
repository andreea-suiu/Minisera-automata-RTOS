#include <Arduino_FreeRTOS.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include "semphr.h"
#include "queue.h"
#define releu PD2
LiquidCrystal_I2C lcd(0x27,20,4); 

TaskHandle_t Task_receptie;
TaskHandle_t Task_LCD;
TaskHandle_t Task_Pompa1;
SemaphoreHandle_t Semaphore;

struct date_primite
{
  uint8_t Val1_Hum ;
  uint8_t Val2_Hum ;
  uint8_t Val1_Temp;
  uint8_t Val2_Temp;
  unsigned int sensorValue_Sol;
};

struct date_primite date_primite;

void setup() 
{  

   Serial.begin(9600);
   xTaskCreate(Task_receptie1, "Task_receptie",100, NULL,1, &Task_receptie);
   xTaskCreate(Task_Display_LCD, "Task_LCD",100, NULL,1, &Task_LCD);
   xTaskCreate(Task_Pompa, "Task_Pompa",100, NULL,1, &Task_Pompa1);
   Semaphore = xSemaphoreCreateBinary();
   if (Semaphore == NULL) 
   {
    Serial.println("Semaphore can not be created");
   }
}


void Task_receptie1(void *param)
{
  (void) param;
  RF24 radio(9, 10); // CE, CSN         
  const byte address[6] = "00002";
  radio.begin();                  
  radio.openReadingPipe(0, address); 
  radio.setPALevel(RF24_PA_MIN);  
  radio.startListening();  
  TickType_t xLastWakeTime = xTaskGetTickCount();
  while(1)
  {  
    if (radio.available())              //Looking for the data.
    {
      radio.read(&date_primite, sizeof(date_primite));    //Reading the data    
      date_primite.Val1_Hum;  
      date_primite.Val2_Hum; 
      date_primite.Val1_Temp;
      date_primite.Val2_Temp; 
      date_primite.sensorValue_Sol;   
      if(date_primite.sensorValue_Sol <= 20)
      {
        xSemaphoreGive(Semaphore);  
      }
    }     
   vTaskDelayUntil(&xLastWakeTime, (1000/portTICK_PERIOD_MS));
  }  
}

void Task_Display_LCD(void *param)
{
  (void) param; 
  lcd.init();                      
  lcd.backlight();
  char data[5];
  char data1[5];
  TickType_t xLastWakeTime = xTaskGetTickCount();
  while(1)
  { 
      itoa(date_primite.Val1_Hum,data,10);    // functie ce face conversia din numeric in string (8 biti in binar transformati in zecimal)
      lcd.setCursor(0,0);
      lcd.print("HUM:");
      lcd.print(data);
      //lcd.print(".");
     // itoa(date_primite.Val2_Hum,data,10);
     // lcd.print(data);
      lcd.print("%");
      lcd.print(" ");
      lcd.print("TMP:");
      itoa(date_primite.Val1_Temp,data,10);
      lcd.print(data);
      lcd.print(".");
      itoa(date_primite.Val2_Temp,data,10);
      lcd.print(data);
      lcd.print("C");  
      itoa(date_primite.sensorValue_Sol,data1,10);    
      lcd.setCursor(0,1);
      lcd.print("HUM SOL:");
      lcd.print(data1);
  }  
  vTaskDelayUntil(&xLastWakeTime, (1000/portTICK_PERIOD_MS));
}

void Task_Pompa(void *param)
{
  (void) param;
  
  TickType_t xLastWakeTime = xTaskGetTickCount();
 // xSemaphoreTake(Semaphore,portMAX_DELAY);
  while(1)
  {
     /* if(date_primite.sensorValue_Sol <= 20)
        {       
           lcd.print("uscat");
           PORTD = 0b00000100;
           vTaskDelay(10000/portTICK_PERIOD_MS); 
           PORTD = 0b00000000;   
        } */
        if(xSemaphoreTake(Semaphore, portMAX_DELAY) == pdPASS)
        {
          Serial.print("aici");
          DDRD |= 0b11111111; // setare pin 2 de pe portul D ca iesire
          PORTD = 0b00000100;
          vTaskDelay(10000/portTICK_PERIOD_MS); 
          PORTD = 0b00000000;
          xSemaphoreGive(Semaphore);
        }

  }  
  vTaskDelayUntil(&xLastWakeTime, (1000/portTICK_PERIOD_MS));
}

void loop() {}
