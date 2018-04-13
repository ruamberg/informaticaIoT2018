#ifndef INCLUDE_STORAGE_H
#define INCLUDE_STORAGE_H

#include <EEPROM.h>
#include "Arduino.h"

namespace Storage
{
    inline void init(void)
    {
        EEPROM.begin(4096);
        Serial.println("Storage ok");
    }

    inline void get(String *ssid, String *password)
    {
        int ssidBegin = EEPROM.read(1);
        int ssidLength = EEPROM.read(0);
        int passwordBegin = EEPROM.read(3);
        int passwordLength = EEPROM.read(2);

        for (int i = 0; i < ssidLength; i++)
            *ssid += (char)EEPROM.read(ssidBegin + i);

        for (int i = 0; i < passwordLength; i++)
            *password += (char)EEPROM.read(passwordBegin + i);
    }

    inline void set(String ssid, String password)
    {
        EEPROM.write(0, ssid.length() & 0x00FF);
        EEPROM.write(1, 4);
        EEPROM.write(2, password.length() & 0x00FF);
        EEPROM.write(3, 4 + ssid.length() & 0x00FF);

        for (int i = 0; i < ssid.length(); i++)
            EEPROM.write(4 + i, ssid.charAt(i));

        for (int i = 0; i < password.length(); i++)
            EEPROM.write(4 + ssid.length() + i, password.charAt(i));

        EEPROM.commit();
    }

    inline bool exists(void)
    {
        return EEPROM.read(0) != 0 && EEPROM.read(0) != 0xFF;
    }

    inline void clear(void)
    {
        if(Storage::exists()) 
        {
            int ssidLength = EEPROM.read(0);
            int passwordLength = EEPROM.read(2);

            for (int i = 0; i < 4 + ssidLength + passwordLength; i++)
            {
                EEPROM.write(i, 0);
            }

            EEPROM.commit();
        }
    }
}

#endif