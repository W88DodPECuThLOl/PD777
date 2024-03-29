/**
 * @file    Registers.h
 * @brief   μPD777のレジスタ
 */

#pragma once

#include "catLowBasicTypes.h"

/**
 * @brief ラインバッファ
 */ 
class LineBufferRegister {
public:
    /**
     * @brief １ラインで記録できるスプライトアドレスの数
     * 
     * １ラインに12個までスプライトを記録出来る
     */
    static constexpr s32 MAX_SPRITE_PER_LINE = 12;

private:
    /**
     * @brief 書き込み位置(0～MAX_SPRITE_PER_LINE-1)
     */
    u8 currentIndex;

    /**
     * @brief 描画するスプライトのアドレス(5bit)
     */
    u8 address[MAX_SPRITE_PER_LINE] {};

public:
    /**
     * @brief コンストラクタ
     */
    LineBufferRegister()
        : currentIndex(0)
    {
    }

    /**
     * @brief リセット
     */
    void reset() noexcept
    {
        currentIndex = 0;
        for(auto& addr : address) { addr = 0x1F; }
    }

    /**
     * @brief ラインバッファに書き込む
     * 
     * @param[in]   H   スプライトのアドレス(5bit)
     * @note バッファオーバーフローしたときの挙動は不明
     */
    void write(const u8 H)
    {
        if(currentIndex >= MAX_SPRITE_PER_LINE) [[unlikely]] {
            return; // １ラインに記録できるスプライス数をオーバーした
        }
        address[currentIndex++] = H & 0x1F;
    }
};


/**
 * @brief   ラインバッファの管理
 */
class LineBufferManager {
    /**
     * @brief   描画用ラインバッファのインデックス(0 or 1)
     */
    u8 currentWriteBufferIndex = 0;
    /**
     * @brief   ラインバッファ
     * 
     * - 描画用と表示用で２つ
     * - どっちがどっちなのかはcurrentWriteBufferIndexで判定
     * @see currentWriteBufferIndex
     */
    LineBufferRegister lineBuffers[2];
public:
    /**
     * @brief   コンストラクタ
     */
    LineBufferManager()
        : currentWriteBufferIndex(0)
    {
    }

    /**
     * @brief リセット
     */
    void reset() noexcept
    {
        currentWriteBufferIndex = 0;
        for(auto& lineBuffer : lineBuffers) { lineBuffer.reset(); }
    }

    /**
     * @brief 描画用のラインバッファと表示用のラインバッファを切り替える
     */
    void flipBuffer() noexcept { currentWriteBufferIndex ^= 1; }

    /**
     * @brief 描画用のラインバッファをクリアして、描画できるようにする
     */
    void clear() noexcept
    {
        auto& lineBuffer = getRenderLineBuffer();
        lineBuffer.reset();
    }

    /**
     * @brief 描画用のラインバッファを取得する
     * @return 描画用のラインバッファ
     */
    inline const LineBufferRegister& getRenderLineBuffer() const noexcept { return lineBuffers[currentWriteBufferIndex]; }
    inline LineBufferRegister& getRenderLineBuffer() noexcept { return lineBuffers[currentWriteBufferIndex]; }
    /**
     * @brief 表示用のラインバッファを取得する
     * @return 表示用のラインバッファ
     */
    inline const LineBufferRegister& getDisplayLineBuffer() const noexcept { return lineBuffers[currentWriteBufferIndex ^ 1]; }

    /**
     * @brief 描画用のラインバッファに書き込む
     * @param[in]   H   スプライトのアドレス(5bit)
     */
    void write(u8 H)
    {
        auto& lineBuffer = getRenderLineBuffer();
        lineBuffer.write(H);
    }
};



/**
 * @brief μPD777のレジスタ
 */
