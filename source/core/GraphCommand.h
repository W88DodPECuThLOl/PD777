#pragma once

#include "catLowBasicTypes.h"

/**
 * @brief   スプライトをラスタライズしたときの１ライン分を描画するコマンド
 */
struct GraphCommand {
    /**
     * @brief ラスタライズしたときの垂直カウンタ
     * 
     * (verticalCounter-24)/4で、描画するスプライトのY座標になる。
     */
    u16 verticalCounter;
    /**
     * @brief 描画するスプライトのデータ
     */
    u8  spriteData[4];

    /**
     * @brief   描画する１ラインのY座標を取得する
     * @return  描画する1ラインのY座標
     */
    inline const u16 getDrawLine() const { return (verticalCounter - 24) / 4; }

    // ------------------------------------
    // スプライトの色々な値を取得する関数
    // ------------------------------------

    /**
     * @brief   スプライトのプライオリティを取得する
     * @return  スプライトのプライオリティ(0～1)
     * @retval      0: プライオリティ低い
     * @retval      1: プライオリティ高い(手前に表示される)
     */
    inline const u8 getSpritePrio() const { return spriteData[0] & 1; }

    /**
     * @brief   スプライトのY座標を取得する
     * @return  スプライトのY座標
     */
    inline const u8 getSpriteY() const { return spriteData[0] >> 1; }

    /**
     * @brief   スプライトのX座標を取得する
     * @return  スプライトのX座標
     */
    inline const u8 getSpriteX() const { return spriteData[1]; }

    /**
     * @brief   スプライトのパターンを取得する
     * @return  スプライトのパターン
     */
    inline const u8 getSpritePattern() const { return spriteData[2]; }

    /**
     * @brief   スプライトのパターンを読み込むときのオフセット位置を取得する
     * @note    Yリピート機能は、HBLK中にこの値とY座標をずらすことで実装している
     * @return  パターンを読み込むときのオフセット位置(0～7)
     */
    inline const u8 getSpritePatternRomAddressOffset() const {
        const u8 data3 = spriteData[3];
        const u8 y     = (data3 >> 4) & 0x7;
        const u8 ySUB  = data3 & 1;
        return (y - ySUB) & 0x7;
    }

    inline const u8 getSpriteColor() const {
        const auto PRIO = getSpritePrio();
        const auto data3 = spriteData[3];
        return ((data3 >> 1) & 7) | (PRIO << 3); // | forgroundColor; // prio rgb
    }
    inline const u8 getSpriteBentType() const {
        return (getSpritePattern() & 1) ? 1 : 0;
    }
};
