#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_rom_sys.h"
#include "esp_log.h"
#include "onewire.h"
#include "ds18b20.h"

#define ONEWIRE_GPIO GPIO_NUM_17
//------------------------------------------------------------------------------------------------------

#define main_tag "DS18B20_TEST"
onewire_t bus;

void app_main(void) {
    onewire_t bus;
    onewire_init(&bus, ONEWIRE_GPIO);

    while (1) {
        if (ds18b20_is_present(&bus)) {
            ESP_LOGI(TAG, "DS18B20 detected!");

            // Start temperature conversion
            if (ds18b20_convert(&bus) == ESP_OK) {
                // Wait for conversion to complete (750 ms timeout)
                if (ds18b20_wait_ready(&bus, pdMS_TO_TICKS(750)) == ESP_OK) {
                    float temp;
                    if (ds18b20_read_temp_celsius(&bus, &temp) == ESP_OK) {
                        ESP_LOGI(TAG, "Temperature: %.2f ºC", temp);
                    } else {
                        ESP_LOGE(TAG, "Failed to read temperature.");
                    }
                } else {
                    ESP_LOGW(TAG, "Timeout waiting for conversion.");
                }
            } else {
                ESP_LOGE(TAG, "Error starting conversion.");
            }
        } else {
            ESP_LOGW(TAG, "Sensor not found.");
        }

        vTaskDelay(pdMS_TO_TICKS(2000)); // wait 2 seconds before repeating
    }
}
