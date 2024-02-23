#pragma once

#include "catLowBasicTypes.h"
#include "GraphCommand.h"
#include "CRT.h"

#if false // for debug
#include <stdio.h>
#endif

/**
 * @brief １ドットの情報
 * 
 * メモ）
 * @todo 色について不十分
 * @todo ベントが途切れる条件が不明（３個重なったときに途切れる？）
 * 
 *                       -,b2,b2,b1,b1, - 
 *      bentEnable       0  1  1  1  1  0   ドットのベントが有効なとき
 *      bentRisingEdge   0  2  0  1  0  0   ベントの立ち上がりエッジ
 *      bentFallingEdge  0  0  2  0  1  0   ベントの立ち下がりエッジ
 *      -----------------------------------------------------------------
 *                       0  1  0  0  0  0   bentEnableの立ち上がりエッジ
 *                       0  0  0  0  0  1   bentEnableの立ち下がりエッジ
 *                       0  2  2  1  1  0   bentType。bentRisingEdge、bentFallingEdgeで更新。RisingEdgeは前処理、FallingEdgeは後処理で。
 *                       0  0  0  0  0  1   bentTypeの立ち下がりエッジ(0以外から0になったとき)
 *                         ／ ■ ■ ■ ＼
 *                         ↑ ↑ ↑ ↑ ↑
 *                         ｜ ｜ ｜ ｜ (bentEnableの立ち下がり && bentTypeの立ち下がり)、余韻の斜めを描画。形状は１つ前のbentType。
 *                         ｜ (!bentEnableの立ち上がり && bentEnable)、■を描画
 *                         (bentEnableの立ち上がり && bentRisingEdge)、bentTypeの形状で描画
 * 
 *                       -,b2,b1, - 
 *      bentEnable       0  1  1  0   ドットのベントが有効なとき
 *      bentRisingEdge   0  2  1  0   ベントの立ち上がりエッジ
 *      bentFallingEdge  0  2  1  0   ベントの立ち下がりエッジ
 *      -----------------------------------------------------------------
 *                       0  1  0  0   bentEnableの立ち上がり
 *                       0  0  0  1   bentEnableの下がり
 *                       0  2  1  0   bentType
 *                       0  0  0  1   bentTypeの下がり(0以外から0になったとき)
 *                         ／ ■ ＼
 *                         ↑ ↑ ↑
 *                         ｜ ｜ (bentEnableの立ち下がり && bentTypeの立ち下がり)、余韻の斜めを描画。形状は１つ前のbentType。
 *                         ｜ (!bentEnableの立ち上がり && bentEnable)、■を描画
 *                         (bentEnableの立ち上がり && bentRisingEdge)、bentTypeの形状で描画
 * 
 * 重なっている時
 *                       -,b2,b2,b2,b2, -
 *                       -, -,b2, -, -, - 
 *      bentEnable       0  1  1  1  1  0   ドットのベントが有効なときに、trueになる
 *      bentRisingEdge   0  2  2  0  0  0   ベントの立ち上がりでフラグが立つ
 *      bentFallingEdge  0  0  2  0  2  0   ベントの下がりでフラグが立つ
 *      -----------------------------------------------------------------
 *                       0  1  0  0  0  0   bentEnableの立ち上がり
 *                       0  0  0  0  0  1   bentEnableの下がり
 *                       0  2  2  0  0  0   bentType
 *                       0  0  0  1  0  0   bentTypeの下がり(0以外から0になったとき)
 *                         ／ ■ □ □
 *                         ↑ ↑ ↑ ↑
 *                         ｜ ｜ 背景色を描画
 *                         ｜ (!bentEnableの立ち上がり && bentEnable)、■を描画
 *                         (bentEnableの上がり && bentRisingEdge)、bentTypeの形状で描画
 */
struct DotInfo {
    /**
     * @brief 色(PRIO:RGB)
     */
    u32     rgb;
    /**
     * @brief 色が有効かどうか
     */
    bool    colorEnable;
    /**
     * @brief ベントが有効かどうか
     */
    bool    bentEnable;
    /**
     * @brief ベントの立ち上がりエッジ
     */
    u8      bentRisingEdge;
    /**
     * @brief ベントの立ち下がりエッジ
     */
    u8      bentFallingEdge;
};

