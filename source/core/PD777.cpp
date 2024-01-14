#include "PD777.h"

#if defined(_WIN32)
#include <string>
#endif // defined(_WIN32)

#define PATCH_BattleVader_HIT (1)
#define PATCH_BattleVader_ScoreSpriteSetting (1)

#define CMP_HA1_7bit (1)

void
PD777::execNOP(const u16 pc, const u16 code)
{
}

void
PD777::execGPL(const u16 pc, const u16 code)
{
//    print(pc, code, "GPL", u8"Skip if (Gun Port Latch) = 1", u8"J");
}

void
PD777::execMoveHtoNRM(const u16 pc, const u16 code)
{
//    print(pc, code, "H=>NRM", u8"Move H[5:1] to Line Buffer Register[5:1]");
    const auto H = regs.getH();
    regs.setLineBufferRegister(H);
}

void
PD777::execExchangeHandX(const u16 pc, const u16 code)
{
//    print(pc, code, "H<==>X", u8"H[5:1]<==>X4[5:1], 0=>X4[7:6], 0=>X3[7:1], 0=>X1'[1], 0=>A1'[1], L[2:1]<==>L'[2:1]");

    const auto H  = regs.getH();
    const auto X4 = regs.getX4();
    regs.setH(X4);
    regs.setX4(H);

    // L[2:1]<==>L'[2:1]
    const auto L   = regs.getL();
    const auto L_  = regs.getL_();
    regs.setL(L_);
    regs.setL_(L);
}

void
PD777::execSRE(const u16 pc, const u16 code)
{
//    print(pc, code, "SRE", u8"Subroutine End, Pop down address stack");
    regs.setPC(stack.stackPop());
}

void
PD777::execSTB(const u16 pc, const u16 code)
{
    const u8 N = code & 0b000000000001;
//    snprintf(mnemonic, sizeof(mnemonic), "N<%d>=>STB", N);
//    print(pc, code, mnemonic, u8"Shift STB[4:1], N=>STB[1]");
    regs.shiftSTB(N);
}

void
PD777::exec4H_BLK(const u16 pc, const u16 code)
{
//    print(pc, code, "4H BLK", u8"Skip if (4H Horizontal Blank) = 1", u8"J");
    if(crt.is4H_BLK()) {
        regs.setSkip();

        // 4HBLKでクリアされるらしい
        for(int i = 0; i <= 0x18; ++i) {
            const auto address = (i << 2) | 3;
            const auto value = readMem(address);
            writeMem(address, value & ~1);
        }
    }
}

void
PD777::execVBLK(const u16 pc, const u16 code)
{
//    print(pc, code, "VBLK", u8"Skip if (Vertical Blank) = 1, 0=>M[[18:00],[3]][1]", u8"J");
    if(crt.isVBLK()) {
        regs.setSkip();

        // VBLKでクリアされるらしい
        for(int i = 0; i <= 0x18; ++i) {
            const auto address = (i << 2) | 3;
            const auto value = readMem(address);
            writeMem(address, value & ~1);
        }
    }
}

void
PD777::execGPSW_(const u16 pc, const u16 code)
{
//    print(pc, code, "GPSW/", u8"Skip if (GP&SW/ input) = 1", u8"J");
}

void
PD777::execMoveAtoMA(const u16 pc, const u16 code)
{
//    print(pc, code, "A=>MA", u8"Move (A4[7:1],A3[7:1],A2[7:1],A1[7:1]) to M[H[5:1]][28:1]");

    // A=>MA
    // Move (A4[7:1],A3[7:1],A2[7:1],A1[7:1]) to M[H[5:1]][28:1]
    const auto A1 = regs.getA1();
    const auto A2 = regs.getA2();
    const auto A3 = regs.getA3();
    const auto A4 = regs.getA4();
    const auto address = (u16)regs.getH() << 2;
    writeMem(address + 0, A1);
    writeMem(address + 1, A2);
    writeMem(address + 2, A3);
    writeMem(address + 3, A4);
}

void
PD777::execMoveMAtoA(const u16 pc, const u16 code)
{
//    print(pc, code, "MA=>A", u8"Move M[H[5:1]][28:1] to (A4[7:1],A3[7:1],A2[7:1],A1[7:1])");

    // MA=>A
    // Move M[H[5:1]][28:1] to (A4[7:1],A3[7:1],A2[7:1],A1[7:1])
    const auto address = (u16)regs.getH() << 2;
    const auto A1 = readMem(address + 0);
    const auto A2 = readMem(address + 1);
    const auto A3 = readMem(address + 2);
    const auto A4 = readMem(address + 3);
    regs.setA1(A1);
    regs.setA2(A2);
    regs.setA3(A3);
    regs.setA4(A4);
}

void
PD777::execExchangeMAandA(const u16 pc, const u16 code)
{
//    print(pc, code, "MA<==>A", u8"Exchange (A4[7:1],A3[7:1],A2[7:1],A1[7:1]) and M[H[5:1]][28:1]");
    const auto address = (u16)regs.getH() << 2;
    const auto A1 = readMem(address + 0);
    const auto A2 = readMem(address + 1);
    const auto A3 = readMem(address + 2);
    const auto A4 = readMem(address + 3);
    const auto A1_ = regs.getA1();
    const auto A2_ = regs.getA2();
    const auto A3_ = regs.getA3();
    const auto A4_ = regs.getA4();
    regs.setA1(A1);
    regs.setA2(A2);
    regs.setA3(A3);
    regs.setA4(A4);
    writeMem(address + 0, A1_);
    writeMem(address + 1, A2_);
    writeMem(address + 2, A3_);
    writeMem(address + 3, A4_);
}

void
PD777::execSRE_1(const u16 pc, const u16 code)
{
//    print(pc, code, "SRE+1", u8"Subroutine End, Pop down address stack, Skip");
    regs.setPC(stack.stackPop());
    regs.setSkip();
}

void
PD777::execPD1_J(const u16 pc, const u16 code)
{
//    print(pc, code, "PD1", u8"Skip if (PD1 input) = 1", u8"J");
    if(isPD1()) {
        regs.setSkip();
    }
}

void
PD777::execPD2_J(const u16 pc, const u16 code)
{
//    print(pc, code, "PD2", u8"Skip if (PD2 input) = 1", u8"J");
    if(isPD2()) {
        regs.setSkip();
    }
}

void
PD777::execPD3_J(const u16 pc, const u16 code)
{
//    print(pc, code, "PD3", u8"Skip if (PD3 input) = 1", u8"J");
    if(isPD3()) {
        regs.setSkip();
    }
}

void
PD777::execPD4_J(const u16 pc, const u16 code)
{
//    print(pc, code, "PD4", u8"Skip if (PD4 input) = 1", u8"J");
    if(isPD4()) {
        regs.setSkip();
    }
}

void
PD777::execPD1_NJ(const u16 pc, const u16 code)
{
//    print(pc, code, "PD1", u8"Skip if (PD1 input) = 0", u8"J/");
    if(!isPD1()) {
        regs.setSkip();
    }
}

void
PD777::execPD2_NJ(const u16 pc, const u16 code)
{
//    print(pc, code, "PD2", u8"Skip if (PD2 input) = 0", u8"J/");
    if(!isPD2()) {
        regs.setSkip();
    }
}

void
PD777::execPD3_NJ(const u16 pc, const u16 code)
{
//    print(pc, code, "PD3", u8"Skip if (PD3 input) = 0", u8"J/");
    if(!isPD3()) {
        regs.setSkip();
    }
}

void
PD777::execPD4_NJ(const u16 pc, const u16 code)
{
//    print(pc, code, "PD4", u8"Skip if (PD4 input) = 0", u8"J/");
    if(!isPD4()) {
        regs.setSkip();
    }
}

void
PD777::execTestSubMK(const u16 pc, const u16 code)
{
//    snprintf(mnemonic, sizeof(mnemonic), "M-K<$%02X>", K);
//    print(pc, code, mnemonic, u8"Skip if (M[H[5:1],L[2:1]][7:1]-K[7:1]) makes borrow", u8"BOJ");

    const u8 K = code & 0b00000'1111111;
    const u8 M = readMemAtHL();
    regs.setSkip(M < K);
}

void
PD777::execAddMKM(const u16 pc, const u16 code)
{
//    snprintf(mnemonic, sizeof(mnemonic), "M+K<$%02X>=>M, N<%d>=>L", K, N);
//    print(pc, code, mnemonic, u8"M[H[5:1],L[2:1]][7:1]+K[7:1]=>M[H[5:1],L[2:1]][7:1], Skip if carry, N=>L[2:1]", u8"CAJ");

    // judge "CAJ"
    // M+K=>M, N=>L
    // M[H[5:1],L[2:1]][7:1]+K[7:1]=>M[H[5:1],L[2:1]][7:1], Skip if carry, N=>L[2:1]

    // @todo FIX ME!!!!

    const u8 N  = (code & 0b000001100000) >> 5;
    const u8 K  =  code & 0b000000011111;
    const u8 M  = readMemAtHL();

    // 7bit計算
    writeMemAtHL(M + K);
    regs.setSkip((M + K) > 0x7F);
    regs.setL(N);
}

void
PD777::execSubMKM(const u16 pc, const u16 code)
{
//    snprintf(mnemonic, sizeof(mnemonic), "M-K<$%02X>=>M, N<%d>=>L", K, N);
//    print(pc, code, mnemonic, u8"M[H[5:1],L[2:1]][7:1]-K[7:1]=>M[H[5:1],L[2:1]][7:1], Skip if borrow, N=>L[2:1]", u8"BOJ");

    // @todo FIX ME!!!!

    const u8 N  = (code & 0b000001100000) >> 5;
    const u8 K  =  code & 0b000000011111;
    const u8 M  = readMemAtHL();

    // 7bit計算
    writeMemAtHL(M - K);
    regs.setSkip(M < K);
    regs.setL(N);
}

void
PD777::execTestAndA1A1_EQJ(const u16 pc, const u16 code)
{
//    snprintf(mnemonic, sizeof(mnemonic), "A1&A1, N<%d>=>L", N);
//    print(pc, code, mnemonic, u8"Skip if (A1[7:1]&A1[7:1]) makes zero, N=>L[2:1]", u8"EQJ");

    const auto N = code & 0b00000'0000011;
    const auto A1 = regs.getA1();
    regs.setSkip((A1 & A1) == 0);
    regs.setL(N);
}

void
PD777::execTestAndA1A1_NOT_EQJ(const u16 pc, const u16 code)
{
//    snprintf(mnemonic, sizeof(mnemonic), "A1&A1, N<%d>=>L", N);
//    print(pc, code, mnemonic, u8"Skip if (A1[7:1]&A1[7:1]) makes non zero, N=>L[2:1]", u8"EQJ/");

    const auto N = code & 0b00000'0000011;
    const auto A1 = regs.getA1();
    regs.setSkip((A1 & A1) != 0);
    regs.setL(N);
}

