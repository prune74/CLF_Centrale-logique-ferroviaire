#include "CLF_TrainManager.h"
#include "CLF_Config.h"
#include "CLF_CAN.h"
#include <Protocol.h>
#include "CLF_Variables.h"
#include "CLF_JsonStorage.h"

/* ------------------------------------------------------------
 * Tableau interne des trains
 * ------------------------------------------------------------ */
TrainState CLF_TrainManager::trains[CLF_Variables::MAX_TRAINS];

/* ------------------------------------------------------------
 * Initialisation
 * ------------------------------------------------------------ */
void CLF_TrainManager::begin()
{
    // Charger le JSON
    CLF_JsonStorage::load(trains);

    // Marquer tous les trains comme "doitRevalider"
    for (uint8_t i = 0; i < CLF_Variables::MAX_TRAINS; i++)
    {
        trains[i].doitRevalider = true;
        trains[i].silenceActif = false;
        trains[i].countVitesse = 0;
        trains[i].countEssieux = 0;

        // Reset cantonHistory
        for (uint8_t j = 0; j < CLF_Variables::NB_MESURES_VALIDATION; j++)
            trains[i].cantonHistory[j] = 0;
    }

    // Demander une mesure à tous les trains connus
    requestAllMeasures();
}

/* ------------------------------------------------------------
 * Reset d’un train
 * ------------------------------------------------------------ */
void CLF_TrainManager::resetTrain(uint16_t trainID)
{
    TrainState *t = findOrCreate(trainID);
    if (!t)
        return;

    t->countVitesse = 0;
    t->countEssieux = 0;
    t->vitesseValidee = false;
    t->essieuxValide = false;
    t->doitRevalider = true;
    t->silenceActif = false;

    for (uint8_t i = 0; i < CLF_Variables::NB_MESURES_VALIDATION; i++)
        t->cantonHistory[i] = 0;
}

/* ------------------------------------------------------------
 * Mise à jour vitesse
 * ------------------------------------------------------------ */
void CLF_TrainManager::updateVitesse(uint16_t trainID, float vitesse, uint16_t cantonID)
{
    TrainState *t = findOrCreate(trainID);
    if (!t)
        return;

    if (t->vitesseValidee)
        return;

    uint8_t idx = t->countVitesse;

    if (idx < CLF_Variables::NB_MESURES_VALIDATION)
    {
        t->vitesseMesures[idx] = vitesse;
        t->cantonHistory[idx] = cantonID;

        t->countVitesse++;

        if (t->countVitesse == CLF_Variables::NB_MESURES_VALIDATION)
        {
            t->vitesseValidee = true;
            LOG_INFO("Train %u → vitesse validée (%u mesures)", trainID, t->countVitesse);
        }
    }
}

/* ------------------------------------------------------------
 * Mise à jour essieux
 * ------------------------------------------------------------ */
void CLF_TrainManager::updateEssieux(uint16_t trainID, uint8_t essieux, uint16_t cantonID)
{
    TrainState *t = findOrCreate(trainID);
    if (!t)
        return;

    if (t->essieuxValide)
        return;

    uint8_t idx = t->countEssieux;

    if (idx < CLF_Variables::NB_MESURES_VALIDATION)
    {
        t->essieuxMesures[idx] = essieux;
        t->cantonHistory[idx] = cantonID;

        t->countEssieux++;

        if (t->countEssieux == CLF_Variables::NB_MESURES_VALIDATION)
        {
            t->essieuxValide = true;
            LOG_INFO("Train %u → essieux validés (%u mesures)", trainID, t->countEssieux);
        }
    }
}

/* ------------------------------------------------------------
 * Validation 5/5
 * ------------------------------------------------------------ */
void CLF_TrainManager::checkValidation(uint16_t trainID)
{
    TrainState *t = findOrCreate(trainID);
    if (!t)
        return;

    if (!t->vitesseValidee || !t->essieuxValide)
        return;

    LOG_INFO("Train %u → validation complète vitesse + essieux", trainID);

    // Affichage topologie des cantons
    char buf[64];
    char *p = buf;
    p += sprintf(p, "[");

    for (uint8_t i = 0; i < CLF_Variables::NB_MESURES_VALIDATION; i++)
    {
        p += sprintf(p, "%u", t->cantonHistory[i]);
        if (i < CLF_Variables::NB_MESURES_VALIDATION - 1)
            p += sprintf(p, ",");
    }

    sprintf(p, "]");
    LOG_INFO("Train %u → cantonHistory = %s", trainID, buf);

    // Calcul vitesse finale
    float somme = 0;
    for (uint8_t i = 0; i < CLF_Variables::NB_MESURES_VALIDATION; i++)
        somme += t->vitesseMesures[i];

    float vitesseFinale = somme / CLF_Variables::NB_MESURES_VALIDATION;

    uint8_t essieuxFinal = t->essieuxMesures[CLF_Variables::NB_MESURES_VALIDATION - 1];
    bool essieuxOntChange = (essieuxFinal != t->essieuxFinal);

    t->vitesseFinale = vitesseFinale;
    t->essieuxFinal = essieuxFinal;
    t->timestampValidation = millis();

    // Calibration CV5 (dépend du profil N / HO)
    float vitesseCible =
        (CLF_TRACK_PROFILE == TrackProfile::HO)
            ? CLF_Variables::VITESSE_CIBLE_VOIE_LIBRE_HO
            : CLF_Variables::VITESSE_CIBLE_VOIE_LIBRE_N;

    float vitesseMesuree = vitesseFinale;

    if (vitesseMesuree > 0.1f)
    {
        uint8_t cv5_old = t->cv5;
        float ratio = vitesseCible / vitesseMesuree;
        float cv5_new_f = cv5_old * ratio;

        if (cv5_new_f < 1.0f)
            cv5_new_f = 1.0f;
        if (cv5_new_f > 255.0f)
            cv5_new_f = 255.0f;

        uint8_t cv5_new = (uint8_t)(cv5_new_f + 0.5f);

        CLF_CAN::sendWriteCV_Main(trainID, 5, cv5_new);

        LOG_INFO("Train %u → calibration CV5: old=%u new=%u (mesurée=%.2f, cible=%.2f)",
                 trainID, cv5_old, cv5_new, vitesseMesuree, vitesseCible);

        t->cv5 = cv5_new;
        CLF_JsonStorage::saveTrain(*t);
    }
    else
    {
        LOG_WARN("Train %u → vitesse mesurée trop faible (%.2f) → calibration CV5 ignorée",
                 trainID, vitesseMesuree);
    }

    if (essieuxOntChange)
    {
        LOG_INFO("Train %u → changement d’essieux détecté (%u → %u)",
                 trainID, t->essieuxFinal, essieuxFinal);
    }

    applySilence(trainID);

    t->countVitesse = 0;
    t->countEssieux = 0;
    t->vitesseValidee = false;
    t->essieuxValide = false;

    LOG_INFO("Train %u → validation terminée", trainID);
}

