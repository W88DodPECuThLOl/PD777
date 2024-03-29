/**
 * @file    CRT.h
 * @brief   画面関連
 */

#pragma once

#include "catLowBasicTypes.h"

/**
 * @brief 画面関連
 */
class CRT {
    /**
     * @brief   フィールド(0 or 1)
     */
    u8  field;
    /**
     * @brief   水平カウンタ
     */
    u8  horizontalCounter;
    /**
     * @brief   垂直カウンタ
     */
    u16 verticalCounter;
public:
    //
    static constexpr u8 HORIZONTAL_DOT_MIN = 0;
    static constexpr u8 HORIZONTAL_DOT_MAX = 90;
    static_assert(HORIZONTAL_DOT_MIN <= HORIZONTAL_DOT_MAX);
    static constexpr u8 DOT_WIDTH = (HORIZONTAL_DOT_MAX - HORIZONTAL_DOT_MIN) + 1;
    //
    static constexpr u16 VERTICAL_DOT_MIN = 0;
    static constexpr u16 VERTICAL_DOT_MAX = 59;
    static_assert(VERTICAL_DOT_MIN <= VERTICAL_DOT_MAX);
    static constexpr u16 DOT_HEIGHT = (VERTICAL_DOT_MAX - VERTICAL_DOT_MIN) + 1;
    //
    static constexpr u16 HORIZONTAL_BLANK_START = 0;
    static constexpr u16 HORIZONTAL_BLANK_END = 15;
    static_assert(HORIZONTAL_BLANK_START <= HORIZONTAL_BLANK_END);
    static constexpr u16 HORIZONTAL_BLANK_WIDTH = (HORIZONTAL_BLANK_END - HORIZONTAL_BLANK_START) + 1;
    //
    static constexpr u16 VERTICAL_BLANK_START = 0;
    static constexpr u16 VERTICAL_BLANK_END = 23;
    static_assert(VERTICAL_BLANK_START <= VERTICAL_BLANK_END);
    static constexpr u16 VERTICAL_BLANK_HEIGHT = (VERTICAL_BLANK_END - VERTICAL_BLANK_START) + 1;
public:
    /**
     * @brief   コンストラクタ
     */
    CRT()
        : field(0)
        , horizontalCounter(0)
        , verticalCounter(0)
    {
    }

    /**
     * @brief   フィールドを取得する
     * @return  フィールド(0 or 1)
     */
    const u8 getField() const noexcept { return field; }
    /**
     * @brief   水平位置を取得する
     * @return  水平位置(0～90)
     */
    const u8 getHorizontalCounter() const noexcept { return horizontalCounter; }
    /**
     * @brief   垂直位置を取得する
     * @return  垂直位置(0～262ぐらい)
     */
    const u8 getVerticalCounter() const noexcept { return verticalCounter; }

    /**
     * @brief   リセット
     */
    void reset() noexcept
    {
        field = 0;
        horizontalCounter = 0;
        verticalCounter   = 0;
    }

    /**
     * @brief   １つ進める
     * @return  VBlankになったらtrueを返す
     */
    bool step()
    {
        // CLOCK input                  3.579545 MHz
        // CPU Clock                    1.431818 MHz ( = 3.579545 MHz / 2.5)
        // NTSC Horizontal Frequency    15.734   KHz
        // １ラインで実行出来るCPUの命令数 1431818 / 15734 = 91.00152535909495
        if(++horizontalCounter >= 91) [[unlikely]] {
            horizontalCounter = 0;

            const auto prevVBLK = isVBLK();

            verticalCounter++;
            if(field == 0) {
                if(verticalCounter > 524 / 2) [[unlikely]] {
                    verticalCounter = 0;
                    field = 1;
                }
            } else {
                if(verticalCounter > 525 / 2) [[unlikely]] {
                    verticalCounter = 0;
                    field = 0;
                }
            }
            return prevVBLK && !isVBLK();
        } else {
            return false;
        }
    }

    /**
     * @brief   4Hかどうか
     * @return  4Hかどうか
     * @retval  true: 4H中
     * @retval  false: 4H中ではない
     */
    bool is4H_BLK() const noexcept { return ((verticalCounter & 3) == 0); }

    /**
     * @brief   HBLK期間中かどうか
     * @return  HBLK期間中かどうか
     * @retval  true: HBLK期間中
     * @retval  false: HBLK期間中ではない
     */
    bool isHBLK() const noexcept { return (horizontalCounter <= HORIZONTAL_BLANK_END); }

    /**
     * @brief   V BLANK期間中かどうか
     * @return  V BLANK期間中かどうか
     * @retval  true: V BLANK期間中
     * @retval  false: V BLANK期間中ではない
     */
    bool isVBLK() const noexcept { return verticalCounter <= VERTICAL_BLANK_END; }
};
