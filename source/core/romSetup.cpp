#include "PD777.h"

#if defined(_WIN32)
#include <string>
#endif // defined(_WIN32)

namespace {

constexpr u32 ID1  = 0x003f17fa;
constexpr u32 ID2  = 0x00440844;
constexpr u32 ID3  = 0x003ea572;
constexpr u32 ID4  = 0x0041b25a;
constexpr u32 ID5  = 0x003decdb;
constexpr u32 ID6  = 0x003d26f4;
constexpr u32 ID7  = 0x0046e633;
constexpr u32 ID8  = 0x003c3405;
constexpr u32 ID9  = 0x003e4bf7;
constexpr u32 ID11 = 0x003b935b;
constexpr u32 ID12 = 0x003ecdfb;

#define CHAR_BENT (0xC)
static const u8 bentCassette1[] = {
    0x30, CHAR_BENT, 0x31, CHAR_BENT, 0x32, CHAR_BENT, 0x33, CHAR_BENT, 0x34, CHAR_BENT, 0x35, CHAR_BENT, 0x36, CHAR_BENT,
    0x40, CHAR_BENT, 0x41, CHAR_BENT, 0x42, CHAR_BENT, 0x43, CHAR_BENT, 0x44, CHAR_BENT, 0x45, CHAR_BENT, 0x46, CHAR_BENT,
    0x48, CHAR_BENT, 0x49, CHAR_BENT, 0x4A, CHAR_BENT, 0x4B, CHAR_BENT, 0x4C, CHAR_BENT, 0x4D, CHAR_BENT, 0x4E, CHAR_BENT,
    0x50, CHAR_BENT, 0x51, CHAR_BENT, 0x52, CHAR_BENT, 0x53, CHAR_BENT, 0x54, CHAR_BENT, 0x55, CHAR_BENT, 0x56, CHAR_BENT,
    0x60, CHAR_BENT, 0x61, CHAR_BENT, 0x62, CHAR_BENT, 0x63, CHAR_BENT, 0x64, CHAR_BENT, 0x65, CHAR_BENT, 0x66, CHAR_BENT,
    0xFF, 0
};
static const u8 bentCassette2[] = {
    0x30, CHAR_BENT, 0x31, CHAR_BENT, 0x32, CHAR_BENT, 0x33, CHAR_BENT, 0x34, CHAR_BENT, 0x35, CHAR_BENT, 0x36, CHAR_BENT,
    0xFF, 0
};
static const u8 bentCassette3[] = {
    0xFF, 0
};
static const u8 bentCassette4[] = {
    0xFF, 0
};
static const u8 bentCassette5[] = {
    0x58, CHAR_BENT, 0x59, CHAR_BENT, 0x5A, CHAR_BENT, 0x5B, CHAR_BENT, 0x5C, CHAR_BENT, 0x5D, CHAR_BENT, 0x5E, CHAR_BENT,
    0xFF, 0
};
static const u8 bentCassette6[] = {
    0x40, CHAR_BENT, 0x41, CHAR_BENT, 0x42, CHAR_BENT, 0x43, CHAR_BENT, 0x44, CHAR_BENT, 0x45, CHAR_BENT, 0x46, CHAR_BENT,
    0x48, CHAR_BENT, 0x49, CHAR_BENT, 0x4A, CHAR_BENT, 0x4B, CHAR_BENT, 0x4C, CHAR_BENT, 0x4D, CHAR_BENT, 0x4E, CHAR_BENT,
    0x50, CHAR_BENT, 0x51, CHAR_BENT, 0x52, CHAR_BENT, 0x53, CHAR_BENT, 0x54, CHAR_BENT, 0x55, CHAR_BENT, 0x56, CHAR_BENT,
    0x58, CHAR_BENT, 0x59, CHAR_BENT, 0x5A, CHAR_BENT, 0x5B, CHAR_BENT, 0x5C, CHAR_BENT, 0x5D, CHAR_BENT, 0x5E, CHAR_BENT,
    0xFF, 0
};

static const u8 bentCassette7[] = {
    0x30, CHAR_BENT, 0x31, CHAR_BENT, 0x32, CHAR_BENT, 0x33, CHAR_BENT, 0x34, CHAR_BENT, 0x35, CHAR_BENT, 0x36, CHAR_BENT,
    0xFF, 0
};
static const u8 bentCassette8[] = {
    0xFF, 0
};
static const u8 bentCassette9[] = {
    0xFF, 0
};
static const u8 bentCassette10[] = {
    0xFF, 0
};
static const u8 bentCassette11[] = {
    0xFF, 0
};
static const u8 bentCassette12[] = {
    0xFF, 0
};

static const u8* characterBentData[] = {
    nullptr,
    bentCassette1,
    bentCassette2,
    bentCassette3,
    bentCassette4,
    bentCassette5,
    bentCassette6,
    bentCassette7,
    bentCassette8,
    bentCassette9,
    bentCassette10,
    bentCassette11,
    bentCassette12,
};
#undef CHAR_BENT

inline bool
getBit(const void* data, const u32 pos)
{
    return (((u8*)data)[pos / 8] & (0x80 >> (pos % 8))) != 0;
}

} // namespace

