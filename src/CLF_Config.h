#pragma once
#include <Arduino.h>
#include <stdint.h>

/* ---------------------------------------------------------------------------
 * Informations projet
 * --------------------------------------------------------------------------- */
#define PROJECT "CLF - Centrale Logique Ferroviaire"
#define VERSION "v1.0"

/* ---------------------------------------------------------------------------
 * Debug
 * --------------------------------------------------------------------------- */
#define DEBUG
#define debug Serial

/* ---------------------------------------------------------------------------
 * WiFi : Mode Point d’Accès (optionnel)
 * --------------------------------------------------------------------------- */
// #define WIFI_AP_MODE   // Décommenter pour forcer le mode AP

namespace CLF_Variables
{
    /* ------------------------------------------------------------
     * PARAMÈTRES DE VALIDATION
     * ------------------------------------------------------------ */

    // Nombre de mesures nécessaires pour valider vitesse + essieux
    static const uint8_t NB_MESURES_VALIDATION = 5;

    /* ------------------------------------------------------------
     * PARAMÈTRES DE SILENCE CC
     * ------------------------------------------------------------ */

    // Durée pendant laquelle les CC doivent arrêter d'envoyer
    // les mesures vitesse/essieux après validation
    static const uint32_t DUREE_SILENCE_MS = 30000; // 30 secondes

    /* ------------------------------------------------------------
     * PARAMÈTRES JSON
     * ------------------------------------------------------------ */

    // Nom du fichier JSON sur la CLF
    static const char *JSON_FILENAME = "/structure_trains.json";

    /* ------------------------------------------------------------
     * PARAMÈTRES LOCO / CV
     * ------------------------------------------------------------ */

    // CV utilisée pour écrire la vitesse validée dans LaBox Locoduino
    static const uint8_t CV_VITESSE = 5; // Exemple : CV5 = vitesse max

    // cm/s, vitesse cible en voie libre pour calibration CV5
    static const float VITESSE_CIBLE_VOIE_LIBRE_HO = 35.0f;
    static const float VITESSE_CIBLE_VOIE_LIBRE_N = 25.0f;

    /*
    ⭐ VITESSE_CIBLE_VOIE_LIBRE = 40.0f
    (≈ 144 km/h réel → parfait pour voyageurs)

    ⭐ VITESSE_CIBLE_VOIE_LIBRE = 30.0f
    (≈ 108 km/h réel → très réaliste pour la majorité des locos HO)
    */

    // Valeur par défault de CV5 si absent dans le json
    static const uint8_t CV5_DEFAULT = 180;

    /* ------------------------------------------------------------
     * PARAMÈTRES GÉNÉRAUX
     * ------------------------------------------------------------ */

    // Nombre maximum de trains gérés simultanément
    static const uint8_t MAX_TRAINS = 32;
}
