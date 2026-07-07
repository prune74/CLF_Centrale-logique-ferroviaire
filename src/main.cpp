#include <Arduino.h>
#include "CLF_main.h"
#include "Debug.h"

void taskLogic(void *param)
{
    while (true)
    {
        CLF_loop();   // CAN + WebHandler.loop()
        vTaskDelay(10);
    }
}

void taskWeb(void *param)
{
    while (true)
    {
        Debug_handleSerialCommand();  // Série sur core 0
        vTaskDelay(1);
    }
}

void setup()
{
    Serial.begin(115200);
    delay(200);

    LOG_INFO("==========================================");
    LOG_INFO("   CLF - CENTRALE LOGIQUE FERROVIAIRE");
    LOG_INFO("==========================================");
    LOG_INFO("DebugLevel initial = %u", DEBUG_LEVEL);

    CLF_setup();

    LOG_INFO("Système CLF prêt");

    // 🟦 Tâche logique ferroviaire (Core 1)
    xTaskCreatePinnedToCore(
        taskLogic,
        "LogicTask",
        8192,
        NULL,
        1,
        NULL,
        1 // Core 1
    );

    // 🟦 Tâche Web / Debug série (Core 0)
    xTaskCreatePinnedToCore(
        taskWeb,
        "WebTask",
        8192,
        NULL,
        1,
        NULL,
        0 // Core 0
    );
}

void loop()
{
    // Ne sert plus à rien → FreeRTOS gère tout
    vTaskDelay(1000);
}