bool
PD777::setupCodeRawAddress(const void* data, size_t dataSize)
{
    for(auto& r : rom) { r = ~0; }

    for(size_t i = 0; i < sizeof(rawRom) / sizeof(rawRom[0]); i += 2) {
        const auto address = rawRom[i + 0]; // アドレス
        rom[address]       = rawRom[i + 1]; // コード
    }
    for(auto& r : this->keyMapping.keyMap) { r = 0; }
    return true;
}

bool
PD777::isCodeRawOrder(const void* codeData,  const size_t codeDataSize) const
{
    return (codeData != nullptr) && (codeDataSize == 0xF00);
}

bool
PD777::setupCodeRawOrder(const void* codeData, const size_t codeDataSize)
{
    if(!isCodeRawOrder(codeData, codeDataSize)) {
        return false;
    }

    for(auto index = 0x780; index < 0x7FF; ++index) { rom[index] = 0; }
    rom[0x7FF] = ~0;

    const u8* byteData = (const u8*)codeData;
    u16 pc = 0;
    for(auto index = 0; index < codeDataSize; index += 2) {
        const u16 code = (((u16)byteData[index+0] << 8) | (u16)byteData[index+1]);
        rom[pc++] = code;
    }

    // メモ) 0番地はNOPにする
    rom[0] = 0x0000;
    return true;
}

bool
PD777::isCodeFormatted(const void* codeData,  const size_t codeDataSize) const
{
    if((codeData == nullptr) || (codeDataSize < 0x100)) {
        return false;
    }

    // マジックナンバー + バージョン
    const u8* header = (const u8*)codeData;
    c8 magicNumber[] = u8"_CassetteVision_000";
    for(auto i = 0; i < 19; ++i) {
        if(header[i] != magicNumber[i]) [[unlikely]] {
            return false;
        }
    }
    if(header[19] == '0') {
        return true; // Ver0
    } else if(header[19] == '1') {
        return true; // Ver1
    } else {
        return false; // 不明なバージョン
    }
}

