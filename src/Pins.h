#pragma once
#include <Arduino.h>

// LED debug
static const gpio_num_t PIN_LED = GPIO_NUM_2;

/* ============================================================================
 *  CAN — CLF : ESP32 interne (TWAI)
 * ============================================================================
 */
#define PIN_CAN_TX GPIO_NUM_4
#define PIN_CAN_RX GPIO_NUM_5
#define CAN_BITRATE 500000UL