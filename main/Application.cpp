// #include <FreeRTOS.h>
#include "freertos/FreeRTOS.h"
#include "Application.h"
#include <I2SMEMSSampler.h>
#include <ADCSampler.h>
// #include "transports/WebSocketTransport.h"
#include "transports/TCPSocketTransport.h"
#include "config.h"
// #include "esp_timer.h"
#include "esp_log.h"
// #include <Arduino.h>
#include "esp32-hal.h"

void Application::begin()
{
#ifdef USE_I2S_MIC_INPUT
  this->input = new I2SMEMSSampler(I2S_NUM_0, i2s_mic_pins, i2s_mic_Config);
#else
  this->input = new ADCSampler(ADC_UNIT_1, ADC1_CHANNEL_7, i2s_adc_config);
#endif

  this->input = input;
  // this->transport1 = new WebSocketTransport();
  this->transport2 = new TCPSocketTransport();
  this->input->start();
  // this->transport1->begin();
  this->transport2->begin();
  TaskHandle_t task_handle;
  // xTaskCreate(Application::streamer_task, "task", 8192, this, 0, &task_handle);
  // xTaskCreatePinnedToCore(Application::streamer_task, "task", 8192, this, 0, &task_handle, 1);
  xTaskCreatePinnedToCore(Application::streamer_task, "task", 8192, this, 6, &task_handle, 1);
  // xTaskCreatePinnedToCore(Application::streamer_task, "task", 8192, this, configMAX_PRIORITIES - 3, &task_handle, 1);
}

int64_t last_time = 0;

void Application::streamer_task(void *param)
{
  Application *app = (Application *)param;
  // now just read from the microphone and send to the clients
  int16_t *samples = (int16_t *)malloc(sizeof(int16_t) * 1024);
  while (true)
  {
    // read from the microphone
    int samples_read = app->input->read(samples, 1024);
    // send to the two transports
    // app->transport1->send(samples, samples_read * sizeof(int16_t));
    app->transport2->send(samples, samples_read * sizeof(int16_t));

    // ESP_LOGI("Application", "streamer_task duration: %lld", esp_timer_get_time() - last_time);
    last_time = esp_timer_get_time();

    uint8_t temp = temperatureRead();
    // ESP_LOGI("Application", "streamer_task temp: %d", temp);

    // vTaskDelay(pdMS_TO_TICKS(1));

  }
}
