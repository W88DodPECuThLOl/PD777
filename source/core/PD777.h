#pragma once

#include "catLowBasicTypes.h"

#include "CRT.h"
#include "Decoder.h"
#include "Disassembler.h"
#include "Stack.h"
#include "Registers.h"
#include "Sound.h"
#include "GraphCommand.h"
#include "GraphCommandBuffer.h"

/**
 * @brief   μPD777
 */
class PD777 : public Decoder {
protected:
    /**
     * @brief ROMデータの元
     * address, dataの組が続いている。
     */
    static const u16 rawRom[4096];
    /**
     * @brief 7x7パターン用のROM
     */
    static const u8 patternRom[686];
    /**
     * @brief 8x7パターン用のROM
     */
    static const u8 patternRom8[98];
    /**
     * @brief パターンの属性
     */
    static const u8 characterAttribute[0x80*2];

    u16 rom[0x800];
    u8  ram[0x80];
    CRT crt;
    Registers regs;
    Stack stack;
    Sound sound;

    // presentで使うフレームバッファ
    /**
     * @brief フレームバッファ上でのμPD777の１ドット高さ
     */
    static constexpr s32 dotHeight = 4;
    /**
     * @brief フレームバッファ上でのμPD777の１ドット幅
     */
    static constexpr s32 dotWidth = 4;
    /**
     * @brief フレームバッファの高さ
     */
    static constexpr u32 frameBufferHeight = 128*dotHeight;
    /**
     * @brief フレームバッファの横幅
     */
    static constexpr u32 frameBufferWidth = 128*dotWidth;
    /**
     * @brief presentで使うフレームバッファ
     */
    u32 frameBuffer[frameBufferHeight*frameBufferWidth];
    /**
     * @brief 描画コマンドバッファ
     */
    GraphCommandBuffer graphBuffer;
#if defined(_WIN32)
    // for debug
    Disassembler disassembler;
#endif // defined(_WIN32)
private:
    /**
     * @brief   romをセットアップする
     */
    void setupRom();

    /**
     * @brief キャラクタの属性を取得する
     * @param[in]   characterNo キャラクタ
     * @param[out]  repeatY     Y方向にリピートするかどうか
     * @param[out]  repeatX     X方向にリピートするかどうか
     * @param[out]  bent1       斜めのドットかどうか
     * @param[out]  bent2       斜めのドットかどうか
     */
    void getCharacterAttribute(const u8 characterNo, bool& repeatY, bool& repeatX, bool& bent1, bool& bent2);
    /**
     * @brief 1ドット描画する
     * @param[in]   x       X座標(0～90)
     * @param[in]   y       Y座標(0～59)
     * @param[in]   rgb     色
     * @param[in]   bent1   ベント1
     * @param[in]   bent2   ベント2
     */
    void pset(s32 x, s32 y, u32 rgb, bool bent1, bool bent2);