void
PD777::execTestEquA1A1_EQJ(const u16 pc, const u16 code)
{
//    snprintf(mnemonic, sizeof(mnemonic), "A1=A1, N<%d>=>L", N);
//    print(pc, code, mnemonic, u8"Skip if (A1[7:1]-A1[7:1]) makes zero, N=>L[2:1]", u8"EQJ");
    const auto N = code & 0b00000'0000011;
    regs.setSkip();
    regs.setL(N);
}

void
PD777::execTestEquA1A1_NOT_EQJ(const u16 pc, const u16 code)
{
//    snprintf(mnemonic, sizeof(mnemonic), "A1=A1, N<%d>=>L", N);
//    print(pc, code, mnemonic, u8"Skip if (A1[7:1]-A1[7:1]) makes non zero, N=>L[2:1]", u8"EQJ/");
    const auto N = code & 0b00000'0000011;
    regs.setL(N);
}

void
PD777::execTestSubA1A1_BOJ(const u16 pc, const u16 code)
{
//    snprintf(mnemonic, sizeof(mnemonic), "A1-A1, N<%d>=>L", N);
//    print(pc, code, mnemonic, u8"Skip if (A1[7:1]-A1[7:1]) makes borrow, N=>L[2:1]", u8"BOJ");
    const auto N = code & 0b00000'0000011;
    const auto A1 = regs.getA1();
    regs.setL(N);
}

void
PD777::execTestSubA1A1_NOT_BOJ(const u16 pc, const u16 code)
{
//    snprintf(mnemonic, sizeof(mnemonic), "A1-A1, N<%d>=>L", N);
//    print(pc, code, mnemonic, u8"Skip if (A1[7:1]-A1[7:1]) makes non borrow, N=>L[2:1]", u8"BOJ/");
    const auto N = code & 0b00000'0000011;
    const auto A1 = regs.getA1();
    regs.setSkip();
    regs.setL(N);
}

void
PD777::execTestAndA1A2_EQJ(const u16 pc, const u16 code)
{
//    snprintf(mnemonic, sizeof(mnemonic), "A1&A2, N<%d>=>L", N);
//    print(pc, code, mnemonic, u8"Skip if (A1[7:1]&A2[7:1]) makes zero, N=>L[2:1]", u8"EQJ");

    const auto N = code & 0b00000'0000011;
    const auto A1 = regs.getA1();
    const auto A2 = regs.getA2();
    regs.setSkip((A1 & A2) == 0);
    regs.setL(N);
}

void
PD777::execTestAndA1A2_NOT_EQJ(const u16 pc, const u16 code)
{
//    snprintf(mnemonic, sizeof(mnemonic), "A1&A2, N<%d>=>L", N);
//    print(pc, code, mnemonic, u8"Skip if (A1[7:1]&A2[7:1]) makes non zero, N=>L[2:1]", u8"EQJ/");

    const auto N = code & 0b00000'0000011;
    const auto A1 = regs.getA1();
    const auto A2 = regs.getA2();
    regs.setSkip((A1 & A2) != 0);
    regs.setL(N);
}

void
PD777::execTestEquA1A2_EQJ(const u16 pc, const u16 code)
{
//    snprintf(mnemonic, sizeof(mnemonic), "A1=A2, N<%d>=>L", N);
//    print(pc, code, mnemonic, u8"Skip if (A1[7:1]-A2[7:1]) makes zero, N=>L[2:1]", u8"EQJ");

    const auto N = code & 0b00000'0000011;
    const auto A1 = regs.getA1();
    const auto A2 = regs.getA2();
    regs.setSkip(A1 == A2);
    regs.setL(N);
}

void
PD777::execTestEquA1A2_NOT_EQJ(const u16 pc, const u16 code)
{
//    snprintf(mnemonic, sizeof(mnemonic), "A1=A2, N<%d>=>L", N);
//    print(pc, code, mnemonic, u8"Skip if (A1[7:1]-A2[7:1]) makes non zero, N=>L[2:1]", u8"EQJ/");

    const auto N = code & 0b00000'0000011;
    const auto A1 = regs.getA1();
    const auto A2 = regs.getA2();
    regs.setSkip(A1 != A2);
    regs.setL(N);
}

void
PD777::execTestSubA1A2_BOJ(const u16 pc, const u16 code)
{
//    snprintf(mnemonic, sizeof(mnemonic), "A1-A2, N<%d>=>L", N);
//    print(pc, code, mnemonic, u8"Skip if (A1[7:1]-A2[7:1]) makes borrow, N=>L[2:1]", u8"BOJ");

    const auto N = code & 0b00000'0000011;
    const auto A1 = regs.getA1();
    const auto A2 = regs.getA2();
    regs.setSkip(A1 < A2);
    regs.setL(N);
}

void
PD777::execTestSubA1A2_NOT_BOJ(const u16 pc, const u16 code)
{
//    snprintf(mnemonic, sizeof(mnemonic), "A1-A2, N<%d>=>L", N);
//    print(pc, code, mnemonic, u8"Skip if (A1[7:1]-A2[7:1]) makes non borrow, N=>L[2:1]", u8"BOJ/");
    const auto N = code & 0b00000'0000011;
    const auto A1 = regs.getA1();
    const auto A2 = regs.getA2();
    regs.setSkip(A1 >= A2);
    regs.setL(N);
}

void
PD777::execTestAndA2A1_EQJ(const u16 pc, const u16 code)
{
//    snprintf(mnemonic, sizeof(mnemonic), "A2&A1, N<%d>=>L", N);
//    print(pc, code, mnemonic, u8"Skip if (A2[7:1]&A1[7:1]) makes zero, N=>L[2:1]", u8"EQJ");
    const auto N = code & 0b00000'0000011;
    const auto A1 = regs.getA1();
    const auto A2 = regs.getA2();
    regs.setSkip((A2 & A1) == 0);
    regs.setL(N);
}

void
PD777::execTestAndA2A1_NOT_EQJ(const u16 pc, const u16 code)
{
//    snprintf(mnemonic, sizeof(mnemonic), "A2&A1, N<%d>=>L", N);
//    print(pc, code, mnemonic, u8"Skip if (A2[7:1]&A1[7:1]) makes non zero, N=>L[2:1]", u8"EQJ/");

    const auto N = code & 0b00000'0000011;
    const auto A1 = regs.getA1();
    const auto A2 = regs.getA2();
    regs.setSkip((A2 & A1) != 0);
    regs.setL(N);
}

void
PD777::execTestEquA2A1_EQJ(const u16 pc, const u16 code)
{
//    snprintf(mnemonic, sizeof(mnemonic), "A2=A1, N<%d>=>L", N);
//    print(pc, code, mnemonic, u8"Skip if (A2[7:1]-A1[7:1]) makes zero, N=>L[2:1]", u8"EQJ");

    const auto N = code & 0b00000'0000011;
    const auto A1 = regs.getA1();
    const auto A2 = regs.getA2();
    regs.setSkip(A2 == A1);
    regs.setL(N);
}

void
PD777::execTestEquA2A1_NOT_EQJ(const u16 pc, const u16 code)
{
//    snprintf(mnemonic, sizeof(mnemonic), "A2=A1, N<%d>=>L", N);
//    print(pc, code, mnemonic, u8"Skip if (A2[7:1]-A1[7:1]) makes non zero, N=>L[2:1]", u8"EQJ/");

    const auto N = code & 0b00000'0000011;
    const auto A1 = regs.getA1();
    const auto A2 = regs.getA2();
    regs.setSkip(A2 != A1);
    regs.setL(N);
}

void
PD777::execTestSubA2A1_BOJ(const u16 pc, const u16 code)
{
//    snprintf(mnemonic, sizeof(mnemonic), "A2-A1, N<%d>=>L", N);
//    print(pc, code, mnemonic, u8"Skip if (A2[7:1]-A1[7:1]) makes borrow, N=>L[2:1]", u8"BOJ");

    const auto N = code & 0b00000'0000011;
    const auto A1 = regs.getA1();
    const auto A2 = regs.getA2();
    regs.setSkip(A2 < A1);
    regs.setL(N);
}

void
PD777::execTestSubA2A1_NOT_BOJ(const u16 pc, const u16 code)
{
//    snprintf(mnemonic, sizeof(mnemonic), "A2-A1, N<%d>=>L", N);
//    print(pc, code, mnemonic, u8"Skip if (A2[7:1]-A1[7:1]) makes non borrow, N=>L[2:1]", u8"BOJ/");

    const auto N = code & 0b00000'0000011;
    const auto A1 = regs.getA1();
    const auto A2 = regs.getA2();
    regs.setSkip(A2 >= A1);
    regs.setL(N);
}

void PD777::execTestAndA2A2_EQJ(const u16 pc, const u16 code)
{
//    snprintf(mnemonic, sizeof(mnemonic), "A2&A2, N<%d>=>L", N);
//    print(pc, code, mnemonic, u8"Skip if (A2[7:1]&A2[7:1]) makes zero, N=>L[2:1]", u8"EQJ");

    const auto N = code & 0b00000'0000011;
    const auto A2 = regs.getA2();
    regs.setSkip((A2 & A2) == 0);
    regs.setL(N);
}

void PD777::execTestAndA2A2_NOT_EQJ(const u16 pc, const u16 code)
{
//    snprintf(mnemonic, sizeof(mnemonic), "A2&A2, N<%d>=>L", N);
//    print(pc, code, mnemonic, u8"Skip if (A2[7:1]&A2[7:1]) makes non zero, N=>L[2:1]", u8"EQJ/");
    const auto N = code & 0b00000'0000011;
    const auto A2 = regs.getA2();
    regs.setSkip((A2 & A2) != 0);
    regs.setL(N);
}

void
PD777::execTestEquA2A2_EQJ(const u16 pc, const u16 code)
{
//    snprintf(mnemonic, sizeof(mnemonic), "A2=A2, N<%d>=>L", N);
//    print(pc, code, mnemonic, u8"Skip if (A2[7:1]-A2[7:1]) makes zero, N=>L[2:1]", u8"EQJ");
    const auto N = code & 0b00000'0000011;
    regs.setSkip();
    regs.setL(N);
}

void
PD777::execTestEquA2A2_NOT_EQJ(const u16 pc, const u16 code)
{
//    snprintf(mnemonic, sizeof(mnemonic), "A2=A2, N<%d>=>L", N);
//    print(pc, code, mnemonic, u8"Skip if (A2[7:1]-A2[7:1]) makes zero, N=>L[2:1]", u8"EQJ/");
    const auto N = code & 0b00000'0000011;
    regs.setL(N);
}

void
PD777::execTestSubA2A2_BOJ(const u16 pc, const u16 code)
{
//    snprintf(mnemonic, sizeof(mnemonic), "A2-A2, N<%d>=>L", N);
//    print(pc, code, mnemonic, u8"Skip if (A2[7:1]-A2[7:1]) makes borrow, N=>L[2:1]", u8"BOJ");
    const auto N = code & 0b00000'0000011;
    regs.setL(N);
}

