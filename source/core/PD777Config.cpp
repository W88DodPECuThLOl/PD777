#include "PD777Config.h"

/**
 * @brief コンストラクタ
 */
PD777Config::PD777Config()
    : debugDrawHorizontalLines(false)
    , debugDrawVerticalLines(false)
{
    setDefault();
}

void
PD777Config::setDefault() noexcept
{
    debugDrawHorizontalLines = false;
    debugDrawVerticalLines = false;

#define MakeRGB(r,g,b) ((b) | ((g) << 8) | ((r) << 16))
    // BGカラー
    for(int i = 0; i < 4; ++i) {
                                    // BRIGHTNESS:HUE:BLACK/PRIO:RGB
                                    // 0         : 0 : 0        :000
        BGColor[ 0 + i * 16] = MakeRGB(0x00, 0xAE, 0xEF);  // ブルーシアン
                                    // BRIGHTNESS:HUE:BLACK/PRIO:RGB
                                    // 0         : 0 : 0        :001
        BGColor[ 1 + i * 16] = MakeRGB(0x00, 0x00, 0xFF);  // 青
                                    // BRIGHTNESS:HUE:BLACK/PRIO:RGB
                                    // 0         : 0 : 0        :010
        BGColor[ 2 + i * 16] = MakeRGB(0x00, 0xB0, 0x00);  // 緑
                                    // BRIGHTNESS:HUE:BLACK/PRIO:RGB
                                    // 0         : 0 : 0        :011
        BGColor[ 3 + i * 16] = MakeRGB(0xE4, 0x00, 0x7F);  // マゼンタ
                                    // BRIGHTNESS:HUE:BLACK/PRIO:RGB
                                    // 0         : 0 : 0        :100
        BGColor[ 4 + i * 16] = MakeRGB(235, 67, 168);      // 赤
                                    // BRIGHTNESS:HUE:BLACK/PRIO:RGB
                                    // 0         : 0 : 0        :101
        BGColor[ 5 + i * 16] = MakeRGB(0x00, 0xA0, 0xE9);  // シアン
                                    // BRIGHTNESS:HUE:BLACK/PRIO:RGB
                                    // 0         : 0 : 0        :110
        BGColor[ 6 + i * 16] = MakeRGB(0xFF, 0xFF, 0x00);  // 黄
                                    // BRIGHTNESS:HUE:BLACK/PRIO:BGR
                                    // 0         : 0 : 0        :111
        BGColor[ 7 + i * 16] = MakeRGB(0xF5, 0x82, 0x20);  // オレンジ
    //--------------------------------------------------------------------
    //--------------------------------------------------------------------
                                    // BRIGHTNESS:HUE:BLACK/PRIO:RGB
                                    // 0         : 0 : 1        :000
        BGColor[ 8 + i * 16] = MakeRGB(0x00, 0x00, 0x00);  // 黒
                                    // BRIGHTNESS:HUE:BLACK/PRIO:RGB
                                    // 0         : 0 : 1        :001
        BGColor[ 9 + i * 16] = MakeRGB(0x00, 0x00, 0xFF);  // 青
                                    // BRIGHTNESS:HUE:BLACK/PRIO:RGB
                                    // 0         : 0 : 1        :010
        BGColor[10 + i * 16] = MakeRGB(0x00, 0xB0, 0x00);  // 緑
                                    // BRIGHTNESS:HUE:BLACK/PRIO:RGB
                                    // 0         : 0 : 1        :011
        BGColor[11 + i * 16] = MakeRGB(0xE4, 0x00, 0x7F);  // マゼンタ
                                    // BRIGHTNESS:HUE:BLACK/PRIO:RGB
                                    // 0         : 0 : 1        :100
        BGColor[12 + i * 16] = MakeRGB(235, 67, 168);      // 赤
                                    // BRIGHTNESS:HUE:BLACK/PRIO:RGB
                                    // 0         : 0 : 1        :101
        BGColor[13 + i * 16] = MakeRGB(0x00, 0xA0, 0xE9);  // シアン
                                    // BRIGHTNESS:HUE:BLACK/PRIO:RGB
                                    // 0         : 0 : 1        :110
        BGColor[14 + i * 16] = MakeRGB(0xFF, 0xFF, 0x00);  // 黄
                                    // BRIGHTNESS:HUE:BLACK/PRIO:BGR
                                    // 0         : 0 : 1        :111
        BGColor[15 + i * 16] = MakeRGB(0xFF, 0xFF, 0xFF);  // 白
    }

    // スプライトカラー
    SpriteColor[ 0] = MakeRGB(0x00, 0x00, 0x00);  // 0:000
    SpriteColor[ 1] = MakeRGB(132, 195, 204);     // 0:001 メモ）1:001と同じ色
    SpriteColor[ 2] = MakeRGB(176, 223,  11);     // 0:010
    SpriteColor[ 3] = MakeRGB(98, 206, 170);      // 0:011
    SpriteColor[ 4] = MakeRGB(255, 129, 196);     // 0:100
    SpriteColor[ 5] = MakeRGB(208, 136, 203);     // 0:101
    SpriteColor[ 6] = MakeRGB(255, 161, 31);      // 0:110
    SpriteColor[ 7] = MakeRGB(0xD0, 0xD0, 0xD0);  // 0:111 メモ）1:111と同じ色
    SpriteColor[ 8] = MakeRGB(0x00, 0x00, 0x00);  // 1:000
    SpriteColor[ 9] = MakeRGB(132, 195, 204);     // 1:001 メモ）0:001と同じ色
    SpriteColor[10] = MakeRGB(176, 223, 11);      // 1:010
    SpriteColor[11] = MakeRGB(164, 234, 150);     // 1:011
    SpriteColor[12] = MakeRGB(255, 135, 190);     // 1:100
    SpriteColor[13] = MakeRGB(198, 177, 255);     // 1:101
    SpriteColor[14] = MakeRGB(255, 160, 34);      // 1:110
    SpriteColor[15] = MakeRGB(0xD0, 0xD0, 0xD0);  // 1:111 メモ）0:111と同じ色
#undef MakeRGB
}
