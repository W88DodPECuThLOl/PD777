#pragma once

#include "cat/misc/catSimpleParserINI.h"
#include "../core/PD777.h"
#include <string>

class WinPD777Config : public cat::SimpleParserINI {
    /**
     * @brief コンフィグ
     */
    PD777Config config;

    /**
     * @brief 解析中のセクション名
     */
    std::string currentSectionName;
protected:
    /**
     * @brief   セクションがあったときに呼び出される
     * 
     * @param[in]   beginSectionName    セクション名の開始ポインタ
     * @param[in]   endSectionName      セクション名の終了ポインタ
     * @return  処理を続けるかどうか
     * @retval  true:  処理を続ける
     * @retval  false: 処理をやめる
     * @note 文字列の範囲は[beginSectionName endSectionName)でendSectionNameを含まない
     */
    virtual bool doSection(const c8* beginSectionName, const c8* endSectionName) override;
    /**
     * @brief   パラメータがあったときに呼び出される
     * 
     * @param[in]   beginParameterName      パラメータ名の開始ポインタ
     * @param[in]   endParameterName        パラメータ名の終了ポインタ
     * @param[in]   beginParameter          パラメータの開始ポインタ
     * @param[in]   endParameter            パラメータの終了ポインタ
     * @return  処理を続けるかどうか
     * @retval  true:  処理を続ける
     * @retval  false: 処理をやめる
     * @note 文字列の範囲は[beginParameterName endParameterName)でendParameterNameを含まない
     */
    virtual bool doParameter(const c8* beginParameterName, const c8* endParameterName, const c8* beginParameter, const c8* endParameter) override;
public:
    WinPD777Config();
    const PD777Config getConfig777() const noexcept { return config; }
};
