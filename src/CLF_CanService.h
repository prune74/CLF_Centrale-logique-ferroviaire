#pragma once

#include "CanMsg.h"
#include "CanBus.h"
#include "CLF_Variables.h"

/*
 * CLF_CanService.h
 */

class CLF_CanService
{
public:
    // Constructeur : initialise l’état interne du service
    CLF_CanService();

    // Initialisation du service CAN
    bool begin();

    // Boucle principale de réception CAN
    void loop();

    // Supervision du bus CAN
    bool checkBus(uint32_t timeoutMs = 500);
    uint32_t lastRxAgeMs() const;
    bool isCanOK() const { return _canOK; }

    // Accès à la dernière trame reçue (utilisé par ERS)
    bool getLastFrame(CanMsg &msg);

    // Envoi public d’une trame CAN
    bool sendMessage(const CanMsg &msg);

    // Gestion des trames CAN reçues (apelée par CLF_CanService::loop)
    void CLF_handleFrame(const CanMsg &msg);

private:
    // Horodatage de la dernière trame reçue
    uint32_t _lastRxTime;

    // Indique si le bus CAN est considéré comme actif
    bool _canOK;

    // Gestion de la dernière trame reçue
    bool hasNewFrame;
    CanMsg lastFrame;
};