bool
PD777::setupCodeFormatted(const void* data, size_t dataSize)
{
    if(!isCodeFormatted(data, dataSize)) {
        return false;
    }

    for(auto& r : rom) { r = ~0; }

    // ヘッダ部分
    {
        // マジックナンバー + バージョン
        const u8* header = (const u8*)data;
        c8 magicNumber[] = u8"_CassetteVision_000";
        for(auto i = 0; i < 19; ++i) {
            if(header[i] != magicNumber[i]) [[unlikely]] {
                return false;
            }
        }
        if(header[19] == '0') {
            // Ver0
            // キーマッピング
            const u8* keyMapping = header + 0x18;
            for(auto i = 0; i < 5; ++i) {
                this->keyMapping.keyMap[i] = keyMapping[i];
            }
            // タイトル
            const c8* title = (const c8*)header + 0x20;
        } else if(header[19] == '1') {
            // Ver1
            // キーマッピング
            const u8* keyMappingA = header + 0x18;
            for(auto i = 0; i < 5; ++i) {
                this->keyMapping.keyMap[i] = keyMappingA[i];
            }
            const u8* keyMappingB = header + 0x20;
            for(auto i = 0; i < 7; ++i) {
                this->keyMapping.bitMap[i] = keyMappingB[i];
            }
            // タイトル
            const c8* title = (const c8*)header + 0x28;
        } else {
            return false; // 不明なバージョン
        }
    }
    // コード部分
    {
        const u16*   code = (const u16*)((const u8*)data + 0x0100);
        const size_t codeSize = (dataSize - 0x100) / sizeof(u16);
        for(size_t i = 0; i < codeSize; i += 2) {
            const auto address = code[i + 0] & 0x7FF; // アドレス
            rom[address]       = code[i + 1] & 0xFFF; // コード
        }
    }
    return true;
}

bool
PD777::isCodeData(const void* codeData, const size_t codeDataSize) const
{
    if(isCodeRawOrder(codeData, codeDataSize)) {
        return true; // 順番に並んだコード
    } else if(codeData == nullptr) {
        return true; // アドレス、コードの順に並んだコード
    } else if(isCodeFormatted(codeData, codeDataSize)) {
        return true; // フォーマットされたコード
    } else {
        return false; // 未対応
    }
}

