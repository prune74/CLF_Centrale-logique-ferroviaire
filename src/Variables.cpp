#include "Variables.h"

#include "CLF_CanService.h"
#include "CLF_WebHandler.h"

/*
 * Variables.cpp
 */

/* ---------------------------------------------------------------------------
   🟩 CLF — CAN : tableau de bus
--------------------------------------------------------------------------- */

CanBus *CAN[1] = {nullptr};
volatile uint32_t g_lastCanError = 0;

/* ---------------------------------------------------------------------------
   🟩 CLF — Logs
--------------------------------------------------------------------------- */

String g_lastLogMessage = "";

/* ---------------------------------------------------------------------------
   🟩 CLF — Services globaux
--------------------------------------------------------------------------- */

CLF_CanService canService;
CLF_WebHandler webHandler(&canService);