class Registers {
    /**
     * @brief   プログラムカウンタ(11ビット)
     * 
     * - プログラムカウンタは、0x00,0x01,0x02, ...という順にはカウントされない
     * - 0x00,0x01,0x03,0x07,0x0F,0x1F, ... , 0x50,0x20,0x40という順にカウントされる
     *   - 0x7Fは使われない子
     * - 0x40の次は0x00となり次のページにはならない
     *   - ページはJP命令、JS命令、SRE命令、N=>A11命令などで変更される
     * 
     * <pre>
     * PC:PPPPyyyyyyy
     *       |      |
     *       |      7 bit Polynomial Address Counter
     *       4 bit Page Address Register
     * </pre>
     * 
     * | ADDR     | HEX | ADDR     | HEX | ADDR     | HEX | ADDR     | HEX | ADDR     | HEX | ADDR     | HEX | ADDR     | HEX | ADDR     | HEX |
     * |----------|-----|----------|-----|----------|-----|----------|-----|----------|-----|----------|-----|----------|-----|----------|-----|
     * | 000 0000 | 00  | 111 0011 | 73  | 000 1101 | 0D  | 001 0101 | 15  | 111 0001 | 71  | 010 0101 | 25  | 011 1001 | 39  | 000 1100 | 0C  |
     * | 000 0001 | 01  | 110 0111 | 67  | 001 1011 | 1B  | 010 1011 | 2B  | 110 0011 | 63  | 100 1010 | 4A  | 111 0010 | 72  | 001 1001 | 19  |
     * | 000 0011 | 03  | 100 1111 | 4F  | 011 0111 | 37  | 101 0110 | 56  | 100 0111 | 47  | 001 0100 | 14  | 110 0101 | 65  | 011 0011 | 33  |
     * | 000 0111 | 07  | 001 1110 | 1E  | 110 1110 | 6E  | 010 1100 | 2C  | 000 1110 | 0E  | 010 1001 | 29  | 100 1011 | 4B  | 110 0110 | 66  |
     * | 000 1111 | 0F  | 011 1101 | 3D  | 101 1101 | 5D  | 101 1000 | 58  | 001 1101 | 1D  | 101 0010 | 52  | 001 0110 | 16  | 100 1101 | 4D  |
     * | 001 1111 | 1F  | 111 1010 | 7A  | 011 1010 | 3A  | 011 0000 | 30  | 011 1011 | 3B  | 010 0100 | 24  | 010 1101 | 2D  | 001 1010 | 1A  |
     * | 011 1111 | 3F  | 111 0101 | 75  | 111 0100 | 74  | 110 0000 | 60  | 111 0110 | 76  | 100 1000 | 48  | 101 1010 | 5A  | 011 0101 | 35  |
     * | 111 1110 | 7E  | 110 1011 | 6B  | 110 1001 | 69  | 100 0001 | 41  | 110 1101 | 6D  | 001 0000 | 10  | 011 0100 | 34  | 110 1010 | 6A  |
     * | 111 1101 | 7D  | 101 0111 | 57  | 101 0011 | 53  | 000 0010 | 02  | 101 1011 | 5B  | 010 0001 | 21  | 110 1000 | 68  | 101 0101 | 55  |
     * | 111 1011 | 7B  | 010 1110 | 2E  | 010 0110 | 26  | 000 0101 | 05  | 011 0110 | 36  | 100 0010 | 42  | 101 0001 | 51  | 010 1010 | 2A  |
     * | 111 0111 | 77  | 101 1100 | 5C  | 100 1100 | 4C  | 000 1011 | 0B  | 110 1100 | 6C  | 000 0100 | 04  | 010 0010 | 22  | 101 0100 | 54  |
     * | 110 1111 | 6F  | 011 1000 | 38  | 001 1000 | 18  | 001 0111 | 17  | 101 1001 | 59  | 000 1001 | 09  | 100 0100 | 44  | 010 1000 | 28  |
     * | 101 1111 | 5F  | 111 0000 | 70  | 011 0001 | 31  | 010 1111 | 2F  | 011 0010 | 32  | 001 0011 | 13  | 000 1000 | 08  | 101 0000 | 50  |
     * | 011 1110 | 3E  | 110 0001 | 61  | 110 0010 | 62  | 101 1110 | 5E  | 110 0100 | 64  | 010 0111 | 27  | 001 0001 | 11  | 010 0000 | 20  |
     * | 111 1100 | 7C  | 100 0011 | 43  | 100 0101 | 45  | 011 1100 | 3C  | 100 1001 | 49  | 100 1110 | 4E  | 010 0011 | 23  | 100 0000 | 40  |
     * | 111 1001 | 79  | 000 0110 | 06  | 000 1010 | 0A  | 111 1000 | 78  | 001 0010 | 12  | 001 1100 | 1C  | 100 0110 | 46  |          |     |
     * Address Sequence (Available addresses = 27 (128) – 1 = 127)
     * 
     * ※「777 Design Note.pdf」 7 bit Polynomial Address Counter Implemented より引用
     */
    u16 pc;

