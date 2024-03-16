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
#include "KeyStatus.h"

/**
 * @brief   μPD777
 */
class PD777 : public Decoder {
public:
    // presentで使うフレームバッファ
    /**
     * @brief フレームバッファ上でのμPD777の１ドット高さ
     */
    static constexpr s32 dotHeight = 4;
    /**
     * @brief フレームバッファ上でのμPD777の１ドット幅
     */
    static constexpr s32 dotWidth = 5;

    /**
     * @brief フレームバッファの高さ
     */
    static constexpr u32 frameBufferHeight = 128*dotHeight;
    /**
     * @brief フレームバッファの横幅
     */
    static constexpr u32 frameBufferWidth = 128*dotWidth;
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

    u16 rom[0x800] {};
    u8 patternCGRom7x7[686] {};
    u8 patternCGRom8x7[98] {};
    u8 characterBent[0x80*2] {};
    u8  ram[0x80] {};
    CRT crt;
    Registers regs;
    Stack stack;
    Sound sound;

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

    u32 cassetteNumber = 0;
    KeyMapping keyMapping;
private:
    /**
     * @brief ドットの形状
     */
    enum class DOT_PATTERN : u8 {
        //
        // 通常
        //

        /**
         * @brief なし
         */
        NONE = 0,
        /**
         * @brief 四角
         * 
         * ■
         */
        NORMAL = 1,

        //
        // ベント
        //

        /**
         * @brief ベント開始の斜め
         *  ___
         *  ＼|
         */
        BENT1_START = 2,
        /**
         * @brief ベント終了時の斜め
         *
         *  |＼
         *  ~~~
         */
        BENT1_END,
        /**
         * @brief ベント開始の斜め
         * 
         *  ／|
         *  ~~~
         */
        BENT2_START = 4,
        /**
         * @brief ベント終了時の斜め
         *  ___
         *  |／
         */
        BENT2_END,
    };

    /**
     * @brief ドットを描画する
     * 
     * @param[in]   x       描画するドットの座標(0～90)
     * @param[in]   y       描画するドットの座標(0～59)
     * @param[in]   color   ドットの色(R8G8B8)
     * @param[in]   bgColor 背景色(R8G8B8)
     * @param[in]   pattern 描画するドットの形状(DOT_PATTERN)
     */
    void pset(s32 x, s32 y, const u32 color, const u32 bgColor, const DOT_PATTERN pattern);

    /**
     * @brief ドット情報を元に１ライン描画する
     * 
     * @param[in]   dotInfo １ラインのドット情報
     * @param[in]   line    ラインの位置（0～59)。ドットを描画するY座標
     * @param[in]   bgColor 背景色(R8G8B8)
     */
    void dotRender(const DotInfo* dotInfo, const s32 line, const u32 bgColor);

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
     * frameBufferにイメージが作成される。
     */
    virtual void makePresentImage();
    /**
     * @brief イメージを画面に出す
     */
    virtual void present() {}

    // sound
    /**
     * @brief Lチャンネルのサウンドの音程を設定する
     * 
     * やんわりメモ）
     * ・FLSに書き込みがされた時に呼び出される
     * ・MIDIでいうところのNoteOnみたいな感じ
     * ・音を出しっぱなしで、MODEレジスタのREV機能（残響音効果）でかっこよく消音させてるっぽい？
     * 
     * @param[in]   value                   音程。1は無音
     * @param[in]   reverberatedSoundEffect 残響音効果が有効かどうか
     */
    virtual void setFLS(const s64 clockCounter, const u8 value, const bool reverberatedSoundEffect) {}
    /**
     * @brief Rチャンネルのサウンドの音程を設定する
     * 
     * やんわりメモ）
     * ・FRSに書き込みがされた時に呼び出される
     * ・MIDIでいうところのNoteOnみたいな感じ
     * ・音を出しっぱなしで、MODEレジスタのREV機能（残響音効果）でかっこよく消音させてるっぽい？
     * 
     * @param[in]   value   音程。1は無音
     * @param[in]   reverberatedSoundEffect 残響音効果が有効かどうか
     */
    virtual void setFRS(const s64 clockCounter, const u8 value, const bool reverberatedSoundEffect) {}

    // input

    /**
     * @brief PD1の入力を取得する
     * 
     * @param[out]  value   パドルの値(PAD_MIN_VALUE～PAD_MAX_VALUE)
     * @return 入力されていたらtrueを返す
     */
    virtual bool isPD1(u8& value) { value = KeyStatus::PAD_MIN_VALUE; return false; }
    /**
     * @brief PD2の入力を取得する
     * 
     * @param[out]  value   パドルの値(PAD_MIN_VALUE～PAD_MAX_VALUE)
     * @return 入力されていたらtrueを返す
     */
    virtual bool isPD2(u8& value) { value = KeyStatus::PAD_MIN_VALUE; return false; }
    /**
     * @brief PD3の入力を取得する
     * 
     * @param[out]  value   パドルの値(PAD_MIN_VALUE～PAD_MAX_VALUE)
     * @return 入力されていたらtrueを返す
     */
    virtual bool isPD3(u8& value) { value = KeyStatus::PAD_MIN_VALUE; return false; }
    /**
     * @brief PD4の入力を取得する
     * 
     * @param[out]  value   パドルの値(PAD_MIN_VALUE～PAD_MAX_VALUE)
     * @return 入力されていたらtrueを返す
     */
    virtual bool isPD4(u8& value) { value = KeyStatus::PAD_MIN_VALUE; return false; }

    /**
     * @brief 光線銃の入力
     * 
     * @param[out]  value   的に当たっているかどうか
     * @return 光線銃の状態？
     * 
     * value                返り値
     * ------------------------------------------
     * 撃った        :0x02  当たった    : true
     * 撃った        :0x02  外れていたら: false
     * ------------------------------------------
     * まだ撃ってない:0x00  falseを返す
     */
    virtual bool isGunPortLatch(u8& value) { value = 0; return false; }

    /**
     * @brief キー入力状態を読み込む
     * @param[out]   key    キー入力状態
     */
    virtual void readKIN(KeyStatus& key) { key.clear(); }
    /**
     * @brief キー状態入力をKINの値に変換する
     * 
     * @param[in]   key キー入力状態
     * @param[in]   STB キー入力をスキャンするときの値
     * @return  KINの値
     */
    virtual u8 convertKeyInput(const KeyStatus& key, const u8 STB) const { return key.convert(STB, keyMapping, getCassetteNumber()); }

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

    /**
     * @brief カセットの番号を返す
     */
    u32 getCassetteNumber() const { return cassetteNumber; }
public:
    /**
     * @brief 初期化
     */
    virtual void init();

    /**
     * @brief   romをセットアップする
     * 
     * @param[in]   data        データ
     * @param[in]   dataSize    データサイズ(バイト単位)
     * @return  成功したらtrueを返す
     */
    bool setupRom(const void* data = nullptr, size_t dataSize = 0);
    /**
     * @brief   cgromをセットアップする
     * 
     * @param[in]   data        データ
     * @param[in]   dataSize    データサイズ(バイト単位)
     * @return  成功したらtrueを返す
     */
    bool setupCGRom(const void* data = nullptr, size_t dataSize = 0);

    /**
     * @brief 終了処理
     */
    virtual void term();

    /**
     * @brief １命令実行する
     */
    void execute();
};
