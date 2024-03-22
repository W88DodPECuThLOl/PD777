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
    u16  stack[STACK_SIZE*2] {};
    /**
     * @brief   スタックポインタ(0～STACK_SIZE-1)
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
    void reset() noexcept;

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

    /**
     * @brief   スタックポインタを取得する
     * @return  スタックポインタ(0～STACK_SIZE-1)
     */
    const u8 getStackPointer() const noexcept;
};
