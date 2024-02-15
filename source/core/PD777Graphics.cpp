#include "PD777.h"
#include "PD777.h"

#if defined(_WIN32)
#include <string>
#endif // defined(_WIN32)

namespace {

// スプライトの色々な値を取得する関数
inline const u8 getSpritePrio(const u8* ram, const s32 index) { return ram[index * 4 + 0] & 1; }
inline const u8 getSpriteY(const u8* ram, const s32 index) { return ram[index * 4 + 0] >> 1; }
inline const u8 getSpriteX(const u8* ram, const s32 index) { return ram[index * 4 + 1]; }
inline const u8 getSpritePattern(const u8* ram, const s32 index) { return ram[index * 4 + 2]; }
inline const u8 getSpritePatternRomAddressOffset(const u8* ram, const s32 index)
{
    const u8 data3 = ram[index * 4 + 3];
    const u8 y     = (data3 >> 4) & 0x7;
    const u8 ySUB  = data3 & 1;
    return (y - ySUB) & 0x7;
}

#define MakeRGB(r,g,b) ((b) | ((g) << 8) | ((r) << 16))

/**
 * @brief 背景色をRGB値へ変換するときのテーブル
 * @todo  ちゃんとした値を設定すること
 */
static const u32 tblBGtoRGB[64] = {
                                // BRIGHTNESS:HUE:BLACK/PRIO:RGB
                                // 0         : 0 : 0        :000
    MakeRGB(0x00, 0xAE, 0xEF),  // ブルーシアン

                                // BRIGHTNESS:HUE:BLACK/PRIO:RGB
                                // 0         : 0 : 0        :001
    MakeRGB(0x00, 0x00, 0xFF),  // 青

                                // BRIGHTNESS:HUE:BLACK/PRIO:RGB
                                // 0         : 0 : 0        :010
    MakeRGB(0x00, 0xB0, 0x00),  // 緑

                                // BRIGHTNESS:HUE:BLACK/PRIO:RGB
                                // 0         : 0 : 0        :011
    MakeRGB(0xE4, 0x00, 0x7F),  // マゼンタ

                                // BRIGHTNESS:HUE:BLACK/PRIO:RGB
                                // 0         : 0 : 0        :100
    MakeRGB(208, 46, 61),       // 赤

                                // BRIGHTNESS:HUE:BLACK/PRIO:RGB
                                // 0         : 0 : 0        :101
    MakeRGB(0x00, 0xA0, 0xE9),  // シアン

                                // BRIGHTNESS:HUE:BLACK/PRIO:RGB
                                // 0         : 0 : 0        :110
    MakeRGB(0xFF, 0xFF, 0x00),  // 黄

                                // BRIGHTNESS:HUE:BLACK/PRIO:BGR
                                // 0         : 0 : 0        :111
    MakeRGB(0xF5, 0x82, 0x20),  // オレンジ
//--------------------------------------------------------------------
//--------------------------------------------------------------------
                                // BRIGHTNESS:HUE:BLACK/PRIO:RGB
                                // 0         : 0 : 1        :000
    MakeRGB(0x00, 0x00, 0x00),  // 黒

                                // BRIGHTNESS:HUE:BLACK/PRIO:RGB
                                // 0         : 0 : 1        :001
    MakeRGB(0x00, 0x00, 0xFF),  // 青

                                // BRIGHTNESS:HUE:BLACK/PRIO:RGB
                                // 0         : 0 : 1        :010
    MakeRGB(0x00, 0xB0, 0x00),  // 緑

                                // BRIGHTNESS:HUE:BLACK/PRIO:RGB
                                // 0         : 0 : 1        :011
    MakeRGB(0xE4, 0x00, 0x7F),  // マゼンタ

                                // BRIGHTNESS:HUE:BLACK/PRIO:RGB
                                // 0         : 0 : 1        :100
    MakeRGB(0xFF, 0x00, 0x00),  // 赤

                                // BRIGHTNESS:HUE:BLACK/PRIO:RGB
                                // 0         : 0 : 1        :101
    MakeRGB(0x00, 0xA0, 0xE9),  // シアン

                                // BRIGHTNESS:HUE:BLACK/PRIO:RGB
                                // 0         : 0 : 1        :110
    MakeRGB(0xFF, 0xFF, 0x00),  // 黄

                                // BRIGHTNESS:HUE:BLACK/PRIO:BGR
                                // 0         : 0 : 1        :111
    MakeRGB(0xFF, 0xFF, 0xFF),  // 白
};

/**
 * @brief スプライトの色変換テーブル
 * @todo 色の調整
 */
static const u32 tblSpriteToRGB[16] = {
    // PRIO:RGB
    //    0:xxx
    MakeRGB(0x00, 0x00, 0x00),  // 0:000
    MakeRGB(111, 190, 255),     // 0:001
    MakeRGB(176, 223,  11),     // 0:010
    MakeRGB(98, 206, 170),      // 0:011
    MakeRGB(255, 129, 196),     // 0:100
    MakeRGB(208, 136, 203),     // 0:101
    MakeRGB(255, 161, 31),      // 0:110
    MakeRGB(0xD0, 0xD0, 0xD0),  // 0:111
    // PRIO:RGB
    //    1:xxx
    MakeRGB(0x00, 0x00, 0x00),  // 1:000
    MakeRGB(132, 195, 204),     // 1:001
    MakeRGB(176, 223, 11),      // 1:010
    MakeRGB(164, 234, 150),     // 1:011
    MakeRGB(255, 135, 190),     // 1:100
    MakeRGB(198, 177, 255),     // 1:101
    MakeRGB(255, 160, 34),      // 1:110
    MakeRGB(0xD0, 0xD0, 0xD0),  // 1:111
};

#undef MakeRGB

} // namespace