class GraphLineCommandBuffer {
    /**
     * @brief 描画コマンドのラインバッファサイズ
     * 
     * １ラインの最大スプライト表示可能数 12個
     */
    static constexpr size_t LineBufferMemorySize = 12;

    /**
     * @brief 描画コマンドのバッファ
     */
    GraphCommand buffer[LineBufferMemorySize];
    /**
     * @brief 書き込み位置。描画コマンドのコマンド数。
     */
    s32 currentIndex = 0;

private:
    /**
     * @brief キャラクタパターンがベントかどうか
     * 
     * @param[in]   characterAttribute  キャラクタ属性
     * @param[in]   characterNo         キャラクタパターン番号
     * @return ベントするキャラクタパターンの場合trueを返す
     */
    bool isBent(const u8* characterAttribute, const u8 characterNo) const {
        for(auto i = 0; i < 0x100; i += 2) {
            if(characterAttribute[i] >= 0x80) {
                return false; // 検索終了
            }

            if(characterAttribute[i] == characterNo) {
                if(characterAttribute[i + 1] & 0xC) [[likely]] {
                    return true;
                }
            }
        }
        return false;
    }

    /**
     * @brief グラフィックコマンドのキャラクタパターンがベントかどうか
     * 
     * @param[in]   characterAttribute  キャラクタ属性
     * @param[in]   cmd                 グラフィックコマンド
     * @return ベントする場合trueを返す
     */
    inline bool isBent(const u8* characterAttribute, const GraphCommand* cmd) const {
        return isBent(characterAttribute, cmd->getSpritePattern());
    }

    /**
     * @brief グラフィックコマンドで描画するスプライトとの当たり判定
     * @param[in]   cmd         グラフィックコマンド
     * @param[in]   patternRom  7x7のキャラクタパターン
     * @param[in]   patternRom8 8x7のキャラクタパターン
     * @param[in]   x           判定するX座標(0～90)
     */
    bool isHitDot(const GraphCommand* cmd, const u8* patternRom, const u8* patternRom8, const u8 x) const
    {
        // ------------------------------------
        // パターン番号での判定
        // ------------------------------------
        const u16 sprPatten = cmd->getSpritePattern();   // パターン
        if(((sprPatten & 0x0F) == 0x0F) || ((sprPatten & 0x0F) == 0x07)) [[unlikely]] {
            return false; // パターンが0x?7と0x?Fのときは表示しない
        }

        // ------------------------------------
        // スプライトのX座標の描画範囲での判定
        // ------------------------------------
        s32 startX; // スプライトのX座標の描画開始位置 [startX endX)
        s32 endX;   // スプライトのX座標の描画終了位置
        {
            startX = cmd->getSpriteX();     // X座標
            endX   = cmd->getSpriteX() + 7; // X座標
            if(cmd->isRepeatX()) {
                // 7x7との描画位置がずれているので補正
                startX--;
                // X方向リピートなので終了位置を変更
                static const s32 repeatEnd[4] = {64+5, 72+5, 80+5, 91};
                endX = repeatEnd[sprPatten & 0x3];
            }
        }
        if((x < startX) || (endX <= x)) {
            return false; // 範囲外
        }

        // ------------------------------------
        // パターン内部での判定
        // ------------------------------------
        // パターンの読み込み位置を補正
        const u32 Y = cmd->getDrawLine(); // 描画する1ラインのY座標(0～59)
        auto patternRomAddressOffset = cmd->getSpritePatternRomAddressOffset(); // パターンROM読み出し時のオフセット
        if(const u32 SprY  = cmd->getSpriteY(); SprY < Y) {
            // 差をパターンのアドレスで吸収する
            const u32 dY = Y - SprY;
            patternRomAddressOffset = (patternRomAddressOffset + dY) % 7;
        }
        // パターンを調べる
        bool hit;
        if(cmd->isSpritePatternSize7x7()) {
            // 7x7
            const auto base = (sprPatten - (sprPatten / 8)) * 7;
            const u8 pattern = patternRom[base + patternRomAddressOffset % 7];
            const s32 patternRomOffsetX = (x - startX) % 7; // パターン内部での位置 0～6
            const u8 mask = 0x40 >> patternRomOffsetX;
            hit = (pattern & mask) != 0;
        } else {
            // 8x7
            const auto base = (sprPatten - 0x70 - ((sprPatten - 0x70) / 8)) * 7;
            const u8 pattern = patternRom8[base + patternRomAddressOffset % 7];
            const s32 patternRomOffsetX = (x - startX) % 8; // パターン内部での位置 0～7
            const u8 mask = 0x80 >> patternRomOffsetX;
            hit = (pattern & mask) != 0;
        }
        return hit;
    }