void
PD777::execTestSubA2A2_NOT_BOJ(const u16 pc, const u16 code)
{
//    snprintf(mnemonic, sizeof(mnemonic), "A2-A2, N<%d>=>L", N);
//    print(pc, code, mnemonic, u8"Skip if (A2[7:1]-A2[7:1]) makes non borrow, N=>L[2:1]", u8"BOJ/");
    const auto N = code & 0b00000'0000011;
    regs.setSkip();
    regs.setL(N);
}

void
PD777::execTestAndMA1_EQJ(const u16 pc, const u16 code)
{
//    snprintf(mnemonic, sizeof(mnemonic), "M&A1, N<%d>=>L", N);
//    print(pc, code, mnemonic, u8"Skip if (M[H[5:1],L[2:1]][7:1]&A1[7:1]) makes zero, N=>L[2:1]", u8"EQJ");

    const u8 N = code & 0b00000'0000011;
    const u8 M  = readMemAtHL();
    const u8 A1 = regs.getA1();
    regs.setSkip((M & A1) == 0);
    regs.setL(N);
}

void
PD777::execTestAndMA1_NOT_EQJ(const u16 pc, const u16 code)
{
//    snprintf(mnemonic, sizeof(mnemonic), "M&A1, N<%d>=>L", N);
//    print(pc, code, mnemonic, u8"Skip if (M[H[5:1],L[2:1]][7:1]&A1[7:1]) makes non zero, N=>L[2:1]", u8"EQJ/");

    const u8 N = code & 0b00000'0000011;
    const u8 M  = readMemAtHL();
    const u8 A1 = regs.getA1();
    regs.setSkip((M & A1) != 0);
    regs.setL(N);
}

void
PD777::execTestEquMA1_EQJ(const u16 pc, const u16 code)
{
//    snprintf(mnemonic, sizeof(mnemonic), "M=A1, N<%d>=>L", N);
//    print(pc, code, mnemonic, u8"Skip if (M[H[5:1],L[2:1]][7:1]-A1[7:1]) makes zero, N=>L[2:1]", u8"EQJ");

    const u8 N = code & 0b00000'0000011;
    const u8 M  = readMemAtHL();
    const u8 A1 = regs.getA1();
    regs.setSkip(M == A1);
    regs.setL(N);
}

void
PD777::execTestEquMA1_NOT_EQJ(const u16 pc, const u16 code)
{
//    snprintf(mnemonic, sizeof(mnemonic), "M=A1, N<%d>=>L", N);
//    print(pc, code, mnemonic, u8"Skip if (M[H[5:1],L[2:1]][7:1]-A1[7:1]) makes non zero, N=>L[2:1]", u8"EQJ/");

    const u8 N = code & 0b00000'0000011;
    const u8 M  = readMemAtHL();
    const u8 A1 = regs.getA1();
    regs.setSkip(M != A1);
    regs.setL(N);
}

void
PD777::execTestSubMA1_BOJ(const u16 pc, const u16 code)
{
//    snprintf(mnemonic, sizeof(mnemonic), "M-A1, N<%d>=>L", N);
//    print(pc, code, mnemonic, u8"Skip if (M[H[5:1],L[2:1]][7:1]-A1[7:1]) makes borrow, N=>L[2:1]", u8"BOJ");
    const u8 N = code & 0b00000'0000011;
    const u8 M  = readMemAtHL();
    const u8 A1 = regs.getA1();
    regs.setSkip(M < A1);
    regs.setL(N);
}

void
PD777::execTestSubMA1_NOT_BOJ(const u16 pc, const u16 code)
{
//    snprintf(mnemonic, sizeof(mnemonic), "M-A1, N<%d>=>L", N);
//    print(pc, code, mnemonic, u8"Skip if (M[H[5:1],L[2:1]][7:1]-A1[7:1]) makes non borrow, N=>L[2:1]", u8"BOJ/");
    const u8 N = code & 0b00000'0000011;
    const u8 M  = readMemAtHL();
    const u8 A1 = regs.getA1();
    regs.setSkip(M >= A1);
    regs.setL(N);
}

void
PD777::execTestAndMA2_EQJ(const u16 pc, const u16 code)
{
//    snprintf(mnemonic, sizeof(mnemonic), "M&A2, N<%d>=>L", N);
//    print(pc, code, mnemonic, u8"Skip if (M[H[5:1],L[2:1]][7:1]&A2[7:1]) makes zero, N=>L[2:1]", u8"EQJ");

    const auto N = code & 0b00000'0000011;
    const auto M  = readMemAtHL();
    const auto A2 = regs.getA2();
    regs.setSkip((M & A2) == 0);
    regs.setL(N);
}

void
PD777::execTestAndMA2_NOT_EQJ(const u16 pc, const u16 code)
{
//    snprintf(mnemonic, sizeof(mnemonic), "M&A2, N<%d>=>L", N);
//    print(pc, code, mnemonic, u8"Skip if (M[H[5:1],L[2:1]][7:1]&A2[7:1]) makes non zero, N=>L[2:1]", u8"EQJ/");

    const auto N = code & 0b00000'0000011;
    const auto M  = readMemAtHL();
    const auto A2 = regs.getA2();
    regs.setSkip((M & A2) != 0);
    regs.setL(N);
}

void
PD777::execTestEquMA2_EQJ(const u16 pc, const u16 code)
{
//    snprintf(mnemonic, sizeof(mnemonic), "M=A2, N<%d>=>L", N);
//    print(pc, code, mnemonic, u8"Skip if (M[H[5:1],L[2:1]][7:1]-A2[7:1]) makes zero, N=>L[2:1]", u8"EQJ");

    const u8 N = code & 0b00000'0000011;
    const u8 M  = readMemAtHL();
    const u8 A2 = regs.getA2();
    regs.setSkip(M == A2);
    regs.setL(N);
}

void
PD777::execTestEquMA2_NOT_EQJ(const u16 pc, const u16 code)
{
//    snprintf(mnemonic, sizeof(mnemonic), "M=A2, N<%d>=>L", N);
//    print(pc, code, mnemonic, u8"Skip if (M[H[5:1],L[2:1]][7:1]-A2[7:1]) makes non zero, N=>L[2:1]", u8"EQJ/");

    const u8 N = code & 0b00000'0000011;
    const u8 M  = readMemAtHL();
    const u8 A2 = regs.getA2();
    regs.setSkip(M != A2);
    regs.setL(N);
}

void
PD777::execTestSubMA2_BOJ(const u16 pc, const u16 code)
{
//    snprintf(mnemonic, sizeof(mnemonic), "M-A2, N<%d>=>L", N);
//    print(pc, code, mnemonic, u8"Skip if (M[H[5:1],L[2:1]][7:1]-A2[7:1]) makes borrow, N=>L[2:1]", u8"BOJ");

    const u8 N = code & 0b00000'0000011;
    const u8 M  = readMemAtHL();
    const u8 A2 = regs.getA2();
    regs.setSkip(M < A2);
    regs.setL(N);
}

void
PD777::execTestSubMA2_NOT_BOJ(const u16 pc, const u16 code)
{
//    snprintf(mnemonic, sizeof(mnemonic), "M-A2, N<%d>=>L", N);
//    print(pc, code, mnemonic, u8"Skip if (M[H[5:1],L[2:1]][7:1]-A2[7:1]) makes non borrow, N=>L[2:1]", u8"BOJ/");

    const u8 N = code & 0b00000'0000011;
    const u8 M  = readMemAtHL();
    const u8 A2 = regs.getA2();
    regs.setSkip(M >= A2);
    regs.setL(N);
}

void
PD777::execTestAndHA1_EQJ(const u16 pc, const u16 code)
{
//    snprintf(mnemonic, sizeof(mnemonic), "H&A1, N<%d>=>L", N);
//    print(pc, code, mnemonic, u8"Skip if (H[5:1]&A1[5:1]) makes zero, N=>L[2:1]", u8"EQJ");

    const auto N       = code & 0b00000'0000011;
    const auto H       = regs.getH();
    const auto A1      = regs.getA1();
    const auto A1_5bit = regs.getA1() & 0x1F;
#if 0 // CMP_HA1_7bit
    regs.setSkip((H & A1) == 0);
#else
    regs.setSkip((H & A1_5bit) == 0);
#endif
    regs.setL(N);
}

void
PD777::execTestAndHA1_NOT_EQJ(const u16 pc, const u16 code)
{
//    snprintf(mnemonic, sizeof(mnemonic), "H&A1, N<%d>=>L", N);
//    print(pc, code, mnemonic, u8"Skip if (H[5:1]&A1[5:1]) makes non zero, N=>L[2:1]", u8"EQJ/");

    const auto N       = code & 0b00000'0000011;
    const auto H       = regs.getH();
    const auto A1      = regs.getA1();
    const auto A1_5bit = regs.getA1() & 0x1F;
#if 0 // CMP_HA1_7bit
    regs.setSkip((H & A1) != 0);
#else
    regs.setSkip((H & A1_5bit) != 0);
#endif
    regs.setL(N);
}

void
PD777::execTestEquHA1_EQJ(const u16 pc, const u16 code)
{
//    snprintf(mnemonic, sizeof(mnemonic), "H=A1, N<%d>=>L", N);
//    print(pc, code, mnemonic, u8"Skip if (H[5:1]-A1[5:1]) makes zero, N=>L[2:1]", u8"EQJ");

    const auto N       = code & 0b00000'0000011;
    const auto H       = regs.getH();
    const auto A1      = regs.getA1();
    const auto A1_5bit = regs.getA1() & 0x1F;
#if CMP_HA1_7bit
    regs.setSkip(H == A1);
#else
    regs.setSkip(H == A1_5bit);
#endif
    regs.setL(N);
}

void
PD777::execTestEquHA1_NOT_EQJ(const u16 pc, const u16 code)
{
//    snprintf(mnemonic, sizeof(mnemonic), "H=A1, N<%d>=>L", N);
//    print(pc, code, mnemonic, u8"Skip if (H[5:1]-A1[5:1]) makes non zero, N=>L[2:1]", u8"EQJ/");

    const auto N       = code & 0b00000'0000011;
    const auto H       = regs.getH();
    const auto A1      = regs.getA1();
    const auto A1_5bit = regs.getA1() & 0x1F;
#if CMP_HA1_7bit
    regs.setSkip(H != A1);
#else
    regs.setSkip(H != A1_5bit);
#endif
    regs.setL(N);
}

void
PD777::execTestSubHA1_BOJ(const u16 pc, const u16 code)
{
//    snprintf(mnemonic, sizeof(mnemonic), "H-A1, N<%d>=>L", N);
//    print(pc, code, mnemonic, u8"Skip if (H[5:1]-A1[5:1]) makes borrow, N=>L[2:1]", u8"BOJ");

    const auto N       = code & 0b00000'0000011;
    const auto H       = regs.getH();
    const auto A1      = regs.getA1();
    const auto A1_5bit = regs.getA1() & 0x1F;
#if CMP_HA1_7bit
    regs.setSkip(H < A1);
#else
    regs.setSkip(H < A1_5bit);
#endif
    regs.setL(N);
}

