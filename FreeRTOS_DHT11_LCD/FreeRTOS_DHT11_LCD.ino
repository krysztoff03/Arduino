#include <FreeRTOS_AVR.h>
#include "utility/FreeRTOS.h"
#include "utility/task.h"
#include "utility/semphr.h"
#include <dht.h>
#include <LiquidCrystal.h>

dht DHT;
LiquidCrystal LCD(8, 9, 4, 5, 6, 7);

#define DHT11_PIN 24

SemaphoreHandle_t sensor_signal;

void vReadTestSensor(void *pvParameters)
{
  for( ;; )
  {
    LCD.clear();
    LCD.setCursor(0, 0);
    LCD.print("DHT11 SensorTest");
    delay(3000);
    int check = DHT.read11(DHT11_PIN);
    switch (check)
    {
      case DHTLIB_OK:
      {
        LCD.setCursor(0, 1);
        LCD.print("Sensor OK");
      }
      break;
      case DHTLIB_ERROR_CHECKSUM:
      {
        LCD.setCursor(0, 1);
        LCD.print("Checksum Error");
      }
      break;
      case DHTLIB_ERROR_TIMEOUT:
      {
        LCD.setCursor(0, 1);
        LCD.print("TimeOut Error");
      }
      break;
      default:
      {
        LCD.setCursor(0, 1);
        LCD.print("Unknown Error");
      }
      break;
      }
      delay(3000);
      if(check == DHTLIB_OK)
      {
        xSemaphoreGive(sensor_signal);
        LCD.clear();
        LCD.setCursor(0, 0);
        LCD.print(" Successful Reading!");
        for(int positionCounter = 0; positionCounter < 4; positionCounter++)
        {
          LCD.scrollDisplayLeft();
          delay(500);
          }
        }
        else if((check == DHTLIB_ERROR_CHECKSUM) || (check == DHTLIB_ERROR_TIMEOUT))
        {
          LCD.clear();
          LCD.setCursor(0, 0);
          LCD.print(" UNSUCCESSFUL Reading!");
          for(int positionCounter = 0; positionCounter < 6; positionCounter++)
          {
            LCD.scrollDisplayLeft();
            delay(500);
            }
          }
      vTaskDelay(5000);
    }
  }
void vDisplaySensor(void *pvParameters)
{
  xSemaphoreTake(sensor_signal, 0);
  for( ;; )
  {
    if(xSemaphoreTake(sensor_signal, portMAX_DELAY))
    {
      LCD.clear();
      LCD.setCursor(0, 0);
      LCD.print("Temp: ");
      LCD.print(DHT.temperature);
      LCD.print((char)223);
      LCD.print("C");
      LCD.setCursor(0, 1);
      LCD.print("Hum:  ");
      LCD.print(DHT.humidity);
      LCD.print(" %");
      delay(5000);
      }
    }
  }
void setup() {
  // put your setup code here, to run once:
  LCD.begin(16, 2);
  
  vSemaphoreCreateBinary(sensor_signal);

  if(sensor_signal != NULL)
  {
    xTaskCreate(vReadTestSensor, "Task1", 1024, NULL, 1, NULL);
    xTaskCreate(vDisplaySensor, "Task2", 1024, NULL, 2, NULL);
  
    vTaskStartScheduler();
  }
}

void loop() {
  // put your main code here, to run repeatedly:

}
