#pragma once
#include "CanConfig.h"
#include "Pins.h"

class CLF_Config : public CanConfigProvider
{
public:

    uint8_t busCount() const override { return 1; }

    const CanBusConfig &bus(uint8_t index) const override
    {

        static CanBusConfig cfg[1] = {

            // -----------------------------------------------------------------
            // CAN0 : ESP32 interne (TWAI) → CLF
            // -----------------------------------------------------------------
            {
                true,        // enabled
                CAN_BITRATE, // speed
                PIN_CAN_TX, // tx_pin
                PIN_CAN_RX, // rx_pin

                GPIO_NUM_NC, // cs_pin
                GPIO_NUM_NC, // int_pin
                GPIO_NUM_NC, // sck_pin
                GPIO_NUM_NC, // mosi_pin
                GPIO_NUM_NC, // miso_pin

                0, // quartz
                0, // tolerance
                false, // Loopback
            },

        };

        return cfg[index];
    }
};

/**
 * Objet global utilisé par l’application pour initialiser le CAN :
 *
 *      CanInit::begin(CLF_CAN_CONFIG);
 *
 * Il doit être défini dans CLF_Config.cpp.
 */
extern CLF_Config CLF_CAN_CONFIG;