void
PD777::execTestSubHA1_NOT_BOJ(const u16 pc, const u16 code)
{
//    snprintf(mnemonic, sizeof(mnemonic), "H-A1, N<%d>=>L", N);
//    print(pc, code, mnemonic, u8"Skip if (H[5:1]-A1[5:1]) makes non borrow, N=>L[2:1]", u8"BOJ/");

    const auto N       = code & 0b00000'0000011;
    const auto H       = regs.getH();
    const auto A1      = regs.getA1();
    const auto A1_5bit = regs.getA1() & 0x1F;
#if CMP_HA1_7bit
    regs.setSkip(H >= A1);
#else
    regs.setSkip(H >= A1_5bit);
#endif
    regs.setL(N);
}

void
PD777::execTestAndHA2_EQJ(const u16 pc, const u16 code)
{
//    snprintf(mnemonic, sizeof(mnemonic), "H&A2, N<%d>=>L", N);
//    print(pc, code, mnemonic, u8"Skip if (H[5:1]&A2[5:1]) makes zero, N=>L[2:1]", u8"EQJ");

    const auto N       = code & 0b00000'0000011;
    const auto H       = regs.getH();
    const auto A2      = regs.getA2();
    const auto A2_5bit = regs.getA2() & 0x1F;
#if 0 // CMP_HA1_7bit
    regs.setSkip((H & A2) == 0);
#else
    regs.setSkip((H & A2_5bit) == 0);
#endif
    regs.setL(N);
}

void
PD777::execTestAndHA2_NOT_EQJ(const u16 pc, const u16 code)
{
//    snprintf(mnemonic, sizeof(mnemonic), "H&A2, N<%d>=>L", N);
//    print(pc, code, mnemonic, u8"Skip if (H[5:1]&A2[5:1]) makes non zero, N=>L[2:1]", u8"EQJ/");

    const auto N       = code & 0b00000'0000011;
    const auto H       = regs.getH();
    const auto A2      = regs.getA2();
    const auto A2_5bit = regs.getA2() & 0x1F;
#if 0 // CMP_HA1_7bit
    regs.setSkip((H & A2) != 0);
#else
    regs.setSkip((H & A2_5bit) != 0);
#endif
    regs.setL(N);
}

void
PD777::execTestEquHA2_EQJ(const u16 pc, const u16 code)
{
//    snprintf(mnemonic, sizeof(mnemonic), "H=A2, N<%d>=>L", N);
//    print(pc, code, mnemonic, u8"Skip if (H[5:1]-A2[5:1]) makes zero, N=>L[2:1]", u8"EQJ");

    const auto N = code & 0b00000'0000011;
    const auto H  = regs.getH();
    const auto A2 = regs.getA2() & 0x1F;
    regs.setSkip(H == A2);
    regs.setL(N);
}

void
PD777::execTestEquHA2_NOT_EQJ(const u16 pc, const u16 code)
{
//    snprintf(mnemonic, sizeof(mnemonic), "H=A2, N<%d>=>L", N);
//    print(pc, code, mnemonic, u8"Skip if (H[5:1]-A2[5:1]) makes non zero, N=>L[2:1]", u8"EQJ/");

    const auto N = code & 0b00000'0000011;
    const auto H  = regs.getH();
    const auto A2 = regs.getA2() & 0x1F;
    regs.setSkip(H != A2);
    regs.setL(N);
}

void
PD777::execTestSubHA2_BOJ(const u16 pc, const u16 code)
{
//    snprintf(mnemonic, sizeof(mnemonic), "H-A2, N<%d>=>L", N);
//    print(pc, code, mnemonic, u8"Skip if (H[5:1]-A2[5:1]) makes borrow, N=>L[2:1]", u8"BOJ");
    const auto N = code & 0b00000'0000011;
    const auto H  = regs.getH();
    const auto A2 = regs.getA2() & 0x1F;
    regs.setSkip(H < A2);
    regs.setL(N);
}

void
PD777::execTestSubHA2_NOT_BOJ(const u16 pc, const u16 code)
{
//    snprintf(mnemonic, sizeof(mnemonic), "H-A2, N<%d>=>L", N);
//    print(pc, code, mnemonic, u8"Skip if (H[5:1]-A2[5:1]) makes non borrow, N=>L[2:1]", u8"BOJ/");
    const auto N = code & 0b00000'0000011;
    const auto H  = regs.getH();
    const auto A2 = regs.getA2() & 0x1F;
    regs.setSkip(H >= A2);
    regs.setL(N);
}

void
PD777::execMoveNtoL(const u16 pc, const u16 code)
{
//    snprintf(mnemonic, sizeof(mnemonic), "N<%d>=>L", N);
//    print(pc, code, mnemonic, u8"N=>L[2:1]");

    const auto N = code & 0b00000'0000011;
    regs.setL(N);
}

void
PD777::execMoveA2toA1(const u16 pc, const u16 code)
{
//    snprintf(mnemonic, sizeof(mnemonic), "A2=>A1, N<%d>=>L", N);
//    print(pc, code, mnemonic, u8"Move A2[7:1] to A1[7:1], N=>L[2:1]");

    const auto N = code & 0b00000'0000011;
    const auto A2 = regs.getA2();
    regs.setA1(A2);
    regs.setL(N);
}

void
PD777::execSubA1andA2toA2(const u16 pc, const u16 code)
{
//    snprintf(mnemonic, sizeof(mnemonic), "A1-A2=>A2, N<%d>=>L", N);
//    print(pc, code, mnemonic, u8"Subtract A1[7:1] and A2[7:1], store to A2[7:1], Skip if borrow, N=>L[2:1]", u8"BOJ");

    const auto N = code & 0b00000'0000011;
    const auto A1 = regs.getA1();
    const auto A2 = regs.getA2();

    regs.setSkip(A1 < A2);
    regs.setA2(A1 - A2);
    regs.setL(N);
}

void
PD777::execMoveA1toFLS(const u16 pc, const u16 code)
{
//    print(pc, code, "A1=>FLS, 0x0=>L", u8"Move A1[7:1] to FLS[7:1], 0=>L[2:1]");

    const auto A1 = regs.getA1();
    setFLS(A1);
    regs.setL(0);
}

void
PD777::execMoveA1toFRS(const u16 pc, const u16 code)
{
//    print(pc, code, "A1=>FRS, 0x1=>L", u8"Move A1[7:1] to FRS[7:1], 1=>L[2:1]");

    const auto A1 = regs.getA1();
    setFRS(A1);
    regs.setL(1);
}

void
PD777::execMoveA1toMODE(const u16 pc, const u16 code)
{
//    snprintf(mnemonic, sizeof(mnemonic), "A1=>MODE, N<%d>=>L", N);
//    print(pc, code, mnemonic, u8"Move A1[7:1] to MODE[7:1], 1N=>L[2:1]");

    const auto N  = code & 0b00000'0000011;
    const auto A1 = regs.getA1();
    setMode(A1);
    regs.setL(N);
}

void
PD777::execRightShiftA1(const u16 pc, const u16 code)
{
//    snprintf(mnemonic, sizeof(mnemonic), "A1=>RS, N<%d>=>L", N);
//    print(pc, code, mnemonic, u8"Right shift A1[7:1], 0=>A1[7], N=>L[2:1]");

    const u8 N = code & 0b00000'0000011;
    const u8 A1 = regs.getA1();
    regs.setA1(A1 >> 1);
    regs.setL(N);
}

void
PD777::execMoveA1toA2(const u16 pc, const u16 code)
{
//    snprintf(mnemonic, sizeof(mnemonic), "A1=>A2, N<%d>=>L", N);
//    print(pc, code, mnemonic, u8"Move A1[7:1] to A2[7:1], N=>L[2:1]");

    const auto N = code & 0b00000'0000011;
    const auto A1 = regs.getA1();
    regs.setA2(A1);
    regs.setL(N);
}

void
PD777::execMoveA2toFLS(const u16 pc, const u16 code)
{
//    print(pc, code, "A2=>FLS, 0x0=>L", u8"Move A2[7:1] to FLS[7:1], 0=>L[2:1]");

    const auto A2 = regs.getA2();
    setFLS(A2);
    regs.setL(0);
}

void
PD777::execMoveA2toFRS(const u16 pc, const u16 code)
{
//    print(pc, code, "A2=>FRS, 0x1=>L", u8"Move A2[7:1] to FRS[7:1], 1=>L[2:1]");

    const auto A2 = regs.getA2();
    setFRS(A2);
    regs.setL(1);
}

void
PD777::execMoveA2toMODE(const u16 pc, const u16 code)
{
//    snprintf(mnemonic, sizeof(mnemonic), "A2=>MODE, N<%d>=>L", N);
//    print(pc, code, mnemonic, u8"Move A2[7:1] to MODE[7:1], 1N=>L[2:1]");

    const auto N = code & 0b00000'0000011;
    const auto A2 = regs.getA2();
    setMode(A2);
    regs.setL(N);
}