    /**
     * @brief   次の命令をスキップするかどうかのフラグ
     * 
     * - 引き算などの命令で条件を満たしたときにセットされる
     * - このフラグがセットされていると次に実行される命令がスキップされる
     * - 命令をスキップするとリセットされる
     * 
     * @arg true  : 次の命令をスキップする
     * @arg false : 次の命令をスキップしない
     */
    bool skip;

    /**
     * @brief   A1レジスタ(7bit)
     * 
     * - 汎用レジスタ
     */
    u8 A1;
    /**
     * @brief   A2レジスタ(7bit)
     * 
     * - 汎用レジスタ
     */
    u8 A2;
    /**
     * @brief   A3レジスタ(7bit)
     * 
     * - 28bitメモリ転送に使用されるレジスタ
     */
    u8 A3;
    /**
     * @brief   A4レジスタ(7bit)
     * 
     * - 28bitメモリ転送に使用されるレジスタ
     */
    u8 A4;

    /**
     * @brief   X4レジスタ(7bit)
     * 
     * - 不明
     * - 「H<=>X」命令で、Hレジスタと交換されるレジスタ
     */
    u8 X4;

    /**
     * @brief   Lレジスタ(2bit)
     * 
     * - メモリアクセス時にHレジスタと共に使用されて、読み書きするアドレスを示す
     * - アクセスするメモリのアドレスは((H << 2) | L)で計算される
     */
    u8 L;
    /**
     * @brief   Hレジスタ(5bit)
     * 
     * - メモリアクセス時にLレジスタと共に使用されて、読み書きするアドレスを示す
     * - アクセスするメモリのアドレスは((H << 2) | L)で計算される
     */
    u8 H;

    /**
     * @brief   Lの裏レジスタL'(2bit)
     * 
     * - 「H<=>X」命令でLレジスタと切り替えられるレジスタ
     */
    u8 L_;

    /**
     * @brief   ラインバッファ(12個 x 5bit x 2つ)
     * 
     * - 1ラインを描画するときに使用されるバッファ
     * - 12個のスプライトアドレス(5bit)を格納(12個 x 5bit)
     * - 描画用と表示用で2つ
     */
    LineBufferManager lineBufferManager;

