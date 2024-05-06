/**
 * @file    KeyStatus.h
 * @brief   キーの状態
 */

#pragma once

#include "catLowBasicTypes.h"

/**
 * @brief カートリッジの結線
 * 
 * * カートリッジ内のCPUとの結線状態の定義
 * * CPUと本体の配線がどのようにつながっているかを定義するもの
 * * カートリッジ毎に違う
 */
struct KeyMapping {
    /**
     * @brief A8～A12の割り当て
     * 
     * keyMap[0]がA8でkeyMap[4]がA12に対応。<br>
     * 値はS1に割り当てる場合は1、S2は2、S3は3、S4は4を設定する。<br>
     * 未割り当ての場合は0を設定する。<br>
     * 
     * 全て0の場合は、デフォルトの変換規則になる。<br>
     * 
     * |     | B15   | B14 | B13 | B12    | B11 | B10   | B9    |
     * |-----|-------|-----|-----|--------|-----|-------|-------|
     * | A8  | START | L1L | L1R | SELECT | AUX |       |       |
     * | A9  |       | L2L | L2R |        |     |       |       |
     * | A10 |       |     |     |        |     | PUSH4 | PUSH3 |
     * | A11 |       |     |     |        |     | PUSH2 | PUSH1 |
     * | A12 | LL    | L   | C   | R      | RR  |       |       |
     * ※LL、L、C、R、RR: コーススイッチ
     */
    u8 keyMap[5] {};
    /**
     * @brief B9～B15の割り当て
     * 
     * レバーやボタンが押されたときのKの値を設定する。<br>
     *  bitMap[0] : B9がONのときにKに設定する値(0x40 B9はCPUのK7とつながっている)<br>
     *  bitMap[1] : B10がONのときにKに設定する値(0x20 B10はCPUのK6と繋がっている)<br>
     *  bitMap[2] : B11がONのときにKに設定する値(0x10 B11はCPUのK5と繋がっている)<br>
     *  bitMap[3] : B12がONのときにKに設定する値(0x08 B12はCPUのK4と繋がっている)<br>
     *  bitMap[4] : B13がONのときにKに設定する値(0x04 B13はCPUのK3と繋がっている)<br>
     *  bitMap[5] : B14がONのときにKに設定する値(0x02 B14はCPUのK2と繋がっている)<br>
     *  bitMap[6] : B15がONのときにKに設定する値(0x01 B15はCPUのK1と繋がっている)<br>
     *  ※括弧はデフォルト値
     */
    u8 bitMap[7] {};

    /**
     * @brief デフォルトの設定かどうかを判定
     * 
     * ::keyMap[]が全て0の場合デフォルトの設定とみなす。
     * 
     * @return デフォルトの設定かどうか
     * @retval  true:  デフォルトの設定
     * @retval  false: 何かしら設定されている
     */
    const bool isDefaultMapping() const noexcept {
        for(const auto& e : keyMap) {
            if(e) { return false; }
        }
        return true;
    }
};

/**
 * キー状態
 * 
 * |     | B15   | B14 | B13 | B12    | B11 | B10   | B9    |
 * |-----|-------|-----|-----|--------|-----|-------|-------|
 * | A8  | START | L1L | L1R | SELECT | AUX |       |       |
 * | A9  |       | L2L | L2R |        |     |       |       |
 * | A10 |       |     |     |        |     | PUSH4 | PUSH3 |
 * | A11 |       |     |     |        |     | PUSH2 | PUSH1 |
 * | A12 | LL    | L   | C   | R      | RR  |       |       |
 * ※LL、L、C、R、RR: コーススイッチ
 */
struct KeyStatus {
    /**
     * @brief KINの値
     */
    enum KIN : u8 {
        None = 0x00,

        //
        // A8
        //

