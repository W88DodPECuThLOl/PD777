#pragma once

#include "catLowBasicTypes.h"
#include "GraphCommand.h"

class GraphLineCommandBuffer {
    /**
     * @brief 描画コマンドのバッファサイズ
     */
    static constexpr size_t BufferSize = 32; // MaxSprite per line
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
