#pragma once

#include "catLowBasicTypes.h"

struct PD777Config {
    /**
     * @brief BGカラー(RGB888形式)
     */
    u32 BGColor[64] {};

    /**
     * @brief Spriteカラー(RGB888形式)
     */
    u32 SpriteColor[16] {};

    /**
     * @brief 方眼紙描画(横線)
     */
    bool debugDrawHorizontalLines = false;
    /**
     * @brief 方眼紙描画(縦線)
     */
    bool debugDrawVerticalLines = false;

    /**
     * @brief コンストラクタ
     */
    PD777Config();

    /**
     * @brief デフォルトの設定にする
     */
    void setDefault() noexcept;
};
