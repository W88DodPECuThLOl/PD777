#include "PD777.h"

namespace {

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
    MakeRGB(132, 195, 204),     // 0:001 メモ）1:001と同じ色
    MakeRGB(176, 223,  11),     // 0:010
    MakeRGB(98, 206, 170),      // 0:011
    MakeRGB(255, 129, 196),     // 0:100
    MakeRGB(208, 136, 203),     // 0:101
    MakeRGB(255, 161, 31),      // 0:110
    MakeRGB(0xD0, 0xD0, 0xD0),  // 0:111 メモ）1:111と同じ色
    // PRIO:RGB
    //    1:xxx
    MakeRGB(0x00, 0x00, 0x00),  // 1:000
    MakeRGB(132, 195, 204),     // 1:001 メモ）0:001と同じ色
    MakeRGB(176, 223, 11),      // 1:010
    MakeRGB(164, 234, 150),     // 1:011
    MakeRGB(255, 135, 190),     // 1:100
    MakeRGB(198, 177, 255),     // 1:101
    MakeRGB(255, 160, 34),      // 1:110
    MakeRGB(0xD0, 0xD0, 0xD0),  // 1:111 メモ）0:111と同じ色
};

#undef MakeRGB

} // namespace

void
PD777::pset(s32 x, s32 y, const u32 color, const u32 bgColor, const DOT_PATTERN ptn)
{
    if((x < CRT::HORIZONTAL_DOT_MIN) || (y < CRT::VERTICAL_DOT_MIN)
    || (x > CRT::HORIZONTAL_DOT_MAX) || (y > CRT::VERTICAL_DOT_MAX)) [[unlikely]] {
        return; // 画面外
    }

    // H.BLK中の描画をしないように
    //if(x <= CRT::HORIZONTAL_BLANK_END) [[unlikely]] {
    //    return; // H.BLK中
    //}

    x *= dotWidth;
    y *= dotHeight;

    static const s32 bentDelayX[dotHeight] = {0, 2, 3, 4}; // ベントのギザギザ用

    switch(ptn) {
        case DOT_PATTERN::NONE:
        default:
            break;
        case DOT_PATTERN::NORMAL:
            for(s32 yy = 0; yy < dotHeight; ++yy) {
                for(s32 xx = 0; xx < dotWidth; ++xx) {
                    frameBuffer[x + xx + (y + yy) * frameBufferWidth] = color;
                }
            }
            break;

        case DOT_PATTERN::BENT1_START:
            for(s32 yy = 0; yy < dotHeight; ++yy) {
                for(s32 xx = 0; xx < bentDelayX[yy]; ++xx) {
                    frameBuffer[x + xx + (y + yy) * frameBufferWidth] = bgColor;
                }
                for(s32 xx = bentDelayX[yy]; xx < dotWidth; ++xx) {
                    frameBuffer[x + xx + (y + yy) * frameBufferWidth] = color;
                }
            }
            break;
        case DOT_PATTERN::BENT1_END:
            for(s32 yy = 0; yy < dotHeight; ++yy) {
                for(s32 xx = 0; xx < bentDelayX[yy]; ++xx) {
                    frameBuffer[x + xx + (y + yy) * frameBufferWidth] = color;
                }
            }
            break;

        case DOT_PATTERN::BENT2_START:
            for(s32 yy = 0; yy < dotHeight; ++yy) {
                for(s32 xx = 0; xx < dotWidth - yy; ++xx) {
                    frameBuffer[x + xx + (y + yy) * frameBufferWidth] = bgColor;
                }
                for(s32 xx = dotWidth - yy; xx < dotWidth; ++xx) {
                    frameBuffer[x + xx + (y + yy) * frameBufferWidth] = color;
                }
            }
            break;
        case DOT_PATTERN::BENT2_END:
            for(s32 yy = 0; yy < dotHeight; ++yy) {
                for(s32 xx = 0; xx < dotWidth - yy; ++xx) {
                    frameBuffer[x + xx + (y + yy) * frameBufferWidth] = color;
                }
            }
            break;
    }
}

