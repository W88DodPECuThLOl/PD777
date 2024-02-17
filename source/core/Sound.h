#pragma once

#include "catLowBasicTypes.h"

/**
 * @brief サウンド
 */
class Sound {
    /**
     * @brief FLSの値
     * 
     * @note 0と1は無音
     * @note 再生する音の周波数は「15734.0 / (FLS - 1)」(2以上) で計算できる。
     */
    u8 FLS = 1;
    /**
     * @brief FRSの値
     * 
     * @see FLS
     */
    u8 FRS = 1;
    /**
     * @brief 音を再生するときのタイミング用のカウンタ
     * 
     * CPUが実行した命令数
     */
    s64 clockCounter = 0;

    /**
     * @brief MODEのREV(Reverberated Sound Effect)の値
     * 残響音効果音
     */
    bool reverberatedSoundEffect = false;
public:
    /**
     * @brief リセット
     */
    void reset()
    {
        clockCounter = 0;
        FLS = 1;
        FRS = 1;
        reverberatedSoundEffect = false;
    }

    void updateCounter() { ++clockCounter; }
    s64 getClockCounter() const { return clockCounter; }

    /**
     * @brief Lチャンネルのサウンドの音程を設定する
     * @param[in]   value   音程。1は無音
     */
    void setFLS(const u8 value) { FLS = value; }
    const u8 getFLS() const { return FLS; }
    /**
     * @brief Rチャンネルのサウンドの音程を設定する
     * @param[in]   value   音程。1は無音
     */
    void setFRS(const u8 value) { FRS = value; }
    const u8 getFRS() const { return FRS; }

    void setReverberatedSoundEffect(const bool reverberatedSoundEffect) { this->reverberatedSoundEffect = reverberatedSoundEffect; }
    bool getReverberatedSoundEffect() const { return reverberatedSoundEffect; }
};