    /**
     * @brief シンプルなリングバッファ
     */
    template<s32 N>
    class SimpleRingbuffer {
        static constexpr s32 BufferSize = N;
        const GraphCommand* buffer[BufferSize];
        s32 writeIndex = 0;
        s32 readIndex  = 0;
        s32 elementSize = 0;
    public:
        void reset()
        {
            writeIndex = 0;
            readIndex  = 0;
            size = 0;
        }
        s32 size() const { return elementSize; }

        void push(const GraphCommand* cmd)
        {
            buffer[writeIndex] = cmd;
            writeIndex = (writeIndex + 1) % BufferSize;
            if(elementSize < BufferSize) {
                elementSize++;
            } else {
                readIndex = (readIndex + 1) % BufferSize;
            }
        }
        const GraphCommand* peek(const s32 offset) const
        {
            return buffer[(readIndex + offset) % BufferSize];
        }
    };

    /**
     * @brief ドットバッファを取得する
     * @param[out]  dotBuffer   ドットバッファ
     * @param[in]   patternRom  7x7のキャラクタパターン
     * @param[in]   patternRom8 8x7のキャラクタパターン
     * @param[in]   prio        取得するプライオリティ(0か1)
     * @param[in]   x           取得するX座標(0～90)
     */
    void getDotBuffer(SimpleRingbuffer<5>& dotBuffer, const u8* patternRom, const u8* patternRom8, const u8 prio, const u8 x) const {
        if(prio == 0) {
            // PRIO:0
            for(decltype(currentIndex) index = 0; index < currentIndex; ++index) {
                const auto* cmd = &buffer[index];
                if(cmd->getSpritePrio()) { continue; }
                if(isHitDot(cmd, patternRom, patternRom8, x)) [[unlikely]] {
                    dotBuffer.push(cmd);
                }
            }
        } else {
            // PRIO:1
            for(decltype(currentIndex) index = currentIndex - 1; index >= 0; --index) {
                const auto* cmd = &buffer[index];
                if(!cmd->getSpritePrio()) { continue; }
                if(isHitDot(cmd, patternRom, patternRom8, x)) [[unlikely]] {
                    dotBuffer.push(cmd);
                }
            }
        }
    }
public:
    /**
     * @brief リセット
     */
    void reset()
    {
        currentIndex = 0; // 書き込み位置をリセット
    }

    /**
     * @brief   描画コマンドを追加する
     * @param[in]   verticalCounter 垂直カウンタ
     * @param[in]   spriteData0     スプライトデータ0
     * @param[in]   spriteData1     スプライトデータ1
     * @param[in]   spriteData2     スプライトデータ2
     * @param[in]   spriteData3     スプライトデータ3
     */
    void addCommand(const u16 verticalCounter, const u8 spriteData0, const u8 spriteData1, const u8 spriteData2, const u8 spriteData3)
    {
        if(currentIndex >= LineBufferMemorySize) [[unlikely]] {
            // 描画コマンドがオーバーした
            // return;
            // 
            // @todo 実験 最初のを消して追加する
            // @todo リングバッファ化
            for(auto i = 1; i < LineBufferMemorySize; ++i) { buffer[i - 1] = buffer[i]; }
            // 最後に積む積む
            buffer[LineBufferMemorySize - 1].verticalCounter = verticalCounter;
            buffer[LineBufferMemorySize - 1].spriteData[0] = spriteData0;
            buffer[LineBufferMemorySize - 1].spriteData[1] = spriteData1;
            buffer[LineBufferMemorySize - 1].spriteData[2] = spriteData2;
            buffer[LineBufferMemorySize - 1].spriteData[3] = spriteData3;
            return;
        }
        buffer[currentIndex].verticalCounter = verticalCounter;
        buffer[currentIndex].spriteData[0] = spriteData0;
        buffer[currentIndex].spriteData[1] = spriteData1;
        buffer[currentIndex].spriteData[2] = spriteData2;
        buffer[currentIndex].spriteData[3] = spriteData3;
        currentIndex++;
    }

