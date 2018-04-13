#ifndef INCLUDE_DATA_H
#define INCLUDE_DATA_H

#include "Arduino.h"
#include "./storage.h"
#include <ArduinoJson.h>

namespace Data
{
    int sensors[3] = {0, 0, 0};
    bool isConnected = false;

    namespace
    {
        inline String ssid(void)
        {
            String ssid;
            String password;

            Storage::get(&ssid, &password);
            return ssid;
        }

        inline String sensorsListAsString(void)
        {
            String sensorsInfo = "[";
            int numOfSensors = sizeof(sensors) / sizeof(int);

            for (int i = 0; i < numOfSensors; i++)
            {
                sensorsInfo += String(sensors[i]);
                if (i < numOfSensors - 1)
                    sensorsInfo += ",";
            }

            sensorsInfo += "]";

            return sensorsInfo;
        }
    }

    inline String asJson(void)
    {
        return "{\"sensors\": " + sensorsListAsString() + ", \"connection\": {\"flag\": " + (isConnected ? "true, \"ssid\": \"" + ssid() + "\"}" : "false}") + "}";
    }

    inline void parse(String json)
    {
        StaticJsonBuffer<200> jsonBuffer;
        JsonObject &root = jsonBuffer.parseObject(json);
    }

    inline String itemAsJson(String keyword)
    {
        if(keyword == "sensors")
        {
            return "{\"sensors\": " + sensorsListAsString() + "}";
        }
        else if(keyword == "connection")
        {
            return "{\"connection\": {\"flag\": " + (isConnected? "true, \"ssid\": \"" + ssid() + "\"}":"false}") + "}";
        }

        return "{}";
    }
}

#endif