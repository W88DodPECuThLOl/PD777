#include "WinPD777Config.h"
#include <string>

bool
WinPD777Config::doSection(const c8* beginSectionName, const c8* endSectionName)
{
    trim(beginSectionName, endSectionName);
    currentSectionName = std::string(beginSectionName, endSectionName);
    return true;
}

bool
WinPD777Config::doParameter(const c8* beginParameterName, const c8* endParameterName, const c8* beginParameter, const c8* endParameter)
{
    trim(beginParameterName, endParameterName);
    trim(beginParameter, endParameter);
    std::string parameterName(beginParameterName, endParameterName);
    std::string parameter(beginParameter, endParameter);
    if(currentSectionName == "BGColor") {
        for(int index = 0; index < 64; ++index) {
            char temp[16];
            std::sprintf(temp, "color[%02d]", index);
            if(parameterName == temp) {
                char* endPtr;
                auto value = std::strtoul(parameter.c_str(), &endPtr, 16);
                config.BGColor[index] = value & 0xFFFFFF;
                break;
            }
        }
    } else if(currentSectionName == "SpriteColor") {
        for(int index = 0; index < 16; ++index) {
            char temp[16];
            std::sprintf(temp, "color[%02d]", index);
            if(parameterName == temp) {
                char* endPtr;
                auto value = std::strtoul(parameter.c_str(), &endPtr, 16);
                config.SpriteColor[index] = value & 0xFFFFFF;
                break;
            }
        }
    } else if(currentSectionName == "Debug") {
        if(parameterName == "debugDrawHorizontalLines") {
            char* endPtr;
            auto value = std::strtoul(parameter.c_str(), &endPtr, 16);
            config.debugDrawHorizontalLines = !!value;
        } else if(parameterName == "debugDrawVerticalLines") {
            char* endPtr;
            auto value = std::strtoul(parameter.c_str(), &endPtr, 16);
            config.debugDrawVerticalLines = !!value;
        }
    }
    return true;
}

WinPD777Config::WinPD777Config()
    : config()
    , currentSectionName()
{
}