    /**
     * @brief スプライトをラスタライズする
     * 
     * スプライトをラスタライズして、描画コマンドバッファに描画コマンドを追加する。
     * 
     * @param[in,out]   commandBuffer   描画コマンドバッファ
     * @param[in]       ram             メモリ
     * @param[in]       verticalCounter 垂直カウンタ（ライスタライズする位置）
     */
    void spriteRasterize(GraphCommandBuffer* commandBuffer, const u8* ram, const u16 verticalCounter);

protected:
    virtual void execNOP(const u16 pc, const u16 code) override;
    virtual void execGPL(const u16 pc, const u16 code) override;
    virtual void execMoveHtoNRM(const u16 pc, const u16 code) override;
    virtual void execExchangeHandX(const u16 pc, const u16 code) override;
    virtual void execSRE(const u16 pc, const u16 code) override;
    virtual void execSTB(const u16 pc, const u16 code) override;
    //
    virtual void exec4H_BLK(const u16 pc, const u16 code) override;
    virtual void execVBLK(const u16 pc, const u16 code) override;
    virtual void execGPSW_(const u16 pc, const u16 code) override;
    //
    virtual void execMoveAtoMA(const u16 pc, const u16 code) override;
    virtual void execMoveMAtoA(const u16 pc, const u16 code) override;
    virtual void execExchangeMAandA(const u16 pc, const u16 code) override;
    //
    virtual void execSRE_1(const u16 pc, const u16 code) override;
    //
    virtual void execPD1_J(const u16 pc, const u16 code) override;
    virtual void execPD2_J(const u16 pc, const u16 code) override;
    virtual void execPD3_J(const u16 pc, const u16 code) override;
    virtual void execPD4_J(const u16 pc, const u16 code) override;
    virtual void execPD1_NJ(const u16 pc, const u16 code) override;
    virtual void execPD2_NJ(const u16 pc, const u16 code) override;
    virtual void execPD3_NJ(const u16 pc, const u16 code) override;
    virtual void execPD4_NJ(const u16 pc, const u16 code) override;
    //
    virtual void execTestSubMK(const u16 pc, const u16 code) override;
    //
    virtual void execAddMKM(const u16 pc, const u16 code) override;
    virtual void execSubMKM(const u16 pc, const u16 code) override;
    //
    virtual void execTestAndA1A1_EQJ(const u16 pc, const u16 code) override;
    virtual void execTestAndA1A1_NOT_EQJ(const u16 pc, const u16 code) override;
    virtual void execTestEquA1A1_EQJ(const u16 pc, const u16 code) override;
    virtual void execTestEquA1A1_NOT_EQJ(const u16 pc, const u16 code) override;
    virtual void execTestSubA1A1_BOJ(const u16 pc, const u16 code) override;
    virtual void execTestSubA1A1_NOT_BOJ(const u16 pc, const u16 code) override;
    virtual void execTestAndA1A2_EQJ(const u16 pc, const u16 code) override;
    virtual void execTestAndA1A2_NOT_EQJ(const u16 pc, const u16 code) override;
    virtual void execTestEquA1A2_EQJ(const u16 pc, const u16 code) override;
    virtual void execTestEquA1A2_NOT_EQJ(const u16 pc, const u16 code) override;
    virtual void execTestSubA1A2_BOJ(const u16 pc, const u16 code) override;
    virtual void execTestSubA1A2_NOT_BOJ(const u16 pc, const u16 code) override;
    virtual void execTestAndA2A1_EQJ(const u16 pc, const u16 code) override;
    virtual void execTestAndA2A1_NOT_EQJ(const u16 pc, const u16 code) override;
    virtual void execTestEquA2A1_EQJ(const u16 pc, const u16 code) override;
    virtual void execTestEquA2A1_NOT_EQJ(const u16 pc, const u16 code) override;
    virtual void execTestSubA2A1_BOJ(const u16 pc, const u16 code) override;
    virtual void execTestSubA2A1_NOT_BOJ(const u16 pc, const u16 code) override;
    virtual void execTestAndA2A2_EQJ(const u16 pc, const u16 code) override;
    virtual void execTestAndA2A2_NOT_EQJ(const u16 pc, const u16 code) override;
    virtual void execTestEquA2A2_EQJ(const u16 pc, const u16 code) override;
    virtual void execTestEquA2A2_NOT_EQJ(const u16 pc, const u16 code) override;
    virtual void execTestSubA2A2_BOJ(const u16 pc, const u16 code) override;
    virtual void execTestSubA2A2_NOT_BOJ(const u16 pc, const u16 code) override;
    //
    virtual void execTestAndMA1_EQJ(const u16 pc, const u16 code) override;
    virtual void execTestAndMA1_NOT_EQJ(const u16 pc, const u16 code) override;
    virtual void execTestEquMA1_EQJ(const u16 pc, const u16 code) override;
    virtual void execTestEquMA1_NOT_EQJ(const u16 pc, const u16 code) override;
    virtual void execTestSubMA1_BOJ(const u16 pc, const u16 code) override;
    virtual void execTestSubMA1_NOT_BOJ(const u16 pc, const u16 code) override;
    virtual void execTestAndMA2_EQJ(const u16 pc, const u16 code) override;
    virtual void execTestAndMA2_NOT_EQJ(const u16 pc, const u16 code) override;
    virtual void execTestEquMA2_EQJ(const u16 pc, const u16 code) override;
    virtual void execTestEquMA2_NOT_EQJ(const u16 pc, const u16 code) override;
    virtual void execTestSubMA2_BOJ(const u16 pc, const u16 code) override;
    virtual void execTestSubMA2_NOT_BOJ(const u16 pc, const u16 code) override;
    virtual void execTestAndHA1_EQJ(const u16 pc, const u16 code) override;
    virtual void execTestAndHA1_NOT_EQJ(const u16 pc, const u16 code) override;
    virtual void execTestEquHA1_EQJ(const u16 pc, const u16 code) override;
    virtual void execTestEquHA1_NOT_EQJ(const u16 pc, const u16 code) override;
    virtual void execTestSubHA1_BOJ(const u16 pc, const u16 code) override;
    virtual void execTestSubHA1_NOT_BOJ(const u16 pc, const u16 code) override;
    virtual void execTestAndHA2_EQJ(const u16 pc, const u16 code) override;
    virtual void execTestAndHA2_NOT_EQJ(const u16 pc, const u16 code) override;
    virtual void execTestEquHA2_EQJ(const u16 pc, const u16 code) override;
    virtual void execTestEquHA2_NOT_EQJ(const u16 pc, const u16 code) override;
    virtual void execTestSubHA2_BOJ(const u16 pc, const u16 code) override;
    virtual void execTestSubHA2_NOT_BOJ(const u16 pc, const u16 code) override;
    //
    virtual void execMoveNtoL(const u16 pc, const u16 code) override;
    virtual void execMoveA2toA1(const u16 pc, const u16 code) override;
    virtual void execSubA1andA2toA2(const u16 pc, const u16 code) override;
    virtual void execMoveA1toFLS(const u16 pc, const u16 code) override;
    virtual void execMoveA1toFRS(const u16 pc, const u16 code) override;
    virtual void execMoveA1toMODE(const u16 pc, const u16 code) override;
    virtual void execRightShiftA1(const u16 pc, const u16 code) override;
    virtual void execMoveA1toA2(const u16 pc, const u16 code) override;
    virtual void execMoveA2toFLS(const u16 pc, const u16 code) override;
    virtual void execMoveA2toFRS(const u16 pc, const u16 code) override;
    virtual void execMoveA2toMODE(const u16 pc, const u16 code) override;
    virtual void execSubA2andA1toA1(const u16 pc, const u16 code) override;
    virtual void execRightShiftA2(const u16 pc, const u16 code) override;
    virtual void execMoveMtoFLS(const u16 pc, const u16 code) override;
    virtual void execMoveMtoFRS(const u16 pc, const u16 code) override;
    virtual void execMoveMtoMODE(const u16 pc, const u16 code) override;
    virtual void execRightShiftM(const u16 pc, const u16 code) override;
    //
    virtual void execAndA1A1toA1(const u16 pc, const u16 code) override;
    virtual void execAddA1A1toA1(const u16 pc, const u16 code) override;
    virtual void execOrA1A1toA1(const u16 pc, const u16 code) override;
    virtual void execSubA1A1toA1(const u16 pc, const u16 code) override;
    virtual void execAndA1A2toA1(const u16 pc, const u16 code) override;
    virtual void execAddA1A2toA1(const u16 pc, const u16 code) override;
    virtual void execOrA1A2toA1(const u16 pc, const u16 code) override;
    virtual void execSubA1A2toA1(const u16 pc, const u16 code) override;
    virtual void execAndA2A1toA2(const u16 pc, const u16 code) override;
    virtual void execAddA2A1toA2(const u16 pc, const u16 code) override;
    virtual void execOrA2A1toA2(const u16 pc, const u16 code) override;
    virtual void execSubA2A1toA2(const u16 pc, const u16 code) override;
    virtual void execAndA2A2toA2(const u16 pc, const u16 code) override;
    virtual void execAddA2A2toA2(const u16 pc, const u16 code) override;
    virtual void execOrA2A2toA2(const u16 pc, const u16 code) override;
    virtual void execSubA2A2toA2(const u16 pc, const u16 code) override;
    //
    virtual void execMoveA1toM(const u16 pc, const u16 code) override;
    virtual void execMoveA2toM(const u16 pc, const u16 code) override;
    virtual void execExchangeMA1(const u16 pc, const u16 code) override;
    virtual void execExchangeMA2(const u16 pc, const u16 code) override;
    virtual void execMoveMtoA1(const u16 pc, const u16 code) override;
    virtual void execMoveMtoA2(const u16 pc, const u16 code) override;
    //
    virtual void execAndMA2toM(const u16 pc, const u16 code) override;
    virtual void execAddMA2toM(const u16 pc, const u16 code) override;
    virtual void execOrMA2toM(const u16 pc, const u16 code) override;
    virtual void execSubMA2toM(const u16 pc, const u16 code) override;
    virtual void execAndMA1toM(const u16 pc, const u16 code) override;
    virtual void execAddMA1toM(const u16 pc, const u16 code) override;
    virtual void execOrMA1toM(const u16 pc, const u16 code) override;
    virtual void execSubMA1toM(const u16 pc, const u16 code) override;
    //
    virtual void execMoveA1toH(const u16 pc, const u16 code) override;
    virtual void execMoveA2toH(const u16 pc, const u16 code) override;
    virtual void execMoveHtoA1(const u16 pc, const u16 code) override;
    virtual void execMoveHtoA2(const u16 pc, const u16 code) override;
    //
    virtual void execAndHA1toH(const u16 pc, const u16 code) override;
    virtual void execAddHA1toH(const u16 pc, const u16 code) override;
    virtual void execOrHA1toH(const u16 pc, const u16 code) override;
    virtual void execSubHA1toH(const u16 pc, const u16 code) override;
    virtual void execAndHA2toH(const u16 pc, const u16 code) override;
    virtual void execAddHA2toH(const u16 pc, const u16 code) override;
    virtual void execOrHA2toH(const u16 pc, const u16 code) override;
    virtual void execSubHA2toH(const u16 pc, const u16 code) override;
    //
    virtual void execSetA11(const u16 pc, const u16 code) override;
    virtual void execJPM(const u16 pc, const u16 code) override;
    virtual void execSetFlags(const u16 pc, const u16 code) override;
    //
    virtual void execSubHKtoH(const u16 pc, const u16 code) override;
    virtual void execAddHKtoH(const u16 pc, const u16 code) override;
    //
    virtual void execMoveKtoM(const u16 pc, const u16 code) override;
    virtual void execMoveKtoLH(const u16 pc, const u16 code) override;
    virtual void execMoveKtoA1(const u16 pc, const u16 code) override;
    virtual void execMoveKtoA2(const u16 pc, const u16 code) override;
    virtual void execMoveKtoA3(const u16 pc, const u16 code) override;
    virtual void execMoveKtoA4(const u16 pc, const u16 code) override;
    //
    virtual void execJP(const u16 pc, const u16 code) override;
    virtual void execJS(const u16 pc, const u16 code) override;

protected:
    void writeMemAtHL(const u8 value);
    u8 readMemAtHL();
    void setMode(const u8 mode);
protected:
    // memory access
    virtual void writeMem(const u16 address, const u8 value);
    virtual u8   readMem(const u16 address);

