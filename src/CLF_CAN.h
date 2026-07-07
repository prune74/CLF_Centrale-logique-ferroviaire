#pragma once
#include <stdint.h>
#include "CanMsg.h"
#include "CanID.h"
#include "CanBus.h"

/*
 * ============================================================================
 *  CLF_CAN — Envoi des trames CAN côté CLF (format CanUniversal)
 * ============================================================================
 */

class CLF_CAN
{
public:
    // Envoi brut (compatibilité)
    static void sendMsg(const CanMsg &msg);

    // API simplifiée (0 à 8 octets)
    static void sendMsg(uint8_t prio, uint8_t cmde, uint8_t resp, uint16_t thisTrainId);
    static void sendMsg(uint8_t prio, uint8_t cmde, uint8_t resp, uint16_t thisTrainId,
                        uint8_t d0);
    static void sendMsg(uint8_t prio, uint8_t cmde, uint8_t resp, uint16_t thisTrainId,
                        uint8_t d0, uint8_t d1);
    static void sendMsg(uint8_t prio, uint8_t cmde, uint8_t resp, uint16_t thisTrainId,
                        uint8_t d0, uint8_t d1, uint8_t d2);
    static void sendMsg(uint8_t prio, uint8_t cmde, uint8_t resp, uint16_t thisTrainId,
                        uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3);
    static void sendMsg(uint8_t prio, uint8_t cmde, uint8_t resp, uint16_t thisTrainId,
                        uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3, uint8_t d4);
    static void sendMsg(uint8_t prio, uint8_t cmde, uint8_t resp, uint16_t thisTrainId,
                        uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3, uint8_t d4, uint8_t d5);
    static void sendMsg(uint8_t prio, uint8_t cmde, uint8_t resp, uint16_t thisTrainId,
                        uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3,
                        uint8_t d4, uint8_t d5, uint8_t d6);
    static void sendMsg(uint8_t prio, uint8_t cmde, uint8_t resp, uint16_t thisTrainId,
                        uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3,
                        uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7);

    static void sendWriteCV_Main(uint16_t locoID, uint16_t cv, uint8_t value);
};
