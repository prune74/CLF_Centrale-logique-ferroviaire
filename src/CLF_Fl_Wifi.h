#pragma once
#include <Arduino.h>

/*
 * 🎯 Rôle
 * Module de gestion du WiFi pour la Carte Maîtresse ERM.
 *
 * Fournit :
 *   • Mode AP (point d’accès)
 *   • Mode STA (client WiFi)
 *   • Timeout de connexion
 *   • Logs détaillés
 */

class CLF_Fl_Wifi
{
public:
    void start();
};