/* ------------------------------------------------------------
 * Silence CC
 * ------------------------------------------------------------ */
#include "CLF_Time.h"

void CLF_TrainManager::applySilence(uint16_t trainID)
{
    TrainState *t = findOrCreate(trainID);
    if (!t)
        return;

    // 1) Heure actuelle CLF (epoch)
    uint32_t now = CLF_Time::nowEpoch();

    if (now == 0)
    {
        LOG_WARN("NTP non synchronisé → silence non appliqué");
        return;
    }

    // 2) Calcul expiration (minutes → secondes)
    uint32_t expirationEpoch =
        now + (CLF_Variables::DUREE_SILENCE_MINUTES * 60);

    // 3) Stockage interne CLF
    t->silenceActif = true;
    t->silenceExpireAt = expirationEpoch;

    // 4) Envoi au CC : TRAIN_VALIDE(trainID, expirationEpoch)
    CLF_CAN::sendMsg(
        1,
        (uint8_t)Cmd_CLF_to_CC::TRAIN_VALIDE,
        0, // CC cible
        trainID,
        (expirationEpoch >> 24) & 0xFF,
        (expirationEpoch >> 16) & 0xFF,
        (expirationEpoch >> 8) & 0xFF,
        (expirationEpoch) & 0xFF);

    LOG_INFO("Train %u → silence CC activé jusqu'à epoch=%u",
             trainID, expirationEpoch);
}

void CLF_TrainManager::checkSilenceExpiration()
{
    uint32_t now = millis();

    for (uint8_t i = 0; i < CLF_Variables::MAX_TRAINS; i++)
    {
        TrainState &t = trains[i];

        if (!t.silenceActif)
            continue;

        if (now >= t.silenceExpireAt)
        {
            t.silenceActif = false;

            CLF_CAN::sendMsg(
                1,
                (uint8_t)Cmd_CLF_to_CC::TRAIN_REARMER,
                0,
                t.trainID);

            LOG_INFO("Train %u → silence expiré → réarmement CC", t.trainID);
        }
    }
}

/* ------------------------------------------------------------
 * Redémarrage : demande mesure
 * ------------------------------------------------------------ */
void CLF_TrainManager::requestAllMeasures()
{
    LOG_INFO("CLF → demande de mesures pour tous les trains connus");

    for (uint8_t i = 0; i < CLF_Variables::MAX_TRAINS; i++)
    {
        uint16_t trainID = trains[i].trainID;

        if (trainID == 0)
            continue;

        CLF_CAN::sendMsg(
            1,
            (uint8_t)Cmd_CLF_to_CC::DEMANDE_MESURE,
            0,
            trainID);

        LOG_INFO("CLF → DEMANDE_MESURE envoyée au train %u", trainID);
    }
}

/* ------------------------------------------------------------
 * Accès interne
 * ------------------------------------------------------------ */
TrainState *CLF_TrainManager::getTrain(uint16_t trainID)
{
    return findOrCreate(trainID);
}

TrainState *CLF_TrainManager::findOrCreate(uint16_t trainID)
{
    for (uint8_t i = 0; i < CLF_Variables::MAX_TRAINS; i++)
    {
        if (trains[i].trainID == trainID)
            return &trains[i];
    }

    for (uint8_t i = 0; i < CLF_Variables::MAX_TRAINS; i++)
    {
        if (trains[i].trainID == 0)
        {
            trains[i].trainID = trainID;
            return &trains[i];
        }
    }

    return nullptr;
}

/* ------------------------------------------------------------
 * Mise à jour du profil de voie (N / HO)
 * ------------------------------------------------------------ */
void CLF_TrainManager::updateTrackProfile(uint8_t profile)
{
    CLF_TRACK_PROFILE = (profile == 1 ? TrackProfile::HO : TrackProfile::N);

    LOG_INFO("CLF → SET_PROFILE appliqué : %s",
             (CLF_TRACK_PROFILE == TrackProfile::HO ? "HO (15V)" : "N (12V)"));

    CLF_JsonStorage::saveProfile(CLF_TRACK_PROFILE);
}