    void getDotInfo(const u8* patternRom, const u8* patternRom8, const u8* characterAttribute, const u8 prio, const u8 x, DotInfo& dotInfo) const
    {
        // メモ）同一座標に5個までらしい
        SimpleRingbuffer<5> dotBuffer;
        getDotBuffer(dotBuffer, patternRom, patternRom8, prio, x);

        // ベント情報
        dotInfo.bentRisingEdge  = 0;
        dotInfo.bentFallingEdge = 0;
        dotInfo.bentEnable      = false;
        const GraphCommand* cmd = nullptr;
        for(s32 index = 0; index < dotBuffer.size(); ++index) {
            cmd = dotBuffer.peek(index);
            if(isBent(characterAttribute, cmd)) {
                dotInfo.bentEnable = true;
                // ベントのエッジ
                const bool prevHit = (x > CRT::HORIZONTAL_DOT_MIN) ? isHitDot(cmd, patternRom, patternRom8, x - 1) : false;
                const bool nextHit = (x < CRT::HORIZONTAL_DOT_MAX) ? isHitDot(cmd, patternRom, patternRom8, x + 1) : false;
                if(!prevHit) {
                    dotInfo.bentRisingEdge |= (cmd->getSpriteBentType() == 0) ? 1 : 2;
                }
                if(!nextHit) {
                    dotInfo.bentFallingEdge |= (cmd->getSpriteBentType() == 0) ? 1 : 2;
                }
            }
        }
        // 色情報
        dotInfo.colorEnable = cmd != nullptr;
        dotInfo.rgb         = cmd ? cmd->getSpriteColor() : 0;
    }

#if false // for DEBUG
    void
    dump(const u8* patternRom, const u8* patternRom8, const u8* characterAttribute)
    {
        for(s32 index = 0; index < currentIndex; ++index) {
            const auto* cmd = &buffer[index];
            const bool bent = isBent(characterAttribute, cmd);

            printf("%2d:", index);
            for(s32 x = 0; x < 91; ++x) {
                const bool hit = isHitDot(cmd, patternRom, patternRom8, x);
                if(hit) {
                    printf("1%d,", cmd->getSpritePrio());
                } else {
                    printf("  ,");
                }
            }
            printf("\n");

            printf("%2x:", cmd->getSpritePattern());
            for(s32 x = 0; x < 91; ++x) {
                const bool hit = isHitDot(cmd, patternRom, patternRom8, x);
                if(hit) {
                    if(bent) {
                        printf("%x%d,", cmd->getSpriteColor(), cmd->getSpriteBentType());
                    } else {
                        printf("%x ,", cmd->getSpriteColor());
                    }
                } else {
                    printf("  ,");
                }
            }
            printf("\n");
        }
    }
#endif
};

/**
 * @brief   描画コマンドのバッファ
 */
class GraphCommandBuffer {
public:
    /**
     * @brief ラインサイズ
     */
    static constexpr size_t LineSize = CRT::DOT_HEIGHT;

    /**
     * @brief 描画コマンドのバッファ
     */
    GraphLineCommandBuffer lineBuffers[LineSize];

    /**
     * @brief リセット
     */
    void reset()
    {
        for(auto& lineBuffer : lineBuffers) { lineBuffer.reset(); }
    }

    /**
     * @brief   描画コマンドを追加する
     * @param[in]   verticalCounter 垂直カウンタ
     * @param[in]   spriteData0     スプライトデータ0
     * @param[in]   spriteData1     スプライトデータ1
     * @param[in]   spriteData2     スプライトデータ2
     * @param[in]   spriteData3     スプライトデータ3
     */
    void addCommand(const u16 verticalCounter, const u8 spriteData0, const u8 spriteData1, const u8 spriteData2, const u8 spriteData3)
    {
        if(verticalCounter < CRT::VERTICAL_BLANK_HEIGHT) [[unlikely]] {
            return; // VBLK期間中なので描画しない
        }
        const s32 line = (verticalCounter - CRT::VERTICAL_BLANK_HEIGHT) / 4;
        if(line >= LineSize) [[unlikely]] {
            return; // バッファオーバー
        }
        lineBuffers[line].addCommand(verticalCounter, spriteData0, spriteData1, spriteData2, spriteData3);
    }
};