void
PD777::dotRender(const DotInfo* dotInfos, const s32 line, const u32 bgColor)
{
#if true
    u32 color = bgColor;
    u8 bentType = 0;
    u8 preBentType = 0;
    for(s32 x = 0; x < CRT::DOT_WIDTH; ++x) {
        // ベントタイプ更新
        bentType |= dotInfos[x].bentRisingEdge;

        // 色
        if(dotInfos[x].rgb != 0) {
            color = tblSpriteToRGB[dotInfos[x].rgb]; // PRIO:RGB
        } else {
            color = bgColor; // 0:000(PRIO:RGB)のときは背景色に
        }
        if(dotInfos[x].bentEnable) {
            if(!bentType) {
                // ベント描画無効
                // メモ)ベントの描画の途中で、他のベントで中断されてしまった
                color = bgColor;
            }
        }

        // ドットの形
        const bool prevBentEnable = ((x > 0) ? dotInfos[x - 1].bentEnable : false); // １個前のbentEnableの値
        DOT_PATTERN ptn = dotInfos[x].colorEnable ? DOT_PATTERN::NORMAL : DOT_PATTERN::NONE;
        if(!prevBentEnable && dotInfos[x].bentEnable) {
            // ベントの開始
            switch(bentType) {
                case 0:
                default:
                    // ここには来ないはず
                    break;
                case 1: // 
                    ptn = DOT_PATTERN::BENT1_START;
                    break;
                case 2: // 
                    ptn = DOT_PATTERN::BENT2_START;
                    break;
                case 3: // 
                    ptn = DOT_PATTERN::NORMAL;
                    break;
            }
        }

        // ドット描画
        pset(x, line, color, bgColor, ptn);
        if(prevBentEnable && !dotInfos[x].bentEnable) {
            // ベントが終わったときのはみ出た部分の描画
            switch(preBentType) {
                case 0:
                default:
                    break;
                case 1: // 
                    ptn = DOT_PATTERN::BENT1_END;
                    break;
                case 2: // 
                    ptn = DOT_PATTERN::BENT2_END;
                    break;
                case 3: // 
                    ptn = DOT_PATTERN::NORMAL;
                    break;
            }
            color = tblSpriteToRGB[dotInfos[x - 1].rgb]; // PRIO:RGB
            pset(x, line, color, bgColor, ptn);
        }

        // ベントタイプ更新
        preBentType = bentType;
        bentType &= ~dotInfos[x].bentFallingEdge;
    }
#else
    //
    // 実験中...
    //

    // @todo 色についての情報が不十分
    // メモ）ベント開始時にベントの色を決定すると、木の部分で全てのライン同一色となってしまう
    // 　　　実機では、途中で木の色に戻る。
    // @todo ベントが途切れる条件が不明（３個重なったときに途切れる？）
    // メモ）NORMAL+BENTは大丈夫そうだが、NORMAL*2+BENTだと、ベントの途切れが再現できていない

    u32 color     = bgColor;
    u32 bentColor = bgColor;

    bool bentEnable = false;
    u8 bentType = 0;
    bool bentTypeFallingEdge = false;
    u8 prevBentType = 0;
    for(s32 x = 0; x < CRT::DOT_WIDTH; ++x) {
        //
        // ベント関連
        // 
        // 有効無効
        const bool bentEnableRisingEdge  = !bentEnable &&  dotInfos[x].bentEnable; // 立ち上がりエッジ
        const bool bentEnableFallingEdge =  bentEnable && !dotInfos[x].bentEnable; // 立ち下がりエッジ
        bentEnable = dotInfos[x].bentEnable;
        // タイプ
        bentType |= dotInfos[x].bentRisingEdge;

        //
        // 色
        //
        if(dotInfos[x].rgb != 0) {
            color = tblSpriteToRGB[dotInfos[x].rgb]; // PRIO:RGB
        } else {
            color = bgColor; // 0:000(PRIO:RGB)のときは背景色に
        }
        if(bentEnableRisingEdge && dotInfos[x].bentRisingEdge) {
            // ベント開始時にベントで使用する色が決まる
            // メモ）と思っていたが時がありました。  @todo
            bentColor = color;
        }
        if(dotInfos[x].bentEnable && !bentType) {
            // ベント描画無効
            // メモ)ベントの描画の途中で、他のベントで中断されてしまった。
            //      これ以降は、背景色で描画する
            bentColor = bgColor;
        }

        //
        // ドットの形
        //
        DOT_PATTERN ptn = dotInfos[x].colorEnable ? DOT_PATTERN::NORMAL : DOT_PATTERN::NONE;
        if(bentEnableRisingEdge && dotInfos[x].bentRisingEdge) {
            // ベントの開始
            color = bentColor;
            switch(bentType) {
                case 0:
                default:
                    // ここには来ないはず
                    break;
                case 1: // 
                    ptn = DOT_PATTERN::BENT1_START;
                    break;
                case 2: // 
                    ptn = DOT_PATTERN::BENT2_START;
                    break;
                case 3: // 
                    ptn = DOT_PATTERN::NORMAL;
                    break;
            }
        }
        if(!bentEnableRisingEdge && bentEnable) {
            // ベント中
            color = bentColor;
            ptn = DOT_PATTERN::NORMAL;
        }

        // ドット描画
        pset(x, line, color, bgColor, ptn);
        if(bentEnableFallingEdge && bentTypeFallingEdge) {
            // ベントの終了
            color = bentColor;
            switch(prevBentType) {
                case 0:
                default:
                    break;
                case 1: // 
                    ptn = DOT_PATTERN::BENT1_END;
                    break;
                case 2: // 
                    ptn = DOT_PATTERN::BENT2_END;
                    break;
                case 3: // 
                    ptn = DOT_PATTERN::NORMAL;
                    break;
            }
            pset(x, line, color, bgColor, ptn);
        }

        // ベントタイプ更新
        prevBentType = bentType;
        bentType &= ~dotInfos[x].bentFallingEdge;
        bentTypeFallingEdge = prevBentType && !bentType;
    }
#endif
}

