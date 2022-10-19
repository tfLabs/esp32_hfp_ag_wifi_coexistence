#include "ADCSampler.h"
#include "math.h"
#include "esp_log.h"

ADCSampler::ADCSampler(adc_unit_t adcUnit, adc1_channel_t adcChannel, const i2s_config_t &i2s_config) : I2SSampler(I2S_NUM_0, i2s_config)
{
    m_adcUnit = adcUnit;
    m_adcChannel = adcChannel;
}

void ADCSampler::configureI2S()
{
    //init ADC pad
    i2s_set_adc_mode(m_adcUnit, m_adcChannel);
    // enable the adc
    i2s_adc_enable(m_i2sPort);
}

void ADCSampler::unConfigureI2S()
{
    // make sure ot do this or the ADC is locked
    i2s_adc_disable(m_i2sPort);
}

int ADCSampler::read(int16_t *samples, int count)
{
    static float freq = 1000;
    static float sps = 16000;
    static float div = freq / sps;
    static float cur = 0;
    // static int16_t amp = 32767;
    static int16_t amp = 4095;
    // static pi = M_PI;

    // read from i2s
    size_t bytes_read = 0;
    int64_t pre = esp_timer_get_time();
    i2s_read(m_i2sPort, samples, sizeof(int16_t) * count, &bytes_read, portMAX_DELAY);
    // ESP_LOGI("TCPSocketTransport", "read duration: %lld", esp_timer_get_time() - pre);
    int samples_read = bytes_read / sizeof(int16_t);
    // ESP_LOGI("TCPSocketTransport", "read samples_read: %d", samples_read);
    for (int i = 0; i < samples_read; i++)
    {
        // samples[i] = (2048 - (uint16_t(samples[i]) & 0xfff)) * 15;
        samples[i] = amp * sin(2 * M_PI * cur);
        cur += div;
    }


    return samples_read;
}
