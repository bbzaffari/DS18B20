#pragma once
#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_rom_sys.h"
#include "esp_log.h"
#include "onewire.h"

BaseType_t ds18b20_is_present(onewire_t *ow);esp_err_t ds18b20_convert(onewire_t *ow);

esp_err_t ds18b20_read_scratchpad(onewire_t *ow, uint8_t *data);

esp_err_t ds18b20_read_temp_raw(onewire_t *ow, int16_t *raw_temp);

esp_err_t ds18b20_read_temp_celsius(onewire_t *ow, float *celsius);

esp_err_t ds18b20_set_resolution(onewire_t *ow, uint8_t resolution);

esp_err_t ds18b20_wait_ready(onewire_t *ow, TickType_t timeout_ticks);

BaseType_t ds18b20_check_crc(const uint8_t *data, uint8_t crc_received);

float read_ds18b20(void);