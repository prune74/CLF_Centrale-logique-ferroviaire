#pragma once
#include <Arduino.h>
#include "CLF_Config.h"
#include "CLF_TrainState.h"

/*
 * CLF_TrainManager
 * Gestion interne des trains :
 *  - tableau TrainState
 *  - accumulation des mesures
 *  - validation 5/5
 *  - comparaison JSON
 *  - gestion du silence
 *  - réarmement après expiration
 *  - revalidation après redémarrage
 */

class CLF_TrainManager
{
public:
    /* ------------------------------------------------------------
     * Initialisation
     * ------------------------------------------------------------ */
    static void begin();                 // Reset interne + chargement JSON
    static void resetTrain(uint16_t id); // Reset d’un train spécifique

    /* ------------------------------------------------------------
     * Mise à jour des mesures
     * ------------------------------------------------------------ */
    static void updateVitesse(uint16_t trainID, float vitesse, uint16_t cantonID);
    static void updateEssieux(uint16_t trainID, uint8_t essieux, uint16_t cantonID);

    /* ------------------------------------------------------------
     * Validation
     * ------------------------------------------------------------ */
    static void checkValidation(uint16_t trainID);

    /* ------------------------------------------------------------
     * Silence CC
     * ------------------------------------------------------------ */
    static void applySilence(uint16_t trainID);
    static void checkSilenceExpiration(); // À appeler dans loop()

    /* ------------------------------------------------------------
     * Redémarrage
     * ------------------------------------------------------------ */
    static void requestAllMeasures(); // Demande mesure pour tous les trains du JSON

    /* ------------------------------------------------------------
     * Accès interne
     * ------------------------------------------------------------ */
    static TrainState* getTrain(uint16_t trainID);

    /* ------------------------------------------------------------
     * Mise à jour de profil de voie (N / HO)
     * ------------------------------------------------------------ */
    static void updateTrackProfile(uint8_t profile);

private:
    static TrainState trains[CLF_Variables::MAX_TRAINS];

    static TrainState* findOrCreate(uint16_t trainID);
};