void
PD777::execSubA2andA1toA1(const u16 pc, const u16 code)
{
//    snprintf(mnemonic, sizeof(mnemonic), "A2-A1=>A1, N<%d>=>L", N);
//    print(pc, code, mnemonic, u8"Subtract A2[7:1] and A1[7:1], store to A1[7:1], Skip if borrow, N=>L[2:1]", u8"BOJ");

    const auto N = code & 0b00000'0000011;
    const auto A1 = regs.getA1();
    const auto A2 = regs.getA2();
    // @todo FIX ME!!!!
#if PATCH_BattleVader_HIT
    // Skip A2 < A1
    // A1 => A2
    regs.setSkip(A2 < A1);
    regs.setA1(A2);
    regs.setL(N);
/*
弾除けの当たり判定部分で、X軸でスプライトを検索しているが、その部分がおかしい。
A1レジスタではなくA2レジスタで判定するようになるようにすると、正常に弾除けに当たるようになる。
　231 34D BOJ  A2-A1 => A1, N<1> => L             ; Subtract A2[7:1] and A1[7:1], store to A1[7:1], Skip if borrow, N => L[2:1]
　// この命令ここでしか使われてないので、挙動を変更した。


//
// 弾除けとの当たり判定
// 当たってない +1に返る
//
Entry22E: ; caller 535
    // HとA1の入れ替え
22E C54      JS $054                            ; Move K[10:1] to A[10:1], 0 to A11, Jump to A[11:1], Push next A[11:1] up to ROM address stack
25C 300      N<0> => L                          ; N => L[2:1]
238 0DA BOJ  M-K<$5A>                           ; Skip if (M[H[5:1],L[2:1]][7:1]-K[7:1]) makes borrow
270 A43      JP $243                            ; Move K[10:1] to A[10:1], Jump to A[11:1]
261 A44      JP $244                            ; Move K[10:1] to A[10:1], Jump to A[11:1]
    // Y座標が0x5A以上のとき 弾除け
Label243: ; from 270
243 0E2 BOJ  M-K<$62>                           ; Skip if (M[H[5:1],L[2:1]][7:1]-K[7:1]) makes borrow
206 A44      JP $244                            ; Move K[10:1] to A[10:1], Jump to A[11:1]
    // Y座標が0x5A以上、0x61以下のとき
    // X座標の判定
20D 301      N<1> => L                          ; N => L[2:1]
21B 698      K<$18> => A2                       ; Store K[7:1] to A2[7:1]
237 09E BOJ  M-K<$1E>                           ; Skip if (M[H[5:1],L[2:1]][7:1]-K[7:1]) makes borrow
26E 6A4      K<$24> => A2                       ; Store K[7:1] to A2[7:1]
25D 0AA BOJ  M-K<$2A>                           ; Skip if (M[H[5:1],L[2:1]][7:1]-K[7:1]) makes borrow
23A 6B0      K<$30> => A2                       ; Store K[7:1] to A2[7:1]
274 0B6 BOJ  M-K<$36>                           ; Skip if (M[H[5:1],L[2:1]][7:1]-K[7:1]) makes borrow
269 6BC      K<$3C> => A2                       ; Store K[7:1] to A2[7:1]
253 0C2 BOJ  M-K<$42>                           ; Skip if (M[H[5:1],L[2:1]][7:1]-K[7:1]) makes borrow
226 A44      JP $244                            ; Move K[10:1] to A[10:1], Jump to A[11:1]
24C 2BD BOJ/ M-A2, N<1> => L                    ; Skip if (M[H[5:1],L[2:1]][7:1]-A2[7:1]) makes non borrow, N => L[2:1]
218 A44      JP $244                            ; Move K[10:1] to A[10:1], Jump to A[11:1]
231 34D BOJ  A2-A1 => A1, N<1> => L             ; Subtract A2[7:1] and A1[7:1], store to A1[7:1], Skip if borrow, N => L[2:1]
262 5B9      K<$39> => L<1>,H<$19>              ; Store K[7:6] to L[2:1] and K[5:1] to H[5:1]
    // 弾除けにヒットした
Label245: ; from 256
    // 衝突判定 X座標と同じものを見つける 
    // A1:X座標   見つかったら +1に返る
    // @todo ここA1ではなく、A2じゃないんだろうか？？
245 D12      JS $112                            ; Move K[10:1] to A[10:1], 0 to A11, Jump to A[11:1], Push next A[11:1] up to ROM address stack
20A A44      JP $244                            ; Move K[10:1] to A[10:1], Jump to A[11:1]
    // 弾除け見つかった
215 302      N<2> => L                          ; N => L[2:1]
22B 0C9 BOJ  M-K<$49>                           ; Skip if (M[H[5:1],L[2:1]][7:1]-K[7:1]) makes borrow
256 A45      JP $245                            ; Move K[10:1] to A[10:1], Jump to A[11:1]
    // パターン[0,0x49) 弾除けのパターン
22C 3CE      H => A1, N<2> => L                 ; Move H[5:1] to A1[5:1], 0 => A1[7:6], N => L[2:1]
258 599      K<$19> => L<0>,H<$19>              ; Store K[7:6] to L[2:1] and K[5:1] to H[5:1]
230 380      A1 => M, N<0> => L                 ; Move A1[7:1] to M[H[5:1],L[2:1]][7:1], N => L[2:1]
*/
#else
    regs.setSkip(A2 < A1);
    regs.setA1(A2 - A1);
    regs.setL(N);
#endif
}

void
PD777::execRightShiftA2(const u16 pc, const u16 code)
{
//    snprintf(mnemonic, sizeof(mnemonic), "A2=>RS, N<%d>=>L", N);
//    print(pc, code, mnemonic, u8"Right shift A2[7:1], 0=>A2[7], N=>L[2:1]");

    const u8 N = code & 0b00000'0000011;
    const u8 A2 = regs.getA2();
    regs.setA2(A2 >> 1);
    regs.setL(N);
}

void
PD777::execMoveMtoFLS(const u16 pc, const u16 code)
{
//    print(pc, code, "M=>FLS, 0x0=>L", u8"Move M[H[5:1],L[2:1]][7:1] to FLS[7:1], 0=>L[2:1]");

    // Move M[H[5:1],L[2:1]][7:1] to FLS[7:1], 0=>L[2:1]
    const auto value = readMemAtHL();
    setFLS(value);
    regs.setL(0);
}

void
PD777::execMoveMtoFRS(const u16 pc, const u16 code)
{
//    print(pc, code, "M=>FRS, 0x1=>L", u8"Move M[H[5:1],L[2:1]][7:1] to FRS[7:1], 1=>L[2:1]");

    // Move M[H[5:1],L[2:1]][7:1] to FRS[7:1], 1=>L[2:1]
    const auto value = readMemAtHL();
    setFRS(value);
    regs.setL(1);
}

void
PD777::execMoveMtoMODE(const u16 pc, const u16 code)
{
//    snprintf(mnemonic, sizeof(mnemonic), "M=>MODE, N<%d>=>L", N);
//    print(pc, code, mnemonic, u8"Move M[H[5:1],L[2:1]][7:1] to MODE[7:1], 1N=>L[2:1]");
    const auto N = code & 0b00000'0000011;
    const auto M = readMemAtHL();
    setMode(M);
    regs.setL(N);
}

void
PD777::execRightShiftM(const u16 pc, const u16 code)
{
//    snprintf(mnemonic, sizeof(mnemonic), "M=>RS, N<%d>=>L", N);
//    print(pc, code, mnemonic, u8"Right shift M[H[5:1],L[2:1]][7:1], 0=>M[H[5:1],L[2:1]][7], N=>L[2:1]");
    const auto N = code & 0b00000'0000011;
    const auto M = readMemAtHL();
    writeMemAtHL(M >> 1);
    regs.setL(N);
}

void
PD777::execAndA1A1toA1(const u16 pc, const u16 code)
{
//    snprintf(mnemonic, sizeof(mnemonic), "A1&A1=>A1, N<%d>=>L", N);
//    print(pc, code, mnemonic, u8"AND A1[7:1] and A1[7:1], store to A1[7:1], N=>L[2:1]");

    const auto N = code & 0b00000'0000011;
    const auto A1 = regs.getA1();
    regs.setA1(A1 & A1);
    regs.setL(N);
}

void
PD777::execAddA1A1toA1(const u16 pc, const u16 code)
{
//    snprintf(mnemonic, sizeof(mnemonic), "A1+A1=>A1, N<%d>=>L", N);
//    print(pc, code, mnemonic, u8"Add A1[7:1] and A1[7:1], store to A1[7:1], N=>L[2:1]", u8"CAJ");

    const auto N = code & 0b00000'0000011;
    const auto A1 = regs.getA1();
    regs.setA1(A1 + A1);
    regs.setL(N);
}

void
PD777::execOrA1A1toA1(const u16 pc, const u16 code)
{
//    snprintf(mnemonic, sizeof(mnemonic), "A1|A1=>A1, N<%d>=>L", N);
//    print(pc, code, mnemonic, u8"OR A1[7:1] and A1[7:1], store to A1[7:1], N=>L[2:1]");

    const auto N = code & 0b00000'0000011;
    const auto A1 = regs.getA1();
    regs.setA1(A1 | A1);
    regs.setL(N);
}

void
PD777::execSubA1A1toA1(const u16 pc, const u16 code)
{
//    snprintf(mnemonic, sizeof(mnemonic), "A1-A1=>A1, N<%d>=>L", N);
//    print(pc, code, mnemonic, u8"Subtract A1[7:1] and A1[7:1], store to A1[7:1], Skip if borrow, N=>L[2:1]", u8"BOJ");

    const auto N = code & 0b00000'0000011;
    const auto A1 = regs.getA1();
    regs.setSkip(A1 < A1);
    regs.setA1(A1 - A1);
    regs.setL(N);
}

void
PD777::execAndA1A2toA1(const u16 pc, const u16 code)
{
//    snprintf(mnemonic, sizeof(mnemonic), "A1&A2=>A1, N<%d>=>L", N);
//    print(pc, code, mnemonic, u8"AND A1[7:1] and A2[7:1], store to A1[7:1], N=>L[2:1]");

    const auto N = code & 0b00000'0000011;
    const auto A1 = regs.getA1();
    const auto A2 = regs.getA2();
    regs.setA1(A1 & A2);
    regs.setL(N);
}

void
PD777::execAddA1A2toA1(const u16 pc, const u16 code)
{
//    snprintf(mnemonic, sizeof(mnemonic), "A1+A2=>A1, N<%d>=>L", N);
//    print(pc, code, mnemonic, u8"Add A1[7:1] and A2[7:1], store to A1[7:1], N=>L[2:1]", u8"CAJ");
    const u8 N = code & 0b00000'0000011;
    const u8 A1 = regs.getA1();
    const u8 A2 = regs.getA2();
    regs.setA1(A1 + A2);
    regs.setL(N);
}

void
PD777::execOrA1A2toA1(const u16 pc, const u16 code)
{
//    snprintf(mnemonic, sizeof(mnemonic), "A1|A2=>A1, N<%d>=>L", N);
//    print(pc, code, mnemonic, u8"OR A1[7:1] and A2[7:1], store to A1[7:1], N=>L[2:1]");

    const auto N = code & 0b00000'0000011;
    const auto A1 = regs.getA1();
    const auto A2 = regs.getA2();
    regs.setA1(A1 | A2);
    regs.setL(N);
}

void
PD777::execSubA1A2toA1(const u16 pc, const u16 code)
{
//    snprintf(mnemonic, sizeof(mnemonic), "A1-A2=>A1, N<%d>=>L", N);
//    print(pc, code, mnemonic, u8"Subtract A1[7:1] and A2[7:1], store to A1[7:1], Skip if borrow, N=>L[2:1]", u8"BOJ");
    const auto N = code & 0b00000'0000011;
    const auto A1 = regs.getA1();
    const auto A2 = regs.getA2();
    regs.setSkip(A1 < A2);
    regs.setA1(A1 - A2);
    regs.setL(N);
}

void
PD777::execAndA2A1toA2(const u16 pc, const u16 code)
{
//    snprintf(mnemonic, sizeof(mnemonic), "A2&A1=>A2, N<%d>=>L", N);
//    print(pc, code, mnemonic, u8"AND A2[7:1] and A1[7:1], store to A2[7:1], N=>L[2:1]");

    const auto N = code & 0b00000'0000011;
    const auto A1 = regs.getA1();
    const auto A2 = regs.getA2();
    regs.setA2(A2 & A1);
    regs.setL(N);
}