void
PD777::makePresentImage()
{
    const auto modeRegister = regs.getMode();
    auto modeBrightness = modeRegister & 0x20; // BRIGHTNESS
    auto modeHUE        = modeRegister & 0x10; // HUE
    auto bgBlackPrio    = modeRegister & 0x08; // BLACK/PRIO
    auto bgRGB          = modeRegister & 0x07; // RGB

#if false // for DEBUG
    {
        auto& lineBuffer = graphBuffer.lineBuffers[19];
        lineBuffer.dump(patternRom, patternRom8, characterAttribute);
    }
#endif

    // -------------
    // 背景色
    // -------------
    const auto bgColor = tblBGtoRGB[bgRGB | bgBlackPrio /* | modeHUE | modeBrightness */]; // @todo パラメータの反映、色の調整
    for(auto& m : frameBuffer) { m = bgColor; }

    // -------------
    // スプライト描画
    // -------------
    // メモ）優先順位とスプライトの描画順番
    // 低い     スプライトの描画順番
    // PRIO:0 | 0x00から0x18の順に描画   |
    // PRIO:1 | 0x18から0x00の逆順で描画 |
    // 高い
    DotInfo dotInfos[CRT::DOT_WIDTH]; // １ライン分のドット情報
    for(s32 drawLine = 0; drawLine < GraphCommandBuffer::LineSize; ++drawLine) {
        const auto& lineBuffer = graphBuffer.lineBuffers[drawLine];
        for(u8 prio = 0; prio < 2; ++prio) {
            // ドットの情報を取得（色情報、ベント情報）
            for(s32 x = 0; x < CRT::DOT_WIDTH; ++x) {
                lineBuffer.getDotInfo(patternCGRom7x7, patternCGRom8x7, characterBent, prio, x, dotInfos[x]);
            }
            // ドットの情報を元に描画
            dotRender(dotInfos, drawLine, bgColor);
        }
    }
    graphBuffer.reset();

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
            if(x == CRT::HORIZONTAL_BLANK_END + 1) { color = 0xFF0000; } // H.BLKの境界
            if(x == 64+5) { color = 0x0000FF; } // リピートXの境界
            if(x == 72+5) { color = 0x0000FF; } // リピートXの境界
            if(x == 80+5) { color = 0x0000FF; } // リピートXの境界
            //if(x == 90)   { color = 0xFF0000; } // リピートXの境界
            frameBuffer[x * dotWidth + y * frameBufferWidth] = color;
        }
    }
#endif
}
