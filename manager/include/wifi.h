#ifndef INCLUDE_WIFI_H
#define INCLUDE_WIFI_H

#include "Arduino.h"
#include "./ap.h"
#include "./storage.h"
#include <ESP8266WiFi.h>

#define TIMEOUT 10000

namespace Wifi
{
    namespace
    {
        int x = 0;
        String aps[50];
    }

    inline bool isConnected(void)
    {
        return WiFi.status() == WL_CONNECTED;
    }

    inline bool connect(String ssid, String password)
    {
        unsigned long timestamp = 0;
        if(isConnected()) WiFi.disconnect(true);

        WiFi.begin(ssid.c_str(), password.c_str());

        timestamp = millis();
        while((millis() - timestamp) < TIMEOUT)
        {
            if(isConnected())
            {
                Serial.println("CONNECTED");
                break;
            }
            else
            {
                delay(500);
                Serial.println("Waiting for connection");
            }
        }

        if(isConnected())
        {
            if(Storage::exists())
            {
                Storage::clear();
            }
            
            Storage::set(ssid, password);
            return true;
        }

        return false;
    }

    inline bool tryToConnect(void)  
    {
        if (Storage::exists())
        {
            String ssid = "";
            String password = "";

            Storage::get(&ssid, &password);
            return connect(ssid, password);
        }

        return false;
    }

    inline void disconnect(void)
    {
        WiFi.disconnect(true);
    }

    inline void init(void)
    {
        tryToConnect();
        Serial.println("Wifi ok");
    }

    inline void clear(void)
    {
        WiFi.disconnect(true);
        WiFi.softAPdisconnect(true);
        WiFi.mode(WIFI_STA);

        delay(500);

        int n = WiFi.scanNetworks();

        for(int i = 0; i < n && i < 50; i++)
        {
            aps[i] = WiFi.SSID(i);
            x += 1;
        }

        WiFi.mode(WIFI_AP_STA);
    }

    inline String *getWifiList(int *size)
    {
        *size = x;
        return aps;
    }
}

#endif