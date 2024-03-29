#pragma once

#include "catLowBasicTypes.h"

struct KeyMapping {
    /**
     * @brief A8～A12の割り当て
     * 
     * keyMap[0]がA8でkeyMap[4]がA12に対応。
     * 値はS1に割り当てる場合は1、S2は2、S3は3、S4は4を設定する。
     * 未割り当ての場合は0を設定する。
     * 
     * 全て0の場合は、デフォルトの変換規則になる。
     */
    u8 keyMap[5] {};
    /**
     * @brief B9～B15の割り当て
     * 
     * bitMap[0] : B9がONのときにKに設定する値
     * bitMap[1] : B10がONのときにKに設定する値
     * bitMap[2] : B11がONのときにKに設定する値
     * bitMap[3] : B12がONのときにKに設定する値
     * bitMap[4] : B13がONのときにKに設定する値
     * bitMap[5] : B14がONのときにKに設定する値
     * bitMap[6] : B15がONのときにKに設定する値
     */
    u8 bitMap[7] {};

    const bool isDefaultMapping() const {
        for(auto& e : keyMap) {
            if(e) return false;
        }
        return true;
    }
};

/**
 * キー状態
 * 
 * K1  K2  K3  K4  K5  K6  K7
 * ----------------------------------------
 * STA L1L L1R SEL AUX 6   7    | [A08]
 * 1   L2L L2R 4   5   6   7    | [A09]
 * 1   2   3   4   5   P4  P3   | [A10]
 * 1   2   3   4   5   P2  P1   | [A11]
 * LL  L   C   R   RR  6   7    | [A12]
 */
struct KeyStatus {
    /**
     * @brief KINの値
     */
    enum KIN : u8 {
        None = 0x00,

        // A8

        /**
         * @brief	ゲームスタートキー
         */
        GameStartKey     = 0x01,
        /**
         * @brief	ゲーム操作キー（レバースイッチ）の左
         */
        LeverSwitchLeft  = 0x02,
        /**
         * @brief	ゲーム操作キー（レバースイッチ）の右
         */
        LeverSwitchRight = 0x04,
        /**
         * @brief	ゲームセレクトキー
         */
        GameSelectKey    = 0x08,
        /**
         * @brief	AUX
         */
        AUX = 0x10,

        // A9

        /**
         * @brief	ゲーム操作キー（レバースイッチ）の左
         */
        LeverSwitch2Left  = 0x02,
        /**
         * @brief	ゲーム操作キー（レバースイッチ）の右
         */
        LeverSwitch2Right = 0x04,

        // A10

        /**
         * @brief	ゲーム操作キー（アクションキー）のPUSH 4
         */
//        Push4            = 0x4,

        /**
         * @brief	ゲーム操作キー（アクションキー）のPUSH 3
         */
//        Push3            = 0x2,

        // A11

        /**
         * @brief	ゲーム操作キー（アクションキー）のPUSH 2
         */
        Push2            = 0x20,

        /**
         * @brief	ゲーム操作キー（アクションキー）のPUSH 1
         */
        Push1            = 0x40,
    };

    union {
        struct {
            /**
             * @brief A08のキーの状態
             */
            u8  a08;
            /**
             * @brief A09のキーの状態
             */
            u8  a09;
            /**
             * @brief A10のキーの状態
             */
            u8  a10;
            /**
             * @brief A11のキーの状態
             */
            u8  a11;
            /**
             * @brief A12のキーの状態
             */
            u8  a12;
        };
        u8 a[5 + 1];
    };

    // パドルの状態

    /**
     * @brief パドルの値の範囲、下限値
     */
    static constexpr u8 PAD_MIN_VALUE = 0;
    /**
     * @brief パドルの値の範囲、上限値
     */
    static constexpr u8 PAD_MAX_VALUE = 125;

    union {
        struct {
            /**
             * @brief パドル1の状態
             */
            u8 PD1;
            /**
             * @brief パドル2の状態
             */
            u8 PD2;
            /**
             * @brief パドル3の状態
             */
            u8 PD3;
            /**
             * @brief パドル4の状態
             */
            u8 PD4;
        };
        u8 PD[4];
    };