void
PD777::spriteRasterize(GraphCommandBuffer* commandBuffer, const u8* ram, const u16 verticalCounter)
{
    if(verticalCounter < 24) [[unlikely]] {
        return; // VBLK期間中なので描画しない
    }
    const u16 drawLine = (verticalCounter - 24) / 4;
    for(auto index = 0; index <= 0x18; ++index) {
#if false // メモ TEST_EQU_MA1_IGNORE_LSBが有効の時はいらない
        // PRIO:1のYリピート
        //
        // メモ
        // ・ハード側で処理するが正解なのかどうか？
        // ・CPU側で間違っている可能性もあり
        //   ⇒ PRIO:1の物がYリピートされていない
        //      「EQU/ M=A1, L=>N」でA1を偶数で検索しており、
        //       PRIO:1のものに引っかからない
        const auto P = getSpritePattern(ram, index);
        bool repeatY, repeatX, bent1, bent2;
        getCharacterAttribute(P, repeatY, repeatX, bent1, bent2);
        if(repeatY) {
            if(getSpriteY(ram, index) <= drawLine) {
                // 登録
                const auto spriteData0 = ram[(index << 2) | 0];
                const auto spriteData1 = ram[(index << 2) | 1];
                const auto spriteData2 = ram[(index << 2) | 2];
                const auto spriteData3 = ram[(index << 2) | 3];
                commandBuffer->addCommand(drawLine * 4 + 24, spriteData0, spriteData1, spriteData2, spriteData3);
            }
        } else
#endif
        {
            if(auto Y = getSpriteY(ram, index); (Y <= drawLine) && (drawLine < Y + 7)) {
                // @todo
                if(getSpritePatternRomAddressOffset(ram, index) == 7) {
                    // メモ）なぜか高さ8になり、変なパターンがでてしまうので描画しないように
                    //       YリピートはNRMの直接書き込みの方で描画させる
                    continue;
                }

                // 登録
                const auto spriteData0 = ram[(index << 2) | 0];
                const auto spriteData1 = ram[(index << 2) | 1];
                const auto spriteData2 = ram[(index << 2) | 2];
                const auto spriteData3 = ram[(index << 2) | 3];
                commandBuffer->addCommand(drawLine * 4 + 24, spriteData0, spriteData1, spriteData2, spriteData3);
            }
        }
    }
}

