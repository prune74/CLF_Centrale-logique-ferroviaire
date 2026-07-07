#include "CLF_Settings.h"
#include "Debug.h"

/*
 * CLF_Settings.cpp
 *
 */

// ---------------------------------------------------------------------------
// VARIABLES STATIQUES
// ---------------------------------------------------------------------------
bool CLF_Settings::WIFI_ON = true;

String CLF_Settings::WIFI_SSID = "";
String CLF_Settings::WIFI_PSW = "";

// ---------------------------------------------------------------------------
// INITIALISATION SPIFFS
// ---------------------------------------------------------------------------
void CLF_Settings::begin()
{
    if (!SPIFFS.begin(true))
    {
        LOG_ERROR("SPIFFS → échec du montage");
        return;
    }

    LOG_INFO("SPIFFS monté avec succès");
}

// ---------------------------------------------------------------------------
// LECTURE DU FICHIER settings.json
// ---------------------------------------------------------------------------
void CLF_Settings::readFile()
{
    File file = SPIFFS.open("/settings.json", "r");
    if (!file)
    {
        LOG_WARN("settings.json introuvable → valeurs par défaut utilisées");
        return;
    }

    JsonDocument doc;

    DeserializationError error = deserializeJson(doc, file);
    if (error)
    {
        LOG_ERROR("Erreur JSON dans settings.json → valeurs par défaut");
        file.close();
        return;
    }

    WIFI_ON   = doc["wifi_on"].as<bool>();
    WIFI_SSID = doc["wifi_ssid"].as<String>();
    WIFI_PSW  = doc["wifi_psw"].as<String>();

    LOG_INFO("Configuration chargée :");
    LOG_INFO(" - wifi_on        = %s", WIFI_ON ? "true" : "false");
    LOG_INFO(" - wifi_ssid      = %s", WIFI_SSID.c_str());

    file.close();
}

// ---------------------------------------------------------------------------
// ÉCRITURE DU FICHIER settings.json
// ---------------------------------------------------------------------------
void CLF_Settings::writeFile()
{
    JsonDocument doc;

    doc["wifi_on"]   = WIFI_ON;
    doc["wifi_ssid"] = WIFI_SSID;
    doc["wifi_psw"]  = WIFI_PSW;

    File file = SPIFFS.open("/settings.json", "w");
    if (!file)
    {
        LOG_ERROR("SPIFFS → impossible d’ouvrir settings.json en écriture");
        return;
    }

    serializeJsonPretty(doc, file);
    file.close();

    LOG_INFO("settings.json mis à jour");
}

// ---------------------------------------------------------------------------
// GETTERS
// ---------------------------------------------------------------------------
// Retourne le SSID WiFi, ou une valeur par défaut si vide.
String CLF_Settings::getWifiSsid()
{
    return WIFI_SSID.length() > 0 ? WIFI_SSID : "digital";
}

// Retourne le mot de passe WiFi, ou une valeur par défaut si vide.
String CLF_Settings::getWifiPassword()
{
    return WIFI_PSW.length() > 0 ? WIFI_PSW : "digital";
}
