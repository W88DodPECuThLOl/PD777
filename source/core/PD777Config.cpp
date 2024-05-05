#include "PD777Config.h"
#include "catColorHSVtoRGB.h"

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
    {
        // BRIGHTNESS: 0
        // HUE       : 0
                                    // BRIGHTNESS:HUE:BLACK/PRIO:RGB
                                    // 0         : 0 : 0        :000
        BGColor[0b0'0'0'000] = // MakeRGB(0x67, 0xAB, 0xF6);  // ブルーシアン
        cat::convertHSVtoRGB888(0.58f, 0.58f, 0.96f);
                                    // BRIGHTNESS:HUE:BLACK/PRIO:RGB
                                    // 0         : 0 : 0        :001
        BGColor[0b0'0'0'001] = // MakeRGB(0x28, 0x2B, 0x8A);  // 青
        cat::convertHSVtoRGB888(0.66f, 0.71f, 0.54f);
                                    // BRIGHTNESS:HUE:BLACK/PRIO:RGB
                                    // 0         : 0 : 0        :010
        BGColor[0b0'0'0'010] = // MakeRGB(0x6B, 0xBF, 0x6F);  // 緑
        cat::convertHSVtoRGB888(0.34f, 0.43f, 0.74f);
                                    // BRIGHTNESS:HUE:BLACK/PRIO:RGB
                                    // 0         : 0 : 0        :011
        BGColor[0b0'0'0'011] = // MakeRGB(0x60, 0xBA, 0xC5);  // シアン
        cat::convertHSVtoRGB888(0.51f, 0.51f, 0.77f);
                                    // BRIGHTNESS:HUE:BLACK/PRIO:RGB
                                    // 0         : 0 : 0        :100
        BGColor[0b0'0'0'100] = // MakeRGB(0xB6, 0x59, 0x4F);      // 赤
        cat::convertHSVtoRGB888(0.01f, 0.56f, 0.71f);
                                    // BRIGHTNESS:HUE:BLACK/PRIO:RGB
                                    // 0         : 0 : 0        :101
        BGColor[0b0'0'0'101] = // MakeRGB(0xE6, 0x91, 0xE0);  // マゼンタ
        cat::convertHSVtoRGB888(0.84f, 0.36f, 0.90f);
                                    // BRIGHTNESS:HUE:BLACK/PRIO:RGB
                                    // 0         : 0 : 0        :110
        BGColor[0b0'0'0'110] = // MakeRGB(0xB4, 0xAE, 0x52);  // 黄
        cat::convertHSVtoRGB888(0.15f, 0.54f, 0.70f);
                                    // BRIGHTNESS:HUE:BLACK/PRIO:BGR
                                    // 0         : 0 : 0        :111
        BGColor[0b0'0'0'111] = // MakeRGB(0xE5, 0x9F, 0x56);  // オレンジ
        cat::convertHSVtoRGB888(0.08f, 0.62f, 0.89f);
                                    // BRIGHTNESS:HUE:BLACK/PRIO:RGB
                                    // 0         : 0 : 1        :000
        BGColor[0b0'0'1'000] = MakeRGB(0x00, 0x00, 0x00);   // 黒
                                    // BRIGHTNESS:HUE:BLACK/PRIO:RGB
                                    // 0         : 0 : 1        :001
        BGColor[0b0'0'1'001] = // MakeRGB(0x28, 0x2B, 0x8A);  // 青
        cat::convertHSVtoRGB888(0.66f, 0.71f, 0.54f);
                                    // BRIGHTNESS:HUE:BLACK/PRIO:RGB
                                    // 0         : 0 : 1        :010
        BGColor[0b0'0'1'010] = // MakeRGB(0x6B, 0xBF, 0x6F);  // 緑
        cat::convertHSVtoRGB888(0.34f, 0.43f, 0.74f);
                                    // BRIGHTNESS:HUE:BLACK/PRIO:RGB
                                    // 0         : 0 : 1        :011
        BGColor[0b0'0'1'011] = // MakeRGB(0x60, 0xBA, 0xC5);  // シアン
        cat::convertHSVtoRGB888(0.51f, 0.51f, 0.77f);
                                    // BRIGHTNESS:HUE:BLACK/PRIO:RGB
                                    // 0         : 0 : 1        :100
        BGColor[0b0'0'1'100] = // MakeRGB(0xB6, 0x59, 0x4F);  // 赤
        cat::convertHSVtoRGB888(0.01f, 0.56f, 0.71f);
                                    // BRIGHTNESS:HUE:BLACK/PRIO:RGB
                                    // 0         : 0 : 1        :101
        BGColor[0b0'0'1'101] = // MakeRGB(0xE6, 0x91, 0xE0);  // マゼンタ
        cat::convertHSVtoRGB888(0.84f, 0.36f, 0.90f);
                                    // BRIGHTNESS:HUE:BLACK/PRIO:RGB
                                    // 0         : 0 : 1        :110
        BGColor[0b0'0'1'110] = // MakeRGB(0xB4, 0xAE, 0x52);  // 黄
        cat::convertHSVtoRGB888(0.15f, 0.54f, 0.70f);
                                    // BRIGHTNESS:HUE:BLACK/PRIO:BGR
                                    // 0         : 0 : 1        :111
        BGColor[0b0'0'1'111] = // MakeRGB(0xA6, 0xA6, 0xA6);  // 灰色
        cat::convertHSVtoRGB888(0.0f, 0.0f, 0.65f);
    //--------------------------------------------------------------------
    //--------------------------------------------------------------------
        // BRIGHTNESS: 0
        // HUE       : 1
                                    // BRIGHTNESS:HUE:BLACK/PRIO:RGB
                                    // 0         : 1 : 0        :000
        BGColor[0b0'1'0'000] = // MakeRGB(0x4C, 0xB5, 0xFF);  // ブルーシアン
        cat::convertHSVtoRGB888(0.56f, 0.70f, 1.0f);
                                    // BRIGHTNESS:HUE:BLACK/PRIO:RGB
                                    // 0         : 1 : 0        :001
        BGColor[0b0'1'0'001] = // MakeRGB(0x1D, 0x27, 0xDD);  // 青
        cat::convertHSVtoRGB888(0.65f, 0.86f, 0.86f);
                                    // BRIGHTNESS:HUE:BLACK/PRIO:RGB
                                    // 0         : 1 : 0        :010
        BGColor[0b0'1'0'010] = // MakeRGB(0x30, 0xDA, 0x38);  // 緑
        cat::convertHSVtoRGB888(0.34f, 0.77f, 0.85f);
                                    // BRIGHTNESS:HUE:BLACK/PRIO:RGB
                                    // 0         : 1 : 0        :011
        BGColor[0b0'1'0'011] = // MakeRGB(0x1C, 0xCE, 0xE5);  // シアン
        cat::convertHSVtoRGB888(0.51f, 0.87f, 0.89f);
                                    // BRIGHTNESS:HUE:BLACK/PRIO:RGB
                                    // 0         : 1 : 0        :100
        BGColor[0b0'1'0'100] = // MakeRGB(0xFF, 0x48, 0x31);      // 赤
        cat::convertHSVtoRGB888(0.01f, 0.80f, 1.0f);
                                    // BRIGHTNESS:HUE:BLACK/PRIO:RGB
                                    // 0         : 1 : 0        :101
        BGColor[0b0'1'0'101] = // MakeRGB(0xFF, 0x78, 0xFF);  // マゼンタ
        cat::convertHSVtoRGB888(0.83f, 0.52f, 1.0f);
                                    // BRIGHTNESS:HUE:BLACK/PRIO:RGB
                                    // 0         : 1 : 0        :110
        BGColor[0b0'1'0'110] = // MakeRGB(0xC0, 0xB3, 0x00);  // 黄
        cat::convertHSVtoRGB888(0.15f, 1.0f, 0.75f);
                                    // BRIGHTNESS:HUE:BLACK/PRIO:BGR
                                    // 0         : 1 : 0        :111
        BGColor[0b0'1'0'111] = // MakeRGB(0xFF, 0x9B, 0x27);  // オレンジ
        cat::convertHSVtoRGB888(0.08f, 0.84f, 1.0f);
                                    // BRIGHTNESS:HUE:BLACK/PRIO:RGB
                                    // 0         : 1 : 1        :000
        BGColor[0b0'1'1'000] = MakeRGB(0x00, 0x00, 0x00);   // 黒
                                    // BRIGHTNESS:HUE:BLACK/PRIO:RGB
                                    // 0         : 1 : 1        :001
        BGColor[0b0'1'1'001] = // MakeRGB(0x1D, 0x27, 0xDD);  // 青
        cat::convertHSVtoRGB888(0.65f, 0.86f, 0.86f);
                                    // BRIGHTNESS:HUE:BLACK/PRIO:RGB
                                    // 0         : 1 : 1        :010
        BGColor[0b0'1'1'010] = // MakeRGB(0x30, 0xDA, 0x38);  // 緑
        cat::convertHSVtoRGB888(0.34f, 0.77f, 0.85f);
                                    // BRIGHTNESS:HUE:BLACK/PRIO:RGB
                                    // 0         : 1 : 1        :011
        BGColor[0b0'0'1'011] = // MakeRGB(0x1C, 0xCE, 0xE5);  // シアン
        cat::convertHSVtoRGB888(0.51f, 0.87f, 0.89f);
                                    // BRIGHTNESS:HUE:BLACK/PRIO:RGB
                                    // 0         : 1 : 1        :100
        BGColor[0b0'1'1'100] = // MakeRGB(0xFF, 0x48, 0x31);  // 赤
        cat::convertHSVtoRGB888(0.01f, 0.80f, 1.0f);
                                    // BRIGHTNESS:HUE:BLACK/PRIO:RGB
                                    // 0         : 1 : 1        :101
        BGColor[0b0'1'1'101] = // MakeRGB(0xFF, 0x78, 0xFF);  // マゼンタ
        cat::convertHSVtoRGB888(0.83f, 0.52f, 1.0f);
                                    // BRIGHTNESS:HUE:BLACK/PRIO:RGB
                                    // 0         : 1 : 1        :110
        BGColor[0b0'1'1'110] = // MakeRGB(0xC0, 0xB3, 0x00);  // 黄
        cat::convertHSVtoRGB888(0.15f, 1.0f, 0.75f);
                                    // BRIGHTNESS:HUE:BLACK/PRIO:BGR
                                    // 0         : 1 : 1        :111
        BGColor[0b0'1'1'111] = // MakeRGB(0xA6, 0xA6, 0xA6);  // 灰色
        cat::convertHSVtoRGB888(0.0f, 0.0f, 0.65f);
    //--------------------------------------------------------------------
    //--------------------------------------------------------------------
        // BRIGHTNESS: 1
        // HUE       : 0
                                    // BRIGHTNESS:HUE:BLACK/PRIO:RGB
                                    // 0         : 0 : 0        :000
        BGColor[0b1'0'0'000] = // MakeRGB(0xCF, 0xFF, 0xFF);  // ブルーシアン
        cat::convertHSVtoRGB888(0.50f, 0.18f, 1.0f);
                                    // BRIGHTNESS:HUE:BLACK/PRIO:RGB
                                    // 0         : 0 : 0        :001
        BGColor[0b1'0'0'001] = // MakeRGB(0x46, 0x49, 0xA8);  // 青
        cat::convertHSVtoRGB888(0.66f, 0.58f, 0.65f);
                                    // BRIGHTNESS:HUE:BLACK/PRIO:RGB
                                    // 0         : 0 : 0        :010
        BGColor[0b1'0'0'010] = // MakeRGB(0xCE, 0xF6, 0xD0);  // 緑
        cat::convertHSVtoRGB888(0.34f, 0.16f, 0.96f);
                                    // BRIGHTNESS:HUE:BLACK/PRIO:RGB
                                    // 0         : 0 : 0        :011
        BGColor[0b1'0'0'011] = // MakeRGB(0xC2, 0xF9, 0xFF);  // シアン
        cat::convertHSVtoRGB888(0.51f, 0.23f, 1.0f);
                                    // BRIGHTNESS:HUE:BLACK/PRIO:RGB
                                    // 0         : 0 : 0        :100
        BGColor[0b1'0'0'100] = // MakeRGB(0xF5, 0x98, 0x8E);      // 赤
        cat::convertHSVtoRGB888(0.01f, 0.42f, 0.96f);
                                    // BRIGHTNESS:HUE:BLACK/PRIO:RGB
                                    // 0         : 0 : 0        :101
        BGColor[0b1'0'0'101] = // MakeRGB(0xFF, 0xE6, 0xFF);  // マゼンタ
        cat::convertHSVtoRGB888(0.83f, 0.09f, 1.0f);
                                    // BRIGHTNESS:HUE:BLACK/PRIO:RGB
                                    // 0         : 0 : 0        :110
        BGColor[0b1'0'0'110] = // MakeRGB(0xFF, 0xFA, 0xB2);  // 黄
        cat::convertHSVtoRGB888(0.15f, 0.30f, 1.0f);
                                    // BRIGHTNESS:HUE:BLACK/PRIO:BGR
                                    // 0         : 0 : 0        :111
        BGColor[0b1'0'0'111] = // MakeRGB(0xFF, 0xF8, 0xB4);  // 黄
        cat::convertHSVtoRGB888(0.15f, 0.29f, 1.0f);

                                    // BRIGHTNESS:HUE:BLACK/PRIO:RGB
                                    // 0         : 0 : 1        :000
        BGColor[0b1'0'1'000] = MakeRGB(0x00, 0x00, 0x00);   // 黒
                                    // BRIGHTNESS:HUE:BLACK/PRIO:RGB
                                    // 0         : 0 : 1        :001
        BGColor[0b1'0'1'001] = // MakeRGB(0x28, 0x2B, 0x8A);  // 青
        cat::convertHSVtoRGB888(0.66f, 0.71f, 0.54f);
                                    // BRIGHTNESS:HUE:BLACK/PRIO:RGB
                                    // 0         : 0 : 1        :010
        BGColor[0b1'0'1'010] = // MakeRGB(0x6B, 0xBF, 0x6F);  // 緑
        cat::convertHSVtoRGB888(0.34f, 0.43f, 0.74f);
                                    // BRIGHTNESS:HUE:BLACK/PRIO:RGB
                                    // 0         : 0 : 1        :011
        BGColor[0b1'0'1'011] = // MakeRGB(0x60, 0xBA, 0xC5);  // シアン
        cat::convertHSVtoRGB888(0.51f, 0.51f, 0.77f);
                                    // BRIGHTNESS:HUE:BLACK/PRIO:RGB
                                    // 0         : 0 : 1        :100
        BGColor[0b1'0'1'100] = // MakeRGB(0xB6, 0x59, 0x4F);  // 赤
        cat::convertHSVtoRGB888(0.01f, 0.56f, 0.71f);
                                    // BRIGHTNESS:HUE:BLACK/PRIO:RGB
                                    // 0         : 0 : 1        :101
        BGColor[0b1'0'1'101] = // MakeRGB(0xE6, 0x91, 0xE0);  // マゼンタ
        cat::convertHSVtoRGB888(0.84f, 0.36f, 0.90f);
                                    // BRIGHTNESS:HUE:BLACK/PRIO:RGB
                                    // 0         : 0 : 1        :110
        BGColor[0b1'0'1'110] = // MakeRGB(0xB4, 0xAE, 0x52);  // 黄
        cat::convertHSVtoRGB888(0.15f, 0.54f, 0.70f);
                                    // BRIGHTNESS:HUE:BLACK/PRIO:BGR
                                    // 0         : 0 : 1        :111
        BGColor[0b1'0'1'111] = // MakeRGB(0xA6, 0xA6, 0xA6);  // 灰色
        cat::convertHSVtoRGB888(0.0f, 0.0f, 0.65f);
    //--------------------------------------------------------------------
    //--------------------------------------------------------------------
        // BRIGHTNESS: 1
        // HUE       : 1
                                    // BRIGHTNESS:HUE:BLACK/PRIO:RGB
                                    // 1         : 1 : 0        :000
        BGColor[0b1'1'0'000] = // MakeRGB(0xB1, 0xF7, 0xFF);  // ブルーシアン
        cat::convertHSVtoRGB888(0.51f, 0.30f, 1.0f);
                                    // BRIGHTNESS:HUE:BLACK/PRIO:RGB
                                    // 1         : 1 : 0        :001
        BGColor[0b1'1'0'001] = // MakeRGB(0x3B, 0x45, 0xFF);  // 青
        cat::convertHSVtoRGB888(0.65f, 0.76f, 1.0f);
                                    // BRIGHTNESS:HUE:BLACK/PRIO:RGB
                                    // 1         : 1 : 0        :010
        BGColor[0b1'1'0'010] = // MakeRGB(0x96, 0xF0, 0x99);  // 緑
        cat::convertHSVtoRGB888(0.33f, 0.37f, 0.94f);
                                    // BRIGHTNESS:HUE:BLACK/PRIO:RGB
                                    // 1         : 1 : 0        :011
        BGColor[0b1'1'0'011] = // MakeRGB(0x7E, 0xF3, 0xFF);  // シアン
        cat::convertHSVtoRGB888(0.51f, 0.50f, 1.0f);
                                    // BRIGHTNESS:HUE:BLACK/PRIO:RGB
                                    // 1         : 1 : 0        :100
        BGColor[0b1'1'0'100] = // MakeRGB(0xFF, 0x83, 0x70);  // 赤
        cat::convertHSVtoRGB888(0.02f, 0.56f, 1.0f);
                                    // BRIGHTNESS:HUE:BLACK/PRIO:RGB
                                    // 1         : 1 : 0        :101
        BGColor[0b1'1'0'101] = // MakeRGB(0xFF, 0xC8, 0xFF);  // マゼンタ
        cat::convertHSVtoRGB888(0.83f, 0.21f, 1.0f);
                                    // BRIGHTNESS:HUE:BLACK/PRIO:RGB
                                    // 1         : 1 : 0        :110
        BGColor[0b1'1'0'110] = // MakeRGB(0xFF, 0xF5, 0x5F);  // 黄
        cat::convertHSVtoRGB888(0.15f, 0.62f, 1.0f);
                                    // BRIGHTNESS:HUE:BLACK/PRIO:BGR
                                    // 1         : 1 : 0        :111
        BGColor[0b1'1'0'111] = // MakeRGB(0xFF, 0xF1, 0x87);  // オレンジ
        cat::convertHSVtoRGB888(0.14f, 0.47f, 1.0f);
                                    // BRIGHTNESS:HUE:BLACK/PRIO:RGB
                                    // 1         : 1 : 1        :000
        BGColor[0b1'1'1'000] = MakeRGB(0x00, 0x00, 0x00);   // 黒
                                    // BRIGHTNESS:HUE:BLACK/PRIO:RGB
                                    // 1         : 1 : 1        :001
        BGColor[0b1'1'1'001] = // MakeRGB(0x1D, 0x27, 0xDD);  // 青
        cat::convertHSVtoRGB888(0.65f, 0.86f, 0.86f);
                                    // BRIGHTNESS:HUE:BLACK/PRIO:RGB
                                    // 1         : 1 : 1        :010
        BGColor[0b1'1'1'010] = // MakeRGB(0x30, 0xDA, 0x38);  // 緑
        cat::convertHSVtoRGB888(0.34f, 0.77f, 0.85f);
                                    // BRIGHTNESS:HUE:BLACK/PRIO:RGB
                                    // 1         : 1 : 1        :011
        BGColor[0b1'0'1'011] = // MakeRGB(0x1C, 0xCE, 0xE5);  // シアン
        cat::convertHSVtoRGB888(0.51f, 0.87f, 0.89f);
                                    // BRIGHTNESS:HUE:BLACK/PRIO:RGB
                                    // 1         : 1 : 1        :100
        BGColor[0b1'1'1'100] = // MakeRGB(0xFF, 0x48, 0x31);  // 赤
        cat::convertHSVtoRGB888(0.01f, 0.80f, 1.0f);
                                    // BRIGHTNESS:HUE:BLACK/PRIO:RGB
                                    // 1         : 1 : 1        :101
        BGColor[0b1'1'1'101] = // MakeRGB(0xFF, 0x78, 0xFF);  // マゼンタ
        cat::convertHSVtoRGB888(0.83f, 0.52f, 1.0f);
                                    // BRIGHTNESS:HUE:BLACK/PRIO:RGB
                                    // 1         : 1 : 1        :110
        BGColor[0b1'1'1'110] = // MakeRGB(0xC0, 0xB3, 0x00);  // 黄
        cat::convertHSVtoRGB888(0.15f, 1.0f, 0.75f);
                                    // BRIGHTNESS:HUE:BLACK/PRIO:BGR
                                    // 1         : 1 : 1        :111
        BGColor[0b1'1'1'111] = // MakeRGB(0xA6, 0xA6, 0xA6);  // 灰色
        cat::convertHSVtoRGB888(0.0f, 0.0f, 0.65f);
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