    /**
     * @brief A8～A12の割り当て
     * 
     * keyMap[0]がA8でkeyMap[4]がA12に対応。
     * 値はS1に割り当てる場合は1、S2は2、S3は3、S4は4を設定する。
     * 未割り当ての場合は0を設定する。
     * 全て0の場合は、デフォルトの変換規則になる。
     */
//    u8 keyMap[5];

    /**
     * @brief コンストラクタ
     */
    KeyStatus();

    /**
     * @brief 入力内容をクリアする
     */
    void clear()
    {
        for(auto& e : a)  { e = 0; }
        for(auto& e : PD) { e = PAD_MIN_VALUE; }
    }

    // A8
    void setGameStartKey() { a08 |= KIN::GameStartKey; }
    const bool getGameStartKey() const { return a08 & KIN::GameStartKey; }
    void setLeverSwitch1Left() { a08 |= KIN::LeverSwitchLeft; }
    const bool getLeverSwitch1Left() const { return a08 & KIN::LeverSwitchLeft; }
    void setLeverSwitch1Right() { a08 |= KIN::LeverSwitchRight; }
    const bool getLeverSwitch1Right() const { return a08 & KIN::LeverSwitchRight; }
    void setGameSelectKey() { a08 |= KIN::GameSelectKey; }
    const bool getGameSelectKey() const { return a08 & KIN::GameSelectKey; }
    void setAux() { a08 |= KIN::AUX; }
    // A9
    void setLeverSwitch2Left() { a09 |= KIN::LeverSwitchLeft; }
    void setLeverSwitch2Right() { a09 |= KIN::LeverSwitchRight; }
    // A10
    void setPush4() { a10 |= 0x20; }
    const u8 getPush4() const { return a10 & 0x20; }
    void setPush3() { a10 |= 0x40; }
    const u8 getPush3() const { return a10 & 0x40; }
    // A11
    void setPush2() { a11 |= KIN::Push2; }
    const u8 getPush2() const { return a11 & KIN::Push2; }
    void setPush1() { a11 |= KIN::Push1; }
    const u8 getPush1() const { return a11 & KIN::Push1; }
    // A12
    void setCourseSwitch(const u8 courseSwitch);
    const u8 getCourseSwitch() const { return a12; }
    // 特殊
    void setUp() { a[5] |= 0x40; }
    const u8 getUp() const { return a[5] & 0x40; }
    void setDown() { a[5] |= 0x20; }
    const u8 getDown() const { return a[5] & 0x20; }
    void setDebugMode() { a[5] |= 0x80; }
    const u8 getDebugMode() const { return a[5] & 0x80; }

    // パドル

    void setPD1(const u8 value) { PD1 = value & 0x7F; }
    const u8 getPD1() const { return PD1 & 0x7F; }
    void setPD2(const u8 value) { PD2 = value & 0x7F; }
    const u8 getPD2() const { return PD2 & 0x7F; }
    void setPD3(const u8 value) { PD3 = value & 0x7F; }
    const u8 getPD3() const { return PD3 & 0x7F; }
    void setPD4(const u8 value) { PD4 = value & 0x7F; }
    const u8 getPD4() const { return PD4 & 0x7F; }

    const u8 convertDefault(const u8 STB, const u32 cassetteNumber) const;
    const u8 convertMap(const u8 STB, const KeyMapping& keyMapping) const;
    /**
     * @brief キー入力の状態からKINの値に変換する
     * @param[in]   STB キー入力をスキャンするときの値<br>
     *                  例えば0b1110だとS1に繋がってるA8～A12が取得される<br>
     *                  例えば0b1100だとS1とS2に繋がってるのの論理和が取得される
     * @param[in]   keyMapping      A8～A12の割り当て
     * @param[in]   cassetteNumber  カセット識別子(0～11)
     *                  @arg 0:     不明
     *                  @arg 1～11: カセット番号
     */
    const u8 convert(const u8 STB, const KeyMapping& keyMapping, const u32 cassetteNumber) const;
};