        /**
         * @brief   ゲームスタートキー
         */
        GameStartKey     = 0x01,
        /**
         * @brief   ゲーム操作キー（レバースイッチ）の左
         */
        LeverSwitchLeft  = 0x02,
        /**
         * @brief   ゲーム操作キー（レバースイッチ）の右
         */
        LeverSwitchRight = 0x04,
        /**
         * @brief   ゲームセレクトキー
         */
        GameSelectKey    = 0x08,
        /**
         * @brief   AUX
         */
        AUX = 0x10,

        //
        // A9
        //

        /**
         * @brief	ゲーム操作キー（レバースイッチ）の左
         */
        LeverSwitch2Left  = 0x02,
        /**
         * @brief	ゲーム操作キー（レバースイッチ）の右
         */
        LeverSwitch2Right = 0x04,

        //
        // A10
        //

        /**
         * @brief   ゲーム操作キー（アクションキー）のPUSH 4
         */
        Push4            = 0x20,

        /**
         * @brief   ゲーム操作キー（アクションキー）のPUSH 3
         */
        Push3            = 0x40,

        //
        // A11
        //

        /**
         * @brief   ゲーム操作キー（アクションキー）のPUSH 2
         */
        Push2            = 0x20,

        /**
         * @brief   ゲーム操作キー（アクションキー）のPUSH 1
         */
        Push1            = 0x40,

        //
        // A12
        //

        /**
         * @brief   コーススイッチの一番左
         */
        LL = 0x01,
        /**
         * @brief   コーススイッチの左
         */
        L = 0x02,
        /**
         * @brief   コーススイッチの真ん中
         */
        C = 0x04,
        /**
         * @brief   コーススイッチの右
         */
        R = 0x08,
        /**
         * @brief   コーススイッチの一番右
         */
        RR = 0x10,
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

    /**
     * @brief パドルの状態
     */
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
     * @brief コンストラクタ
     */
    KeyStatus();

    /**
     * @brief 入力をクリアする
     */
    void clear() noexcept
    {
        for(auto& e : a)  { e = 0; }
        for(auto& e : PD) { e = PAD_MIN_VALUE; }
    }

    // A8
    void setGameStartKey() noexcept { a08 |= KIN::GameStartKey; }
    const bool getGameStartKey() const noexcept { return a08 & KIN::GameStartKey; }
    void setLeverSwitch1Left() noexcept { a08 |= KIN::LeverSwitchLeft; }
    const bool getLeverSwitch1Left() const noexcept { return a08 & KIN::LeverSwitchLeft; }
    void setLeverSwitch1Right() noexcept { a08 |= KIN::LeverSwitchRight; }
    const bool getLeverSwitch1Right() const noexcept { return a08 & KIN::LeverSwitchRight; }
    void setGameSelectKey() noexcept { a08 |= KIN::GameSelectKey; }
    const bool getGameSelectKey() const noexcept { return a08 & KIN::GameSelectKey; }
    void setAux() noexcept { a08 |= KIN::AUX; }
    const bool getAux() const noexcept { return a08 & KIN::AUX; }
    // A9
    void setLeverSwitch2Left() noexcept { a09 |= KIN::LeverSwitchLeft; }
    void setLeverSwitch2Right() noexcept { a09 |= KIN::LeverSwitchRight; }
    // A10
    void setPush4() noexcept { a10 |= KIN::Push4; }
    const u8 getPush4() const noexcept { return a10 & KIN::Push4; }
    void setPush3() noexcept { a10 |= KIN::Push3; }
    const u8 getPush3() const noexcept { return a10 & KIN::Push3; }
    // A11
    void setPush2() noexcept { a11 |= KIN::Push2; }
    const u8 getPush2() const noexcept { return a11 & KIN::Push2; }
    void setPush1() noexcept { a11 |= KIN::Push1; }
    const u8 getPush1() const noexcept { return a11 & KIN::Push1; }
    // A12
    /**
     * @brief コーススイッチを設定する
     * @param[in]   courseSwitch    コーススイッチの状態(1～5)
     *                                  @arg 1: LL 一番左
     *                                  @arg 2: L  左
     *                                  @arg 3: C  真ん中
     *                                  @arg 4: R  右
     *                                  @arg 5: RR 一番右
     */
    void setCourseSwitch(const u8 courseSwitch) noexcept;
    /**
     * @brief コーススイッチの状態を取得する
     * @return  コーススイッチの状態
     * @retval  KIN::LL:    一番左
     * @retval  KIN::L:     左
     * @retval  KIN::C:     真ん中
     * @retval  KIN::R:     右
     * @retval  KIN::RR:    一番右
     * 
     * @note 返り値は1～5ではないので注意
     */
    const u8 getCourseSwitch() const noexcept { return a12; }

