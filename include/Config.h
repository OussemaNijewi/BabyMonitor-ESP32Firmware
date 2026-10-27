#ifndef CONFIG_H
#define CONFIG_H
#include <cstdint>

// --- I2C Bus Pins ---
constexpr uint8_t PIN_SDA = 21;
constexpr uint8_t PIN_SCL = 22;
constexpr uint32_t I2C_FREQUENCY = 100000; // 100 kHz

// --- Sensor Specific Pins ---
constexpr uint8_t PIN_AIR_QUALITY = 32;

// --- Network Settings ---
constexpr uint16_t WEBSERVER_PORT = 80;

// --- FreeRTOS Queue & Task Configuration ---
constexpr uint32_t SENSOR_QUEUE_LENGTH = 15;

#endif // CONFIG_H