/*
 * CLF_main.cpp
 */

#include "CLF_main.h"
#include "CLF_Settings.h"
#include "CLF_CanService.h"
#include "CLF_WebHandler.h"
#include "CLF_Fl_Wifi.h"
#include "CLF_Config.h"

#include "CanInit.h"
#include "CAN_Config.h"
#include "Debug.h"

// Configuration CAN globale
extern CLF_Config CLF_CAN_CONFIG;

// Gestion WiFi (local au module)
CLF_Fl_Wifi wifi;

/* ---------------------------------------------------------------------------
 * 🚀 SETUP PRINCIPAL DU MODULE CLF
 * ------------------------------------------------------------------------- */
void CLF_setup()
{
    LOG_INFO("===============================================");
    LOG_INFO("Projet  : %s", PROJECT);
    LOG_INFO("Version : %s", VERSION);
    LOG_INFO("Compilé : %s - %s", __DATE__, __TIME__);
    LOG_INFO("===============================================");

    // LED onboard (optionnel)
    pinMode(2, OUTPUT);
    digitalWrite(2, LOW);

    /* -----------------------------------------------------------
     * PARAMÈTRES
     * --------------------------------------------------------- */
    LOG_INFO("Chargement des paramètres (settings.json)...");
    CLF_Settings::begin();
    CLF_Settings::readFile();

    /* -----------------------------------------------------------
     * INITIALISATION DES BUS CAN
     * --------------------------------------------------------- */
    LOG_INFO("Initialisation des bus CAN (CAN0 + CAN1)...");
    CanInit::begin(CLF_CAN_CONFIG);

    // Laisse le contrôleur TWAI sortir du RESET
    vTaskDelay(pdMS_TO_TICKS(5));

    /* -----------------------------------------------------------
     * WIFI + WEB
     * --------------------------------------------------------- */
    LOG_INFO("Initialisation WiFi...");
    wifi.start();

    LOG_INFO("Synchronisation NTP...");
    configTime(0, 0, "pool.ntp.org", "time.nist.gov");

    // Attendre la synchro NTP
    time_t now = 0;
    int retry = 0;
    while (now < 1000000000 && retry < 20) // epoch < ~2001 → pas encore sync
    {
        vTaskDelay(pdMS_TO_TICKS(500));
        time(&now);
        retry++;
    }

    if (now < 1000000000)
    {
        LOG_WARN("⚠ NTP non synchronisé après 10 secondes");
    }
    else
    {
        LOG_INFO("NTP synchronisé → epoch = %u", (uint32_t)now);
    }

    LOG_INFO("Initialisation WebHandler (port 80)...");
    webHandler.init(80);

    /* -----------------------------------------------------------
     * SERVICE CAN
     * --------------------------------------------------------- */
    LOG_INFO("Initialisation du service CAN CLF...");
    canService.begin();
}

/* ---------------------------------------------------------------------------
 * 🔁 BOUCLE PRINCIPALE DU MODULE CLF
 * ---------------------------------------------------------------------------
 * Exécutée en continu par FreeRTOS.
 * Chaque sous-système dispose de sa propre boucle non bloquante.
 * ------------------------------------------------------------------------- */
void CLF_loop()
{
    canService.loop();
    webHandler.loop();

    // Pause légère pour éviter de saturer le CPU
    vTaskDelay(pdMS_TO_TICKS(10));
}
