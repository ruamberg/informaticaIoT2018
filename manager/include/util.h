#ifndef INCLUDE_UTIL_H
#define INCLUDE_UTIL_H

#include "Arduino.h"
#include <ESP8266WiFi.h>

#define TOKEN_LENGTH 10

namespace Util
{
    String securityToken = "";
    String randomChars = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

    inline void updateToken(void)
    {
        securityToken = "";
        randomSeed(analogRead(0));

        for (int i = 0; i < TOKEN_LENGTH; i++)
        {
            int x = random(randomChars.length());
            securityToken += randomChars.charAt(x);
        }
    }

    inline String getToken(void)
    {
        return securityToken;
    }
}

#endif