    /**
     * @brief   STBレジスタ(4bit)
     * 
     * - キー入力のときに使用されるレジスタ
     * - 負論理
     * - 値はCPUのS1～S4に出力される
     * - S1～S4の信号はカセットのA8～A12ピンに結線されている
     *   - 結線のされかたは、カセットによって異なっている
     *   - S2やS3に結線されているカセットは無く、S1とS2だけが結線され使用されている。
     * 
     * <pre>
     * STB:xxx4321
     *        ||||
     *        |||S1 : CPUのS1ピン  カセットのA8～A12に結線される
     *        ||S2  : CPUのS2ピン  カセットのA8～A12に結線される
     *        |S3   : CPUのS3ピン  カセットのA8～A12に結線される（未使用）
     *        S4    : CPUのS4ピン  カセットのA8～A12に結線される（未使用）
     * </pre>
     * 
     * A8～A12とキー入力の関係表
     * | ピン   | B15            | B14              | B13                | B12               | B11            | B10    | B9     |
     * |--------|----------------|------------------|--------------------|-------------------|----------------|--------|--------|
     * | A8     | GameStartKey   | LeverSwitch1Left | LeverSwitch1Right  | GameSelectKey     | AUX            | ---    | ---    |
     * | A9     | ---            | LeverSwitch2Left | LeverSwitch2Right  | ---               | ---            | ---    | ---    |
     * | A10    | ---            | ---              | ---                | ---               | ---            | Push4  | Push3  |
     * | A11    | ---            | ---              | ---                | ---               | ---            | Push2  | Push1  |
     * | A12    | CourseSwitchLL | CourseSwitchLeft | CourseSwitchCenter | CourseSwitchRight | CourseSwitchRR | ---    | ---    |
     * 
     * ※B15～B9は『概ね』K1～K7に結線されているが、カセットによって異なっていることに注意
     */
    u8 STB;

    /**
     * @brief   画面表示フラグ
     * 
     * 画面を表示するかどうかのフラグ
     * @arg     true:画面表示
     * @arg     false:画面非表示
     */
    bool DISP;

    /**
     * @brief   不明
     */
    bool GPE;

    /**
     * @brief   キー入力の有効、無効フラグ
     * 
     * @arg true: 「K=>M」命令でのキー入力が有効
     * @arg false: 「K=>M」命令でのキー入力は無効
     * 
     * 「K=>M」命令で、フラグがセットされている場合、K1～K7からの入力になる。
     * 「K=>M」命令のKの値は無視される。
     * 
     * @see STB
     */
    bool KIE;

    /**
     * @brief   不明
     */
    bool SME;

    /**
     * @brief   モードレジスタ(7bit)
     * 
     * <pre>
     * mode:RBHPRGB
     *      |||||||
     *      ||||||Background color Blue bit  BG B
     *      |||||Background color Green bit  BG G
     *      ||||Background color Red bit     BG R
     *      |||Black&White                   BLACK/PRIO 1:Enabled  0:Color only
     *      ||Hue                            HUE        1:Dense    0:Thin
     *      |Brightness                      BRIGHTNESS 1:High     0:Low
     *      Reverberated sound effect        REV        1:Enabled  0:Disabled
     * </pre>
     * 
     * ※「777 Design Note.pdf」 MODE Registers より参照
     */
    u8 mode;
public:
    /**
     * コンストラクタ
     */
    Registers()
        : pc()
        , skip()
        , A1()
        , A2()
        , A3()
        , A4()
        , X4()
        , L()
        , H()
        , L_()
        , lineBufferManager()
        , STB()
        , DISP()
        , GPE()
        , KIE()
        , SME()
        , mode()
    {
    }

