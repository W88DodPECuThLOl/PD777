#pragma once

#include "Decoder.h"

#if defined(_WIN32)

/**
 * @brief   μPD777命令のディスアセンブラ
 */
class Disassembler : public Decoder {
    c8 mnemonic[64];

    enum : u16 {
        FLAG_JUMP = 1,
        FLAG_CALL = 2,
    };
    u32 addressLabelSize = 0; 
    u16 addressLabel[0x1000];
    u16 addressLabelFlag[0x1000];
    u16 addressLabelCaller[0x1000];
    void registerAddress(const u16 address, const u16 callerAddress, const u16 flags = 0)
    {
        if(pass != 0) {
            return;
        }
        if(findAddress(address)) {
            return;
        }
        addressLabel[addressLabelSize] = address;
        addressLabelFlag[addressLabelSize] = flags;
        addressLabelCaller[addressLabelSize++] = callerAddress;
    }
public:
    u16 pass = 1;
    u16 findAddress(const u16 address)
    {
        for(auto i = 0; i < addressLabelSize; i++) {
            if(addressLabel[i] == address) {
                return addressLabelFlag[i];
            }
        }
        return 0;
    }
    u16 findAddressCaller(const u16 address)
    {
        for(auto i = 0; i < addressLabelSize; i++) {
            if(addressLabel[i] == address) {
                return addressLabelCaller[i];
            }
        }
        return 0;
    }
protected:
    virtual void print(const u16 pc, const u16 code, const c8* mnemonic, const c8* function = nullptr, const c8* judge = u8"");
    virtual void execUnknown(const u16 pc, const u16 code) override;
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
public:
};

#endif // defined(_WIN32)