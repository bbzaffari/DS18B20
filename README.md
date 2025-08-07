# DS18B20 Driver (ESP-IDF, C)  Explained
**This code is shared publicly with no copyrights; anyone is free to use, modify, or redistribute it.**

This driver was developed by me, Bruno Bavaresco Zaffari, as part of my undergraduate final thesis project in Computer Engineering. It is one of the modules included in the TCC directory of the main repository, serving as a key component for managing low-level hardware communication in the final system. Everyone is free to use, modify, and adapt this code as they wish, with no need for copyrights.

## What is DS18B20?

The DS18B20 is a digital temperature sensor using the OneWire protocol. It provides 9–12 bit temperature readings with a simple two-wire interface (data + ground) and supports multiple devices on the same bus via unique 64-bit addresses.

**Key characteristics:**

* Digital output, no need for analog pins.
* Temperature range: −55°C to +125°C.
* Resolution configurable (9 to 12 bits).
* Built-in scratchpad memory.
* CRC-8 error checking.
  
## Documentation

For detailed electrical characteristics, communication protocols, and performance specifications, refer to the official  [DS18B20 Datasheet](https://cdn.sparkfun.com/datasheets/Sensors/Temp/DS18B20.pdf)

## What does this driver do?

This driver allows ESP32 (with ESP-IDF) to communicate with DS18B20 sensors over a OneWire bus, covering all critical operations.

The DS18B20 sensor communicates over OneWire using a sequence of **commands (CMDs)** sent as bytes after a reset + presence handshake.

## Basic Communication Flow

**Reset + Presence**
The master pulls the line low (\~480 µs), then releases it. The sensor responds with a presence pulse (\~60–240 µs) indicating it is ready.

2️ **ROM Commands**
These commands manage addressing on the bus:

* `0x33` READ ROM → Reads the sensor's unique 64-bit address (only works if one device is present).
* `0xCC` SKIP ROM → Broadcasts to all devices, skipping individual selection.
* `0x55` MATCH ROM → Targets a specific device by its ROM code.

**Function Commands**
These perform actions on the sensor:

* `0x44` CONVERT T → Starts a temperature measurement; sensor pulls line low during conversion.
* `0xBE` READ SCRATCHPAD → Reads 9 bytes: temperature, alarm thresholds (TH, TL), config, CRC.
* `0x4E` WRITE SCRATCHPAD → Writes alarm thresholds and resolution configuration.
* `0x48` COPY SCRATCHPAD → Saves scratchpad values to EEPROM.
* `0xB8` RECALL E² → Loads stored EEPROM values into scratchpad.

## Typical Read Sequence

1. Reset → SKIP ROM (0xCC) → CONVERT T (0x44)
2. Wait \~750 ms (for 12-bit resolution) or poll ready.
3. Reset → SKIP ROM (0xCC) → READ SCRATCHPAD (0xBE)
4. Read 9 bytes → Calculate temperature from first two bytes.

## Notes

* Temperature = raw value × 0.0625 (for 12-bit).
* CRC ensures data integrity.
* Lower resolution shortens conversion time but reduces precision.
