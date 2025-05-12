#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>

// LED invervalos de piscada
#define RATE_1 500  // ms
#define RATE_2 300  // ms

TaskHandle_t task1;
TaskHandle_t task2;

// Pins
#define led_pin_1 26
#define led_pin_2 27

SemaphoreHandle_t mutex = NULL; //semáforo

// t1: piscar led com rate_1
void toggleLED_1(void *parameter) {
  TaskStatus_t xTaskDetails;
  TaskHandle_t xHandle;
  xHandle = xTaskGetHandle("t1");
  vTaskGetInfo(xHandle, &xTaskDetails, pdTRUE, eInvalid );
  Serial.printf("%s no nucleo %d com prioridade %d\n", xTaskDetails.pcTaskName, xPortGetCoreID(), uxTaskPriorityGet(xHandle));
  while(1) {
    
    if (xSemaphoreTake(mutex, portMAX_DELAY) == pdTRUE) {
      digitalWrite(led_pin_1, HIGH);
      vTaskDelay(RATE_1 / portTICK_PERIOD_MS);
      digitalWrite(led_pin_1, LOW);
      vTaskDelay(RATE_1 / portTICK_PERIOD_MS);
      xSemaphoreGive(mutex);
    }
    vTaskDelay(pdTICKS_TO_MS(10));
  }
}

// t2: piscar led com rate_2
void toggleLED_2(void *parameter) {
  TaskStatus_t xTaskDetails;
  TaskHandle_t xHandle;
  xHandle = xTaskGetHandle("t2");
  vTaskGetInfo(xHandle, &xTaskDetails, pdTRUE, eInvalid );
  Serial.printf("%s no nucleo %d com prioridade %d\n", xTaskDetails.pcTaskName, xPortGetCoreID(), uxTaskPriorityGet(xHandle));
  while(1) {
    if (xSemaphoreTake(mutex, portMAX_DELAY) == pdTRUE) {//portMAX_DELAY
      digitalWrite(led_pin_2, HIGH);
      vTaskDelay(RATE_2 / portTICK_PERIOD_MS);
      digitalWrite(led_pin_2, LOW);
      vTaskDelay(RATE_2 / portTICK_PERIOD_MS);
      xSemaphoreGive(mutex);
    }
    //vTaskDelay(pdTICKS_TO_MS(10));
  }
}

void setup() {

  Serial.begin(115200);
  digitalWrite(led_pin_1, HIGH);
  digitalWrite(led_pin_2, HIGH);
  vTaskDelay(pdTICKS_TO_MS(1000));
  digitalWrite(led_pin_1, LOW);
  digitalWrite(led_pin_2, LOW);
  
  // Configura pin
  pinMode(led_pin_1, OUTPUT);
  pinMode(led_pin_2, OUTPUT);

  mutex = xSemaphoreCreateBinary(); // equivalente a xSemaphoreCreateBinary()
  //Serial.println(uxSemaphoreGetCount(mutex));
  xSemaphoreGive(mutex);
  //Serial.println(uxSemaphoreGetCount(mutex));
  
  // Tarefas em loop infinito
  xTaskCreatePinnedToCore(  // Use xTaskCreate() in vanilla FreeRTOS
              toggleLED_1,  // função a ser chamada
              "t1",   // nome da task
              4096,         // tamanho em bytes
              NULL,         // parâmetros a passar a função
              3,            // Prioridade (0 a configMAX_PRIORITIES - 1)
              &task1,         // handle da task
              1);     // núcleo: 0 (pro) 1 (app)
  
  // Tarefas em loop infinito
  xTaskCreatePinnedToCore(  
              toggleLED_2,  
              "t2",   
              4096,         
              NULL,         
              1,            
              &task2,         
              1);     

  // If this was vanilla FreeRTOS, you'd want to call vTaskStartScheduler() in
  // main after setting up your tasks.
  vTaskDelete(NULL);
}

void loop() {
  
}