#ifndef INCLUDE_SOCKET_H
#define INCLUDE_SOCKET_H

#include "Arduino.h"
#include <ArduinoJson.h>
#include <WebSocketsServer.h>

#define WS_PORT 3210
#define WS_EVENTS 10

namespace Socket
{
    typedef void(*event_handler_t)(uint8_t, String);

    namespace
    {
        int pointer = 0;
        int port = WS_PORT;
        int size = WS_EVENTS;
        String events[WS_EVENTS];
        event_handler_t handlers[WS_EVENTS];
        WebSocketsServer ws(WS_PORT);

        inline void dispatch(String event, uint8_t client, String payload)
        {
            for (int i = 0; i < pointer; i++)
            {
                if (events[i] == event)
                {
                    handlers[i](client, payload);
                    break;
                }
            }
        }

        inline void handleWs(uint8_t num, WStype_t type, uint8_t *payload, size_t length)
        {
            if (type == WStype_DISCONNECTED)
            {
                Serial.printf("[%u] Disconnected!\r\n", num);
                dispatch("disconnection", num, "");
            }
            else if (type == WStype_CONNECTED)
            {
                IPAddress ip = ws.remoteIP(num);
                Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\r\n", num, ip[0], ip[1], ip[2], ip[3], payload);

                dispatch("connection", num, "");
            }
            else if (type == WStype_TEXT)
            {
                Serial.printf("[%u] get Text: %s\r\n", num, payload);
                StaticJsonBuffer<200> jsonBuffer;
                JsonObject &root = jsonBuffer.parseObject(payload);

                String event = root["event"].asString();
                String payload = root["payload"].asString();

                dispatch(event, num, payload);
            }
            else
            {
                Serial.printf("Invalid WStype [%d]\r\n", type);
            }
        }
    }

    inline void init(void)
    {
        ws.begin();
    }

    inline void on(String event, event_handler_t handler)
    {
        if (pointer < size)
        {
            events[pointer] = event;
            handlers[pointer++] = handler;
        }
    }

    inline void emit(uint8_t client, String event, String payload)
    {
        String request = "{\"event\": \"" + event +
            "\", \"payload\": " + payload + "}";
        ws.sendTXT(client, request);
    }

    inline void broadcast(String event, String payload)
    {
        String request = "{\"event\": \"" + event + 
            "\", \"payload\": " + payload + "}";
        ws.broadcastTXT(request);
    }

    inline void doTheLoop(void)
    {
        ws.loop();
    }

};

#endif