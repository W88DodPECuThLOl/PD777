#include "Disassembler.h"

#if defined(_WIN32)
#include <string>

void
Disassembler::print(const u16 pc, const u16 code, const c8* mnemonic, const c8* function, const c8* judge)
{
    if(pass != 1) {
        return;
    }

    if(function == nullptr) {
        printf("%03X %03X %-4s %-32s\n", pc, code, (const char*)judge, (const char*)mnemonic);
    } else {
        printf("%03X %03X %-4s %-32s \t; %s\n", pc, code, (const char*)judge, (const char*)mnemonic, (const char*)function);
    }
}

void
Disassembler::execUnknown(const u16 pc, const u16 code)
{
    print(pc, code, u8"???");
}

void
Disassembler::execNOP(const u16 pc, const u16 code)
{
    print(pc, code, u8"NOP");
}

void
Disassembler::execGPL(const u16 pc, const u16 code)
{
    print(pc, code, u8"GPL", u8"Skip if (Gun Port Latch) = 1", u8"J");
}

void
Disassembler::execMoveHtoNRM(const u16 pc, const u16 code)
{
    print(pc, code, u8"H => NRM", u8"Move H[5:1] to Line Buffer Register[5:1]");
}

void
Disassembler::execExchangeHandX(const u16 pc, const u16 code)
{
    print(pc, code, u8"H <=> X", u8"H[5:1] <=> X4[5:1], 0 => X4[7:6], 0 => X3[7:1], 0 => X1'[1], 0 => A1'[1], L[2:1] <=> L'[2:1]");
}

void
Disassembler::execSRE(const u16 pc, const u16 code)
{
    print(pc, code, u8"SRE", u8"Subroutine End, Pop down address stack\n\n");
}

void
Disassembler::execSTB(const u16 pc, const u16 code)
{
    u16 N = code & 0b000000000001;
    snprintf((char*)mnemonic, sizeof(mnemonic), "$%d => STB", N);
    print(pc, code, mnemonic, u8"Shift STB[4:1], N => STB[1]");
}

void
Disassembler::exec4H_BLK(const u16 pc, const u16 code)
{
    print(pc, code, u8"4H BLK", u8"Skip if (4H Horizontal Blank) = 1", u8"J");
}

void
Disassembler::execVBLK(const u16 pc, const u16 code)
{
    print(pc, code, u8"VBLK", u8"Skip if (Vertical Blank) = 1, 0 => M[[18:00],[3]][1]", u8"J");
}

void
Disassembler::execGPSW_(const u16 pc, const u16 code)
{
    print(pc, code, u8"GPSW/", u8"Skip if (GP&SW/ input) = 1", u8"J");
}

void
Disassembler::execMoveAtoMA(const u16 pc, const u16 code)
{
    print(pc, code, u8"A => MA", u8"Move (A4[7:1],A3[7:1],A2[7:1],A1[7:1]) to M[H[5:1]][28:1]");
}

void
Disassembler::execMoveMAtoA(const u16 pc, const u16 code)
{
    print(pc, code, u8"MA => A", u8"Move M[H[5:1]][28:1] to (A4[7:1],A3[7:1],A2[7:1],A1[7:1])");
}

void
Disassembler::execExchangeMAandA(const u16 pc, const u16 code)
{
    print(pc, code, u8"MA <=> A", u8"Exchange (A4[7:1],A3[7:1],A2[7:1],A1[7:1]) and M[H[5:1]][28:1]");
}

void
Disassembler::execSRE_1(const u16 pc, const u16 code)
{
    print(pc, code, u8"SRE+1", u8"Subroutine End, Pop down address stack, Skip\n\n");
}

void
Disassembler::execPD1_J(const u16 pc, const u16 code)
{
    print(pc, code, u8"PD1", u8"Skip if (PD1 input) = 1", u8"J");
}

void
Disassembler::execPD2_J(const u16 pc, const u16 code)
{
    print(pc, code, u8"PD2", u8"Skip if (PD2 input) = 1", u8"J");
}

void
Disassembler::execPD3_J(const u16 pc, const u16 code)
{
    print(pc, code, u8"PD3", u8"Skip if (PD3 input) = 1", u8"J");
}

void
Disassembler::execPD4_J(const u16 pc, const u16 code)
{
    print(pc, code, u8"PD4", u8"Skip if (PD4 input) = 1", u8"J");
}

void
Disassembler::execPD1_NJ(const u16 pc, const u16 code)
{
    print(pc, code, u8"PD1", u8"Skip if (PD1 input) = 0", u8"J/");
}

void
Disassembler::execPD2_NJ(const u16 pc, const u16 code)
{
    print(pc, code, u8"PD2", u8"Skip if (PD2 input) = 0", u8"J/");
}

void
Disassembler::execPD3_NJ(const u16 pc, const u16 code)
{
    print(pc, code, u8"PD3", u8"Skip if (PD3 input) = 0", u8"J/");
}

void
Disassembler::execPD4_NJ(const u16 pc, const u16 code)
{
    print(pc, code, u8"PD4", u8"Skip if (PD4 input) = 0", u8"J/");
}

void
Disassembler::execTestSubMK(const u16 pc, const u16 code)
{
    const auto K = code & 0b00000'1111111;
    snprintf((char*)mnemonic, sizeof(mnemonic), "M-$%02X", K);
    print(pc, code, mnemonic, u8"Skip if (M[H[5:1],L[2:1]][7:1]-K[7:1]) makes borrow", u8"BOJ");
}

