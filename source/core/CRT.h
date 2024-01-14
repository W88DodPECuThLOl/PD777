#pragma once

#include "catLowBasicTypes.h"
#include "catLowMemory.h"

/**
 * @brief 画面関連
 */
class CRT {
    /**
     * @brief   フィールド
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
    /**
     * @brief   コンストラクタ
     */
    CRT()
        : field(0)
        , horizontalCounter(0)
        , verticalCounter(0)
    {
    }

    const u8 geField() const { return field; }
    const u8 geHorizontalCounter() const { return horizontalCounter; }
    const u8 geVerticalCounter() const { return verticalCounter; }

    /**
     * @brief   リセット
     */
    void reset()
    {
        field = 0;
        horizontalCounter = 0;
        verticalCounter   = 0;
    }

    /**
     * @brief   １つ進める
     */
    void step()
    {
        horizontalCounter++;
        if(horizontalCounter > 90) [[unlikely]] {
            horizontalCounter = 0;

            verticalCounter++;
            if(field == 0) {
                if(verticalCounter > 524) [[unlikely]] {
                    verticalCounter = 0;
                    field = 1;
                }
            } else {
                if(verticalCounter > 525) [[unlikely]] {
                    verticalCounter = 0;
                    field = 0;
                }
            }
        }
    }

    /**
     * @brief   4Hかどうか
     * @return  4Hかどうか
     * @retval  true: 4H中
     * @retval  false: 4H中ではない
     */
    bool is4H_BLK()
    {
        return (horizontalCounter & 7) == 7;
    }

    /**
     * @brief   V BLANK期間中かどうか
     * @return  V BLANK期間中かどうか
     * @retval  true: V BLANK期間中
     * @retval  false: V BLANK期間中ではない
     */
    bool isVBLK()
    {
        if(field == 0) {
            return verticalCounter <= 47;
        } else {
            return verticalCounter <= 48;
        }
    }
};
