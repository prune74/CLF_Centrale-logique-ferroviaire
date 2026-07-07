/*
 * CLF_CanService.cpp
 */

#include "CLF_CanService.h"
#include "Pins.h"
#include "CLF_Settings.h"

#include "CLF_Variables.h"

#include "Protocol.h"
#include "CanBus.h"
#include "Debug.h"

#include "CLF_TrainManager.h"
#include "CLF_Config.h"
#include "CLF_JsonStorage.h"

/* ---------------------------------------------------------------------------
 * CONSTRUCTEUR
 * ------------------------------------------------------------------------- */
CLF_CanService::CLF_CanService()
{
    hasNewFrame = false;
    _lastRxTime = millis();
    _canOK = false;
}

/* ---------------------------------------------------------------------------
 * INITIALISATION
 * ------------------------------------------------------------------------- */
bool CLF_CanService::begin()
{
    LOG_INFO("CLF → CAN opérationnel");
    return true;
}

/* ---------------------------------------------------------------------------
 * RÉCEPTION CAN
 * ------------------------------------------------------------------------- */
void CLF_CanService::loop()
{
    CanMsg msg;

    if (CAN[0] && CAN[0]->receive(msg))
    {
        _lastRxTime = millis();
        _canOK = true;

        lastFrame = msg;
        hasNewFrame = true;

        LOG_INFO("CAN RX → ID=0x%X len=%u", msg.id, msg.dlc);

        CLF_handleFrame(msg);
    }
}

/* ---------------------------------------------------------------------------
 * SUPERVISION BUS
 * ------------------------------------------------------------------------- */
bool CLF_CanService::checkBus(uint32_t timeoutMs)
{
    uint32_t age = millis() - _lastRxTime;
    _canOK = (age <= timeoutMs);

    if (!_canOK)
        LOG_WARN("CLF → aucune trame CAN depuis %u ms", age);

    return _canOK;
}

uint32_t CLF_CanService::lastRxAgeMs() const
{
    return millis() - _lastRxTime;
}

/* ---------------------------------------------------------------------------
 * ACCÈS À LA DERNIÈRE TRAME
 * ------------------------------------------------------------------------- */
bool CLF_CanService::getLastFrame(CanMsg &msg)
{
    if (hasNewFrame)
    {
        msg = lastFrame;
        hasNewFrame = false;
        return true;
    }
    return false;
}

/* ---------------------------------------------------------------------------
 * ENVOI PUBLIC
 * ------------------------------------------------------------------------- */
bool CLF_CanService::sendMessage(const CanMsg &msg)
{
    if (CAN[0])
        return CAN[0]->send(msg);

    return false;
}

/* ---------------------------------------------------------------------------
 * Gestion des trames CAN reçues (appelée par CLF_CanService::loop)
 * ------------------------------------------------------------------------- */
void CLF_CanService::CLF_handleFrame(const CanMsg &msg)
{
    LOG_INFO("CLF → trame CAN reçue : ID=0x%X len=%u", msg.id, msg.dlc);

    switch (msg.id)
    {
    // ------------------------------------------------------------
    // 0xA0 → MESURE_VITESSE (CC → CLF)
    // ------------------------------------------------------------
    case (uint16_t)Cmd_CC_to_CLF::MESURE_VITESSE:
    {
        if (msg.dlc < 5)
            break;

        uint16_t cantonID = msg.data[0];
        uint16_t locoID = (msg.data[1] << 8) | msg.data[2];
        uint16_t v1000 = (msg.data[3] << 8) | msg.data[4];

        float vitesse = (float)v1000 / 1000.0f;

        LOG_INFO("CLF → vitesse reçue : canton=%u loco=%u v=%.3f",
                 cantonID, locoID, vitesse);

        // Mise à jour interne CLF
        CLF_TrainManager::updateVitesse(locoID, vitesse, cantonID);

        break;
    }

    // ------------------------------------------------------------
    // 0xA1 → ESSIEUX_TRAIN (CC → CLF)
    // ------------------------------------------------------------
    case (uint16_t)Cmd_CC_to_CLF::ESSIEUX_TRAIN:
    {
        if (msg.dlc < 4)
            break;

        uint16_t cantonID = msg.data[0];
        uint16_t locoID = (msg.data[1] << 8) | msg.data[2];
        uint8_t essieux = msg.data[3];

        LOG_INFO("CLF → essieux reçus : canton=%u loco=%u essieux=%u",
                 cantonID, locoID, essieux);

        // Mise à jour interne CLF
        CLF_TrainManager::updateEssieux(locoID, essieux, cantonID);

        break;
    }

    // ------------------------------------------------------------
    // 0x20 → SET_PROFILE (ERM → CC → CLF)
    // ------------------------------------------------------------
    case (uint16_t)Cmd_ERM_to_CC::SET_PROFILE:
    {
        if (msg.dlc < 1)
            break;

        uint8_t profile = msg.data[0];

        LOG_INFO("CLF → SET_PROFILE reçu : %s",
                 (profile == 1 ? "HO (15V)" : "N (12V)"));

        CLF_TrainManager::updateTrackProfile(profile);

        break;
    }

    default:
        break;
    }
}