bool
PD777::setupCode(const void* data, size_t dataSize)
{
    if(!isCodeData(data, dataSize)) {
        return false;
    }
   
    for(auto& r : rom) { r = ~0; }
    keyMapping.bitMap[0] = 0x40;            // B09 => K7
    keyMapping.bitMap[1] = 0x20;            // B10 => K6
    keyMapping.bitMap[2] = 0x10;            // B11 => K5
    keyMapping.bitMap[3] = 0x08;            // B12 => K4
    keyMapping.bitMap[4] = 0x04;            // B13 => K3
    keyMapping.bitMap[5] = 0x02;            // B14 => K2
    keyMapping.bitMap[6] = 0x01;            // B15 => K1

    if(isCodeRawOrder(data, dataSize)) {
        // 順番に並んだコード
        if(!setupCodeRawOrder(data, dataSize)) [[unlikely]] { return false; }
    } else if(data == nullptr) {
        // アドレス、コードの順に並んだコード
        if(!setupCodeRawAddress(data, dataSize)) [[unlikely]] { return false; }
    } else if(isCodeFormatted(data, dataSize)) {
        // フォーマットされたコード
        if(!setupCodeFormatted(data, dataSize)) [[unlikely]] { return false; }
    } else {
        // 未対応
        return false;
    }

    // 適当計算
    // @todo かっこいいのに変更すること！
    {
        u32 checkSum = 0;
        for(const auto& r : rom) { checkSum += r; }
        cassetteNumber = 0;
        switch(checkSum) {
            case ID1:
                cassetteNumber = 1;
                keyMapping.keyMap[0] = 1;
                keyMapping.keyMap[1] = 1;
                keyMapping.keyMap[2] = 2;
                keyMapping.keyMap[3] = 2;
                keyMapping.keyMap[4] = 0;
                break;
            case ID2:
                cassetteNumber = 2;
                keyMapping.keyMap[0] = 1;
                keyMapping.keyMap[1] = 0;
                keyMapping.keyMap[2] = 2;
                keyMapping.keyMap[3] = 0;
                keyMapping.keyMap[4] = 2;
                break;
            case ID3:
                cassetteNumber = 3;
                keyMapping.keyMap[0] = 1;
                keyMapping.keyMap[1] = 1;
                keyMapping.keyMap[2] = 1;
                keyMapping.keyMap[3] = 1;
                keyMapping.keyMap[4] = 0;
                keyMapping.bitMap[0] = 0x20;           // B09 => K6
                keyMapping.bitMap[1] = 0x20;           // B10 => K6
                keyMapping.bitMap[2] = 0x10;           // B11 => K5
                keyMapping.bitMap[3] = 0x08;           // B12 => K4
                keyMapping.bitMap[4] = 0x04;           // B13 => K3
                keyMapping.bitMap[5] = 0x02;           // B14 => K2
                keyMapping.bitMap[6] = 0x01;           // B15 => K1
                break;
            case ID4:
                cassetteNumber = 4;
                keyMapping.keyMap[0] = 1;
                keyMapping.keyMap[1] = 0;
                keyMapping.keyMap[2] = 0;
                keyMapping.keyMap[3] = 0;
                keyMapping.keyMap[4] = 0;
                keyMapping.bitMap[0] = 0x00;           // B09
                keyMapping.bitMap[1] = 0x00;           // B10
                keyMapping.bitMap[2] = 0x00;           // B11
                keyMapping.bitMap[3] = 0x08;           // B12 => K4
                keyMapping.bitMap[4] = 0x00;           // B13
                keyMapping.bitMap[5] = 0x00;           // B14
                keyMapping.bitMap[6] = 0x01;           // B15 => K1
                break;
            case ID5:
                cassetteNumber = 5;
                keyMapping.keyMap[ 0] = 1;
                keyMapping.keyMap[ 1] = 1;
                keyMapping.keyMap[ 2] = 2;
                keyMapping.keyMap[ 3] = 2;
                keyMapping.keyMap[ 4] = 0;
                break;
            case ID6:
                cassetteNumber = 6;
                keyMapping.keyMap[0] = 1;
                keyMapping.keyMap[1] = 0;
                keyMapping.keyMap[2] = 2 | (1 << 2);   // A10  UPをPUSH3、DOWNをPUSH4に割り当てる
                keyMapping.keyMap[3] = 1 | (2 << 2);   // A11  レバー1左右をPUSH1、PUSH2に割り当てる
                keyMapping.keyMap[4] = 0;
                keyMapping.bitMap[0] = 0x02;           // B09 => K2
                keyMapping.bitMap[1] = 0x04;           // B10 => K3
                keyMapping.bitMap[2] = 0x00;           // B11
                keyMapping.bitMap[3] = 0x08;           // B12 => K4
                keyMapping.bitMap[4] = 0x00;           // B13
                keyMapping.bitMap[5] = 0x00;           // B14
                keyMapping.bitMap[6] = 0x01;           // B15 => K1
                break;
            case ID7:
                cassetteNumber = 7;
                keyMapping.keyMap[0] = 1;
                keyMapping.keyMap[1] = 0;
                keyMapping.keyMap[2] = 2;
                keyMapping.keyMap[3] = 0;
                keyMapping.keyMap[4] = 2;
                break;

            case ID8:
                cassetteNumber = 8;
                keyMapping.keyMap[0] = 1;
                keyMapping.keyMap[1] = 1;
                keyMapping.keyMap[2] = 2 | (1 << 2);    // UPをPUSH3、DOWNをPUSH4に割り当てる
                keyMapping.keyMap[3] = 2 | (1 << 2);    // UPをPUSH1、DOWNをPUSH2に割り当てる
                keyMapping.keyMap[4] = 0;
                break;
            case ID9:
                cassetteNumber = 9;
                keyMapping.keyMap[ 0] = 1;
                keyMapping.keyMap[ 1] = 1;
                keyMapping.keyMap[ 2] = 2;
                keyMapping.keyMap[ 3] = 2;
                keyMapping.keyMap[ 4] = 0;
                break;
            case ID11:
                cassetteNumber = 11;
                keyMapping.keyMap[0] = 1;
                keyMapping.keyMap[1] = 0;
                keyMapping.keyMap[2] = 2 | (1 << 2);   // A10  UPをPUSH3、DOWNをPUSH4に割り当てる
                keyMapping.keyMap[3] = 1 | (2 << 2);   // A11  レバー1左右をPUSH1、PUSH2に割り当てる
                keyMapping.keyMap[4] = 0;
                keyMapping.bitMap[0] = 0x02;           // B09 => K2
                keyMapping.bitMap[1] = 0x04;           // B10 => K3
                keyMapping.bitMap[2] = 0x00;           // B11
                keyMapping.bitMap[3] = 0x08;           // B12 => K4
                keyMapping.bitMap[4] = 0x00;           // B13
                keyMapping.bitMap[5] = 0x00;           // B14
                keyMapping.bitMap[6] = 0x01;           // B15 => K1
                break;
            case ID12:
                cassetteNumber = 12;
                keyMapping.keyMap[0] = 1;
                keyMapping.keyMap[1] = 1;
                keyMapping.keyMap[2] = 2 | (1 << 2);    // UPをPUSH3、DOWNをPUSH4に割り当てる
                keyMapping.keyMap[3] = 2 | (1 << 2);    // UPをPUSH1、DOWNをPUSH2に割り当てる
                keyMapping.keyMap[4] = 0;
                break;
        }
    }
    return true;
}