    /**
     * @brief 画面表示フラグを設定する
     * @param[in]   D   画面表示フラグ
     *              @arg    true:  画面表示
     *              @arg    false: 画面非表示
     * @see DISP
     */
    inline void setDISP(const bool D) noexcept { DISP = D; }
    /**
     * @brief 画面表示フラグを取得する
     * @return 画面表示フラグ
     * @retval true:  画面表示
     * @retval false: 画面非表示
     * @see DISP
     */
    inline bool getDISP() const noexcept { return DISP; }
    inline void setGPE(const bool G) noexcept { GPE = G; }
    /**
     * @brief キー入力の有効無効を設定する
     * @param[in]   K   有効無効のフラグ
     *              @arg    true:  キー入力有効
     *              @arg    false: キー入力無効
     * @see KIE
     */
    inline void setKIE(const bool K) noexcept { KIE = K; }
    /**
     * @brief キー入力の有効無効を取得する
     * @return キー入力の有効無効のフラグ
     * @retval true:  キー入力有効(「K=>M」命令でキー入力される)
     * @retval false: キー入力無効
     * @see KIE
     */
    inline bool getKIE() const noexcept { return KIE; }
    /**
     * @brief SMEフラグを設定する
     * @param[in]   S   不明
     */
    inline void setSME(const bool S) noexcept { SME = S; }
    /**
     * @brief SMEフラグを取得する
     * @return 不明
     */
    inline bool getSME() const noexcept { return SME; }
    /**
     * @brief   PCのA11(10ビット目)を設定する
     * 
     * - PCの10ビット目を個別に設定する
     * - 10ビット目は、JP命令では変更されず、JS命令、SRE命令、N=>A11命令などで設定される
     * 
     * @param[in]   flag    フラグ
     *              @arg    true:  A11(10ビット目)をセットする
     *              @arg    false: A11(10ビット目)をリセットする
     * @see pc
     */
    inline void setA11(const bool flag) noexcept { if(flag) { pc |= 0x400; } else { pc &= ~0x400; } }
    /**
     * @brief   PCを設定する
     * @param[in]   address PCの値
     * @see pc
     */
    inline void setPC(const u16 address) noexcept { pc = address & 0x7FF; }
    /**
     * @brief   PCを取得する
     * @return  PCの値
     * @see pc
     */
    inline u16 getPC() const noexcept { return pc & 0x7FF; }
    /**
     * @brief   PCを１つ進める
     * @see pc
     */
    void nextPC() noexcept { setPC(nextPCAddress(getPC())); }
    /**
     * @brief   次のPCを生成する
     * @param[in]   address PCのアドレス
     * @return  次のPCのアドレス
     * @see pc
     */
    u16 nextPCAddress(const u16 address) const noexcept
    {
        u16 a6 = (address >> 5) & 1;
        u16 a7 = (address >> 6) & 1;
        u16 a1 = 1 ^ (a6 ^ a7);
        return (((address << 1) | a1) & 0x7F)
            | (address & 0x780);
    }

    /**
     * @brief 描画用のラインバッファに書き込む
     * @param[in]   value   スプライトのアドレス(5bit)
     */
    void setLineBufferRegister(const u8 value) { lineBufferManager.write(value); }
    /**
     * @brief 描画用ラインバッファと表示用ラインバッファを入れ替える
     */
    void flipLineBufferRegister() noexcept { lineBufferManager.flipBuffer(); }
    /**
     * @brief 描画用ラインバッファクリアして、書き込めるようにする
     */
    void clearLineBufferRegister() noexcept { lineBufferManager.clear(); }
    /**
     * @brief 表示用のラインバッファを取得する
     * @return 表示用のラインバッファ
     */
    inline const LineBufferRegister& getDisplayLineBuffer() const noexcept { return lineBufferManager.getDisplayLineBuffer(); }

    /**
     * @brief   Lレジスタ(2bit)を設定する
     * @param[in]   value   設定する値(0～3)
     * @see L
     */
    inline void setL(const u8 value) noexcept{ L = value & 0x03; }
    inline u8 getL() const noexcept { return L & 0x03; }
    /**
     * @brief   Hレジスタ(5bit)を設定する
     * @param[in]   value   設定する値(0～0x1F)
     * @see H
     */
    inline void setH(const u8 value) noexcept { H = value & 0x1F; }
    inline u8 getH() const noexcept { return H & 0x1F; }
    /**
     * @brief   メモリアクセスを行うときのアドレスを取得する
     * @return  メモリアクセスを行うときのアドレス
     * @see L
     * @see H
     */
    inline u16 getAddressHL() const noexcept { return ((u16)getH() << 2) | (u16)getL(); }

