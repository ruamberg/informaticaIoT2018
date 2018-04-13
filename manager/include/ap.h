#ifndef INCLUDE_AP_H
#define INCLUDE_AP_H

#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>

#include "./socket.h"

#define AP_MDNS     "iotdevice"
#define AP_SSID     "[IoT] Local Device"
#define AP_PASSWORD "iotapaccess"
#define HTTP_PORT   80

namespace AP
{
    ESP8266WebServer server(HTTP_PORT);
    MDNSResponder mdns;

    inline void init(void)
    {
        WiFi.softAPdisconnect(true);
        WiFi.softAP(AP_SSID, AP_PASSWORD);
        
        if (mdns.begin(AP_MDNS, WiFi.localIP()))
        {
            Serial.println("MDNS responder started");
            mdns.addService("http", "tcp", HTTP_PORT);
            mdns.addService("ws", "tcp", WS_PORT);
        }
        else
        {
            Serial.println("MDNS.begin failed");
        }

        mdns.notifyAPChange();

            // server.onNotFound(root);
            // server.on("/save", connectRoute);
            // server.on("/connect", credentialsRoute);

            server.begin();
        Serial.println("AP ok");
    }

    inline void onNotFound(void (*handler)(void))
    {
        server.onNotFound(handler);
    }

    inline void on(String route, HTTPMethod method, void (*handler)(void))
    {
        server.on(route, method, handler);
    }

    inline void doTheLoop(void)
    {
        server.handleClient();
    }
}

#endif