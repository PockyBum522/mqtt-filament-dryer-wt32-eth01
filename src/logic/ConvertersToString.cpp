#include "ConvertersToString.h"


String ConvertersToString::getThermostatModeAsString(ThermostatModeStates thermostatMode)
{
    switch (thermostatMode)
    {
        case ModeUninitialized:
            return String("uninitialized");

        case ModeOff:
            return String("off");

        case ModeCooling:
            return String("cool");

        case ModeMaintainingRange:
            return String("Maintaining Range");

        case ModeHeating:
            return String("heat");
    }

    return String("ERROR");
}

String ConvertersToString::getFanModeAsString(FanStates fanMode)
{
    switch (fanMode)
    {
        case FanUninitialized:
            return String("Uninitialized");

        case FanAlwaysOn:
            return String("Always On");

        case FanOnAutomatically:
            return String("Automatic");

        case FanAlwaysOff:
            return String("Always Off");
    }

    return String("ERROR");
}