    //
    // 特殊
    //

    /**
     * @brief 上方向キーを設定する
     * @note 実機には存在しない物で、操作性を良くするために使用
     */
    void setUp() noexcept { a[5] |= 0x40; }
    const u8 getUp() const noexcept { return a[5] & 0x40; }
    /**
     * @brief 下方向キーを設定する
     * @note 実機には存在しない物で、操作性を良くするために使用
     */
    void setDown() noexcept { a[5] |= 0x20; }
    const u8 getDown() const noexcept { return a[5] & 0x20; }
    /**
     * @brief デバッグ用途のキーを設定する
     * @note デバッグ用
     */
    void setDebugMode() noexcept { a[5] |= 0x80; }
    const u8 getDebugMode() const noexcept { return a[5] & 0x80; }

    //
    // パドル
    //

    void setPD1(const u8 value) noexcept { PD1 = value & 0x7F; }
    const u8 getPD1() const noexcept { return PD1 & 0x7F; }
    void setPD2(const u8 value) noexcept { PD2 = value & 0x7F; }
    const u8 getPD2() const noexcept { return PD2 & 0x7F; }
    void setPD3(const u8 value) noexcept { PD3 = value & 0x7F; }
    const u8 getPD3() const noexcept { return PD3 & 0x7F; }
    void setPD4(const u8 value) noexcept { PD4 = value & 0x7F; }
    const u8 getPD4() const noexcept { return PD4 & 0x7F; }

    //
    // 変換
    //
private:
    /**
     * @brief キー入力の状態からKINの値にデフォルトの変換をする
     * 
     * @param[in]   STB キー入力をスキャンするときの値<br>
     *                  例えば0b1110だとS1に繋がってるA8～A12が取得される<br>
     *                  例えば0b1100だとS1とS2に繋がってるのの論理和が取得される
     * @param[in]   cassetteNumber  カセット識別子(0～11)
     *                  @arg 0:     不明
     *                  @arg 1～11: カセット番号
     */
    const u8 convertDefault(const u8 STB, const u32 cassetteNumber) const;
    /**
     * @brief キー入力の状態からKINの値にキーマッピングを使って変換する
     * @param[in]   STB キー入力をスキャンするときの値<br>
     *                  例えば0b1110だとS1に繋がってるA8～A12が取得される<br>
     *                  例えば0b1100だとS1とS2に繋がってるのの論理和が取得される
     * @param[in]   keyMapping      A8～A12の割り当て、B9～B15の割り当て
     */
    const u8 convertMap(const u8 STB, const KeyMapping& keyMapping) const;

public:
    /**
     * @brief キー入力の状態からKINの値に変換する
     * @param[in]   STB キー入力をスキャンするときの値<br>
     *                  例えば0b1110だとS1に繋がってるA8～A12が取得される<br>
     *                  例えば0b1100だとS1とS2に繋がってるのの論理和が取得される
     * @param[in]   keyMapping      A8～A12の割り当て、B9～B15の割り当て
     * @param[in]   cassetteNumber  カセット識別子(0～11)
     *                  @arg 0:     不明
     *                  @arg 1～11: カセット番号
     */
    const u8 convert(const u8 STB, const KeyMapping& keyMapping, const u32 cassetteNumber) const;
};
