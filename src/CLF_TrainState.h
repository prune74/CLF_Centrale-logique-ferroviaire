#pragma once
#include <stdint.h>
#include "CLF_Config.h"

/*
 * Structure interne représentant l'état d'un train dans la CLF.
 * Séparée en deux parties :
 *
 * 1) Données VOLATILES (jamais enregistrées dans le JSON)
 *    - buffers de mesures
 *    - compteurs
 *    - états de validation
 *    - cantonHistory (topologie des cantons utilisés)
 *
 * 2) Données PERSISTANTES (enregistrées dans le JSON)
 *    - essieuxFinal
 *    - vitesseFinale
 *    - cv5
 *    - silenceActif
 *    - silenceExpireAt
 *    - timestampValidation
 */

struct TrainState
{
    /* ------------------------------------------------------------
     * IDENTITÉ DU TRAIN (persistant)
     * ------------------------------------------------------------ */
    uint16_t trainID = 0;

    /* ------------------------------------------------------------
     * DONNÉES VOLATILES (jamais JSON)
     * ------------------------------------------------------------ */

    // Accumulation des mesures
    uint8_t countVitesse = 0;
    uint8_t countEssieux = 0;

    float vitesseMesures[CLF_Variables::NB_MESURES_VALIDATION] = {0};
    uint8_t essieuxMesures[CLF_Variables::NB_MESURES_VALIDATION] = {0};

    // Validation interne
    bool vitesseValidee = false;
    bool essieuxValide = false;

    // Indique si, après redémarrage, on doit refaire une validation 5/5
    bool doitRevalider = false;

    // Historique des cantons utilisés pour les 5 mesures
    uint16_t cantonHistory[CLF_Variables::NB_MESURES_VALIDATION] = {0};

    /* ------------------------------------------------------------
     * DONNÉES PERSISTANTES (JSON)
     * ------------------------------------------------------------ */

    // vitesse validée (mesurée)
    float vitesseFinale = 0.0f;

    // nombre d'essieux validé
    uint8_t essieuxFinal = 0;

    // CV5 actuelle de la loco (pour calibration vitesse)
    uint8_t cv5 = CLF_Variables::CV5_DEFAULT;

    // les CC doivent arrêter d'envoyer les mesures
    bool silenceActif = false;

    // timestamp d'expiration du silence
    uint32_t silenceExpireAt = 0;

    // date/heure de la dernière validation
    uint32_t timestampValidation = 0;
};
