#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>
#include <SPIFFS.h>
#include "CLF_Config.h"

/*
 * 🎯 Rôle
 * Module de gestion des paramètres persistants de la Carte Maîtresse
 * d’Exploration du Réseau (ERM).
 *
 * Les paramètres sont stockés dans :
 *   • /settings.json
 *
 * Ce module fournit :
 *   • le chargement au démarrage
 *   • la sauvegarde à la demande
 *   • des variables statiques accessibles globalement
 *   • des getters simplifiés pour le WiFi
 */

class CLF_Settings
{
public:
    // --- Gestion du fichier settings.json ---
    static void begin();     // Monte SPIFFS
    static void readFile();  // Charge les paramètres
    static void writeFile(); // Sauvegarde les paramètres

    // --- Accès simplifié aux credentials WiFi ---
    static String getWifiSsid();
    static String getWifiPassword();

public:
    // --- Paramètres persistants ---
    static bool WIFI_ON;        // Activation WiFi

    static String WIFI_SSID; // SSID WiFi
    static String WIFI_PSW;  // Mot de passe WiFi

    // Empêche l’instanciation
    CLF_Settings() = delete;
};