void
PD777::pset(s32 x, s32 y, u32 rgb, bool bent1, bool bent2)
{
    if((x < 0) || (y < 0) || (x > 90) || (y > 59)) [[unlikely]] {
        return; // 画面外
    }

    // H.BLK中の描画をしないように
//    if(x < 16) [[unlikely]] {
//        return; // H.BLK中
//    }

    x *= dotWidth;
    y *= dotHeight;

    if(bent1) {
        // 斜め
        for(s32 yy = 0; yy < dotHeight; ++yy) {
            //for(s32 xx = 0; xx < dotWidth; ++xx) { // @todo 4dotだとUFOに隙間ができてしまう
            for(s32 xx = 0; xx < dotWidth*2; ++xx) {
                frameBuffer[x + xx + yy + (y + yy) * frameBufferWidth] = rgb;
            }
        }
    } else if(bent2) {
        // 斜め
        for(s32 yy = 0; yy < dotHeight; ++yy) {
            //for(s32 xx = 0; xx < dotWidth; ++xx) { // @todo 4dotだとUFOに隙間ができてしまう
            for(s32 xx = 0; xx < dotWidth*2; ++xx) {
                frameBuffer[x + xx + dotWidth - yy + (y + yy) * frameBufferWidth] = rgb;
            }
        }
    } else {
        for(s32 yy = 0; yy < dotHeight; ++yy) {
            for(s32 xx = 0; xx < dotWidth; ++xx) {
                frameBuffer[x + xx + (y + yy) * frameBufferWidth] = rgb;
            }
        }
    }
}


void
PD777::getCharacterAttribute(const u8 characterNo, bool& repeatY, bool& repeatX, bool& bent1, bool& bent2)
{
    repeatY = false;
    repeatX = characterNo >= 0x70; // @todo 0x70～はXリピート？
    bent1   = false;
    bent2   = false;
    for(auto i = 0; i < 0x100; i += 2) {
        if(characterAttribute[i] >= 0x80) {
            return;
        }

        if(characterAttribute[i] == characterNo) {
            //repeatX = (characterAttribute[i + 1] & 0x1) != 0;
            repeatY = (characterAttribute[i + 1] & 0x2) != 0;
            if(characterAttribute[i + 1] & 0xC) {
                // メモ）ベントはパターン番号の最下位ビットでどっちか決まる
                bent2 = characterNo & 1;
                bent1 = !bent2;
            }
            return;
        }
    }
}

