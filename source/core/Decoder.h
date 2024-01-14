#pragma once

#include "catLowBasicTypes.h"
#include "catLowMemory.h"

/**
 * @brief   μPD777命令のデコーダ
 */
class Decoder {
    using ExecuteFunctionType = void (Decoder::*)(const u16 pc, const u16);
    struct MatchPattern {
        u16   mask;
        u16   value;
        ExecuteFunctionType execute;
    };
    MatchPattern matchPatterns[145] = {
        /* 000 */   { 0b111111111111, 0b000000000000, &Decoder::execNOP },
        /* 004 */   { 0b111111111111, 0b000000000100, &Decoder::execGPL },
        /* 008 */   { 0b111111111111, 0b000000001000, &Decoder::execMoveHtoNRM },
        /* 018 */   { 0b111111111111, 0b000000011000, &Decoder::execExchangeHandX },
        /* 020 */   { 0b111111111111, 0b000000100000, &Decoder::execSRE },
        /* 028 */   { 0b111111111110, 0b000000101000, &Decoder::execSTB },
        //
        /* 049 */   { 0b111111111111, 0b000001001001, &Decoder::exec4H_BLK },
        /* 04A */   { 0b111111111111, 0b000001001010, &Decoder::execVBLK },
        /* 04C */   { 0b111111111111, 0b000001001100, &Decoder::execGPSW_ },
        //
        /* 054 */   { 0b111111111111, 0b000001010100, &Decoder::execMoveAtoMA },
        /* 058 */   { 0b111111111111, 0b000001011000, &Decoder::execMoveMAtoA },
        /* 05C */   { 0b111111111111, 0b000001011100, &Decoder::execExchangeMAandA },
        //
        /* 060 */   { 0b111111111111, 0b000001100000, &Decoder::execSRE_1 },
        //
        /* 030 */   { 0b111111111111, 0b000000110000, &Decoder::execPD1_J },
        /* 034 */   { 0b111111111111, 0b000000110100, &Decoder::execPD2_J },
        /* 038 */   { 0b111111111111, 0b000000111000, &Decoder::execPD3_J },
        /* 03C */   { 0b111111111111, 0b000000111100, &Decoder::execPD4_J },
        /* 070 */   { 0b111111111111, 0b000001110000, &Decoder::execPD1_NJ },
        /* 074 */   { 0b111111111111, 0b000001110100, &Decoder::execPD2_NJ },
        /* 078 */   { 0b111111111111, 0b000001111000, &Decoder::execPD3_NJ },
        /* 07C */   { 0b111111111111, 0b000001111100, &Decoder::execPD4_NJ },
        //
        /* 080 */   { 0b111110000000, 0b000010000000, &Decoder::execTestSubMK },
        //
        /* 100 */   { 0b111110000000, 0b000100000000, &Decoder::execAddMKM },
        /* 180 */   { 0b111110000000, 0b000110000000, &Decoder::execSubMKM },
        // A1 A1                        001000 0
        /* 200 */   { 0b111111111100, 0b001000000000, &Decoder::execTestAndA1A1_EQJ },
        /* 220 */   { 0b111111111100, 0b001000100000, &Decoder::execTestAndA1A1_NOT_EQJ },
        /* 208 */   { 0b111111111100, 0b001000001000, &Decoder::execTestEquA1A1_EQJ },
        /* 228 */   { 0b111111111100, 0b001000101000, &Decoder::execTestEquA1A1_NOT_EQJ },
        /* 20C */   { 0b111111111100, 0b001000001100, &Decoder::execTestSubA1A1_BOJ },
        /* 22C */   { 0b111111111100, 0b001000101100, &Decoder::execTestSubA1A1_NOT_BOJ },
        // A1 A2                        001000 1
        /* 210 */   { 0b111111111100, 0b001000010000, &Decoder::execTestAndA1A2_EQJ },
        /* 230 */   { 0b111111111100, 0b001000110000, &Decoder::execTestAndA1A2_NOT_EQJ },
        /* 218 */   { 0b111111111100, 0b001000011000, &Decoder::execTestEquA1A2_EQJ },
        /* 238 */   { 0b111111111100, 0b001000111000, &Decoder::execTestEquA1A2_NOT_EQJ },
        /* 21C */   { 0b111111111100, 0b001000011100, &Decoder::execTestSubA1A2_BOJ },
        /* 23C */   { 0b111111111100, 0b001000111100, &Decoder::execTestSubA1A2_NOT_BOJ },
        // A2 A1                        001001 0
        /* 240 */   { 0b111111111100, 0b001001000000, &Decoder::execTestAndA2A1_EQJ },
        /* 260 */   { 0b111111111100, 0b001001100000, &Decoder::execTestAndA2A1_NOT_EQJ },
        /* 248 */   { 0b111111111100, 0b001001001000, &Decoder::execTestEquA2A1_EQJ },
        /* 268 */   { 0b111111111100, 0b001001101000, &Decoder::execTestEquA2A1_NOT_EQJ },
        /* 24C */   { 0b111111111100, 0b001001001100, &Decoder::execTestSubA2A1_BOJ },
        /* 26C */   { 0b111111111100, 0b001001101100, &Decoder::execTestSubA2A1_NOT_BOJ },
        // A2 A2                        001001 1
        /* 250 */   { 0b111111111100, 0b001001010000, &Decoder::execTestAndA2A2_EQJ },
        /* 270 */   { 0b111111111100, 0b001001110000, &Decoder::execTestAndA2A2_NOT_EQJ },
        /* 258 */   { 0b111111111100, 0b001001011000, &Decoder::execTestEquA2A2_EQJ },
        /* 278 */   { 0b111111111100, 0b001001111000, &Decoder::execTestEquA2A2_NOT_EQJ },
        /* 25C */   { 0b111111111100, 0b001001011100, &Decoder::execTestSubA2A2_BOJ },
        /* 27C */   { 0b111111111100, 0b001001111100, &Decoder::execTestSubA2A2_NOT_BOJ },
        // M A1                         001010 0
        /* 280 */   { 0b111111111100, 0b001010000000, &Decoder::execTestAndMA1_EQJ },
        /* 2A0 */   { 0b111111111100, 0b001010100000, &Decoder::execTestAndMA1_NOT_EQJ },
        /* 288 */   { 0b111111111100, 0b001010001000, &Decoder::execTestEquMA1_EQJ },
        /* 2A8 */   { 0b111111111100, 0b001010101000, &Decoder::execTestEquMA1_NOT_EQJ },
        /* 28C */   { 0b111111111100, 0b001010001100, &Decoder::execTestSubMA1_BOJ },
        /* 2AC */   { 0b111111111100, 0b001010101100, &Decoder::execTestSubMA1_NOT_BOJ },
        // M A2                         001010 1
        /* 290 */   { 0b111111111100, 0b001010010000, &Decoder::execTestAndMA2_EQJ },
        /* 2B0 */   { 0b111111111100, 0b001010110000, &Decoder::execTestAndMA2_NOT_EQJ },
        /* 298 */   { 0b111111111100, 0b001010011000, &Decoder::execTestEquMA2_EQJ },
        /* 2B8 */   { 0b111111111100, 0b001010111000, &Decoder::execTestEquMA2_NOT_EQJ },
        /* 29C */   { 0b111111111100, 0b001010011100, &Decoder::execTestSubMA2_BOJ },
        /* 2BC */   { 0b111111111100, 0b001010111100, &Decoder::execTestSubMA2_NOT_BOJ },
        // H A1                         001011 0
        /* 2C0 */   { 0b111111111100, 0b001011000000, &Decoder::execTestAndHA1_EQJ },
        /* 2E0 */   { 0b111111111100, 0b001011100000, &Decoder::execTestAndHA1_NOT_EQJ },
        /* 2C8 */   { 0b111111111100, 0b001011001000, &Decoder::execTestEquHA1_EQJ },
        /* 2E8 */   { 0b111111111100, 0b001011101000, &Decoder::execTestEquHA1_NOT_EQJ },
        /* 2CC */   { 0b111111111100, 0b001011001100, &Decoder::execTestSubHA1_BOJ },
        /* 2EC */   { 0b111111111100, 0b001011101100, &Decoder::execTestSubHA1_NOT_BOJ },
        // H A2                         001011 1
        /* 2D0 */   { 0b111111111100, 0b001011010000, &Decoder::execTestAndHA2_EQJ },
        /* 2F0 */   { 0b111111111100, 0b001011110000, &Decoder::execTestAndHA2_NOT_EQJ },
        /* 2D8 */   { 0b111111111100, 0b001011011000, &Decoder::execTestEquHA2_EQJ },
        /* 2F8 */   { 0b111111111100, 0b001011111000, &Decoder::execTestEquHA2_NOT_EQJ },
        /* 2DC */   { 0b111111111100, 0b001011011100, &Decoder::execTestSubHA2_BOJ },
        /* 2FC */   { 0b111111111100, 0b001011111100, &Decoder::execTestSubHA2_NOT_BOJ },

        //                              001100
        /* 300 */   { 0b111111111100, 0b001100000000, &Decoder::execMoveNtoL },
        /* 310 */   { 0b111111111100, 0b001100010000, &Decoder::execMoveA2toA1 },
        /* 31C */   { 0b111111111100, 0b001100011100, &Decoder::execSubA1andA2toA2 },
        /* 308 */   { 0b111111111111, 0b001100001000, &Decoder::execMoveA1toFLS },
        /* 309 */   { 0b111111111111, 0b001100001001, &Decoder::execMoveA1toFRS },
        /* 30A */   { 0b111111111110, 0b001100001010, &Decoder::execMoveA1toMODE },
        /* 318 */   { 0b111111111100, 0b001100011000, &Decoder::execRightShiftA1 },
        //                              001101
        /* 340 */   { 0b111111111100, 0b001101000000, &Decoder::execMoveA1toA2 },
        /* 348 */   { 0b111111111111, 0b001101001000, &Decoder::execMoveA2toFLS },
        /* 349 */   { 0b111111111111, 0b001101001001, &Decoder::execMoveA2toFRS },
        /* 34A */   { 0b111111111110, 0b001101001010, &Decoder::execMoveA2toMODE },
        /* 34C */   { 0b111111111110, 0b001101001100, &Decoder::execSubA2andA1toA1 },
        /* 358 */   { 0b111111111100, 0b001101011000, &Decoder::execRightShiftA2 },
        //                              001110
        /* 388 */   { 0b111111111111, 0b001110001000, &Decoder::execMoveMtoFLS },
        /* 389 */   { 0b111111111111, 0b001110001001, &Decoder::execMoveMtoFRS },
        /* 38A */   { 0b111111111110, 0b001110001010, &Decoder::execMoveMtoMODE },
        /* 398 */   { 0b111111111100, 0b001110011000, &Decoder::execRightShiftM },

        //
        /* 320 */   { 0b111111111100, 0b001100100000, &Decoder::execAndA1A1toA1 },
        /* 324 */   { 0b111111111100, 0b001100100100, &Decoder::execAddA1A1toA1 },
        /* 328 */   { 0b111111111100, 0b001100101000, &Decoder::execOrA1A1toA1 },
        /* 32C */   { 0b111111111100, 0b001100101100, &Decoder::execSubA1A1toA1 },
        /* 330 */   { 0b111111111100, 0b001100110000, &Decoder::execAndA1A2toA1 },
        /* 334 */   { 0b111111111100, 0b001100110100, &Decoder::execAddA1A2toA1 },
        /* 338 */   { 0b111111111100, 0b001100111000, &Decoder::execOrA1A2toA1 },
        /* 33C */   { 0b111111111100, 0b001100111100, &Decoder::execSubA1A2toA1 },
        /* 360 */   { 0b111111111100, 0b001101100000, &Decoder::execAndA2A1toA2 },
        /* 364 */   { 0b111111111100, 0b001101100100, &Decoder::execAddA2A1toA2 },
        /* 368 */   { 0b111111111100, 0b001101101000, &Decoder::execOrA2A1toA2 },
        /* 36C */   { 0b111111111100, 0b001101101100, &Decoder::execSubA2A1toA2 },
        /* 370 */   { 0b111111111100, 0b001101110000, &Decoder::execAndA2A2toA2 },
        /* 374 */   { 0b111111111100, 0b001101110100, &Decoder::execAddA2A2toA2 },
        /* 378 */   { 0b111111111100, 0b001101111000, &Decoder::execOrA2A2toA2 },
        /* 37C */   { 0b111111111100, 0b001101111100, &Decoder::execSubA2A2toA2 },

        //
        /* 380 */   { 0b111111111100, 0b001110000000, &Decoder::execMoveA1toM },
        /* 390 */   { 0b111111111100, 0b001110010000, &Decoder::execMoveA2toM },
        /* 384 */   { 0b111111111100, 0b001110000100, &Decoder::execExchangeMA1 },
        /* 394 */   { 0b111111111100, 0b001110010100, &Decoder::execExchangeMA2 },
        /* 38C */   { 0b111111111100, 0b001110001100, &Decoder::execMoveMtoA1 },
        /* 39C */   { 0b111111111100, 0b001110011100, &Decoder::execMoveMtoA2 },

        //
        /* 3B0 */   { 0b111111111100, 0b001110110000, &Decoder::execAndMA2toM },
        /* 3B4 */   { 0b111111111100, 0b001110110100, &Decoder::execAddMA2toM },
        /* 3B8 */   { 0b111111111100, 0b001110111000, &Decoder::execOrMA2toM },
        /* 3BC */   { 0b111111111100, 0b001110111100, &Decoder::execSubMA2toM },
        /* 3A0 */   { 0b111111111100, 0b001110100000, &Decoder::execAndMA1toM },
        /* 3A4 */   { 0b111111111100, 0b001110100100, &Decoder::execAddMA1toM },
        /* 3A8 */   { 0b111111111100, 0b001110101000, &Decoder::execOrMA1toM },
        /* 3AC */   { 0b111111111100, 0b001110101100, &Decoder::execSubMA1toM },

        //
        /* 3C0 */   { 0b111111111100, 0b001111000000, &Decoder::execMoveA1toH },
        /* 3D0 */   { 0b111111111100, 0b001111010000, &Decoder::execMoveA2toH },
        /* 3CC */   { 0b111111111100, 0b001111001100, &Decoder::execMoveHtoA1 },
        /* 3DC */   { 0b111111111100, 0b001111011100, &Decoder::execMoveHtoA2 },

        //
        /* 3E0 */   { 0b111111111100, 0b001111100000, &Decoder::execAndHA1toH },
        /* 3E4 */   { 0b111111111100, 0b001111100100, &Decoder::execAddHA1toH },
        /* 3E8 */   { 0b111111111100, 0b001111101000, &Decoder::execOrHA1toH },
        /* 3EC */   { 0b111111111100, 0b001111101100, &Decoder::execSubHA1toH },
        /* 3F0 */   { 0b111111111100, 0b001111110000, &Decoder::execAndHA2toH },
        /* 3F4 */   { 0b111111111100, 0b001111110100, &Decoder::execAddHA2toH },
        /* 3F8 */   { 0b111111111100, 0b001111111000, &Decoder::execOrHA2toH },
        /* 3FC */   { 0b111111111100, 0b001111111100, &Decoder::execSubHA2toH },

        //
        /* 400 */   { 0b111111111110, 0b010000000000, &Decoder::execSetA11 },
        /* 402 */   { 0b111111111110, 0b010000000010, &Decoder::execJPM },
        /* 440 */   { 0b111111000010, 0b010001000000, &Decoder::execSetFlags },

        //
        /* 480 */   { 0b111111100000, 0b010010000000, &Decoder::execSubHKtoH },
        /* 4C0 */   { 0b111111100000, 0b010011000000, &Decoder::execAddHKtoH },

        //
        /* 500 */   { 0b111110000000, 0b010100000000, &Decoder::execMoveKtoM },
        /* 580 */   { 0b111110000000, 0b010110000000, &Decoder::execMoveKtoLH },
        /* 600 */   { 0b111110000000, 0b011000000000, &Decoder::execMoveKtoA1 },
        /* 680 */   { 0b111110000000, 0b011010000000, &Decoder::execMoveKtoA2 },
        /* 700 */   { 0b111110000000, 0b011100000000, &Decoder::execMoveKtoA3 },
        /* 780 */   { 0b111110000000, 0b011110000000, &Decoder::execMoveKtoA4 },

        //
        /* 800 */   { 0b110000000000, 0b100000000000, &Decoder::execJP },
        /* C00 */   { 0b110000000000, 0b110000000000, &Decoder::execJS },

        //
        // 不明な命令（番兵）
        //
        /*     */   { 0b000000000000, 0b000000000000, &Decoder::execUnknown },
    };

protected:
    virtual void execUnknown(const u16 pc, const u16 code) {}

