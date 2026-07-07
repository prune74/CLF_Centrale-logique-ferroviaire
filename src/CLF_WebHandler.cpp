/*
 * CLF_WebHandler.cpp
 */

#include "CLF_WebHandler.h"
#include "CLF_Settings.h"
#include "CLF_CanService.h"
#include "CLF_Config.h"
#include "CLF_Variables.h"

#include "Debug.h"

// Instances externes
extern CLF_CanService canService;

// États globaux (UI)
extern volatile uint8_t g_stopState;
extern volatile uint8_t g_saveState;
extern volatile uint8_t g_restartState;

/* ---------------------------------------------------------------------------
 * 🧩 CONSTRUCTEUR
 * ------------------------------------------------------------------------- */
CLF_WebHandler::CLF_WebHandler(CLF_CanService *canService)
    : _server(nullptr), _ws(nullptr), _can(canService)
{
}

/* ---------------------------------------------------------------------------
 * 🚀 INITIALISATION DU SERVEUR WEB + WEBSOCKET
 * ------------------------------------------------------------------------- */
void CLF_WebHandler::init(uint16_t webPort)
{
    LOG_INFO("CLF_WebHandler → initialisation (port %u)", webPort);

    _server = new AsyncWebServer(webPort);
    _ws = new AsyncWebSocket("/ws");

    _ws->onEvent(std::bind(&CLF_WebHandler::CLF_wsEvent,
                           this,
                           std::placeholders::_1,
                           std::placeholders::_2,
                           std::placeholders::_3,
                           std::placeholders::_4,
                           std::placeholders::_5,
                           std::placeholders::_6));

    CLF_route();

    _server->addHandler(_ws);
    _server->begin();

    LOG_INFO("CLF_WebHandler → serveur HTTP/WebSocket démarré");
}

void CLF_WebHandler::loop()
{
    _ws->cleanupClients();
}

/* ---------------------------------------------------------------------------
 * 🌐 ÉVÉNEMENTS WEBSOCKET
 * ------------------------------------------------------------------------- */
void CLF_WebHandler::CLF_wsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len)
{
    switch (type)
    {
    case WS_EVT_CONNECT:
        LOG_INFO("CLF_WebHandler → client connecté (ID: %u)", client->id());
        break;
    case WS_EVT_DISCONNECT:
        LOG_INFO("CLF_WebHandler → client déconnecté (ID: %u)", client->id());
        break;
    case WS_EVT_DATA:
        LOG_INFO("CLF_WebHandler → données reçues du client (ID: %u)", client->id());
        CLF_handleSimpleMessage(arg, data, len);
        break;
    default:
        LOG_ERROR("CLF_WebHandler → événement WebSocket inconnu");
        break;
    }
}

/* ---------------------------------------------------------------------------
* Pousser l'état du système
---------------------------------------------------------------------------*/
void CLF_WebHandler::pushStatus()
{
    JsonDocument doc;

    doc["wifi_on"] = CLF_Settings::WIFI_ON;
    doc["can_ok"] = _can->isCanOK();
    doc["last_rx"] = _can->lastRxAgeMs();

    String json;
    serializeJson(doc, json);
    _ws->textAll(json);
}

void CLF_WebHandler::CLF_handleSimpleMessage(void *arg, uint8_t *data, size_t len)
{
    // Convertir les données en String
    String msg;
    msg.reserve(len);
    for (size_t i = 0; i < len; i++)
        msg += (char)data[i];

    LOG_INFO("CLF_WebHandler → message reçu : %s", msg.c_str());

    // Parser JSON
    JsonDocument doc;
    DeserializationError err = deserializeJson(doc, msg);

    if (err)
    {
        LOG_ERROR("CLF_WebHandler → JSON invalide");
        return;
    }

    // Lecture de la commande
    const char *cmd = doc["cmd"].as<const char *>();

    // -----------------------------------------------------------------------
    // 🔥 Commande : PING
    // -----------------------------------------------------------------------
    if (strcmp(cmd, "ping") == 0)
    {
        JsonDocument reply;
        reply["reply"] = "pong";

        String out;
        serializeJson(reply, out);
        _ws->textAll(out);

        LOG_INFO("CLF_WebHandler → PING → PONG");
        return;
    }

    // -----------------------------------------------------------------------
    // 🔥 Commande : WIFI ON/OFF
    // -----------------------------------------------------------------------
    if (strcmp(cmd, "wifi") == 0)
    {
        bool enable = doc["enable"].as<bool>();

        CLF_Settings::WIFI_ON = enable;
        CLF_Settings::writeFile();

        JsonDocument reply; // AJ7
        reply["reply"] = enable ? "wifi_on" : "wifi_off";

        String out;
        serializeJson(reply, out);
        _ws->textAll(out);

        pushStatus();
        LOG_INFO("CLF_WebHandler → WiFi %s", enable ? "activé" : "désactivé");
        return;
    }

    // -----------------------------------------------------------------------
    // 🔥 Commande : ENVOI TRAME CAN
    // -----------------------------------------------------------------------
    if (strcmp(cmd, "send_can") == 0)
    {
        uint32_t id = doc["id"].as<uint32_t>();
        const char *payload = doc["data"].as<const char *>();

        CanMsg msg;
        msg.id = id;
        msg.dlc = strlen(payload);

        for (uint8_t i = 0; i < msg.dlc && i < 8; i++)
            msg.data[i] = payload[i];

        bool ok = _can->sendMessage(msg);

        JsonDocument reply;
        reply["reply"] = ok ? "can_sent" : "can_error";

        String out;
        serializeJson(reply, out);
        _ws->textAll(out);
        pushStatus();

        LOG_INFO("CLF_WebHandler → CAN envoyé : ID=0x%X", id);
        return;
    }

    // -----------------------------------------------------------------------
    // 🔥 Commande inconnue
    // -----------------------------------------------------------------------
    LOG_WARN("CLF_WebHandler → commande inconnue : %s", cmd);
}

/* ---------------------------------------------------------------------------
 * 🌐 ROUTES HTTP
 * ------------------------------------------------------------------------- */
void CLF_WebHandler::CLF_route()
{
    LOG_INFO("CLF_WebHandler → configuration des routes HTTP");

    _server->on("/", HTTP_GET, [](AsyncWebServerRequest *request)
                { request->send(SPIFFS, "/index.html", "text/html"); });

    _server->on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request)
                { request->send(SPIFFS, "/style.css", "text/css"); });

    _server->on("/script.js", HTTP_GET, [](AsyncWebServerRequest *request)
                { request->send(SPIFFS, "/script.js", "text/javascript"); });

    _server->on("/favicon.png", HTTP_GET, [](AsyncWebServerRequest *request)
                { request->send(SPIFFS, "/favicon.png", "image/png"); });

    _server->onNotFound([](AsyncWebServerRequest *request)
                        {
                            LOG_WARN("HTTP 404 → %s", request->url().c_str());
                            request->send(404); });
}