void
PD777::execAddA2A1toA2(const u16 pc, const u16 code)
{
//    snprintf(mnemonic, sizeof(mnemonic), "A2+A1=>A2, N<%d>=>L", N);
//    print(pc, code, mnemonic, u8"Add A2[7:1] and A1[7:1], store to A2[7:1], N=>L[2:1]", u8"CAJ");

    const auto N = code & 0b00000'0000011;
    const auto A1 = regs.getA1();
    const auto A2 = regs.getA2();
    regs.setA2(A2 + A1);
    regs.setL(N);
}

void
PD777::execOrA2A1toA2(const u16 pc, const u16 code)
{
//    snprintf(mnemonic, sizeof(mnemonic), "A2|A1=>A2, N<%d>=>L", N);
//    print(pc, code, mnemonic, u8"OR A2[7:1] and A1[7:1], store to A2[7:1], N=>L[2:1]");

    const auto N = code & 0b00000'0000011;
    const auto A1 = regs.getA1();
    const auto A2 = regs.getA2();
    regs.setA2(A2 | A1);
    regs.setL(N);
}

void
PD777::execSubA2A1toA2(const u16 pc, const u16 code)
{
//    snprintf(mnemonic, sizeof(mnemonic), "A2-A1=>A2, N<%d>=>L", N);
//    print(pc, code, mnemonic, u8"Subtract A2[7:1] and A1[7:1], store to A2[7:1], Skip if borrow, N=>L[2:1]", u8"BOJ");
    const auto N = code & 0b00000'0000011;
    const auto A1 = regs.getA1();
    const auto A2 = regs.getA2();
    regs.setSkip(A2 < A1);
    regs.setA2(A2 - A1);
    regs.setL(N);
}

void
PD777::execAndA2A2toA2(const u16 pc, const u16 code)
{
//    snprintf(mnemonic, sizeof(mnemonic), "A2&A2=>A2, N<%d>=>L", N);
//    print(pc, code, mnemonic, u8"AND A2[7:1] and A2[7:1], store to A2[7:1], N=>L[2:1]");
    const auto N = code & 0b00000'0000011;
//    const auto A2 = regs.getA2();
//    regs.setA2(A2 & A2);
    regs.setL(N);
}

void
PD777::execAddA2A2toA2(const u16 pc, const u16 code)
{
//    snprintf(mnemonic, sizeof(mnemonic), "A2+A2=>A2, N<%d>=>L", N);
//    print(pc, code, mnemonic, u8"Add A2[7:1] and A2[7:1], store to A2[7:1], N=>L[2:1]", u8"CAJ");
    const auto N = code & 0b00000'0000011;
    const auto A2 = regs.getA2();
    regs.setA2(A2 + A2);
    regs.setL(N);
}

void
PD777::execOrA2A2toA2(const u16 pc, const u16 code)
{
//    snprintf(mnemonic, sizeof(mnemonic), "A2|A2=>A2, N<%d>=>L", N);
//    print(pc, code, mnemonic, u8"OR A2[7:1] and A2[7:1], store to A2[7:1], N=>L[2:1]");
    const auto N = code & 0b00000'0000011;
    regs.setL(N);
}

void
PD777::execSubA2A2toA2(const u16 pc, const u16 code)
{
//    snprintf(mnemonic, sizeof(mnemonic), "A2-A2=>A2, N<%d>=>L", N);
//    print(pc, code, mnemonic, u8"Subtract A2[7:1] and A2[7:1], store to A2[7:1], Skip if borrow, N=>L[2:1]", u8"BOJ");
    const auto N = code & 0b00000'0000011;
    const auto A2 = regs.getA2();
    regs.setSkip(A2 < A2);
    regs.setA2(A2 - A2);
    regs.setL(N);
}

void
PD777::execMoveA1toM(const u16 pc, const u16 code)
{
//    snprintf(mnemonic, sizeof(mnemonic), "A1=>M, N<%d>=>L", N);
//    print(pc, code, mnemonic, u8"Move A1[7:1] to M[H[5:1],L[2:1]][7:1], N=>L[2:1]");

    const auto N = code & 0b00000'0000011;
    const auto A1 = regs.getA1();
    writeMemAtHL(A1);
    regs.setL(N);
}

void
PD777::execMoveA2toM(const u16 pc, const u16 code)
{
//    snprintf(mnemonic, sizeof(mnemonic), "A2=>M, N<%d>=>L", N);
//    print(pc, code, mnemonic, u8"Move A2[7:1] to M[H[5:1],L[2:1]][7:1], N=>L[2:1]");

    const auto N = code & 0b00000'0000011;
    const auto A2 = regs.getA2();
    writeMemAtHL(A2);
    regs.setL(N);
}

void
PD777::execExchangeMA1(const u16 pc, const u16 code)
{
//    snprintf(mnemonic, sizeof(mnemonic), "M<==>A1, N<%d>=>L", N);
//    print(pc, code, mnemonic, u8"Exchange M[H[5:1],L[2:1]][7:1] and A1[7:1], N=>L[2:1]");

    const auto N = code & 0b00000'0000011;
    const auto A1 = regs.getA1();
    const auto M  = readMemAtHL();
    writeMemAtHL(A1);
    regs.setA1(M);
    regs.setL(N);
}

void
PD777::execExchangeMA2(const u16 pc, const u16 code)
{
//    snprintf(mnemonic, sizeof(mnemonic), "M<==>A2, N<%d>=>L", N);
//    print(pc, code, mnemonic, u8"Exchange M[H[5:1],L[2:1]][7:1] and A2[7:1], N=>L[2:1]");

    const auto N = code & 0b00000'0000011;
    const auto A2 = regs.getA2();
    const auto M  = readMemAtHL();
    writeMemAtHL(A2);
    regs.setA2(M);
    regs.setL(N);
}

void
PD777::execMoveMtoA1(const u16 pc, const u16 code)
{
//    snprintf(mnemonic, sizeof(mnemonic), "M=>A1, N<%d>=>L", N);
//    print(pc, code, mnemonic, u8"Move M[H[5:1],L[2:1]][7:1] to A1[7:1], N=>L[2:1]");

    const auto N = code & 0b00000'0000011;
    const auto M  = readMemAtHL();
    regs.setA1(M);
    regs.setL(N);
}

void
PD777::execMoveMtoA2(const u16 pc, const u16 code)
{
//    snprintf(mnemonic, sizeof(mnemonic), "M=>A2, N<%d>=>L", N);
//    print(pc, code, mnemonic, u8"Move M[H[5:1],L[2:1]][7:1] to A2[7:1], N=>L[2:1]");

    const auto N = code & 0b00000'0000011;
    const auto M  = readMemAtHL();
    regs.setA2(M);
    regs.setL(N);
}

void
PD777::execAndMA2toM(const u16 pc, const u16 code)
{
//    snprintf(mnemonic, sizeof(mnemonic), "M&A2=>M, N<%d>=>L", N);
//    print(pc, code, mnemonic, u8"AND M[H[5:1],L[2:1]][7:1] and A2[7:1], store to M[H[5:1],L[2:1]][7:1], N=>L[2:1]");

    const auto N = code & 0b00000'0000011;
    const auto A2 = regs.getA2();
    const auto M  = readMemAtHL();
    writeMemAtHL(M & A2);
    regs.setL(N);
}

void
PD777::execAddMA2toM(const u16 pc, const u16 code)
{
//    snprintf(mnemonic, sizeof(mnemonic), "M+A2=>M, N<%d>=>L", N);
//    print(pc, code, mnemonic, u8"Add M[H[5:1],L[2:1]][7:1] and A2[7:1], store to M[H[5:1],L[2:1]][7:1], N=>L[2:1], Skip if carry", u8"CAJ");
    const auto N = code & 0b00000'0000011;
    const auto A2 = regs.getA2();
    const auto M  = readMemAtHL();
    regs.setSkip((M + A2) > 0x7F);
    writeMemAtHL(M + A2);
    regs.setL(N);
}

void
PD777::execOrMA2toM(const u16 pc, const u16 code)
{
//    snprintf(mnemonic, sizeof(mnemonic), "M|A2=>M, N<%d>=>L", N);
//    print(pc, code, mnemonic, u8"OR M[H[5:1],L[2:1]][7:1] and A2[7:1], store to M[H[5:1],L[2:1]][7:1], N=>L[2:1]");

    const auto N = code & 0b00000'0000011;
    const auto A2 = regs.getA2();
    const auto M  = readMemAtHL();
    writeMemAtHL(M | A2);
    regs.setL(N);
}

void
PD777::execSubMA2toM(const u16 pc, const u16 code)
{
//    snprintf(mnemonic, sizeof(mnemonic), "M-A2=>M, N<%d>=>L", N);
//    print(pc, code, mnemonic, u8"Subtract M[H[5:1],L[2:1]][7:1] and A2[7:1], store to M[H[5:1],L[2:1]][7:1], N=>L[2:1], Skip if borrow", u8"BOJ");
    const auto N = code & 0b00000'0000011;
    const auto A2 = regs.getA2();
    const auto M  = readMemAtHL();
    regs.setSkip(M < A2);
    writeMemAtHL(M - A2);
    regs.setL(N);
}

void
PD777::execAndMA1toM(const u16 pc, const u16 code)
{
//    snprintf(mnemonic, sizeof(mnemonic), "M&A1=>M, N<%d>=>L", N);
//    print(pc, code, mnemonic, u8"AND M[H[5:1],L[2:1]][7:1] and A1[7:1], store to M[H[5:1],L[2:1]][7:1], N=>L[2:1]");

    const auto N = code & 0b00000'0000011;
    const auto A1 = regs.getA1();
    const auto M  = readMemAtHL();
    writeMemAtHL(M & A1);
    regs.setL(N);
}

void
PD777::execAddMA1toM(const u16 pc, const u16 code)
{
//    snprintf(mnemonic, sizeof(mnemonic), "M+A1=>M, N<%d>=>L", N);
//    print(pc, code, mnemonic, u8"Add M[H[5:1],L[2:1]][7:1] and A1[7:1], store to M[H[5:1],L[2:1]][7:1], N=>L[2:1], Skip if carry", u8"CAJ");

    const auto N = code & 0b00000'0000011;
    const auto A1 = regs.getA1();
    const auto M  = readMemAtHL();
    regs.setSkip((M + A1) > 0x7F);
    writeMemAtHL(M + A1);
    regs.setL(N);
}

void
PD777::execOrMA1toM(const u16 pc, const u16 code)
{
//    snprintf(mnemonic, sizeof(mnemonic), "M|A1=>M, N<%d>=>L", N);
//    print(pc, code, mnemonic, u8"OR M[H[5:1],L[2:1]][7:1] and A1[7:1], store to M[H[5:1],L[2:1]][7:1], N=>L[2:1]");

    const auto N = code & 0b00000'0000011;
    const auto A1 = regs.getA1();
    const auto M  = readMemAtHL();
    writeMemAtHL(M | A1);
    regs.setL(N);
}

