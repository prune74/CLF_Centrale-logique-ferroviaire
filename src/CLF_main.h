#pragma once

#include <Arduino.h>

// ---------------------------------------------------------------------------
// Fonctions principales du module CLF
// ---------------------------------------------------------------------------

// Initialisation complète du système
void CLF_setup();

// Boucle principale FreeRTOS
void CLF_loop();
