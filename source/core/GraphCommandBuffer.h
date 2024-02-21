#pragma once

#include "catLowBasicTypes.h"
#include "GraphCommand.h"

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
     * @brief 描画コマンドのバッファサイズ
     */
    static constexpr size_t BufferSize = 32; // MaxSprite per line


    bool isBent(const u8* characterAttribute, const u8 characterNo) const {
        for(auto i = 0; i < 0x100; i += 2) {
            if(characterAttribute[i] >= 0x80) {
                return false;
            }

            if(characterAttribute[i] == characterNo) {
                if(characterAttribute[i + 1] & 0xC) {
                    return true;
                }
            }
        }
        return false;
    }
    bool isBent(const u8* characterAttribute, const GraphCommand* cmd) const {
        if(cmd == nullptr) [[unlikely]] {
            return false;
        }
        return isBent(characterAttribute, cmd->getSpritePattern());
    }
    u8 getBentType(const u8* characterAttribute, const GraphCommand* cmd) const {
        if(cmd == nullptr) [[unlikely]] {
            return 0;
        }
        return cmd->getSpriteBentType();
    }
public:
    /**
     * @brief 描画コマンドのバッファ
     */
    GraphCommand buffer[BufferSize];
    /**
     * @brief 書き込み位置。描画コマンドのコマンド数。
     */
    s32 currentIndex = 0;

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
        if(currentIndex >= BufferSize) [[unlikely]] {
            return; // 描画コマンドがオーバーした
        }
        buffer[currentIndex].verticalCounter = verticalCounter;
        buffer[currentIndex].spriteData[0] = spriteData0;
        buffer[currentIndex].spriteData[1] = spriteData1;
        buffer[currentIndex].spriteData[2] = spriteData2;
        buffer[currentIndex].spriteData[3] = spriteData3;
        currentIndex++;
    }

    bool isHitDot(const GraphCommand* cmd, const u8* patternRom, const u8* patternRom8, const u8 x) const
    {
        // ------------------------------------
        // パターン番号での判定
        // ------------------------------------
        u16 sprPatten = cmd->getSpritePattern();   // パターン
        if(((sprPatten & 0x0F) == 0x0F) || ((sprPatten & 0x0F) == 0x07)) [[unlikely]] {
            return false; // パターンが0x?7と0x?Fのときは表示しない
        }

        // ------------------------------------
        // スプライトのX座標の描画範囲での判定
        // ------------------------------------
        s32 startX; // スプライトのX座標の描画開始位置 [startX endX)
        s32 endX;   // スプライトのX座標の描画終了位置
        {
            const bool repeatX = sprPatten >= 0x70;

            startX = cmd->getSpriteX();     // X座標
            endX   = cmd->getSpriteX() + 7; // X座標
            if(sprPatten >= 0x70) {
                startX--;
            }
            if(repeatX) {
                // X方向リピートなので終了位置を変更
                static const s32 repeatEnd[4] = {64+4, 72+4, 80+4, 88};
                endX = repeatEnd[sprPatten & 0x3];
            }
        }
        if(startX > x || x >= endX) {
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
            u32 dY = Y - SprY;
            patternRomAddressOffset = (patternRomAddressOffset + dY) % 7;
        }
        // パターンを調べる
        bool hit;
        if(sprPatten < 0x70) {
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

    bool getDotInfoPass1(const u8* patternRom, const u8* patternRom8, const u8* characterAttribute, const u8 x, DotInfo& dotInfo) const
    {
        const GraphCommand* hitCmd = nullptr;
        for(s32 index = 0; index < currentIndex; ++index) {
            const auto* cmd = &buffer[index];
            const auto PRIO = cmd->getSpritePrio();
            if(PRIO || !(cmd->getSpritePattern() >= 0x70)) {
                continue;
            }

            // リピートあり PRIO:0
            const bool hit     =           isHitDot(cmd, patternRom, patternRom8, x);
            if(hit) {
                hitCmd = cmd;
            }
        }
        for(s32 index = 0; index < currentIndex; ++index) {
            const auto* cmd = &buffer[index];
            const auto PRIO = cmd->getSpritePrio();
            if(PRIO || (cmd->getSpritePattern() >= 0x70)) {
                continue;
            }

            // リピートなし PRIO:0
            const bool hit     =           isHitDot(cmd, patternRom, patternRom8, x);
            if(hit) {
                hitCmd = cmd;
            }
        }
        dotInfo.colorEnable = hitCmd != nullptr;
        dotInfo.rgb         = hitCmd ? hitCmd->getSpriteColor() : 0;
        return hitCmd != nullptr;
    }
    void getDotInfoPass2(const u8* patternRom, const u8* patternRom8, const u8* characterAttribute, const u8 x, DotInfo& dotInfo) const
    {
        dotInfo.bentRisingEdge  = 0;
        dotInfo.bentFallingEdge = 0;
        dotInfo.bentEnable = false;

        const GraphCommand* hitCmd = nullptr;
        for(s32 index = 0; index < currentIndex; ++index) {
            const auto* cmd = &buffer[index];
            const auto PRIO = cmd->getSpritePrio();
            if(PRIO || (cmd->getSpritePattern() >= 0x70) || !isBent(characterAttribute, cmd)) {
                continue;
            }
            // リピートなし PRIO:0 ベントあり
            const bool prevHit = (x > CRT::HORIZONTAL_DOT_MIN) ? isHitDot(cmd, patternRom, patternRom8, x - 1) : false;
            const bool hit     = isHitDot(cmd, patternRom, patternRom8, x);
            const bool nextHit = (x < CRT::HORIZONTAL_DOT_MAX) ? isHitDot(cmd, patternRom, patternRom8, x + 1) : false;

            if(!prevHit && hit) {
                dotInfo.bentRisingEdge |= (getBentType(characterAttribute, cmd) == 0) ? 1 : 2;
            }
            if(hit && !nextHit) {
                dotInfo.bentFallingEdge |= (getBentType(characterAttribute, cmd) == 0) ? 1 : 2;
            }

            if(hit) {
                dotInfo.bentEnable  = dotInfo.bentEnable || true;
            }
        }
    }

    bool getDotInfoPass3(const u8* patternRom, const u8* patternRom8, const u8* characterAttribute, const u8 x, DotInfo& dotInfo) const
    {
        dotInfo.bentRisingEdge  = 0;
        dotInfo.bentFallingEdge = 0;
        dotInfo.bentEnable = false;
        const GraphCommand* hitCmd = nullptr;
        for(s32 index = currentIndex - 1; index >= 0; --index) {
            const auto* cmd = &buffer[index];
            const auto PRIO = cmd->getSpritePrio();
            if(!PRIO) {
                continue;
            }

            // PRIO:1
            const bool prevHit = (x > CRT::HORIZONTAL_DOT_MIN) ? isHitDot(cmd, patternRom, patternRom8, x - 1) : false;
            const bool hit     = isHitDot(cmd, patternRom, patternRom8, x);
            const bool nextHit = (x < CRT::HORIZONTAL_DOT_MAX) ? isHitDot(cmd, patternRom, patternRom8, x + 1) : false;

            if(!prevHit && hit) {
                if(isBent(characterAttribute, cmd)) {
                    dotInfo.bentRisingEdge |= (getBentType(characterAttribute, cmd) == 0) ? 1 : 2;
                }
            }
            if(hit && !nextHit) {
                if(isBent(characterAttribute, cmd)) {
                    dotInfo.bentFallingEdge |= (getBentType(characterAttribute, cmd) == 0) ? 1 : 2;
                }
            }

            if(hit) {
                if(isBent(characterAttribute, cmd)) {
                    dotInfo.bentEnable  = dotInfo.bentEnable || true;
                }
            }
            if(hit) {
                hitCmd = cmd;
            }
        }
        dotInfo.colorEnable = hitCmd != nullptr;
        dotInfo.rgb         = hitCmd ? hitCmd->getSpriteColor() : 0;
        return hitCmd != nullptr;
    }
};

/**
 * @brief   描画コマンドのバッファ
 */
class GraphCommandBuffer {
public:
    /**
     * @brief ラインサイズ
     */
    static constexpr size_t LineSize = 60;

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
        if(verticalCounter < 24) [[unlikely]] {
            return; // VBLK期間中なので描画しない
        }
        const s32 line = (verticalCounter - 24) / 4;
        if(line >= LineSize) [[unlikely]] {
            return; // バッファオーバー
        }
        lineBuffers[line].addCommand(verticalCounter, spriteData0, spriteData1, spriteData2, spriteData3);
    }
};