void
PD777::execSubMA1toM(const u16 pc, const u16 code)
{
//    snprintf(mnemonic, sizeof(mnemonic), "M-A1=>M, N<%d>=>L", N);
//    print(pc, code, mnemonic, u8"Subtract M[H[5:1],L[2:1]][7:1] and A1[7:1], store to M[H[5:1],L[2:1]][7:1], N=>L[2:1], Skip if borrow", u8"BOJ");

    const auto N = code & 0b00000'0000011;
    const auto A1 = regs.getA1();
    const auto M  = readMemAtHL();
    regs.setSkip(M < A1);
    writeMemAtHL(M - A1);
    regs.setL(N);
}

void
PD777::execMoveA1toH(const u16 pc, const u16 code)
{
//    snprintf(mnemonic, sizeof(mnemonic), "A1=>H, N<%d>=>L", N);
//    print(pc, code, mnemonic, u8"Move A1[5:1] to H[5:1], N=>L[2:1]");

    const auto N = code & 0b00000'0000011;
    const auto A1 = regs.getA1() & 0x1F;
    regs.setH(A1);
    regs.setL(N);
}

void
PD777::execMoveA2toH(const u16 pc, const u16 code)
{
//    snprintf(mnemonic, sizeof(mnemonic), "A2=>H, N<%d>=>L", N);
//    print(pc, code, mnemonic, u8"Move A2[5:1] to H[5:1], N=>L[2:1]");
    const auto N = code & 0b00000'0000011;
    const auto A2 = regs.getA2() & 0x1F;
    regs.setH(A2);
    regs.setL(N);
}

void
PD777::execMoveHtoA1(const u16 pc, const u16 code)
{
//    snprintf(mnemonic, sizeof(mnemonic), "H=>A1, N<%d>=>L", N);
//    print(pc, code, mnemonic, u8"Move H[5:1] to A1[5:1], 0=>A1[7:6], N=>L[2:1]");
    const auto N = code & 0b00000'0000011;
    const auto H = regs.getH();
    regs.setA1(H);
    regs.setL(N);
}

void
PD777::execMoveHtoA2(const u16 pc, const u16 code)
{
//    snprintf(mnemonic, sizeof(mnemonic), "H=>A2, N<%d>=>L", N);
//    print(pc, code, mnemonic, u8"Move H[5:1] to A2[5:1], 0=>A1[7:6], N=>L[2:1]");

    const auto N = code & 0b00000'0000011;
    const auto H = regs.getH();
    regs.setA2(H);
    regs.setL(N);
}




void PD777::execAndHA1toH(const u16 pc, const u16 code)
{
//    snprintf(mnemonic, sizeof(mnemonic), "H&A1=>H, N<%d>=>L", N);
//    print(pc, code, mnemonic, u8"AND H[5:1] and A1[5:1], store to H[5:1], N=>L[2:1]");
    const auto N  = code & 0b00000'0000011;
    const auto H  = regs.getH();
    const auto A1 = regs.getA1() & 0x1F;
    regs.setH(H & A1);
    regs.setL(N);
}
void PD777::execAddHA1toH(const u16 pc, const u16 code)
{
//    snprintf(mnemonic, sizeof(mnemonic), "H+A1=>H, N<%d>=>L", N);
//    print(pc, code, mnemonic, u8"Add H[5:1] and A1[5:1], store to H[5:1], N=>L[2:1]", u8"CAJ");
    const auto N  = code & 0b00000'0000011;
    const auto H  = regs.getH();
    const auto A1 = regs.getA1() & 0x1F;
    regs.setH(H + A1);
    regs.setL(N);
}
void PD777::execOrHA1toH(const u16 pc, const u16 code)
{
//    snprintf(mnemonic, sizeof(mnemonic), "H|A1=>H, N<%d>=>L", N);
//    print(pc, code, mnemonic, u8"OR H[5:1] and A1[5:1], store to H[5:1], N=>L[2:1]");
    const auto N  = code & 0b00000'0000011;
    const auto H  = regs.getH();
    const auto A1 = regs.getA1() & 0x1F;
    regs.setH(H | A1);
    regs.setL(N);
}
void PD777::execSubHA1toH(const u16 pc, const u16 code)
{
//    snprintf(mnemonic, sizeof(mnemonic), "H-A1=>H, N<%d>=>L", N);
//    print(pc, code, mnemonic, u8"Subtract H[5:1] and A1[5:1], store to H[5:1], N=>L[2:1], Skip if borrow", u8"BOJ");
    const auto N  = code & 0b00000'0000011;
    const auto H  = regs.getH();
    const auto A1 = regs.getA1() & 0x1F;
    regs.setH(H - A1);
    regs.setSkip(H < A1);
    regs.setL(N);
}
void PD777::execAndHA2toH(const u16 pc, const u16 code)
{
//    snprintf(mnemonic, sizeof(mnemonic), "H&A2=>H, N<%d>=>L", N);
//    print(pc, code, mnemonic, u8"AND H[5:1] and A2[5:1], store to H[5:1], N=>L[2:1]");

    const auto N  = code & 0b00000'0000011;
    const auto H  = regs.getH();
    const auto A2 = regs.getA2() & 0x1F;
    regs.setH(H & A2);
    regs.setL(N);
}
void PD777::execAddHA2toH(const u16 pc, const u16 code)
{
//    snprintf(mnemonic, sizeof(mnemonic), "H+A2=>H, N<%d>=>L", N);
//    print(pc, code, mnemonic, u8"Add H[5:1] and A2[5:1], store to H[5:1], N=>L[2:1]", u8"CAJ");

    const auto N  = code & 0b00000'0000011;
    const auto H  = regs.getH();
    const auto A2 = regs.getA2() & 0x1F;
    regs.setH(H + A2);
    regs.setL(N);
}
void PD777::execOrHA2toH(const u16 pc, const u16 code)
{
//    snprintf(mnemonic, sizeof(mnemonic), "H|A2=>H, N<%d>=>L", N);
//    print(pc, code, mnemonic, u8"OR H[5:1] and A2[5:1], store to H[5:1], N=>L[2:1]");

    const auto N  = code & 0b00000'0000011;
    const auto H  = regs.getH();
    const auto A2 = regs.getA2() & 0x1F;
    regs.setH(H | A2);
    regs.setL(N);
}
void PD777::execSubHA2toH(const u16 pc, const u16 code)
{
//    snprintf(mnemonic, sizeof(mnemonic), "H-A2=>H, N<%d>=>L", N);
//    print(pc, code, mnemonic, u8"Subtract H[5:1] and A2[5:1], store to H[5:1], N=>L[2:1], Skip if borrow", u8"BOJ");

    const auto N  = code & 0b00000'0000011;
    const auto H  = regs.getH();
    const auto A2 = regs.getA2() & 0x1F;
    regs.setH(H - A2);
    regs.setSkip(H < A2);
    regs.setL(N);
}

void
PD777::execSetA11(const u16 pc, const u16 code)
{
//    snprintf(mnemonic, sizeof(mnemonic), "N<%d>=>A11", N);
//    print(pc, code, mnemonic, u8"N=>A[11]");

    //mnemonic = "N=>A11";
    const auto N = (code & 0b00000'000000'1) != 0;
    regs.setA11(N);
}

void
PD777::execJPM(const u16 pc, const u16 code)
{
//    snprintf(mnemonic, sizeof(mnemonic), "JPM, 0=>L, N<%d>=>A11", N);
//    print(pc, code, mnemonic, u8"Jump to (000,M[H[5:1],L[2:1]][5:1],1N), 0=>L[2:1], N=>A[11]");
    const auto N = code & 0b00000'0000001;
    const u16 M = readMemAtHL();
    const u16 jumpAddress = ((M & 0x1F) << 2) | (0b10) | N;
    regs.setPC(jumpAddress);
    regs.setL(0);
    regs.setA11(N);
}

void
PD777::execSetFlags(const u16 pc, const u16 code)
{
    const auto N =  code & 0b00000'000000'1;
    const auto D = (code & 0b00000'010000'0) ? 1 : 0;
    const auto G = (code & 0b00000'001000'0) ? 1 : 0;
    const auto K = (code & 0b00000'000100'0) ? 1 : 0;
    const auto S = (code & 0b00000'000010'0) ? 1 : 0;
    regs.setDISP(D);
    regs.setGPE(G);
    regs.setKIE(K);
    regs.setSME(S);
    regs.setA11(N);
}

void
PD777::execSubHKtoH(const u16 pc, const u16 code)
{
//    snprintf(mnemonic, sizeof(mnemonic), "H-K<$%02X>=>H", K);
//    print(pc, code, mnemonic, u8"H[5:1]-K[5:1]=>H[5:1], Skip if borrow", u8"BOJ");

    // judge "BOJ"
    // H-K=>H
    // H[5:1]-K[5:1]=>H[5:1], Skip if borrow
    const u8 K = code & 0x1F;
    const u8 H = regs.getH();
    regs.setSkip(H < K);
    regs.setH(H - K);
}

void
PD777::execAddHKtoH(const u16 pc, const u16 code)
{
//    snprintf(mnemonic, sizeof(mnemonic), "H+K<$%02X>=>H", K);
//    print(pc, code, mnemonic, u8"H[5:1]+K[5:1]=>H[5:1], Skip if carry", u8"CAJ");

    // judge "CAJ"
    // H+K=>H
    // H[5:1]+K[5:1]=>H[5:1], Skip if carry
    const u8 K = code & 0x1F;
    const u8 H = regs.getH();



#if PATCH_BattleVader_ScoreSpriteSetting
/*
    得点をスプライトに反映させるときに、キャリーによるスキップを行うと変な挙動を取る。
    初回の「21F 4C1 CAJ  H+K<$01> => H」でスキップをするとA1が不定になる。

// @param A2: 数字のパターン番号
Entry207: ; caller 3F3
207 59F      K<$1F> => L<0>,H<$1F>              ; Store K[7:6] to L[2:1] and K[5:1] to H[5:1]
Label20F: ; from 201
20F 300      N<0> => L                          ; N => L[2:1]
21F 4C1 CAJ  H+K<$01> => H                      ; H[5:1]+K[5:1] => H[5:1], Skip if carry
23F 605      K<$05> => A1                       ; Store K[7:1] to A1[7:1]
27E 28A EQJ  M=A1, N<2> => L                    ; Skip if (M[H[5:1],L[2:1]][7:1]-A1[7:1]) makes zero, N => L[2:1]
27D A0F      JP $20F                            ; Move K[10:1] to A[10:1], Jump to A[11:1]
27B 392      A2 => M, N<2> => L                 ; Move A2[7:1] to M[H[5:1],L[2:1]][7:1], N => L[2:1]
277 058      MA => A                            ; Move M[H[5:1]][28:1] to (A4[7:1],A3[7:1],A2[7:1],A1[7:1])
*/


    //regs.setSkip((H + K) > 0x1F);
    regs.setH(H + K);
#else
    regs.setSkip((H + K) > 0x1F);
    regs.setH(H + K);
#endif
}

