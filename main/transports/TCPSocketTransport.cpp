#include <WiFiServer.h>
#include "TCPSocketTransport.h"
#include "esp_log.h"

void TCPSocketTransport::begin()
{
  // Serial.println("Connect to TCP socket microphone.local:9090 to try out TCP socket streaming");
  ESP_LOGI("loop", "Connect to TCP socket microphone.local:9090 to try out TCP socket streaming");

  server = new WiFiServer(9090);
  server->begin();
}

void TCPSocketTransport::send(void *data, size_t len)
{
  // get any new connections
      // int64_t pre = esp_timer_get_time();
  WiFiClient client = server->available();
      // ESP_LOGI("TCPSocketTransport", "client get duration: %lld", esp_timer_get_time() - pre);
  // ESP_LOGI("loop", "sending");
  if (client)
  {
    // Serial.println("New Client");
    ESP_LOGI("TCPSocketTransport", "New Client");
    // add to the list of clients
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
      if (NULL == clients[i])
      {
        clients[i] = new WiFiClient(client);
        break;
      }
    }
  }
  // send the audio data to any clients
      int64_t pre = esp_timer_get_time();
  for (int i = 0; i < MAX_CLIENTS; i++)
  {
    if (clients[i] != NULL && (*clients[i]))
    {
      // ESP_LOGI("TCPSocketTransport", "send");
      // send the samples to the client
      // int64_t pre = esp_timer_get_time();
      clients[i]->write((uint8_t *)data, len);
      // ESP_LOGI("TCPSocketTransport", "write duration: %lld", esp_timer_get_time() - pre);

    }
    else
    {
      // client has gone away, remove it from the list
      clients[i] = NULL;
    }
  }
      // ESP_LOGI("TCPSocketTransport", "for duration: %lld", esp_timer_get_time() - pre);
}