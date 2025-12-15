#pragma once

#include "../../core/catLowBasicTypes.h"

class SourceInfo {
    public:
        float freq;
        float phase;

        bool rev;
        u16 revTime;
        float revVolume;
};

class CatAudio {
    public:
        void init() {};
        void present();
        float convert(u8 code);
        void addScore(u8 channel, u8 value, bool reverberatedSoundEffect);
        void writeFLS(const u8 code, const bool reverberatedSoundEffect);
        void writeFRS(const u8 code, const bool reverberatedSoundEffect);

        SourceInfo sourceInfo[2];  // TODO (mittonk): dynamic allocation?

        const float samplesPerSec = 48000.0;
        /**
         * 残響効果の開始時間(秒)
         *
         * @type {number}
         */
        const float REV_EFFECT_NOTE_OFF_TIME = 0.04;

        /**
         * 残響効果の減衰の係数（1に近い程、余韻が長くなる）
         *
         * @type {number}
         */
        const float REV_EFFECT_ATTENUATION = 0.9994;
};
