#include <FreeRTOS_AVR.h>
#include "utility/FreeRTOS.h"
#include "utility/task.h"
#include "utility/semphr.h"
#include <dht.h>

dht DHT;
SemaphoreHandle_t sensor_signal;

#define DHT11_PIN 24

void vReadTestSensor(void *pvParameters)
{
  for( ;; )
  {
    Serial.println("DHT11 Sensor Test");
    int check = DHT.read11(DHT11_PIN);
    switch (check)
    {
      case DHTLIB_OK:
      Serial.println("Sensor OK");
      break;
      case DHTLIB_ERROR_CHECKSUM:
      Serial.println("Checksum Error");
      break;
      case DHTLIB_ERROR_TIMEOUT:
      Serial.println("TimeOut Error");
      break;
      default:
      Serial.println("Unknown Error");
      break;
      }
      if(check == DHTLIB_OK)
      {
        xSemaphoreGive(sensor_signal);
        Serial.println("Successful Reading!\n");
        }
        else if((check == DHTLIB_ERROR_CHECKSUM) || (check == DHTLIB_ERROR_TIMEOUT))
        {
          Serial.println("UNSUCCESSFUL Reading!\n");
          }
      vTaskDelay(3000);
    }
  }
void vDisplaySensor(void *pvParameters)
{
  xSemaphoreTake(sensor_signal, 0);
  for( ;; )
  {
    if(xSemaphoreTake(sensor_signal, portMAX_DELAY))
    {
      Serial.print("Temperature: ");
      Serial.print(DHT.temperature);
      Serial.print((char)223);
      Serial.print("C\n");
      Serial.print("Humidity: ");
      Serial.print(DHT.humidity);
      Serial.print(" %\n");
      }
    }
  }
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  
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