    /**
     * @brief A1レジスタ(7bit)を設定する
     * @param[in]   value   設定する値
     * @see A1
     */
    inline void setA1(const u8 value) noexcept { A1 = value & 0x7F; }
    inline u8 getA1() const noexcept { return A1 & 0x7F; }
    /**
     * @brief A2レジスタ(7bit)を設定する
     * @param[in]   value   設定する値
     * @see A2
     */
    inline void setA2(const u8 value) noexcept { A2 = value & 0x7F; }
    inline u8 getA2() const noexcept { return A2 & 0x7F; }
    /**
     * @brief A3レジスタ(7bit)を設定する
     * @param[in]   value   設定する値
     * @see A3
     */
    inline void setA3(const u8 value) noexcept { A3 = value & 0x7F; }
    inline u8 getA3() const noexcept { return A3 & 0x7F; }
    /**
     * @brief A4レジスタ(7bit)を設定する
     * @param[in]   value   設定する値
     * @see A4
     */
    inline void setA4(const u8 value) noexcept { A4 = value & 0x7F; }
    inline u8 getA4() const noexcept { return A4 & 0x7F; }

    /**
     * @brief X4レジスタ(7bit)を設定する
     * @param[in]   value   設定する値
     * @see X4
     */
    inline void setX4(const u8 value) noexcept { X4 = value & 0x7F; }
    inline u8 getX4() const noexcept { return X4 & 0x7F; }
    /**
     * @brief 裏レジスタL'(2bit)を設定する
     * @param[in]   value   設定する値
     * @see L_
     */
    inline void setL_(const u8 value) noexcept { L_ = value & 0x03; }
    inline u8 getL_() const noexcept { return L_ & 0x03; }

    /**
     * @brief       STBレジスタをシフトし値を設定する
     * @param[in]   value   レジスタをシフト後に設定する値
     *              @arg    0:      0を設定する
     *              @arg    0以外:  1を設定する
     * @see STB
     */
    void shiftSTB(const u8 value) noexcept {
        STB <<= 1;
        STB |= value ? 1 : 0;
        STB &= 0xF;
    }
    /**
     * @brief   STBレジスタを取得する
     * @return  STBの値
     * @see STB
     */
    inline u8 getSTB() const noexcept { return STB & 0xF; }

    /**
     * @brief   モードレジスタを設定する
     * @param[in]   value   モードレジスタの値
     * @see mode
     */
    inline void setMode(const u8 value) noexcept { mode = value & 0x7F; }
    /**
     * @brief   モードレジスタを取得する
     * @return  モードレジスタの値
     * @see mode
     */
    inline u8 getMode() const noexcept { return mode & 0x7F; }
    /**
     * @brief   モードレジスタのREVを取得する
     * @return  モードレジスタのREVの値
     * @see mode
     */
    inline bool getREV() const noexcept { return (mode & 0x40) != 0; }

    /**
     * @brief 命令スキップフラグを設定する
     * @param[in]   flag    命令スキップフラグ
     *              @arg    true: 次の命令をスキップする
     *              @arg    false: 次の命令をスキップしない
     */
    inline void setSkip(const bool flag) noexcept { skip = flag; }
    /**
     * @brief   命令スキップフラグを取得する
     * @return  命令スキップフラグ
     * @retval  true:  次の命令をスキップする
     * @retval  false: 次の命令をスキップしない
     */
    inline bool isSkip() const noexcept { return skip; }
    /**
     * @brief   命令スキップフラグをセットする
     * @see     setSkip(const bool)
     */
    inline void setSkip() noexcept { setSkip(true); }
    /**
     * @brief   命令スキップフラグをリセットする
     * @see     setSkip(const bool)
     */
    inline void resetSkip() noexcept { setSkip(false); }

    /**
     * @brief リセット
     */
    void reset()
    {
        pc = 0x000;
        skip = false;
        A1 = 0;
        A2 = 0;
        A3 = 0;
        A4 = 0;
        X4 = 0;
        H = 0;
        L = 0;
        L_ = 0;
        lineBufferManager.reset();
        STB = 0;

        DISP = false;
        GPE = false;
        KIE = false;
        SME = false;

        mode = 0;
    }
};
