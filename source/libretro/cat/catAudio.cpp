#include <math.h>

#include "catAudio.h"
#include "../libretro.h"

extern retro_audio_sample_t audio_cb;

extern unsigned phase;

void CatAudio::writeFLS(const u8 code, const bool reverberatedSoundEffect) {
    addScore(0, code, reverberatedSoundEffect);
};

void CatAudio::writeFRS(const u8 code, const bool reverberatedSoundEffect) {
    addScore(1, code, reverberatedSoundEffect);
};

float CatAudio::convert(u8 value) {
    if(value >= 2) {
        return 15734.0 / (value - 1);
    } else {
        return 0.0;
    }
}

void CatAudio::addScore(u8 channel, u8 value, bool reverberatedSoundEffect) {
    SourceInfo &src = sourceInfo[channel];
    src.freq = convert(value);

    if(src.rev != reverberatedSoundEffect) {
        src.rev = reverberatedSoundEffect;
        if(src.rev) {
            // REV on
            src.revTime = 0;
            src.revVolume = 1.0;
        }
    }
};

void CatAudio::present() {
    // const len = outputs[0][0].length;
    float masterVolume = 0.5; // 0.3;
    float v = 0.0;
    float value = 0.0;
    const u16 len = samplesPerSec / 60;

    for(u16 index = 0; index < len; ++index) {
        value = 0.0;
        for(u16 ch = 0; ch < 2; ++ch) {
            SourceInfo &src = sourceInfo[ch];
            if(src.freq > 0.0) {
                v = (src.phase > 3.141592653) ? 0.0 : 0.12;
                const float step = src.freq *(2.0*3.141592653) / samplesPerSec;
                src.phase += step;
                while(src.phase > 2.0 * 3.141592653) {
                    src.phase -= 2.0 * 3.141592653;
                }

                // REVの処理
                if(src.rev) {
                    v *= src.revVolume;
                    if(src.revTime++ > samplesPerSec * REV_EFFECT_NOTE_OFF_TIME) {
                        src.revVolume *= REV_EFFECT_ATTENUATION;
                    }
                }

                // 合成
                value += v * ((ch == 1) ? 0.25 : 1.0);
            }
        }
        value *= masterVolume;
        u16 sample = 0x800 * value;
        audio_cb(sample, sample);

    }


};
