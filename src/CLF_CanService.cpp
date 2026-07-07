/*
 * CLF_CanService.cpp
 */

#include "CLF_CanService.h"
#include "Pins.h"
#include "CLF_Settings.h"

#include "Variables.h"

#include "Protocol.h"
#include "CanBus.h"
#include "Debug.h"


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
 * Gestion des trames CAN reçues (apelée par CLF_CanService::loop)
 * ------------------------------------------------------------------------- */
void CLF_CanService::CLF_handleFrame(const CanMsg &msg)
{
    // 🔥 TODO : implémenter la logique de traitement des trames CAN
    LOG_INFO("CLF → trame CAN reçue : ID=0x%X len=%u", msg.id, msg.dlc);
}