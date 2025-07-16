idf_component_register(
    SRCS ds18b20.c
    INCLUDE_DIRS .
    REQUIRES onewire driver freertos log esp_timer
)