void
PD777::makePresentImage()
{
    const auto modeRegister = regs.getMode();
    auto modeBrightness = modeRegister & 0x20; // BRIGHTNESS
    auto modeHUE        = modeRegister & 0x10; // HUE
    auto bgBlackPrio    = modeRegister & 0x08; // BLACK/PRIO
    auto bgRGB          = modeRegister & 0x07; // RGB

    // -------------
    // 背景色
    // -------------
    const auto bgColor = tblBGtoRGB[bgRGB | bgBlackPrio /* | modeHUE | modeBrightness */]; // @todo パラメータの反映、色の調整
    for(auto& m : frameBuffer) { m = bgColor; }

    // 画面非表示
    //if(!regs.getDISP()) {
    //    // @todo ちらつきの頻度が多く、目に悪いので省略
    //    graphBuffer.reset();
    //    return;
    //}

#if true
    // コマンドバッファの描画
    // pass0: PRIO==0の物を描画
    // pass1: PRIO==1の物を描画
    for(auto pass = 0; pass < 2; ++pass) {
        for(int i = 0; i < graphBuffer.currentIndex; ++i) {
            const auto& cmd = graphBuffer.buffer[i];
            const auto PRIO = cmd.getSpritePrio();
            if((pass == 0 && PRIO != 0) || (pass == 1 && PRIO == 0)) {
                continue;
            }
            u16 P = cmd.getSpritePattern();   // パターン
            if(((P & 0x0F) == 0x0F) || ((P & 0x0F) == 0x07)) [[unlikely]] {
                continue; // パターンが0x?7と0x?Fのときは表示しない
            }

            const u32 Y = cmd.getDrawLine(); // 描画する1ラインのY座標(0～59)
            auto patternRomAddressOffset = cmd.getSpritePatternRomAddressOffset(); // パターンROM読み出し時のオフセット
            if(const u32 SprY  = cmd.getSpriteY(); SprY < Y) {
                // 差をパターンのアドレスで吸収する
                u32 dY = Y - SprY;
                patternRomAddressOffset = (patternRomAddressOffset + dY) % 7;
            }

            // @todo 色
            const auto data3 = cmd.spriteData[3];
            u32 rgb   = ((data3 >> 1) & 7) | (PRIO << 3); // | forgroundColor; // prio rgb
            if(rgb != 0) {
                rgb = tblSpriteToRGB[rgb]; // PRIO:RGB
            } else {
                rgb = bgColor; // 0:000(PRIO:RGB)のときは背景色に
            }

            // リピートやベントのパターンの属性の取得
            bool repeatY, repeatX, bent1, bent2;
            getCharacterAttribute(P, repeatY, repeatX, bent1, bent2);

            static const s32 repeatEnd[4] = {64+4, 72+4, 80+4, 88};
            s32 endX = repeatX ? repeatEnd[P & 0x3] : 90; // 右端
            s32 X = cmd.getSpriteX(); // X座標
            if(P >= 0x70) {
                X -= 1; // @todo 8x7のパターンで、位置が1ドット右にずれているので調整
            }
            do {
                if(P < 0x70) {
                    // 7x7
                    const auto base = (P - (P / 8)) * 7;
                    const auto pattern = patternRom[base + patternRomAddressOffset % 7];
                    for(u8 mask = 0x40; mask > 0 && X <= endX; ++X, mask >>= 1) {
                        if(pattern & mask) { pset(X, Y, rgb, bent1, bent2); }
                    }
                } else {
                    // 8x7
                    const auto base = (P - 0x70 - ((P - 0x70) / 8)) * 7;
                    const auto pattern = patternRom8[base + patternRomAddressOffset % 7];
                    for(u8 mask = 0x80; mask > 0 && X <= endX; ++X, mask >>= 1) {
                        if(pattern & mask) { pset(X, Y, rgb, bent1, bent2); }
                    }
                }
            } while(repeatX && X <= endX);
        }
    }
#endif
    graphBuffer.reset();


#if false
    // -------------
    // スプライト
    // -------------
    // ソート
    // 
    // メモ）優先順位
    // 低い
    //   X,YリピートありでPRIO:0
    //   X,YリピートなしでPRIO:0
    //                    PRIO:1
    // 高い
    s32 order[0x19] = {};
    s32 idx = 0;
    // X,YリピートありでPRIO:0
    for(auto index = 0; index <= 0x18; ++index) {
        if(!getSpritePrio(ram, index)) {
            bool repeatY, repeatX, bent1, bent2;
            getCharacterAttribute(getSpritePattern(ram, index), repeatY, repeatX, bent1, bent2);
            if(repeatY || repeatX) { order[idx++] = index; }
        }
    }
    // X,YリピートなしでPRIO:0
    for(auto index = 0; index <= 0x18; ++index) {
        if(!getSpritePrio(ram, index)) {
            bool repeatY, repeatX, bent1, bent2;
            getCharacterAttribute(getSpritePattern(ram, index), repeatY, repeatX, bent1, bent2);
            if(!(repeatY || repeatX)) { order[idx++] = index; }
        }
    }
    // PRIO:1
    for(auto index = 0; index <= 0x18; ++index) {
        if(getSpritePrio(ram, index)) { order[idx++] = index; }
    }

    // 優先をつけたスプライトを描画
    for(const auto index : order) {
        u16 P     = getSpritePattern(ram, index);   // パターン

        // パターンが0x?7と0x?Fのときは表示しない
        if(((P & 0x0F) == 0x0F) || ((P & 0x0F) == 0x07)) [[unlikely]] {
            continue;
        }

        auto PRIO = getSpritePrio(ram, index);      // 表示優先順位？（色にも影響する？）
        u32 Y     = getSpriteY(ram, index);         // Y座標
        u32 SX    = getSpriteX(ram, index);         // X座標
        const auto patternRomAddressOffset = getSpritePatternRomAddressOffset(ram, index); // パターンROM読み出し時のオフセット

        // @todo 色
        const auto data3 = ram[index * 4 + 3];
        u32 rgb   = ((data3 >> 1) & 7) | (PRIO << 3); // | forgroundColor; // prio rgb
        rgb = tblSpriteToRGB[rgb]; // PRIO:RGB

        // リピートやベントのパターンの属性の取得
        bool repeatY, repeatX, bent1, bent2;
        getCharacterAttribute(P, repeatY, repeatX, bent1, bent2);
        repeatY = false;

        do {
            auto X = SX;
            do {
                if(P < 0x70) {
                    // 7x7
                    const auto base = (P - (P / 8)) * 7;
                    for(auto line = 0; line < 7; ++line) {
                        const auto pattern = patternRom[base + (line + patternRomAddressOffset) % 7];
                        if(pattern & 0x40) pset(X + 0, Y + line, rgb, bent1, bent2);
                        if(pattern & 0x20) pset(X + 1, Y + line, rgb, bent1, bent2);
                        if(pattern & 0x10) pset(X + 2, Y + line, rgb, bent1, bent2);
                        if(pattern & 0x08) pset(X + 3, Y + line, rgb, bent1, bent2);
                        if(pattern & 0x04) pset(X + 4, Y + line, rgb, bent1, bent2);
                        if(pattern & 0x02) pset(X + 5, Y + line, rgb, bent1, bent2);
                        if(pattern & 0x01) pset(X + 6, Y + line, rgb, bent1, bent2);
                    }
                    X += 7;
                } else {
                    // 8x7
                    const auto base = (P - 0x70 - ((P - 0x70) / 8)) * 7;
                    for(auto line = 0; line < 7; ++line) {
                        const auto pattern = patternRom8[base + (line + patternRomAddressOffset) % 7];
                        if(pattern & 0x80) pset(X + 0, Y + line, rgb, bent1, bent2);
                        if(pattern & 0x40) pset(X + 1, Y + line, rgb, bent1, bent2);
                        if(pattern & 0x20) pset(X + 2, Y + line, rgb, bent1, bent2);
                        if(pattern & 0x10) pset(X + 3, Y + line, rgb, bent1, bent2);
                        if(pattern & 0x08) pset(X + 4, Y + line, rgb, bent1, bent2);
                        if(pattern & 0x04) pset(X + 5, Y + line, rgb, bent1, bent2);
                        if(pattern & 0x02) pset(X + 6, Y + line, rgb, bent1, bent2);
                        if(pattern & 0x01) pset(X + 7, Y + line, rgb, bent1, bent2);
                    }
                    X += 8;
                }
            } while(repeatX && X <= (75+8));
            Y += 7;
        } while(repeatY && Y < (60+7));
    }
#endif

#if false // for DEBUG 目盛りの描画 横線
    for(auto y = 1; y < frameBufferHeight / dotHeight; ++y) {
        for(auto x = 0; x < frameBufferWidth; ++x) {
            auto color = ((y & 3) == 0) ? 0xFFFFFF : 0x606060;
            frameBuffer[x + y * dotHeight * frameBufferWidth] = color;
        }
    }
#endif
#if false // for DEBUG 目盛りの描画 縦線
    for(auto x = 1; x < frameBufferWidth / dotWidth; ++x) {
        for(auto y = 0; y < frameBufferHeight; ++y) {
            auto color = ((x & 3) == 0) ? 0xFFFFFF : 0x606060;
            if(x == 16) { color = 0xFF0000; } // H.BLKの境界
            //if(x == 64+5) { color = 0x0000FF; } // リピートXの境界
            //if(x == 72+5) { color = 0x0000FF; } // リピートXの境界
            //if(x == 80+5) { color = 0x0000FF; } // リピートXの境界
            //if(x == 88)   { color = 0xFF0000; } // リピートXの境界
            frameBuffer[x * dotWidth + y * frameBufferWidth] = color;
        }
    }
#endif
}
