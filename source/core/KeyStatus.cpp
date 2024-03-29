#include "KeyStatus.h"

KeyStatus::KeyStatus()
{
    clear();
}

void
KeyStatus::setCourseSwitch(const u8 courseSwitch)
{
    switch(courseSwitch) {
        case 1: a12 = (u8)0x01; break; // コーススイッチ1
        case 2: a12 = (u8)0x02; break; // コーススイッチ2
        case 3: a12 = (u8)0x04; break; // コーススイッチ3
        case 4: a12 = (u8)0x08; break; // コーススイッチ4
        case 5: a12 = (u8)0x10; break; // コーススイッチ5
        default:
            a12 = 0;
            break;
    }
}

const u8
KeyStatus::convertDefault(const u8 STB, const u32 cassetteNumber) const
{
    u8 value = 0;
    switch(STB) {
        case 0xD:
            {
                // ４方向使用するときの上下
                if(getUp()) {
                    value |= 0x02;
                }
                if(getDown()) {
                    value |= 0x04;
                }
                //
                if(getPush1() || getPush3()) {
                    value |= 0x40; // PUSH1、PUSH3
                }
                if(getPush2() || getPush4()) {
                    value |= 0x20; // PUSH2 PUSH4
                }
            }
            break;
        default:
        case 0xE: // A8
            {
                if(getGameStartKey()) { value |= 0x01; }
                if(getLeverSwitch1Left()) { value |= 0x02; }
                if(getLeverSwitch1Right()) { value |= 0x04; }
                if(getGameSelectKey()) { value |= 0x08; }

                // PUSH1、PUSH2、PUSH3、PUSH4
                if(getPush1() || getPush2() || getPush3() || getPush4()) {
                    value |= (u8)0x20;
                }

                // 特殊処理
                if(cassetteNumber == 6) {
                    if(getPush1()) {
                        value |= 0x2;
                    }
                    if(getPush2()) {
                        value |= 0x4;
                    }
                }
            }
            break;
        case 0x5:
            {
                value |= getCourseSwitch();
                // PUSH1、PUSH3
                if(getPush1() || getPush3()) {
                    value |= (u8)0x40;
                }
                // PUSH2、PUSH4
                if(getPush2() || getPush4()) {
                    value |= (u8)0x20;
                }
            }
            break;
        case 0xA:
            {
                if(getGameStartKey()) {
                    value |= (u8)0x01;
                }
                if(getGameSelectKey()) {
                    value |= (u8)0x08;
                }

                if(getLeverSwitch1Left()) {
                    value |= (u8)0x02;
                }
                if(getLeverSwitch1Right()) {
                    value |= (u8)0x04;
                }
            }
            break;

        case 0xC:
            {
                if(getGameStartKey()) {
                    value |= (u8)0x01;
                }
                if(getGameSelectKey()) {
                    value |= (u8)0x08;
                }
                if(getLeverSwitch1Left()) {
                    value |= (u8)0x02;
                }
                if(getLeverSwitch1Right()) {
                    value |= (u8)0x04;
                }
                if(getPush1() || getPush3()) {
                    value |= 0x40;
                }
                if(getPush2() || getPush4()) {
                    value |= 0x20;
                }
            }
            break;
    }
    return value & 0x7F;
}

const u8
KeyStatus::convertMap(const u8 STB, const KeyMapping& keyMapping) const
{
    /*
     * K1  K2  K3  K4  K5  K6  K7
     * ----------------------------------------
     * STA L1L L1R SEL AUX 6   7    | [A08]
     * 1   L2L L2R 4   5   6   7    | [A09]
     * 1   2   3   4   5   P4  P3   | [A10]
     * 1   2   3   4   5   P2  P1   | [A11]
     * LL  L   C   R   RR  6   7    | [A12]
     */
    u8 value = 0;
    for(s32 i = 0; i < 5; ++i) {
        if(const auto map = keyMapping.keyMap[i]; map > 0) {
            const auto s  =  (map & 0x3) - 1;
            const auto op = (map & 0xC) >> 2;
            if(~STB & (1 << s)) {
                u8 input = a[i];
                // 特殊処理
                if(op == 0x1) {
                    // UPを0x40に割り当てる
                    // DOWNを0x20に割り当てる
                    if(getUp())   { input |= 0x40; }
                    if(getDown()) { input |= 0x20; }
                }
                if(op == 0x2) {
                    // レバー1左を0x40に割り当てる
                    // レバー1右を0x20に割り当てる
                    if(getLeverSwitch1Left())  { input |= KIN::Push1; }
                    if(getLeverSwitch1Right()) { input |= KIN::Push2; }
                }
                u8 b = 0;
                if(input & 0x01) { b |= keyMapping.bitMap[6]; }
                if(input & 0x02) { b |= keyMapping.bitMap[5]; }
                if(input & 0x04) { b |= keyMapping.bitMap[4]; }
                if(input & 0x08) { b |= keyMapping.bitMap[3]; }
                if(input & 0x10) { b |= keyMapping.bitMap[2]; }
                if(input & 0x20) { b |= keyMapping.bitMap[1]; }
                if(input & 0x40) { b |= keyMapping.bitMap[0]; }
                value |= b;
            }
        }
    }
    return value;
}

const u8
KeyStatus::convert(const u8 STB, const KeyMapping& keyMapping, const u32 cassetteNumber) const
{
    if(keyMapping.isDefaultMapping()) {
        // デフォルトの変換
        return convertDefault(STB, cassetteNumber);
    } else {
        // keyMapに従った変換
        return convertMap(STB, keyMapping);
    }
}
