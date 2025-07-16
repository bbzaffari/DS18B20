#pragma once

#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_rom_sys.h"
#include "esp_log.h"
#include "onewire.h"

/**
 * Checks if a DS18B20 sensor is present on the OneWire bus.
 * Reads the ROM code and verifies family code (0x28).
 */
BaseType_t ds18b20_is_present(onewire_t *ow);

/**
 * Starts temperature conversion on the DS18B20 sensor.
 * Issues SKIP ROM + CONVERT T commands.
 */
esp_err_t ds18b20_convert(onewire_t *ow);

/**
 * Reads the 9-byte scratchpad memory from the DS18B20.
 * Contains temperature data, thresholds, config, and CRC.
 */
esp_err_t ds18b20_read_scratchpad(onewire_t *ow, uint8_t *data);

/**
 * Reads raw 16-bit temperature data from the sensor's scratchpad.
 * Combines LSB and MSB into signed integer.
 */
esp_err_t ds18b20_read_temp_raw(onewire_t *ow, int16_t *raw_temp);

/**
 * Reads temperature and converts it to Celsius.
 * Raw value * 0.0625 for degrees Celsius.
 */
esp_err_t ds18b20_read_temp_celsius(onewire_t *ow, float *celsius);

/**
 * Sets the sensor's resolution (9–12 bits).
 * Writes config byte to scratchpad: 0x1F, 0x3F, 0x5F, or 0x7F.
 */
esp_err_t ds18b20_set_resolution(onewire_t *ow, uint8_t resolution);

/**
 * Waits until the sensor finishes temperature conversion.
 * Checks for ready signal with timeout in ticks.
 */
esp_err_t ds18b20_wait_ready(onewire_t *ow, TickType_t timeout_ticks);

/**
 * Checks if data matches provided CRC.
 * Computes CRC-8 (polynomial 0x31 reflected) over 8 bytes.
 */
BaseType_t ds18b20_check_crc(const uint8_t *data, uint8_t crc_received);

/**
 * Application-level helper: reads temperature in Celsius from global bus.
 * Logs result; returns -50 on error.
 */
float read_ds18b20(void);
