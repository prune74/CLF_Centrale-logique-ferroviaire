#pragma once

#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include <SPIFFS.h>

#include "CLF_WebHandler.h"
#include "CanMsg.h"
#include "CLF_CanService.h"

class CLF_WebHandler
{
public:
    CLF_WebHandler(CLF_CanService *canService);

    void init(uint16_t webPort);
    void loop();
    void pushStatus();

private:
    void CLF_route();

    void CLF_wsEvent(AsyncWebSocket *server,
                     AsyncWebSocketClient *client,
                     AwsEventType type,
                     void *arg,
                     uint8_t *data,
                     size_t len);

    void CLF_handleSimpleMessage(void *arg, uint8_t *data, size_t len);

private:
    AsyncWebServer *_server;
    AsyncWebSocket *_ws;
    CLF_CanService *_can;
};