bool
PD777::isPatternRawBitStream(const void* patternData, const size_t patternDataSize) const
{
    return (patternData != nullptr) && (patternDataSize == 0x4D0);
}

bool
PD777::setupCGRomRawBitStream(const void* patternData, const size_t patternDataSize)
{
    if(!isPatternRawBitStream(patternData, patternDataSize)) {
        return false;
    }

    for(auto& r : patternCGRom7x7) { r = 0; }
    for(auto& r : patternCGRom8x7) { r = 0; }
    for(auto& r : characterBent)   { r = 0xFF; }

    // 7x7
    u8* ptn = patternCGRom7x7;
    for(auto ptnNo = 0; ptnNo < 0x70; ++ptnNo) {
        if(((ptnNo & 0xF) == 7) || ((ptnNo & 0xF) == 0xF)) {
            continue; // 除外
        }
        for(int y = 0; y < 7; y++) {
            u8 ptnData = 0;
            for(int x = 0; x < 8; x++) {
                bool flag = getBit(patternData, 3 + x + 11 * y + (ptnNo * 77));
                if(flag) {
                    ptnData |= (0x80 >> x);
                }
            }
            *ptn++ = ptnData;
        }
    }
    // 8x7
    ptn = patternCGRom8x7;
    for(auto ptnNo = 0x70; ptnNo < 0x80; ++ptnNo) {
        if(((ptnNo & 0xF) == 7) || ((ptnNo & 0xF) == 0xF)) {
            continue; // 除外
        }
        for(int y = 0; y < 7; y++) {
            u8 ptnData = 0;
            for(int x = 0; x < 8; x++) {
                bool flag = getBit(patternData, 3 + x + 11 * y + (ptnNo * 77));
                if(flag) {
                    ptnData |= (0x80 >> x);
                }
            }
            *ptn++ = ptnData;
        }
    }

    // ベントパターンはカセットにワイヤードで実装してある……
    if(const auto cassetteNumber = getCassetteNumber(); cassetteNumber > 0) {
        const u8* bentData = characterBentData[cassetteNumber];
        for(int i = 0; i < sizeof(characterBent) / sizeof(characterBent[0]); ++i) {
            characterBent[i] = bentData[i];
            if(bentData[i] == 0xFF) {
                characterBent[i + 1] = bentData[i + 1];
                break;
            }
        }
    } else {
        for(auto& r : characterBent) { r = 0xFF; }
    }
    return true;
}