void
Disassembler::execAddMKM(const u16 pc, const u16 code)
{
    const auto N = (code & 0b00000'1100000) >> 5;
    const auto K =  code & 0b00000'0011111;
    snprintf((char*)mnemonic, sizeof(mnemonic), "M+K$%02X => M, $%d => L", K, N);
    print(pc, code, mnemonic, u8"M[H[5:1],L[2:1]][7:1]+K[7:1] => M[H[5:1],L[2:1]][7:1], Skip if carry, N => L[2:1]", u8"CAJ");
}

void
Disassembler::execSubMKM(const u16 pc, const u16 code)
{
    const auto N = (code & 0b00000'1100000) >> 5;
    const auto K =  code & 0b00000'0011111;
    snprintf((char*)mnemonic, sizeof(mnemonic), "M-$%02X => M, $%d => L", K, N);
    print(pc, code, mnemonic, u8"M[H[5:1],L[2:1]][7:1]-K[7:1] => M[H[5:1],L[2:1]][7:1], Skip if borrow, N => L[2:1]", u8"BOJ");
}

void
Disassembler::execTestAndA1A1_EQJ(const u16 pc, const u16 code)
{
    const auto N = code & 0b00000'0000011;
    snprintf((char*)mnemonic, sizeof(mnemonic), "A1&A1, $%d => L", N);
    print(pc, code, mnemonic, u8"Skip if (A1[7:1]&A1[7:1]) makes zero, N => L[2:1]", u8"EQJ");
}

void
Disassembler::execTestAndA1A1_NOT_EQJ(const u16 pc, const u16 code)
{
    const auto N = code & 0b00000'0000011;
    snprintf((char*)mnemonic, sizeof(mnemonic), "A1&A1, $%d => L", N);
    print(pc, code, mnemonic, u8"Skip if (A1[7:1]&A1[7:1]) makes non zero, N => L[2:1]", u8"EQJ/");
}

void
Disassembler::execTestEquA1A1_EQJ(const u16 pc, const u16 code)
{
    const auto N = code & 0b00000'0000011;
    snprintf((char*)mnemonic, sizeof(mnemonic), "A1=A1, $%d => L", N);
    print(pc, code, mnemonic, u8"Skip if (A1[7:1]-A1[7:1]) makes zero, N => L[2:1]", u8"EQJ");
}

void
Disassembler::execTestEquA1A1_NOT_EQJ(const u16 pc, const u16 code)
{
    const auto N = code & 0b00000'0000011;
    snprintf((char*)mnemonic, sizeof(mnemonic), "A1=A1, $%d => L", N);
    print(pc, code, mnemonic, u8"Skip if (A1[7:1]-A1[7:1]) makes non zero, N => L[2:1]", u8"EQJ/");
}

void
Disassembler::execTestSubA1A1_BOJ(const u16 pc, const u16 code)
{
    const auto N = code & 0b00000'0000011;
    snprintf((char*)mnemonic, sizeof(mnemonic), "A1-A1, $%d => L", N);
    print(pc, code, mnemonic, u8"Skip if (A1[7:1]-A1[7:1]) makes borrow, N => L[2:1]", u8"BOJ");
}

void
Disassembler::execTestSubA1A1_NOT_BOJ(const u16 pc, const u16 code)
{
    const auto N = code & 0b00000'0000011;
    snprintf((char*)mnemonic, sizeof(mnemonic), "A1-A1, $%d => L", N);
    print(pc, code, mnemonic, u8"Skip if (A1[7:1]-A1[7:1]) makes non borrow, N => L[2:1]", u8"BOJ/");
}

void
Disassembler::execTestAndA1A2_EQJ(const u16 pc, const u16 code)
{
    const auto N = code & 0b00000'0000011;
    snprintf((char*)mnemonic, sizeof(mnemonic), "A1&A2, $%d => L", N);
    print(pc, code, mnemonic, u8"Skip if (A1[7:1]&A2[7:1]) makes zero, N => L[2:1]", u8"EQJ");
}

void
Disassembler::execTestAndA1A2_NOT_EQJ(const u16 pc, const u16 code)
{
    const auto N = code & 0b00000'0000011;
    snprintf((char*)mnemonic, sizeof(mnemonic), "A1&A2, $%d => L", N);
    print(pc, code, mnemonic, u8"Skip if (A1[7:1]&A2[7:1]) makes non zero, N => L[2:1]", u8"EQJ/");
}

void
Disassembler::execTestEquA1A2_EQJ(const u16 pc, const u16 code)
{
    const auto N = code & 0b00000'0000011;
    snprintf((char*)mnemonic, sizeof(mnemonic), "A1=A2, $%d => L", N);
    print(pc, code, mnemonic, u8"Skip if (A1[7:1]-A2[7:1]) makes zero, N => L[2:1]", u8"EQJ");
}

void
Disassembler::execTestEquA1A2_NOT_EQJ(const u16 pc, const u16 code)
{
    const auto N = code & 0b00000'0000011;
    snprintf((char*)mnemonic, sizeof(mnemonic), "A1=A2, $%d => L", N);
    print(pc, code, mnemonic, u8"Skip if (A1[7:1]-A2[7:1]) makes non zero, N => L[2:1]", u8"EQJ/");
}

void
Disassembler::execTestSubA1A2_BOJ(const u16 pc, const u16 code)
{
    const auto N = code & 0b00000'0000011;
    snprintf((char*)mnemonic, sizeof(mnemonic), "A1-A2, $%d => L", N);
    print(pc, code, mnemonic, u8"Skip if (A1[7:1]-A2[7:1]) makes borrow, N => L[2:1]", u8"BOJ");
}

void
Disassembler::execTestSubA1A2_NOT_BOJ(const u16 pc, const u16 code)
{
    const auto N = code & 0b00000'0000011;
    snprintf((char*)mnemonic, sizeof(mnemonic), "A1-A2, $%d => L", N);
    print(pc, code, mnemonic, u8"Skip if (A1[7:1]-A2[7:1]) makes non borrow, N => L[2:1]", u8"BOJ/");
}

void
Disassembler::execTestAndA2A1_EQJ(const u16 pc, const u16 code)
{
    const auto N = code & 0b00000'0000011;
    snprintf((char*)mnemonic, sizeof(mnemonic), "A2&A1, $%d => L", N);
    print(pc, code, mnemonic, u8"Skip if (A2[7:1]&A1[7:1]) makes zero, N => L[2:1]", u8"EQJ");
}

void
Disassembler::execTestAndA2A1_NOT_EQJ(const u16 pc, const u16 code)
{
    const auto N = code & 0b00000'0000011;
    snprintf((char*)mnemonic, sizeof(mnemonic), "A2&A1, $%d => L", N);
    print(pc, code, mnemonic, u8"Skip if (A2[7:1]&A1[7:1]) makes non zero, N => L[2:1]", u8"EQJ/");
}

void
Disassembler::execTestEquA2A1_EQJ(const u16 pc, const u16 code)
{
    const auto N = code & 0b00000'0000011;
    snprintf((char*)mnemonic, sizeof(mnemonic), "A2=A1, $%d => L", N);
    print(pc, code, mnemonic, u8"Skip if (A2[7:1]-A1[7:1]) makes zero, N => L[2:1]", u8"EQJ");
}

void
Disassembler::execTestEquA2A1_NOT_EQJ(const u16 pc, const u16 code)
{
    const auto N = code & 0b00000'0000011;
    snprintf((char*)mnemonic, sizeof(mnemonic), "A2=A1, $%d => L", N);
    print(pc, code, mnemonic, u8"Skip if (A2[7:1]-A1[7:1]) makes non zero, N => L[2:1]", u8"EQJ/");
}

void
Disassembler::execTestSubA2A1_BOJ(const u16 pc, const u16 code)
{
    const auto N = code & 0b00000'0000011;
    snprintf((char*)mnemonic, sizeof(mnemonic), "A2-A1, $%d => L", N);
    print(pc, code, mnemonic, u8"Skip if (A2[7:1]-A1[7:1]) makes borrow, N => L[2:1]", u8"BOJ");
}

void
Disassembler::execTestSubA2A1_NOT_BOJ(const u16 pc, const u16 code)
{
    const auto N = code & 0b00000'0000011;
    snprintf((char*)mnemonic, sizeof(mnemonic), "A2-A1, $%d => L", N);
    print(pc, code, mnemonic, u8"Skip if (A2[7:1]-A1[7:1]) makes non borrow, N => L[2:1]", u8"BOJ/");
}

void Disassembler::execTestAndA2A2_EQJ(const u16 pc, const u16 code)
{
    const auto N = code & 0b00000'0000011;
    snprintf((char*)mnemonic, sizeof(mnemonic), "A2&A2, $%d => L", N);
    print(pc, code, mnemonic, u8"Skip if (A2[7:1]&A2[7:1]) makes zero, N => L[2:1]", u8"EQJ");
}

void Disassembler::execTestAndA2A2_NOT_EQJ(const u16 pc, const u16 code)
{
    const auto N = code & 0b00000'0000011;
    snprintf((char*)mnemonic, sizeof(mnemonic), "A2&A2, $%d => L", N);
    print(pc, code, mnemonic, u8"Skip if (A2[7:1]&A2[7:1]) makes non zero, N => L[2:1]", u8"EQJ/");
}

void
Disassembler::execTestEquA2A2_EQJ(const u16 pc, const u16 code)
{
    const auto N = code & 0b00000'0000011;
    snprintf((char*)mnemonic, sizeof(mnemonic), "A2=A2, $%d => L", N);
    print(pc, code, mnemonic, u8"Skip if (A2[7:1]-A2[7:1]) makes zero, N => L[2:1]", u8"EQJ");
}

void
Disassembler::execTestEquA2A2_NOT_EQJ(const u16 pc, const u16 code)
{
    const auto N = code & 0b00000'0000011;
    snprintf((char*)mnemonic, sizeof(mnemonic), "A2=A2, $%d => L", N);
    print(pc, code, mnemonic, u8"Skip if (A2[7:1]-A2[7:1]) makes zero, N => L[2:1]", u8"EQJ/");
}

void
Disassembler::execTestSubA2A2_BOJ(const u16 pc, const u16 code)
{
    const auto N = code & 0b00000'0000011;
    snprintf((char*)mnemonic, sizeof(mnemonic), "A2-A2, $%d => L", N);
    print(pc, code, mnemonic, u8"Skip if (A2[7:1]-A2[7:1]) makes borrow, N => L[2:1]", u8"BOJ");
}

void
Disassembler::execTestSubA2A2_NOT_BOJ(const u16 pc, const u16 code)
{
    const auto N = code & 0b00000'0000011;
    snprintf((char*)mnemonic, sizeof(mnemonic), "A2-A2, $%d => L", N);
    print(pc, code, mnemonic, u8"Skip if (A2[7:1]-A2[7:1]) makes non borrow, N => L[2:1]", u8"BOJ/");
}

void
Disassembler::execTestAndMA1_EQJ(const u16 pc, const u16 code)
{
    const auto N = code & 0b00000'0000011;
    snprintf((char*)mnemonic, sizeof(mnemonic), "M&A1, $%d => L", N);
    print(pc, code, mnemonic, u8"Skip if (M[H[5:1],L[2:1]][7:1]&A1[7:1]) makes zero, N => L[2:1]", u8"EQJ");
}

void
Disassembler::execTestAndMA1_NOT_EQJ(const u16 pc, const u16 code)
{
    const auto N = code & 0b00000'0000011;
    snprintf((char*)mnemonic, sizeof(mnemonic), "M&A1, $%d => L", N);
    print(pc, code, mnemonic, u8"Skip if (M[H[5:1],L[2:1]][7:1]&A1[7:1]) makes non zero, N => L[2:1]", u8"EQJ/");
}

void
Disassembler::execTestEquMA1_EQJ(const u16 pc, const u16 code)
{
    const auto N = code & 0b00000'0000011;
    snprintf((char*)mnemonic, sizeof(mnemonic), "M=A1, $%d => L", N);
    print(pc, code, mnemonic, u8"Skip if (M[H[5:1],L[2:1]][7:1]-A1[7:1]) makes zero, N => L[2:1]", u8"EQJ");
}

void
Disassembler::execTestEquMA1_NOT_EQJ(const u16 pc, const u16 code)
{
    const auto N = code & 0b00000'0000011;
    snprintf((char*)mnemonic, sizeof(mnemonic), "M=A1, $%d => L", N);
    print(pc, code, mnemonic, u8"Skip if (M[H[5:1],L[2:1]][7:1]-A1[7:1]) makes non zero, N => L[2:1]", u8"EQJ/");
}

void
Disassembler::execTestSubMA1_BOJ(const u16 pc, const u16 code)
{
    const auto N = code & 0b00000'0000011;
    snprintf((char*)mnemonic, sizeof(mnemonic), "M-A1, $%d => L", N);
    print(pc, code, mnemonic, u8"Skip if (M[H[5:1],L[2:1]][7:1]-A1[7:1]) makes borrow, N => L[2:1]", u8"BOJ");
}

void
Disassembler::execTestSubMA1_NOT_BOJ(const u16 pc, const u16 code)
{
    const auto N = code & 0b00000'0000011;
    snprintf((char*)mnemonic, sizeof(mnemonic), "M-A1, $%d => L", N);
    print(pc, code, mnemonic, u8"Skip if (M[H[5:1],L[2:1]][7:1]-A1[7:1]) makes non borrow, N => L[2:1]", u8"BOJ/");
}

void
Disassembler::execTestAndMA2_EQJ(const u16 pc, const u16 code)
{
    const auto N = code & 0b00000'0000011;
    snprintf((char*)mnemonic, sizeof(mnemonic), "M&A2, $%d => L", N);
    print(pc, code, mnemonic, u8"Skip if (M[H[5:1],L[2:1]][7:1]&A2[7:1]) makes zero, N => L[2:1]", u8"EQJ");
}

void
Disassembler::execTestAndMA2_NOT_EQJ(const u16 pc, const u16 code)
{
    const auto N = code & 0b00000'0000011;
    snprintf((char*)mnemonic, sizeof(mnemonic), "M&A2, $%d => L", N);
    print(pc, code, mnemonic, u8"Skip if (M[H[5:1],L[2:1]][7:1]&A2[7:1]) makes non zero, N => L[2:1]", u8"EQJ/");
}

void
Disassembler::execTestEquMA2_EQJ(const u16 pc, const u16 code)
{
    const auto N = code & 0b00000'0000011;
    snprintf((char*)mnemonic, sizeof(mnemonic), "M=A2, $%d => L", N);
    print(pc, code, mnemonic, u8"Skip if (M[H[5:1],L[2:1]][7:1]-A2[7:1]) makes zero, N => L[2:1]", u8"EQJ");
}

void
Disassembler::execTestEquMA2_NOT_EQJ(const u16 pc, const u16 code)
{
    const auto N = code & 0b00000'0000011;
    snprintf((char*)mnemonic, sizeof(mnemonic), "M=A2, $%d => L", N);
    print(pc, code, mnemonic, u8"Skip if (M[H[5:1],L[2:1]][7:1]-A2[7:1]) makes non zero, N => L[2:1]", u8"EQJ/");
}

void
Disassembler::execTestSubMA2_BOJ(const u16 pc, const u16 code)
{
    const auto N = code & 0b00000'0000011;
    snprintf((char*)mnemonic, sizeof(mnemonic), "M-A2, $%d => L", N);
    print(pc, code, mnemonic, u8"Skip if (M[H[5:1],L[2:1]][7:1]-A2[7:1]) makes borrow, N => L[2:1]", u8"BOJ");
}

void
Disassembler::execTestSubMA2_NOT_BOJ(const u16 pc, const u16 code)
{
    const auto N = code & 0b00000'0000011;
    snprintf((char*)mnemonic, sizeof(mnemonic), "M-A2, $%d => L", N);
    print(pc, code, mnemonic, u8"Skip if (M[H[5:1],L[2:1]][7:1]-A2[7:1]) makes non borrow, N => L[2:1]", u8"BOJ/");
}

void
Disassembler::execTestAndHA1_EQJ(const u16 pc, const u16 code)
{
    const auto N = code & 0b00000'0000011;
    snprintf((char*)mnemonic, sizeof(mnemonic), "H&A1, $%d => L", N);
    print(pc, code, mnemonic, u8"Skip if (H[5:1]&A1[5:1]) makes zero, N => L[2:1]", u8"EQJ");
}

void
Disassembler::execTestAndHA1_NOT_EQJ(const u16 pc, const u16 code)
{
    const auto N = code & 0b00000'0000011;
    snprintf((char*)mnemonic, sizeof(mnemonic), "H&A1, $%d => L", N);
    print(pc, code, mnemonic, u8"Skip if (H[5:1]&A1[5:1]) makes non zero, N => L[2:1]", u8"EQJ/");
}

void
Disassembler::execTestEquHA1_EQJ(const u16 pc, const u16 code)
{
    const auto N = code & 0b00000'0000011;
    snprintf((char*)mnemonic, sizeof(mnemonic), "H=A1, $%d => L", N);
    print(pc, code, mnemonic, u8"Skip if (H[5:1]-A1[5:1]) makes zero, N => L[2:1]", u8"EQJ");
}

void
Disassembler::execTestEquHA1_NOT_EQJ(const u16 pc, const u16 code)
{
    const auto N = code & 0b00000'0000011;
    snprintf((char*)mnemonic, sizeof(mnemonic), "H=A1, $%d => L", N);
    print(pc, code, mnemonic, u8"Skip if (H[5:1]-A1[5:1]) makes non zero, N => L[2:1]", u8"EQJ/");
}

void
Disassembler::execTestSubHA1_BOJ(const u16 pc, const u16 code)
{
    const auto N = code & 0b00000'0000011;
    snprintf((char*)mnemonic, sizeof(mnemonic), "H-A1, $%d => L", N);
    print(pc, code, mnemonic, u8"Skip if (H[5:1]-A1[5:1]) makes borrow, N => L[2:1]", u8"BOJ");
}

void
Disassembler::execTestSubHA1_NOT_BOJ(const u16 pc, const u16 code)
{
    const auto N = code & 0b00000'0000011;
    snprintf((char*)mnemonic, sizeof(mnemonic), "H-A1, $%d => L", N);
    print(pc, code, mnemonic, u8"Skip if (H[5:1]-A1[5:1]) makes non borrow, N => L[2:1]", u8"BOJ/");
}

void
Disassembler::execTestAndHA2_EQJ(const u16 pc, const u16 code)
{
    const auto N = code & 0b00000'0000011;
    snprintf((char*)mnemonic, sizeof(mnemonic), "H&A2, $%d => L", N);
    print(pc, code, mnemonic, u8"Skip if (H[5:1]&A2[5:1]) makes zero, N => L[2:1]", u8"EQJ");
}

void
Disassembler::execTestAndHA2_NOT_EQJ(const u16 pc, const u16 code)
{
    const auto N = code & 0b00000'0000011;
    snprintf((char*)mnemonic, sizeof(mnemonic), "H&A2, $%d => L", N);
    print(pc, code, mnemonic, u8"Skip if (H[5:1]&A2[5:1]) makes non zero, N => L[2:1]", u8"EQJ/");
}

void
Disassembler::execTestEquHA2_EQJ(const u16 pc, const u16 code)
{
    const auto N = code & 0b00000'0000011;
    snprintf((char*)mnemonic, sizeof(mnemonic), "H=A2, $%d => L", N);
    print(pc, code, mnemonic, u8"Skip if (H[5:1]-A2[5:1]) makes zero, N => L[2:1]", u8"EQJ");
}

void
Disassembler::execTestEquHA2_NOT_EQJ(const u16 pc, const u16 code)
{
    const auto N = code & 0b00000'0000011;
    snprintf((char*)mnemonic, sizeof(mnemonic), "H=A2, $%d => L", N);
    print(pc, code, mnemonic, u8"Skip if (H[5:1]-A2[5:1]) makes non zero, N => L[2:1]", u8"EQJ/");
}

void
Disassembler::execTestSubHA2_BOJ(const u16 pc, const u16 code)
{
    const auto N = code & 0b00000'0000011;
    snprintf((char*)mnemonic, sizeof(mnemonic), "H-A2, $%d => L", N);
    print(pc, code, mnemonic, u8"Skip if (H[5:1]-A2[5:1]) makes borrow, N => L[2:1]", u8"BOJ");
}

void
Disassembler::execTestSubHA2_NOT_BOJ(const u16 pc, const u16 code)
{
    const auto N = code & 0b00000'0000011;
    snprintf((char*)mnemonic, sizeof(mnemonic), "H-A2, $%d => L", N);
    print(pc, code, mnemonic, u8"Skip if (H[5:1]-A2[5:1]) makes non borrow, N => L[2:1]", u8"BOJ/");
}

void
Disassembler::execMoveNtoL(const u16 pc, const u16 code)
{
    const auto N = code & 0b00000'0000011;
    snprintf((char*)mnemonic, sizeof(mnemonic), "$%d => L", N);
    print(pc, code, mnemonic, u8"N => L[2:1]");
}

void
Disassembler::execMoveA2toA1(const u16 pc, const u16 code)
{
    const auto N = code & 0b00000'0000011;
    snprintf((char*)mnemonic, sizeof(mnemonic), "A2 => A1, $%d => L", N);
    print(pc, code, mnemonic, u8"Move A2[7:1] to A1[7:1], N => L[2:1]");
}

void
Disassembler::execSubA1andA2toA2(const u16 pc, const u16 code)
{
    const auto N = code & 0b00000'0000011;
    snprintf((char*)mnemonic, sizeof(mnemonic), "A1-A2 => A2, $%d => L", N);
    print(pc, code, mnemonic, u8"Subtract A1[7:1] and A2[7:1], store to A2[7:1], Skip if borrow, N => L[2:1]", u8"BOJ");
}

void
Disassembler::execMoveA1toFLS(const u16 pc, const u16 code)
{
    print(pc, code, u8"A1 => FLS, $0 => L", u8"Move A1[7:1] to FLS[7:1], 0 => L[2:1]");
}

void
Disassembler::execMoveA1toFRS(const u16 pc, const u16 code)
{
    print(pc, code, u8"A1 => FRS, $1 => L", u8"Move A1[7:1] to FRS[7:1], 1 => L[2:1]");
}

void
Disassembler::execMoveA1toMODE(const u16 pc, const u16 code)
{
    const auto N = code & 0b00000'0000011;
    snprintf((char*)mnemonic, sizeof(mnemonic), "A1 => MODE, $%d => L", N);
    print(pc, code, mnemonic, u8"Move A1[7:1] to MODE[7:1], 1N => L[2:1]");
}

void
Disassembler::execRightShiftA1(const u16 pc, const u16 code)
{
    const auto N = code & 0b00000'0000011;
    snprintf((char*)mnemonic, sizeof(mnemonic), "A1 => RS, $%d => L", N);
    print(pc, code, mnemonic, u8"Right shift A1[7:1], 0 => A1[7], N => L[2:1]");
}


void
Disassembler::execMoveA1toA2(const u16 pc, const u16 code)
{
    const auto N = code & 0b00000'0000011;
    snprintf((char*)mnemonic, sizeof(mnemonic), "A1 => A2, $%d => L", N);
    print(pc, code, mnemonic, u8"Move A1[7:1] to A2[7:1], N => L[2:1]");
}

void
Disassembler::execMoveA2toFLS(const u16 pc, const u16 code)
{
    print(pc, code, u8"A2 => FLS, $0 => L", u8"Move A2[7:1] to FLS[7:1], 0 => L[2:1]");
}

void
Disassembler::execMoveA2toFRS(const u16 pc, const u16 code)
{
    print(pc, code, u8"A2 => FRS, $1 => L", u8"Move A2[7:1] to FRS[7:1], 1 => L[2:1]");
}

void
Disassembler::execMoveA2toMODE(const u16 pc, const u16 code)
{
    const auto N = code & 0b00000'0000011;
    snprintf((char*)mnemonic, sizeof(mnemonic), "A2 => MODE, $%d => L", N);
    print(pc, code, mnemonic, u8"Move A2[7:1] to MODE[7:1], 1N => L[2:1]");
}

void
Disassembler::execSubA2andA1toA1(const u16 pc, const u16 code)
{
    const auto N = code & 0b00000'0000011;
    snprintf((char*)mnemonic, sizeof(mnemonic), "A2-A1 => A1, $%d => L", N);
    print(pc, code, mnemonic, u8"Subtract A2[7:1] and A1[7:1], store to A1[7:1], Skip if borrow, N => L[2:1]", u8"BOJ");
}

void
Disassembler::execRightShiftA2(const u16 pc, const u16 code)
{
    const auto N = code & 0b00000'0000011;
    snprintf((char*)mnemonic, sizeof(mnemonic), "A2 => RS, $%d => L", N);
    print(pc, code, mnemonic, u8"Right shift A2[7:1], 0 => A2[7], N => L[2:1]");
}

void
Disassembler::execMoveMtoFLS(const u16 pc, const u16 code)
{
    print(pc, code, u8"M => FLS, $0 => L", u8"Move M[H[5:1],L[2:1]][7:1] to FLS[7:1], 0 => L[2:1]");
}

void
Disassembler::execMoveMtoFRS(const u16 pc, const u16 code)
{
    print(pc, code, u8"M => FRS, $1 => L", u8"Move M[H[5:1],L[2:1]][7:1] to FRS[7:1], 1 => L[2:1]");
}

void
Disassembler::execMoveMtoMODE(const u16 pc, const u16 code)
{
    const auto N = code & 0b00000'0000011;
    snprintf((char*)mnemonic, sizeof(mnemonic), "M => MODE, $%d => L", N);
    print(pc, code, mnemonic, u8"Move M[H[5:1],L[2:1]][7:1] to MODE[7:1], 1N => L[2:1]");
}

void
Disassembler::execRightShiftM(const u16 pc, const u16 code)
{
    const auto N = code & 0b00000'0000011;
    snprintf((char*)mnemonic, sizeof(mnemonic), "M => RS, $%d => L", N);
    print(pc, code, mnemonic, u8"Right shift M[H[5:1],L[2:1]][7:1], 0 => M[H[5:1],L[2:1]][7], N => L[2:1]");
}

void
Disassembler::execAndA1A1toA1(const u16 pc, const u16 code)
{
    const auto N = code & 0b00000'0000011;
    snprintf((char*)mnemonic, sizeof(mnemonic), "A1&A1 => A1, $%d => L", N);
    print(pc, code, mnemonic, u8"AND A1[7:1] and A1[7:1], store to A1[7:1], N => L[2:1]");
}

void
Disassembler::execAddA1A1toA1(const u16 pc, const u16 code)
{
    const auto N = code & 0b00000'0000011;
    snprintf((char*)mnemonic, sizeof(mnemonic), "A1+A1 => A1, $%d => L", N);
    print(pc, code, mnemonic, u8"Add A1[7:1] and A1[7:1], store to A1[7:1], N => L[2:1]", u8"CAJ");
}

void
Disassembler::execOrA1A1toA1(const u16 pc, const u16 code)
{
    const auto N = code & 0b00000'0000011;
    snprintf((char*)mnemonic, sizeof(mnemonic), "A1|A1 => A1, $%d => L", N);
    print(pc, code, mnemonic, u8"OR A1[7:1] and A1[7:1], store to A1[7:1], N => L[2:1]");
}

void
Disassembler::execSubA1A1toA1(const u16 pc, const u16 code)
{
    const auto N = code & 0b00000'0000011;
    snprintf((char*)mnemonic, sizeof(mnemonic), "A1-A1 => A1, $%d => L", N);
    print(pc, code, mnemonic, u8"Subtract A1[7:1] and A1[7:1], store to A1[7:1], Skip if borrow, N => L[2:1]", u8"BOJ");
}

void
Disassembler::execAndA1A2toA1(const u16 pc, const u16 code)
{
    const auto N = code & 0b00000'0000011;
    snprintf((char*)mnemonic, sizeof(mnemonic), "A1&A2 => A1, $%d => L", N);
    print(pc, code, mnemonic, u8"AND A1[7:1] and A2[7:1], store to A1[7:1], N => L[2:1]");
}

void
Disassembler::execAddA1A2toA1(const u16 pc, const u16 code)
{
    const auto N = code & 0b00000'0000011;
    snprintf((char*)mnemonic, sizeof(mnemonic), "A1+A2 => A1, $%d => L", N);
    print(pc, code, mnemonic, u8"Add A1[7:1] and A2[7:1], store to A1[7:1], N => L[2:1]", u8"CAJ");
}

void
Disassembler::execOrA1A2toA1(const u16 pc, const u16 code)
{
    const auto N = code & 0b00000'0000011;
    snprintf((char*)mnemonic, sizeof(mnemonic), "A1|A2 => A1, $%d => L", N);
    print(pc, code, mnemonic, u8"OR A1[7:1] and A2[7:1], store to A1[7:1], N => L[2:1]");
}

void
Disassembler::execSubA1A2toA1(const u16 pc, const u16 code)
{
    const auto N = code & 0b00000'0000011;
    snprintf((char*)mnemonic, sizeof(mnemonic), "A1-A2 => A1, $%d => L", N);
    print(pc, code, mnemonic, u8"Subtract A1[7:1] and A2[7:1], store to A1[7:1], Skip if borrow, N => L[2:1]", u8"BOJ");
}



















void
Disassembler::execAndA2A1toA2(const u16 pc, const u16 code)
{
    const auto N = code & 0b00000'0000011;
    snprintf((char*)mnemonic, sizeof(mnemonic), "A2&A1 => A2, $%d => L", N);
    print(pc, code, mnemonic, u8"AND A2[7:1] and A1[7:1], store to A2[7:1], N => L[2:1]");
}

void
Disassembler::execAddA2A1toA2(const u16 pc, const u16 code)
{
    const auto N = code & 0b00000'0000011;
    snprintf((char*)mnemonic, sizeof(mnemonic), "A2+A1 => A2, $%d => L", N);
    print(pc, code, mnemonic, u8"Add A2[7:1] and A1[7:1], store to A2[7:1], N => L[2:1]", u8"CAJ");
}

void
Disassembler::execOrA2A1toA2(const u16 pc, const u16 code)
{
    const auto N = code & 0b00000'0000011;
    snprintf((char*)mnemonic, sizeof(mnemonic), "A2|A1 => A2, $%d => L", N);
    print(pc, code, mnemonic, u8"OR A2[7:1] and A1[7:1], store to A2[7:1], N => L[2:1]");
}

void
Disassembler::execSubA2A1toA2(const u16 pc, const u16 code)
{
    const auto N = code & 0b00000'0000011;
    snprintf((char*)mnemonic, sizeof(mnemonic), "A2-A1 => A2, $%d => L", N);
    print(pc, code, mnemonic, u8"Subtract A2[7:1] and A1[7:1], store to A2[7:1], Skip if borrow, N => L[2:1]", u8"BOJ");
}

void
Disassembler::execAndA2A2toA2(const u16 pc, const u16 code)
{
    const auto N = code & 0b00000'0000011;
    snprintf((char*)mnemonic, sizeof(mnemonic), "A2&A2 => A2, $%d => L", N);
    print(pc, code, mnemonic, u8"AND A2[7:1] and A2[7:1], store to A2[7:1], N => L[2:1]");
}

void
Disassembler::execAddA2A2toA2(const u16 pc, const u16 code)
{
    const auto N = code & 0b00000'0000011;
    snprintf((char*)mnemonic, sizeof(mnemonic), "A2+A2 => A2, $%d => L", N);
    print(pc, code, mnemonic, u8"Add A2[7:1] and A2[7:1], store to A2[7:1], N => L[2:1]", u8"CAJ");
}

void
Disassembler::execOrA2A2toA2(const u16 pc, const u16 code)
{
    const auto N = code & 0b00000'0000011;
    snprintf((char*)mnemonic, sizeof(mnemonic), "A2|A2 => A2, $%d => L", N);
    print(pc, code, mnemonic, u8"OR A2[7:1] and A2[7:1], store to A2[7:1], N => L[2:1]");
}

void
Disassembler::execSubA2A2toA2(const u16 pc, const u16 code)
{
    const auto N = code & 0b00000'0000011;
    snprintf((char*)mnemonic, sizeof(mnemonic), "A2-A2 => A2, $%d => L", N);
    print(pc, code, mnemonic, u8"Subtract A2[7:1] and A2[7:1], store to A2[7:1], Skip if borrow, N => L[2:1]", u8"BOJ");
}

void
Disassembler::execMoveA1toM(const u16 pc, const u16 code)
{
    const auto N = code & 0b00000'0000011;
    snprintf((char*)mnemonic, sizeof(mnemonic), "A1 => M, $%d => L", N);
    print(pc, code, mnemonic, u8"Move A1[7:1] to M[H[5:1],L[2:1]][7:1], N => L[2:1]");
}

void
Disassembler::execMoveA2toM(const u16 pc, const u16 code)
{
    const auto N = code & 0b00000'0000011;
    snprintf((char*)mnemonic, sizeof(mnemonic), "A2 => M, $%d => L", N);
    print(pc, code, mnemonic, u8"Move A2[7:1] to M[H[5:1],L[2:1]][7:1], N => L[2:1]");
}

void
Disassembler::execExchangeMA1(const u16 pc, const u16 code)
{
    const auto N = code & 0b00000'0000011;
    snprintf((char*)mnemonic, sizeof(mnemonic), "M <=> A1, $%d => L", N);
    print(pc, code, mnemonic, u8"Exchange M[H[5:1],L[2:1]][7:1] and A1[7:1], N => L[2:1]");
}

void
Disassembler::execExchangeMA2(const u16 pc, const u16 code)
{
    const auto N = code & 0b00000'0000011;
    snprintf((char*)mnemonic, sizeof(mnemonic), "M <=> A2, $%d => L", N);
    print(pc, code, mnemonic, u8"Exchange M[H[5:1],L[2:1]][7:1] and A2[7:1], N => L[2:1]");
}

void
Disassembler::execMoveMtoA1(const u16 pc, const u16 code)
{
    const auto N = code & 0b00000'0000011;
    snprintf((char*)mnemonic, sizeof(mnemonic), "M => A1, $%d => L", N);
    print(pc, code, mnemonic, u8"Move M[H[5:1],L[2:1]][7:1] to A1[7:1], N => L[2:1]");
}

void
Disassembler::execMoveMtoA2(const u16 pc, const u16 code)
{
    const auto N = code & 0b00000'0000011;
    snprintf((char*)mnemonic, sizeof(mnemonic), "M => A2, $%d => L", N);
    print(pc, code, mnemonic, u8"Move M[H[5:1],L[2:1]][7:1] to A2[7:1], N => L[2:1]");
}

void
Disassembler::execAndMA2toM(const u16 pc, const u16 code)
{
    const auto N = code & 0b00000'0000011;
    snprintf((char*)mnemonic, sizeof(mnemonic), "M&A2 => M, $%d => L", N);
    print(pc, code, mnemonic, u8"AND M[H[5:1],L[2:1]][7:1] and A2[7:1], store to M[H[5:1],L[2:1]][7:1], N => L[2:1]");
}

void
Disassembler::execAddMA2toM(const u16 pc, const u16 code)
{
    const auto N = code & 0b00000'0000011;
    snprintf((char*)mnemonic, sizeof(mnemonic), "M+A2 => M, $%d => L", N);
    print(pc, code, mnemonic, u8"Add M[H[5:1],L[2:1]][7:1] and A2[7:1], store to M[H[5:1],L[2:1]][7:1], N => L[2:1], Skip if carry", u8"CAJ");
}

void
Disassembler::execOrMA2toM(const u16 pc, const u16 code)
{
    const auto N = code & 0b00000'0000011;
    snprintf((char*)mnemonic, sizeof(mnemonic), "M|A2 => M, $%d => L", N);
    print(pc, code, mnemonic, u8"OR M[H[5:1],L[2:1]][7:1] and A2[7:1], store to M[H[5:1],L[2:1]][7:1], N => L[2:1]");
}

void
Disassembler::execSubMA2toM(const u16 pc, const u16 code)
{
    const auto N = code & 0b00000'0000011;
    snprintf((char*)mnemonic, sizeof(mnemonic), "M-A2 => M, $%d => L", N);
    print(pc, code, mnemonic, u8"Subtract M[H[5:1],L[2:1]][7:1] and A2[7:1], store to M[H[5:1],L[2:1]][7:1], N => L[2:1], Skip if borrow", u8"BOJ");
}

void
Disassembler::execAndMA1toM(const u16 pc, const u16 code)
{
    const auto N = code & 0b00000'0000011;
    snprintf((char*)mnemonic, sizeof(mnemonic), "M&A1 => M, $%d => L", N);
    print(pc, code, mnemonic, u8"AND M[H[5:1],L[2:1]][7:1] and A1[7:1], store to M[H[5:1],L[2:1]][7:1], N => L[2:1]");
}

void
Disassembler::execAddMA1toM(const u16 pc, const u16 code)
{
    const auto N = code & 0b00000'0000011;
    snprintf((char*)mnemonic, sizeof(mnemonic), "M+A1 => M, $%d => L", N);
    print(pc, code, mnemonic, u8"Add M[H[5:1],L[2:1]][7:1] and A1[7:1], store to M[H[5:1],L[2:1]][7:1], N => L[2:1], Skip if carry", u8"CAJ");
}

void
Disassembler::execOrMA1toM(const u16 pc, const u16 code)
{
    const auto N = code & 0b00000'0000011;
    snprintf((char*)mnemonic, sizeof(mnemonic), "M|A1 => M, $%d => L", N);
    print(pc, code, mnemonic, u8"OR M[H[5:1],L[2:1]][7:1] and A1[7:1], store to M[H[5:1],L[2:1]][7:1], N => L[2:1]");
}

void
Disassembler::execSubMA1toM(const u16 pc, const u16 code)
{
    const auto N = code & 0b00000'0000011;
    snprintf((char*)mnemonic, sizeof(mnemonic), "M-A1 => M, $%d => L", N);
    print(pc, code, mnemonic, u8"Subtract M[H[5:1],L[2:1]][7:1] and A1[7:1], store to M[H[5:1],L[2:1]][7:1], N => L[2:1], Skip if borrow", u8"BOJ");
}

void
Disassembler::execMoveA1toH(const u16 pc, const u16 code)
{
    const auto N = code & 0b00000'0000011;
    snprintf((char*)mnemonic, sizeof(mnemonic), "A1 => H, $%d => L", N);
    print(pc, code, mnemonic, u8"Move A1[5:1] to H[5:1], N => L[2:1]");
}

void
Disassembler::execMoveA2toH(const u16 pc, const u16 code)
{
    const auto N = code & 0b00000'0000011;
    snprintf((char*)mnemonic, sizeof(mnemonic), "A2 => H, $%d => L", N);
    print(pc, code, mnemonic, u8"Move A2[5:1] to H[5:1], N => L[2:1]");
}

void
Disassembler::execMoveHtoA1(const u16 pc, const u16 code)
{
    const auto N = code & 0b00000'0000011;
    snprintf((char*)mnemonic, sizeof(mnemonic), "H => A1, $%d => L", N);
    print(pc, code, mnemonic, u8"Move H[5:1] to A1[5:1], 0 => A1[7:6], N => L[2:1]");
}

void
Disassembler::execMoveHtoA2(const u16 pc, const u16 code)
{
    const auto N = code & 0b00000'0000011;
    snprintf((char*)mnemonic, sizeof(mnemonic), "H => A2, $%d => L", N);
    print(pc, code, mnemonic, u8"Move H[5:1] to A2[5:1], 0 => A1[7:6], N => L[2:1]");
}




void Disassembler::execAndHA1toH(const u16 pc, const u16 code)
{
    const auto N = code & 0b00000'0000011;
    snprintf((char*)mnemonic, sizeof(mnemonic), "H&A1 => H, $%d => L", N);
    print(pc, code, mnemonic, u8"AND H[5:1] and A1[5:1], store to H[5:1], N => L[2:1]");
}
void Disassembler::execAddHA1toH(const u16 pc, const u16 code)
{
    const auto N = code & 0b00000'0000011;
    snprintf((char*)mnemonic, sizeof(mnemonic), "H+A1 => H, $%d => L", N);
    print(pc, code, mnemonic, u8"Add H[5:1] and A1[5:1], store to H[5:1], N => L[2:1]", u8"CAJ");
}
void Disassembler::execOrHA1toH(const u16 pc, const u16 code)
{
    const auto N = code & 0b00000'0000011;
    snprintf((char*)mnemonic, sizeof(mnemonic), "H|A1 => H, $%d => L", N);
    print(pc, code, mnemonic, u8"OR H[5:1] and A1[5:1], store to H[5:1], N => L[2:1]");
}
void Disassembler::execSubHA1toH(const u16 pc, const u16 code)
{
    const auto N = code & 0b00000'0000011;
    snprintf((char*)mnemonic, sizeof(mnemonic), "H-A1 => H, $%d => L", N);
    print(pc, code, mnemonic, u8"Subtract H[5:1] and A1[5:1], store to H[5:1], N => L[2:1], Skip if borrow", u8"BOJ");
}
void Disassembler::execAndHA2toH(const u16 pc, const u16 code)
{
    const auto N = code & 0b00000'0000011;
    snprintf((char*)mnemonic, sizeof(mnemonic), "H&A2 => H, $%d => L", N);
    print(pc, code, mnemonic, u8"AND H[5:1] and A2[5:1], store to H[5:1], N => L[2:1]");
}
void Disassembler::execAddHA2toH(const u16 pc, const u16 code)
{
    const auto N = code & 0b00000'0000011;
    snprintf((char*)mnemonic, sizeof(mnemonic), "H+A2 => H, $%d => L", N);
    print(pc, code, mnemonic, u8"Add H[5:1] and A2[5:1], store to H[5:1], N => L[2:1]", u8"CAJ");
}
void Disassembler::execOrHA2toH(const u16 pc, const u16 code)
{
    const auto N = code & 0b00000'0000011;
    snprintf((char*)mnemonic, sizeof(mnemonic), "H|A2 => H, $%d => L", N);
    print(pc, code, mnemonic, u8"OR H[5:1] and A2[5:1], store to H[5:1], N => L[2:1]");
}
void Disassembler::execSubHA2toH(const u16 pc, const u16 code)
{
    const auto N = code & 0b00000'0000011;
    snprintf((char*)mnemonic, sizeof(mnemonic), "H-A2 => H, $%d => L", N);
    print(pc, code, mnemonic, u8"Subtract H[5:1] and A2[5:1], store to H[5:1], N => L[2:1], Skip if borrow", u8"BOJ");
}

void
Disassembler::execSetA11(const u16 pc, const u16 code)
{
    const auto N = code & 0b00000'0000001;
    snprintf((char*)mnemonic, sizeof(mnemonic), "$%d => A11", N);
    print(pc, code, mnemonic, u8"N => A[11]");
}

void
Disassembler::execJPM(const u16 pc, const u16 code)
{
    const auto N = code & 0b00000'0000001;
    snprintf((char*)mnemonic, sizeof(mnemonic), "JPM, 0 => L, $%d => A11", N);
    print(pc, code, mnemonic, u8"Jump to (000,M[H[5:1],L[2:1]][5:1],1N), 0 => L[2:1], N => A[11]");
}

void
Disassembler::execSetFlags(const u16 pc, const u16 code)
{
    const auto N =  code & 0b00000'000000'1;
    const auto D = (code & 0b00000'010000'0) ? 1 : 0;
    const auto G = (code & 0b00000'001000'0) ? 1 : 0;
    const auto K = (code & 0b00000'000100'0) ? 1 : 0;
    const auto S = (code & 0b00000'000010'0) ? 1 : 0;
    snprintf((char*)mnemonic, sizeof(mnemonic), "$%d => D, $%d => G, $%d => K, $%d => S, $%d => A11", D, G, K, S, N);
    print(pc, code, mnemonic, u8"Set D to DISP, G to GPE, K to KIE, S to SME, N => A[11]");
}

void
Disassembler::execSubHKtoH(const u16 pc, const u16 code)
{
    const auto K = code & 0b0000000'11111;
    snprintf((char*)mnemonic, sizeof(mnemonic), "H-$%02X => H", K);
    print(pc, code, mnemonic, u8"H[5:1]-K[5:1] => H[5:1], Skip if borrow", u8"BOJ");
}

void
Disassembler::execAddHKtoH(const u16 pc, const u16 code)
{
    const auto K = code & 0b0000000'11111;
    snprintf((char*)mnemonic, sizeof(mnemonic), "H+$%02X => H", K);
    print(pc, code, mnemonic, u8"H[5:1]+K[5:1] => H[5:1], Skip if carry", u8"CAJ");
}

void
Disassembler::execMoveKtoM(const u16 pc, const u16 code)
{
    const auto K = code & 0b00000'1111111;
    snprintf((char*)mnemonic, sizeof(mnemonic), "$%02X => M", K);
    print(pc, code, mnemonic, u8"When (KIE=0)&(SME=0), Store K[7:1] to M[H[5:1],L[2:1]][7:1] / When (KIE=1), Store KIN[7:1] to M[H[5:1],L[2:1]][7:1] / When (SME=1), Store HCL[7:1] to M[H[5:1],L[2:1]][7:1]");
}

void
Disassembler::execMoveKtoLH(const u16 pc, const u16 code)
{
    const auto K = code & 0b00000'1111111;
    const auto L = (K >> 5) & 0x3;
    const auto H =  K       & 0x1F;
    snprintf((char*)mnemonic, sizeof(mnemonic), "$%02X => L,H", K);
    print(pc, code, mnemonic, u8"Store K[7:6] to L[2:1] and K[5:1] to H[5:1]");
}

void
Disassembler::execMoveKtoA1(const u16 pc, const u16 code)
{
    const auto K = code & 0b00000'1111111;
    snprintf((char*)mnemonic, sizeof(mnemonic), "$%02X => A1", K);
    print(pc, code, mnemonic, u8"Store K[7:1] to A1[7:1]");
}

void
Disassembler::execMoveKtoA2(const u16 pc, const u16 code)
{
    const auto K = code & 0b00000'1111111;
    snprintf((char*)mnemonic, sizeof(mnemonic), "$%02X => A2", K);
    print(pc, code, mnemonic, u8"Store K[7:1] to A2[7:1]");
}

void
Disassembler::execMoveKtoA3(const u16 pc, const u16 code)
{
    const auto K = code & 0b00000'1111111;
    snprintf((char*)mnemonic, sizeof(mnemonic), "$%02X => A3", K);
    print(pc, code, mnemonic, u8"Store K[7:1] to A3[7:1]");
}

void
Disassembler::execMoveKtoA4(const u16 pc, const u16 code)
{
    const auto K = code & 0b00000'1111111;
    snprintf((char*)mnemonic, sizeof(mnemonic), "$%02X => A4", K);
    print(pc, code, mnemonic, u8"Store K[7:1] to A4[7:1]");
}

void
Disassembler::execJP(const u16 pc, const u16 code)
{
    const auto K = code & 0b00'1111111111;
    snprintf((char*)mnemonic, sizeof(mnemonic), "JP $%03X", K | (pc & 0x400));
    print(pc, code, mnemonic, u8"Move K[10:1] to A[10:1], Jump to A[11:1]");

    registerAddress(K | (pc & 0x400), pc, FLAG_JUMP);
}

void
Disassembler::execJS(const u16 pc, const u16 code)
{
    const auto K = code & 0b00'1111111111;
    snprintf((char*)mnemonic, sizeof(mnemonic), "JS $%03X", K);
    print(pc, code, mnemonic, u8"Move K[10:1] to A[10:1], 0 to A11, Jump to A[11:1], Push next A[11:1] up to ROM address stack");

    registerAddress(K, pc, FLAG_CALL);
}

#endif // defined(_WIN32)