    // graph
    /**
     * @brief present()で使用するイメージを作成する
     * 128x128のframeBufferにイメージが作成される。
     */
    virtual void makePresentImage();
    /**
     * @brief イメージを画面に出す
     */
    virtual void present() {}

    // sound
    /**
     * @brief Lチャンネルのサウンドの音程を設定する
     * @param[in]   value   音程。1は無音
     */
    virtual void setFLS(const s64 clockCounter, const u8 value) {}
    /**
     * @brief Rチャンネルのサウンドの音程を設定する
     * @param[in]   value   音程。1は無音
     */
    virtual void setFRS(const s64 clockCounter, const u8 value) {}

    // input

    /**
     * @brief パッドの値の範囲、下限値
     */
    static constexpr u8 PAD_MIN_VALUE = 0;
    /**
     * @brief パッドの値の範囲、上限値
     */
    static constexpr u8 PAD_MAX_VALUE = 105;

    /**
     * @brief PD1の入力を取得する
     * 
     * @param[out]  value   パッドの値(PAD_MIN_VALUE～PAD_MAX_VALUE)
     * @return 入力されていたらtrueを返す
     */
    virtual bool isPD1(u8& value) { value = PAD_MIN_VALUE; return false; }
    /**
     * @brief PD2の入力を取得する
     * 
     * @param[out]  value   パッドの値(PAD_MIN_VALUE～PAD_MAX_VALUE)
     * @return 入力されていたらtrueを返す
     */
    virtual bool isPD2(u8& value) { value = PAD_MIN_VALUE; return false; }
    /**
     * @brief PD3の入力を取得する
     * 
     * @param[out]  value   パッドの値(PAD_MIN_VALUE～PAD_MAX_VALUE)
     * @return 入力されていたらtrueを返す
     */
    virtual bool isPD3(u8& value) { value = PAD_MIN_VALUE; return false; }
    /**
     * @brief PD4の入力を取得する
     * 
     * @param[out]  value   パッドの値(PAD_MIN_VALUE～PAD_MAX_VALUE)
     * @return 入力されていたらtrueを返す
     */
    virtual bool isPD4(u8& value) { value = PAD_MIN_VALUE; return false; }

