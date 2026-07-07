#pragma once
#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

/*
 * Variables.h
 *
 */

#include "CanBus.h"

// 🔥 Forward declarations pour casser les boucles d’includes
class CLF_CanService;
class CLF_WebHandler;

/* ---------------------------------------------------------------------------
   🟩 CLF — CAN : tableau de bus
--------------------------------------------------------------------------- */

extern CanBus *CAN[1];
extern volatile uint32_t g_lastCanError;

/* ---------------------------------------------------------------------------
   🟩 CLF — Logs
--------------------------------------------------------------------------- */

extern String g_lastLogMessage;

/* ---------------------------------------------------------------------------
   🟩 CLF — Services globaux
--------------------------------------------------------------------------- */

extern CLF_CanService canService;
extern CLF_WebHandler webHandler;