void
PD777::execMoveKtoM(const u16 pc, const u16 code)
{
//    snprintf(mnemonic, sizeof(mnemonic), "K<$%02X>=>M", K);
//    print(pc, code, mnemonic, u8"When (KIE=0)&(SME=0), Store K[7:1] to M[H[5:1],L[2:1]][7:1] / When (KIE=1), Store KIN[7:1] to M[H[5:1],L[2:1]][7:1] / When (SME=1), Store HCL[7:1] to M[H[5:1],L[2:1]][7:1]");

    // K=>M
    // When (KIE=0)&(SME=0), Store K[7:1] to M[H[5:1],L[2:1]][7:1]
    // When (KIE=1), Store KIN[7:1] to M[H[5:1],L[2:1]][7:1]
    // When (SME=1), Store HCL[7:1] to M[H[5:1],L[2:1]][7:1]
    if(!regs.getKIE() && !regs.getSME()) {
        const auto K = code & 0b00000'1111111;
        writeMemAtHL(K);
    } else if(regs.getKIE()) {
        const auto KIN = readKIN();
        writeMemAtHL(KIN);
    } else if(regs.getSME()) {
        const auto HCL = readHCL();
        writeMemAtHL(HCL);
    }
}

void
PD777::execMoveKtoLH(const u16 pc, const u16 code)
{
//    snprintf(mnemonic, sizeof(mnemonic), "K<$%02X>=>L<%d>,H<$%02X>", K, L, H);
//    print(pc, code, mnemonic, u8"Store K[7:6] to L[2:1] and K[5:1] to H[5:1]");

    // K=>L,H
    // Store K[7:6] to L[2:1] and K[5:1] to H[5:1]
    const auto K = code & 0b00000'1111111;
    const auto L = (K >> 5) & 0x3;
    const auto H =  K       & 0x1F;
    regs.setL(L);
    regs.setH(H);
}

void
PD777::execMoveKtoA1(const u16 pc, const u16 code)
{
//    snprintf(mnemonic, sizeof(mnemonic), "K<$%02X>=>A1", K);
//    print(pc, code, mnemonic, u8"Store K[7:1] to A1[7:1]");

    // K=>A1
    // Store K[7:1] to A1[7:1]
    const auto K = code & 0b00000'1111111;
    regs.setA1(K);
}

void
PD777::execMoveKtoA2(const u16 pc, const u16 code)
{
//    snprintf(mnemonic, sizeof(mnemonic), "K<$%02X>=>A2", K);
//    print(pc, code, mnemonic, u8"Store K[7:1] to A2[7:1]");

    // K=>A2
    // Store K[7:1] to A2[7:1]
    const auto K = code & 0b00000'1111111;
    regs.setA2(K);
}

void
PD777::execMoveKtoA3(const u16 pc, const u16 code)
{
//    snprintf(mnemonic, sizeof(mnemonic), "K<$%02X>=>A3", K);
//    print(pc, code, mnemonic, u8"Store K[7:1] to A3[7:1]");

    // K=>A3
    // Store K[7:1] to A3[7:1]
    const auto K = code & 0b00000'1111111;
    regs.setA3(K);
}

void
PD777::execMoveKtoA4(const u16 pc, const u16 code)
{
//    snprintf(mnemonic, sizeof(mnemonic), "K<$%02X>=>A4", K);
//    print(pc, code, mnemonic, u8"Store K[7:1] to A4[7:1]");

    // K=>A4
    // Store K[7:1] to A4[7:1]
    const auto K = code & 0b00000'1111111;
    regs.setA4(K);
}

void
PD777::execJP(const u16 pc, const u16 code)
{
//    snprintf(mnemonic, sizeof(mnemonic), "JP $%03X", K);
//    print(pc, code, mnemonic, u8"Move K[10:1] to A[10:1], Jump to A[11:1]");

    const u16 K = code & 0x3FF;
    const u16 jumpAddress = (regs.getPC() & ~0x3FF) | K;
    regs.setPC(jumpAddress);
}

void
PD777::execJS(const u16 pc, const u16 code)
{
//    snprintf(mnemonic, sizeof(mnemonic), "JS $%03X", K);
//    print(pc, code, mnemonic, u8"Move K[10:1] to A[10:1], 0 to A11, Jump to A[11:1], Push next A[11:1] up to ROM address stack");

    const auto nextAddress = regs.getPC();
    const u16 K = code & 0x3FF;
    const u16 jumpAddress = (regs.getPC() & ~0x3FF) | K;
    regs.setPC(jumpAddress);
    regs.setA11(0);
    stack.stackPush(nextAddress);
}







void
PD777::setupRom()
{
    for(auto& r : rom) { r = ~0; }
    for(size_t i = 0; i < sizeof(rawRom) / sizeof(rawRom[0]); i += 2) {
        const auto address = rawRom[i + 0]; // アドレス
        const auto code    = rawRom[i + 1]; // データ
        rom[address] = code;
    }
}

void
PD777::init()
{
    setupRom();
    crt.reset();
    stack.reset();
    regs.reset();
}

void
PD777::term()
{
}

void PD777::execute()
{
    if(!regs.isSkip()) [[likely]] {
        // fetch
        auto address = regs.getPC();
        auto code = rom[address];
        regs.nextPC();

        // for debug
        //registerDump();
        //disassemble(address, code);

        // execute
        Decoder::execute(address, code);
    } else {
        regs.resetSkip();
        regs.nextPC();
    }

    // @todo タイミングなどをちゃんとする必要がある
    if(crtCounter++ == 22) {
        crtCounter = 0;
        const auto prevVBLK = crt.isVBLK();
        crt.step();
        if(prevVBLK && !crt.isVBLK()) [[unlikely]] {
            // 新しいフレーム

            // イメージ作成
            makePresentImage();
            // 作成したイメージを画面に出力する
            present();
        }
    }
}

u8
PD777::readMem(const u16 address)
{
    return ram[address & 0x7F] & 0x7F;
}

void
PD777::writeMem(const u16 address, const u8 value)
{
    ram[address & 0x7F] = value & 0x7F;
}

void
PD777::writeMemAtHL(const u8 value)
{
    const auto address = regs.getAddressHL();
    writeMem(address, value);
}

u8
PD777::readMemAtHL()
{
    const auto address = regs.getAddressHL();
    return readMem(address);
}


void
PD777::setMode(const u8 mode)
{
//    printf("set Mode $%02X\n", mode);
    regs.setMode(mode);
}

void
PD777::makePresentImage()
{
    // @todo
    const auto modeRegister = regs.getMode();
    auto modeBrightness = modeRegister & 0x20; // BRIGHTNESS
    auto modeHUE        = modeRegister & 0x10; // HUE
    auto bgBlackPrio    = modeRegister & 0x08; // BLACK/PRIO
    auto bgRGB          = modeRegister & 0x07; // RGB
    //u8 bgColor = bgRGB | (modeBrightness | modeHUE | bgBlackPrio); // RGB
    u8 bgColor = bgRGB; // RGB

    //const u8 forgroundColor = (modeBrightness | modeHUE | bgBlackPrio) << 1;
    const u8 forgroundColor = 0;

    for(auto& m : frameBuffer) { m = bgColor; }
    for(int index = 0; index <= 0x18; index++) {
        const auto data0 = ram[index * 4 + 0];
        const auto data1 = ram[index * 4 + 1];
        const auto data2 = ram[index * 4 + 2];
        const auto data3 = ram[index * 4 + 3];

        auto prio = data0 & 1;
        u32 y     = data0 >> 1;
        u32 x     = data1;
        u16 ptn  = data2;
        auto y3   = data3 >> 4;
        auto rgb  = ((data3 >> 1) & 7) | (prio << 3) | forgroundColor;

        if(ptn < 0x70) {
            auto base = (ptn - (ptn / 8)) * 7;
            for(int line = 0; line < 7; ++line) {
                const auto pattern = patternRom[base + line];
                if(pattern & 0x40) frameBuffer[x + 0 + (y + line)*frameBufferWidth] = rgb;
                if(pattern & 0x20) frameBuffer[x + 1 + (y + line)*frameBufferWidth] = rgb;
                if(pattern & 0x10) frameBuffer[x + 2 + (y + line)*frameBufferWidth] = rgb;
                if(pattern & 0x08) frameBuffer[x + 3 + (y + line)*frameBufferWidth] = rgb;
                if(pattern & 0x04) frameBuffer[x + 4 + (y + line)*frameBufferWidth] = rgb;
                if(pattern & 0x02) frameBuffer[x + 5 + (y + line)*frameBufferWidth] = rgb;
                if(pattern & 0x01) frameBuffer[x + 6 + (y + line)*frameBufferWidth] = rgb;
            }
        } else {
            ptn -= 0x70;
            auto base = (ptn - (ptn / 8)) * 7;
            for(int line = 0; line < 7; ++line) {
                const auto pattern = patternRom8[base + line];
                if(pattern & 0x80) frameBuffer[x + 0 + (y + line)*frameBufferWidth] = rgb;
                if(pattern & 0x40) frameBuffer[x + 1 + (y + line)*frameBufferWidth] = rgb;
                if(pattern & 0x20) frameBuffer[x + 2 + (y + line)*frameBufferWidth] = rgb;
                if(pattern & 0x10) frameBuffer[x + 3 + (y + line)*frameBufferWidth] = rgb;
                if(pattern & 0x08) frameBuffer[x + 4 + (y + line)*frameBufferWidth] = rgb;
                if(pattern & 0x04) frameBuffer[x + 5 + (y + line)*frameBufferWidth] = rgb;
                if(pattern & 0x02) frameBuffer[x + 6 + (y + line)*frameBufferWidth] = rgb;
                if(pattern & 0x01) frameBuffer[x + 7 + (y + line)*frameBufferWidth] = rgb;
            }
        }
    }
}

void
PD777::disassemble(const u16 pc, const u16 code)
{
#if defined(_WIN32)
    if(disassembler.pass != 0) {
        if(auto flags = disassembler.findAddress(pc); flags) {
            const auto caller = disassembler.findAddressCaller(pc);
            if(flags & 1) {
                printf("Label%03X: ; from %03X\n", pc, caller);
            } else {
                printf("\nEntry%03X: ; caller %03X \n", pc, caller);
            }
        }
    }
    disassembler.execute(pc, code);
#endif // defined(_WIN32)
}

void
PD777::disassembleAll()
{
#if defined(_WIN32)
    disassembler.pass = 0;
    for(u16 j = 0; j < 16; j++) {
        u16 pc = 0x80 * j;
        for(u8 i = 0; i < 127; i++) {
            disassemble(pc, rom[pc]);
            pc = regs.nextPCAddress(pc);
        }
    }

    disassembler.pass = 1;
    for(u16 j = 0; j < 16; j++) {
        u16 pc = 0x80 * j;
        for(u8 i = 0; i < 127; i++) {
            disassemble(pc, rom[pc]);
            pc = regs.nextPCAddress(pc);
        }
    }
#endif
}
