#include "Decoder.h"

void
Decoder::execute(const u16 pc, const u16 code)
{
    for(const auto& matchPattern : matchPatterns) {
        if((code & matchPattern.mask) == matchPattern.value) {
            (this->*matchPattern.execute)(pc, code);
            break;
        }
    }
}
