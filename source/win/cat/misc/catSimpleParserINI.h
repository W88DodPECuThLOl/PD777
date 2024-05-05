/**
 * @file    catSimpleParserINI.h
 * @brief   INIファイルの簡易パーサー
 */
#pragma once

#include "../../../core/catLowBasicTypes.h"

namespace cat {

class SimpleParserINI {
    static constexpr c8 CR = u8'\r';
    static constexpr c8 LF = u8'\n';
    static constexpr c8 SP = u8' ';
    static constexpr c8 TAB = u8'\t';
    static constexpr c8 COMMENT = u8';';
    static constexpr c8 PARAM_SEPARATOR = u8'=';
    static constexpr c8 SECTION_START = u8'[';
    static constexpr c8 SECTION_END = u8']';
    static constexpr c8 DOUBLE_QUAUT = u8'"';
    static constexpr c8 SINGLE_QUAUT = u8'\'';

    inline bool chechNewLineAndProgress(const c8* end, const c8*& ch) const noexcept {
        if(ch == end) [[unlikely]] { return false; }
        if(*ch == CR) {
            ch++;
            if((ch == end) && (ch[1] == LF)) {
                // CR+LF
                ch++;
                return true;
            }
            return true;
        }
        return *ch++ == LF;
    }
    /**
     * @brief 空白文字かどうか
     * 
     * @return  空白文字かどうか
     * @retval  true:   空白文字
     * @retval  false:  空白文字ではない
     * @note    空白文字は、スペース、タブ、改行。
     */
    inline bool isSpace(const c8 ch) const noexcept {
        return (ch == SP) || (ch == TAB) || (ch == CR) || (ch == LF);
    }
protected:
    template<typename T>
    void ltrim(const T*& start, const T*& end) { while((start != end) && (*start  == SP || *start  == TAB)) { start++; } }
    template<typename T>
    void rtrim(const T*& start, const T*& end) { while((start != end) && (end[-1] == SP || end[-1] == TAB)) { end--;   } }
    template<typename T>
    void trim(const T*& start, const T*& end) {
        ltrim(start, end);
        rtrim(start, end);
    }
    template<typename T>
    size_t stringLength(const T* start, const T* end) { return end - start; }
    template<typename T>
    bool removeQuaut(const T*& start, const T*& end, const T character) {
        if(stringLength(start, end) <= 1) [[unlikely]] { return false; }
        if(*start == character && end[-1] == character) {
            start++;
            end--;
            return true;
        }
        return false;
    }
    template<typename T>
    bool removeQuaut(const T*& start, const T*& end) {
        if(removeQuaut(start, end, DOUBLE_QUAUT)) { return true; }
        return removeQuaut(start, end, SINGLE_QUAUT);
    }
protected:
    /**
     * @brief   セクションがあったときに呼び出される
     * 
     * @param[in]   beginSectionName    セクション名の開始ポインタ
     * @param[in]   endSectionName      セクション名の終了ポインタ
     * @return  処理を続けるかどうか
     * @retval  true:  処理を続ける
     * @retval  false: 処理をやめて失敗させる
     * @note 文字列の範囲は[beginSectionName endSectionName)でendSectionNameを含まない
     */
    virtual bool doSection(const c8* beginSectionName, const c8* endSectionName) { return true; }
    /**
     * @brief   パラメータがあったときに呼び出される
     * 
     * @param[in]   beginParameterName      パラメータ名の開始ポインタ
     * @param[in]   endParameterName        パラメータ名の終了ポインタ
     * @param[in]   beginParameter          パラメータの開始ポインタ
     * @param[in]   endParameter            パラメータの終了ポインタ
     * @return  処理を続けるかどうか
     * @retval  true:  処理を続ける
     * @retval  false: 処理をやめて失敗させる
     * @note 文字列の範囲は[beginParameterName endParameterName)でendParameterNameを含まない
     */
    virtual bool doParameter(const c8* beginParameterName, const c8* endParameterName, const c8* beginParameter, const c8* endParameter) { return true; }
public:
    SimpleParserINI() = default;
    virtual ~SimpleParserINI() = default;

    /**
     * @brief INIファイルをパースする
     * 
     * - セミコロン以降はコメントになる。
     * - セクションがあったらdoSection()を呼び出す。
     * - パラメータがあったらdoParameter()を呼び出す。
     * - 前後のスペースなども含まれるので、適時トリムして使用する
     *   例）[a bcde   ]
     *       みたいになっていたら"a bcde   "となる
     *   例）Param1   =   a
     *       みたいになっていたら"Param1   "、"   a"となる。
     *   例）Param 2= 2        ; comment
     *       みたいになっていたら"Param 2"、" 2        "となる。
     * - ファイル先頭のBOMはスキップされる
     * 
     * @param[in]   data        データ
     * @param[in]   dataSize    データサイズ
     * @return  処理結果
     * @retval  true: 成功
     * @retval  false: 失敗
     */
    virtual bool parse(const void* data, const size_t dataSize) {
        if(data == nullptr) [[unlikely]] {
            return false;
        }
        auto p = (const c8*)data;
        auto end = p + dataSize;

        if((dataSize >= 3) && (p[0] == 0xEF) && (p[1] == 0xBB) && (p[2] == 0xBF)) {
            p += 3; // BOMをスキップ
        }
        while((p != end) && isSpace(*p)) { ++p; }
        while(*p && p != end) {
            if(*p == SECTION_START) {
                // セクション
                auto beginSection = ++p;
                // セクションの終わりを探す
                if(p == end) [[unlikely]] { return false; }
                while(*p != SECTION_END) {
                    if((*p == 0) || (*p == CR) || (*p == LF)) [[unlikely]] {
                        return false; // 途中で終わってしまった
                    }
                    ++p;
                    if(p == end) [[unlikely]] { return false; }
                }
                if(!doSection(beginSection, p)) [[unlikely]] { return false; }
                ++p;
                while((p != end) && *p && !chechNewLineAndProgress(end, p)) {} // 行末までスキップ
            } else if(*p == COMMENT) {
                // コメント
                while((p != end) && *p && !chechNewLineAndProgress(end, p)) {} // 行末までスキップ
            } else {
                // パラメータ
                auto beginParameterName = p;
                while(*p != PARAM_SEPARATOR) {
                    if(*p == 0 || *p == CR || *p == LF) [[unlikely]] {
                        return false; // 途中で終わってしまった
                    }
                    ++p;
                    if(p == end) [[unlikely]] { return false; }
                }
                auto endParameterName = p;
                auto beginParameter = ++p;
                while((p != end) && *p && (*p != COMMENT) && (*p != CR) && (*p != LF)) { p++; }
                if(!doParameter(beginParameterName, endParameterName, beginParameter, p)) [[unlikely]] {
                    return false;
                }
            }
            while((p != end) && isSpace(*p)) { ++p; }
        }
        return true;
    }
};

} // namespace cat
