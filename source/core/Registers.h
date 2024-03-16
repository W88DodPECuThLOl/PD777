#pragma once

#include "catLowBasicTypes.h"

class Registers {
    /**
     * @brief   プログラムカウンタ
     */
    u16 pc;
    /**
     * @brief   次の命令をスキップするかどうかのフラグ
     * 
     * 引き算などの命令で条件を満たしたときにセットされる。
     * 命令をスキップするとリセットされる。
     */
    bool skip;

    /**
     * @brief   A1レジスタ(7bit)
     */
    u8 A1;
    /**
     * @brief   A2レジスタ(7bit)
     */
    u8 A2;
    /**
     * @brief   A3レジスタ(7bit)
     */
    u8 A3;
    /**
     * @brief   A4レジスタ(7bit)
     */
    u8 A4;
    /**
     * @brief   X4レジスタ(7bit)
     * 
     * 不明
     */
    u8 X4;

    /**
     * @brief   Lレジスタ(2bit)
     * 
     * メモリアクセス時にHレジスタと共に使用されて、読み書きするアドレスを示す。
     */
    u8 L;
    /**
     * @brief   LレジスタH(5bit)
     * 
     * メモリアクセス時にLレジスタと共に使用されて、読み書きするアドレスを示す。
     */
    u8 H;

    /**
     * @brief   Lの裏レジスタL'
     * 
     * 「H <=> X」命令でLレジスタと切り替えられる物
     */
    u8 L_;

    /**
     * @brief   ラインバッファレジスタ(5bit)
     */
    u8 lineBufferRegister;

    /**
     * @brief   STBレジスタ(4bit)
     * 
     * メモ）
     * ・キー入力のときに使用されるレジスタ
     * ・負論理
     * ・CPUのS1～S4に出力される
     * ・S1～S4はカセット毎にA8～A12に結線されている
     */
    u8 STB;

    /**
     * @brief   画面表示フラグ
     */
    bool DISP;
    /**
     * @brief   不明
     */
    bool GPE;
    /**
     * @brief   不明
     * 
     * 入力するときにセットされる。
     */
    bool KIE;
    /**
     * @brief   不明
     */
    bool SME;

    /**
     * @brief   モードレジスタ
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
        , lineBufferRegister()
        , STB()
        , DISP()
        , GPE()
        , KIE()
        , SME()
        , mode()
    {
    }

    void setDISP(bool D) { DISP = D; }
    bool getDISP() const { return DISP; }
    void setGPE(bool G) { GPE = G; }
    void setKIE(bool K) { KIE = K; }
    bool getKIE() const{ return KIE; }
    void setSME(bool S) { SME = S; }
    bool getSME() const { return SME; }
    void setA11(bool flag) { if(flag) { pc |= 0x400; } else { pc &= ~0x400; } }
    void setPC(const u16 address) { pc = address & 0xFFF; }
    u16 getPC() const { return pc & 0xFFF; }
    void nextPC() { setPC(nextPCAddress(getPC())); }
    /**
     * @brief 次のPCを生成する
     * @param[in]   address PCのアドレス
     * @return  次のPCのアドレス
     */
    u16 nextPCAddress(const u16 address)
    {
        u16 a6 = (address >> 5) & 1;
        u16 a7 = (address >> 6) & 1;
        u16 a1 = 1 ^ (a6 ^ a7);
        return (((address << 1) | a1) & 0x7F)
            | (address & 0x780);
    }

    void setLineBufferRegister(const u8 value) { lineBufferRegister = value & 0x1F; }
    
    void setL(const u8 value) { L = value & 0x03; }
    u8 getL() const { return L & 0x03; }
    void setH(u8 value) { H = value & 0x1F; }
    u8 getH() const { return H & 0x1F; }
    /**
     * @brief   メモリアクセスを行うときのアドレスを取得する
     * @return  メモリアクセスを行うときのアドレス
     */
    u16 getAddressHL() const { return ((u16)getH() << 2) | (u16)getL(); }
    void setA1(u8 value) { A1 = value & 0x7F; }
    u8 getA1() const { return A1 & 0x7F; }
    void setA2(u8 value) { A2 = value & 0x7F; }
    u8 getA2() const { return A2 & 0x7F; }
    void setA3(u8 value) { A3 = value & 0x7F; }
    u8 getA3() const { return A3 & 0x7F; }
    void setA4(u8 value) { A4 = value & 0x7F; }
    u8 getA4() const { return A4 & 0x7F; }

    void setX4(u8 value) { X4 = value & 0x7F; }
    u8 getX4() const { return X4 & 0x7F; }
    void setL_(const u8 value) { L_ = value & 0x03; }
    u8 getL_() const { return L_ & 0x03; }

    void shiftSTB(const u8 value) {
        STB <<= 1;
        STB |= value ? 1 : 0;
        STB &= 0xF;
    }
    u8 getSTB() const { return STB & 0xF; }

    void setMode(const u8 value) { mode = value & 0x7F; }
    u8 getMode() const { return mode & 0x7F; }
    bool getREV() const { return (mode & 0x40) != 0; }

    void setSkip() { skip = true; }
    void resetSkip() { skip = false; }
    void setSkip(bool flag) { skip = flag; }
    bool isSkip() const { return skip; }

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
        lineBufferRegister = 0;
        STB = 0;

        DISP = false;
        GPE = false;
        KIE = false;
        SME = false;

        mode = 0;
    }
};