    /**
     * @brief KINの値
     */
    enum class KIN : u8 {
        None = 0x00,

        /**
         * @brief	ゲームスタートキー
         */
        GameStartKey     = 0x01,
        /**
         * @brief	ゲームセレクトキー
         */
        GameSelectKey    = 0x08,
        /**
         * @brief	ゲーム操作キー（レバースイッチ）の左
         */
        LeverSwitchLeft  = 0x02,
        /**
         * @brief	ゲーム操作キー（レバースイッチ）の右
         */
        LeverSwitchRight = 0x04,

        /**
         * @brief	ゲーム操作キー（アクションキー）のPUSH 1
         */
        Push1            = 0x40,
        /**
         * @brief	ゲーム操作キー（アクションキー）のPUSH 2
         */
        Push2            = 0x20,

        /**
         * @brief	ゲーム操作キー（アクションキー）のPUSH 3
         * @note STBが0x0Dの時
         */
        Push3            = 0x2,

        /**
         * @brief	ゲーム操作キー（アクションキー）のPUSH 4
         * @note STBが0x0Dの時
         */
        Push4            = 0x4,
    };
    /**
     * @brief キーの入力
     * 
     * @param[in]   STB     不明
     * @return  押下されているキーの論理和
     * @see     enum KIN
     */
    virtual u8 readKIN(const u8 STB) { return 0; }
    /**
     * @brief 不明。水平カウンタの取得？
     * @return 不明
     */
    virtual u8 readHCL() { return crt.getHorizontalCounter(); }

public:
    // for debug
    virtual void registerDump() {}
    void disassemble(const u16 pc, const u16 code);
    void disassembleAll();

public:
    /**
     * @brief 初期化
     */
    void init();

    /**
     * @brief 終了処理
     */
    void term();

    /**
     * @brief １命令実行する
     */
    void execute();
};