    virtual void execNOP(const u16 pc, const u16 code) {}
    virtual void execGPL(const u16 pc, const u16 code) {}
    virtual void execMoveHtoNRM(const u16 pc, const u16 code) {}
    virtual void execExchangeHandX(const u16 pc, const u16 code) {}
    virtual void execSRE(const u16 pc, const u16 code) {}
    virtual void execSTB(const u16 pc, const u16 code) {}
    //
    virtual void exec4H_BLK(const u16 pc, const u16 code) {}
    virtual void execVBLK(const u16 pc, const u16 code) {}
    virtual void execGPSW_(const u16 pc, const u16 code) {}
    //
    virtual void execMoveAtoMA(const u16 pc, const u16 code) {}
    virtual void execMoveMAtoA(const u16 pc, const u16 code) {}
    virtual void execExchangeMAandA(const u16 pc, const u16 code) {}
    //
    virtual void execSRE_1(const u16 pc, const u16 code) {}
    //
    virtual void execPD1_J(const u16 pc, const u16 code) {}
    virtual void execPD2_J(const u16 pc, const u16 code) {}
    virtual void execPD3_J(const u16 pc, const u16 code) {}
    virtual void execPD4_J(const u16 pc, const u16 code) {}
    virtual void execPD1_NJ(const u16 pc, const u16 code) {}
    virtual void execPD2_NJ(const u16 pc, const u16 code) {}
    virtual void execPD3_NJ(const u16 pc, const u16 code) {}
    virtual void execPD4_NJ(const u16 pc, const u16 code) {}
    //
    virtual void execTestSubMK(const u16 pc, const u16 code) {}
    //
    virtual void execAddMKM(const u16 pc, const u16 code) {}
    virtual void execSubMKM(const u16 pc, const u16 code) {}
    //
    virtual void execTestAndA1A1_EQJ(const u16 pc, const u16 code) {}
    virtual void execTestAndA1A1_NOT_EQJ(const u16 pc, const u16 code) {}
    virtual void execTestEquA1A1_EQJ(const u16 pc, const u16 code) {}
    virtual void execTestEquA1A1_NOT_EQJ(const u16 pc, const u16 code) {}
    virtual void execTestSubA1A1_BOJ(const u16 pc, const u16 code) {}
    virtual void execTestSubA1A1_NOT_BOJ(const u16 pc, const u16 code) {}
    virtual void execTestAndA1A2_EQJ(const u16 pc, const u16 code) {}
    virtual void execTestAndA1A2_NOT_EQJ(const u16 pc, const u16 code) {}
    virtual void execTestEquA1A2_EQJ(const u16 pc, const u16 code) {}
    virtual void execTestEquA1A2_NOT_EQJ(const u16 pc, const u16 code) {}
    virtual void execTestSubA1A2_BOJ(const u16 pc, const u16 code) {}
    virtual void execTestSubA1A2_NOT_BOJ(const u16 pc, const u16 code) {}
    virtual void execTestAndA2A1_EQJ(const u16 pc, const u16 code) {}
    virtual void execTestAndA2A1_NOT_EQJ(const u16 pc, const u16 code) {}
    virtual void execTestEquA2A1_EQJ(const u16 pc, const u16 code) {}
    virtual void execTestEquA2A1_NOT_EQJ(const u16 pc, const u16 code) {}
    virtual void execTestSubA2A1_BOJ(const u16 pc, const u16 code) {}
    virtual void execTestSubA2A1_NOT_BOJ(const u16 pc, const u16 code) {}
    virtual void execTestAndA2A2_EQJ(const u16 pc, const u16 code) {}
    virtual void execTestAndA2A2_NOT_EQJ(const u16 pc, const u16 code) {}
    virtual void execTestEquA2A2_EQJ(const u16 pc, const u16 code) {}
    virtual void execTestEquA2A2_NOT_EQJ(const u16 pc, const u16 code) {}
    virtual void execTestSubA2A2_BOJ(const u16 pc, const u16 code) {}
    virtual void execTestSubA2A2_NOT_BOJ(const u16 pc, const u16 code) {}
    //
    virtual void execTestAndMA1_EQJ(const u16 pc, const u16 code) {}
    virtual void execTestAndMA1_NOT_EQJ(const u16 pc, const u16 code) {}
    virtual void execTestEquMA1_EQJ(const u16 pc, const u16 code) {}
    virtual void execTestEquMA1_NOT_EQJ(const u16 pc, const u16 code) {}
    virtual void execTestSubMA1_BOJ(const u16 pc, const u16 code) {}
    virtual void execTestSubMA1_NOT_BOJ(const u16 pc, const u16 code) {}
    virtual void execTestAndMA2_EQJ(const u16 pc, const u16 code) {}
    virtual void execTestAndMA2_NOT_EQJ(const u16 pc, const u16 code) {}
    virtual void execTestEquMA2_EQJ(const u16 pc, const u16 code) {}
    virtual void execTestEquMA2_NOT_EQJ(const u16 pc, const u16 code) {}
    virtual void execTestSubMA2_BOJ(const u16 pc, const u16 code) {}
    virtual void execTestSubMA2_NOT_BOJ(const u16 pc, const u16 code) {}
    virtual void execTestAndHA1_EQJ(const u16 pc, const u16 code) {}
    virtual void execTestAndHA1_NOT_EQJ(const u16 pc, const u16 code) {}
    virtual void execTestEquHA1_EQJ(const u16 pc, const u16 code) {}
    virtual void execTestEquHA1_NOT_EQJ(const u16 pc, const u16 code) {}
    virtual void execTestSubHA1_BOJ(const u16 pc, const u16 code) {}
    virtual void execTestSubHA1_NOT_BOJ(const u16 pc, const u16 code) {}
    virtual void execTestAndHA2_EQJ(const u16 pc, const u16 code) {}
    virtual void execTestAndHA2_NOT_EQJ(const u16 pc, const u16 code) {}
    virtual void execTestEquHA2_EQJ(const u16 pc, const u16 code) {}
    virtual void execTestEquHA2_NOT_EQJ(const u16 pc, const u16 code) {}
    virtual void execTestSubHA2_BOJ(const u16 pc, const u16 code) {}
    virtual void execTestSubHA2_NOT_BOJ(const u16 pc, const u16 code) {}
    //
    virtual void execMoveNtoL(const u16 pc, const u16 code) {}
    virtual void execMoveA2toA1(const u16 pc, const u16 code) {}
    virtual void execSubA1andA2toA2(const u16 pc, const u16 code) {}
    virtual void execMoveA1toFLS(const u16 pc, const u16 code) {}
    virtual void execMoveA1toFRS(const u16 pc, const u16 code) {}
    virtual void execMoveA1toMODE(const u16 pc, const u16 code) {}
    virtual void execRightShiftA1(const u16 pc, const u16 code) {}
    virtual void execMoveA1toA2(const u16 pc, const u16 code) {}
    virtual void execMoveA2toFLS(const u16 pc, const u16 code) {}
    virtual void execMoveA2toFRS(const u16 pc, const u16 code) {}
    virtual void execMoveA2toMODE(const u16 pc, const u16 code) {}
    virtual void execSubA2andA1toA1(const u16 pc, const u16 code) {}
    virtual void execRightShiftA2(const u16 pc, const u16 code) {}
    virtual void execMoveMtoFLS(const u16 pc, const u16 code) {}
    virtual void execMoveMtoFRS(const u16 pc, const u16 code) {}
    virtual void execMoveMtoMODE(const u16 pc, const u16 code) {}
    virtual void execRightShiftM(const u16 pc, const u16 code) {}
    //
    virtual void execAndA1A1toA1(const u16 pc, const u16 code) {}
    virtual void execAddA1A1toA1(const u16 pc, const u16 code) {}
    virtual void execOrA1A1toA1(const u16 pc, const u16 code) {}
    virtual void execSubA1A1toA1(const u16 pc, const u16 code) {}
    virtual void execAndA1A2toA1(const u16 pc, const u16 code) {}
    virtual void execAddA1A2toA1(const u16 pc, const u16 code) {}
    virtual void execOrA1A2toA1(const u16 pc, const u16 code) {}
    virtual void execSubA1A2toA1(const u16 pc, const u16 code) {}
    virtual void execAndA2A1toA2(const u16 pc, const u16 code) {}
    virtual void execAddA2A1toA2(const u16 pc, const u16 code) {}
    virtual void execOrA2A1toA2(const u16 pc, const u16 code) {}
    virtual void execSubA2A1toA2(const u16 pc, const u16 code) {}
    virtual void execAndA2A2toA2(const u16 pc, const u16 code) {}
    virtual void execAddA2A2toA2(const u16 pc, const u16 code) {}
    virtual void execOrA2A2toA2(const u16 pc, const u16 code) {}
    virtual void execSubA2A2toA2(const u16 pc, const u16 code) {}
    //
    virtual void execMoveA1toM(const u16 pc, const u16 code) {}
    virtual void execMoveA2toM(const u16 pc, const u16 code) {}
    virtual void execExchangeMA1(const u16 pc, const u16 code) {}
    virtual void execExchangeMA2(const u16 pc, const u16 code) {}
    virtual void execMoveMtoA1(const u16 pc, const u16 code) {}
    virtual void execMoveMtoA2(const u16 pc, const u16 code) {}
    //
    virtual void execAndMA2toM(const u16 pc, const u16 code) {}
    virtual void execAddMA2toM(const u16 pc, const u16 code) {}
    virtual void execOrMA2toM(const u16 pc, const u16 code) {}
    virtual void execSubMA2toM(const u16 pc, const u16 code) {}
    virtual void execAndMA1toM(const u16 pc, const u16 code) {}
    virtual void execAddMA1toM(const u16 pc, const u16 code) {}
    virtual void execOrMA1toM(const u16 pc, const u16 code) {}
    virtual void execSubMA1toM(const u16 pc, const u16 code) {}
    //
    virtual void execMoveA1toH(const u16 pc, const u16 code) {}
    virtual void execMoveA2toH(const u16 pc, const u16 code) {}
    virtual void execMoveHtoA1(const u16 pc, const u16 code) {}
    virtual void execMoveHtoA2(const u16 pc, const u16 code) {}
    //
    virtual void execAndHA1toH(const u16 pc, const u16 code) {}
    virtual void execAddHA1toH(const u16 pc, const u16 code) {}
    virtual void execOrHA1toH(const u16 pc, const u16 code) {}
    virtual void execSubHA1toH(const u16 pc, const u16 code) {}
    virtual void execAndHA2toH(const u16 pc, const u16 code) {}
    virtual void execAddHA2toH(const u16 pc, const u16 code) {}
    virtual void execOrHA2toH(const u16 pc, const u16 code) {}
    virtual void execSubHA2toH(const u16 pc, const u16 code) {}
    //
    virtual void execSetA11(const u16 pc, const u16 code) {}
    virtual void execJPM(const u16 pc, const u16 code) {}
    virtual void execSetFlags(const u16 pc, const u16 code) {}
    //
    virtual void execSubHKtoH(const u16 pc, const u16 code) {}
    virtual void execAddHKtoH(const u16 pc, const u16 code) {}
    //
    virtual void execMoveKtoM(const u16 pc, const u16 code) {}
    virtual void execMoveKtoLH(const u16 pc, const u16 code) {}
    virtual void execMoveKtoA1(const u16 pc, const u16 code) {}
    virtual void execMoveKtoA2(const u16 pc, const u16 code) {}
    virtual void execMoveKtoA3(const u16 pc, const u16 code) {}
    virtual void execMoveKtoA4(const u16 pc, const u16 code) {}
    //
    virtual void execJP(const u16 pc, const u16 code) {}
    virtual void execJS(const u16 pc, const u16 code) {}
public:
    virtual ~Decoder() {}

    /**
     * @brief   命令をデコードする
     * 
     * 命令をデコードして対応するexeHogeHoge()を呼び出す。
     * 
     * @param[in]   pc      命令のアドレス
     * @param[in]   code    デコードする命令
     */
    virtual void execute(const u16 pc, const u16 code);
};
