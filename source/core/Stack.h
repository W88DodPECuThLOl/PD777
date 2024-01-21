#pragma once

#include "catLowBasicTypes.h"

/**
 * @brief   コールスタック
 */
class Stack {
    /**
     * @brief   コールスタックのアドレス
     */
    u16  stack[3];
    /**
     * @brief   スタックポインタ
     */
    u8   stackPointer;
public:
    /**
     * @brief   コンストラクタ
     */
    Stack() = default;
    /**
     * @brief   デストラクタ
     */
    ~Stack() = default;
public:
    /**
     * @brief   リセット
     * 
     * コールスタックを初期化する
     */
    void reset();

    /**
     * @brief   コールスタックに戻るアドレスを積む
     * @param[in]   address 積むアドレス
     */
    void stackPush(const u16 address);

    /**
     * @brief   コールスタックから戻るアドレスを降ろす
     * @return  戻るアドレス
     */
    const u16 stackPop();
};
