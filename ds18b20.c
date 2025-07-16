#include "ds18b20.h"

#define TAG "DS18B20"

// Testa se o sensor DS18B20 está presente
    BaseType_t ds18b20_is_present(onewire_t *ow) {
    if (!onewire_reset(ow)) return false;
    onewire_write_byte(ow, 0x33); // READ ROM
    uint8_t rom[8];
    for (int i = 0; i < 8; i++) {
        rom[i] = onewire_read_byte(ow);
    }
    ESP_LOGI(TAG, "ROM: %02X-%02X-%02X-%02X-%02X-%02X-%02X-%02X",
             rom[0], rom[1], rom[2], rom[3], rom[4], rom[5], rom[6], rom[7]);
    return (rom[0] == 0x28); // verifica código da família DS18B20
}

esp_err_t ds18b20_convert(onewire_t *ow) {
    if (!onewire_reset(ow)) return ESP_FAIL;
    onewire_write_byte(ow, 0xCC); // SKIP ROM
    onewire_write_byte(ow, 0x44); // CONVERT T
    return ESP_OK;
}

esp_err_t ds18b20_read_scratchpad(onewire_t *ow, uint8_t *scratchpad) {
    if (!onewire_reset(ow)) return ESP_FAIL;
    onewire_write_byte(ow, 0xCC); // SKIP ROM
    onewire_write_byte(ow, 0xBE); // READ SCRATCHPAD
    for (int i = 0; i < 9; i++) {
        scratchpad[i] = onewire_read_byte(ow);
    }
    return ESP_OK;
}

esp_err_t ds18b20_read_temp_raw(onewire_t *ow, int16_t *raw_temp) {
    uint8_t scratchpad[9];
    esp_err_t err = ds18b20_read_scratchpad(ow, scratchpad);
    if (err != ESP_OK) return err;
    *raw_temp = (scratchpad[1] << 8) | scratchpad[0];
    return ESP_OK;
}

esp_err_t ds18b20_read_temp_celsius(onewire_t *ow, float *celsius) {
    int16_t raw;
    esp_err_t err = ds18b20_read_temp_raw(ow, &raw);
    if (err != ESP_OK) return err;
    *celsius = raw * 0.0625f;
    return ESP_OK;
}

float read_ds18b20(void){
    float temp_ds;
    if (ds18b20_is_present(&onewire_bus)) {
        if (ds18b20_convert(&onewire_bus) == ESP_OK &&
            ds18b20_wait_ready(&onewire_bus, pdMS_TO_TICKS(750)) == ESP_OK) {

            if (ds18b20_read_temp_celsius(&onewire_bus, &temp_ds) == ESP_OK) {
                ESP_LOGI(TAG, "DS18B20: %.2f °C", temp_ds);
                return temp_ds;
            } else {
                ESP_LOGE(TAG, "Falha ao ler DS18B20.");
                return -50;
            }
        } else {
            ESP_LOGW(TAG, "DS18B20: Timeout ou erro na conversão.");
            return -50;
        }
    } else {
        ESP_LOGW(TAG, "DS18B20: Sensor não detectado.");
        return -50;
    }
}

esp_err_t ds18b20_set_resolution(onewire_t *ow, uint8_t resolution) {
    if (!onewire_reset(ow)) return ESP_FAIL;

    // Define byte de configuração conforme resolução
    uint8_t config;
    switch (resolution) {
        case 9:  config = 0x1F; break;
        case 10: config = 0x3F; break;
        case 11: config = 0x5F; break;
        case 12: config = 0x7F; break;
        default: return ESP_ERR_INVALID_ARG;
    }

    onewire_write_byte(ow, 0xCC);    // SKIP ROM
    onewire_write_byte(ow, 0x4E);    // WRITE SCRATCHPAD
    onewire_write_byte(ow, 0x00);    // TH (ignorado)
    onewire_write_byte(ow, 0x00);    // TL (ignorado)
    onewire_write_byte(ow, config);  // configuração da resolução

    return ESP_OK;
}

esp_err_t ds18b20_wait_ready(onewire_t *ow, TickType_t timeout_ticks) {
    TickType_t start = xTaskGetTickCount();
    while (!onewire_read_bit(ow)) {
        if ((xTaskGetTickCount() - start) > timeout_ticks) return ESP_ERR_TIMEOUT;
        vTaskDelay(1); // evitar busy-loop total
    }
    return ESP_OK;
}

BaseType_t ds18b20_check_crc(const uint8_t *data, uint8_t crc_received) {
    uint8_t crc = 0;

    for (int i = 0; i < 8; i++) {
        uint8_t inbyte = data[i];
        for (int j = 0; j < 8; j++) {
            uint8_t mix = (crc ^ inbyte) & 0x01;
            crc >>= 1;
            if (mix) crc ^= 0x8C; // 0x8C = 0b10001100 ← polinômio 0x31 refletido
            inbyte >>= 1;
        }
    }

    return (crc == crc_received);
}