bool
PD777::isPatternFormatted(const void* patternData, const size_t patternDataSize) const
{
    if(patternData == nullptr) {
        return false;
    }
    if(patternDataSize != 0x340) {
        return false;
    }

    // マジックナンバー + バージョン
    const u8* header = (const u8*)patternData;
    c8 magicNumber[] = u8"*CassetteVision*0000";
    for(auto i = 0; i < 20; ++i) {
        if(header[i] != magicNumber[i]) return false;
    }
    return true;
}

bool
PD777::setupCGRomFormatted(const void* patternData, const size_t patternDataSize)
{
    if(!isPatternFormatted(patternData, patternDataSize)) {
        return false;
    }

    for(auto& r : patternCGRom7x7) { r = 0; }
    for(auto& r : patternCGRom8x7) { r = 0; }
    for(auto& r : characterBent)   { r = 0xFF; }

    // ヘッダ部分
    {
        // マジックナンバー + バージョン
        const u8* header = (const u8*)patternData;
        c8 magicNumber[] = u8"*CassetteVision*0000";
        for(auto i = 0; i < 20; ++i) {
            if(header[i] != magicNumber[i]) return false;
        }
        // ベント
        auto index = 0;
        const u8* bent = header + 0x20;
        for(auto i = 0; i < 16; ++i) {
            if(bent[i] == 0xFF) {
                break;
            }
            for(auto j = 0; j < 7; ++j) {
                characterBent[index++] = bent[i] + j;
                characterBent[index++] = 0x0C;
            }
        }
        characterBent[index++] = 0xFF;
        characterBent[index++] = 0;
    }
    // パターン部分
    {
        const u8* ptn = ((const u8*)patternData + 0x030);
        for(auto i = 0; i < 686; ++i) { patternCGRom7x7[i] = *ptn++; }
        for(auto i = 0; i <  98; ++i) { patternCGRom8x7[i] = *ptn++; }
    }
    return true;
}

bool
PD777::isPatternRaw(const void* patternData, const size_t patternDataSize) const
{
    return patternData == nullptr;
}

bool
PD777::setupPatternRaw(const void* patternData, const size_t patternDataSize)
{
    if(!isPatternRaw(patternData, patternDataSize)) {
        return false;
    }

    for(auto i = 0; i < 686; ++i) { patternCGRom7x7[i] = patternRom[i]; }
    for(auto i = 0; i <  98; ++i) { patternCGRom8x7[i] = patternRom8[i]; }
    for(auto i = 0; i < 256; ++i) { characterBent[i]   = characterAttribute[i]; }
    return true;
}

bool
PD777::isPatternData(const void* patternData, const size_t patternDataSize) const
{
    if(isPatternFormatted(patternData, patternDataSize)) {
        return true;
    } else if(isPatternRawBitStream(patternData, patternDataSize)) {
        return true;
    } else if(isPatternRaw(patternData, patternDataSize)) {
        return true;
    } else {
        return false; // 不明なフォーマット
    }
}

bool
PD777::setupPattern(const void* patternData, size_t patternDataSize)
{
    if(isPatternFormatted(patternData, patternDataSize)) {
        return setupCGRomFormatted(patternData, patternDataSize);
    } else if(isPatternRawBitStream(patternData, patternDataSize)) {
        return setupCGRomRawBitStream(patternData, patternDataSize);
    } else if(isPatternRaw(patternData, patternDataSize)) {
        return setupPatternRaw(patternData, patternDataSize);
    } else {
        // 不明なフォーマット
        return false;
    }
}

bool
PD777::setupConfig(const PD777Config& config777)
{
    config = config777;

    for(auto i = 0; i < 64; ++i) {
        tblBGtoRGB[i] = config777.BGColor[i];
    }
    for(auto i = 0; i < 16; ++i) {
        tblSpriteToRGB[i] = config777.SpriteColor[i];
    }
    return true;
}
