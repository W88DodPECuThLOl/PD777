#pragma once

#include "catLowBasicTypes.h"

/**
 * @brief   コールスタック
 */
class Stack {
    /**
     * @brief スタックサイズ
     */
    static constexpr u8 STACK_SIZE = 3;

    /**
     * @brief   コールスタックのアドレス
     */
    u16  stack[STACK_SIZE] {};
    /**
     * @brief   スタックポインタ(0～)
     */
    u8   stackPointer;
public:
    /**
     * @brief   コンストラクタ
     */
    Stack();
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
