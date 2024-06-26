﻿#pragma once

#include "../core/PD777.h"
#include "../core/catLowBasicTypes.h"
#include <Windows.h>
#include <vector>
#include <optional>

class WinPD777 : public PD777 {
    class WinImage* image;
    bool finished = false;

    /**
     * @brief キー入力の状態
     */
    KeyStatus keyStatus;

    /**
     * @brief キーボードの状態
     */
    u8 keyboardState[256];

    /**
     * @brief パドル1～パドル4の値
     */
    f32 padValue[4] = {};

    /**
     * @brief 光線銃
     */
    struct Gun {
        /**
         * @brief 光線銃が有効かどうか
         */
        bool enableGun = false;

        /**
         * @brief 的に当たっているかどうか
         */
        bool hit = false;

        /**
         * @brief 照準を描画するかどうか
         */
        bool drawGunTarget = true;

        /**
         * @brief 光線銃が有効かどうか
         * @return  光線銃が有効の場合trueを返す
         */
        bool isEnableGun() const { return enableGun; }
        void setEnableGun() { enableGun = true; }

        /**
         * @brief 光線銃の照準を描画するかどうか
         * @return  描画する場合trueを返す
         */
        bool isDrawGunTarget() const { return drawGunTarget; }
        void setDrawGunTarget(const bool drawGunTarget) { this->drawGunTarget = drawGunTarget; }

        /**
         * @brief 光線銃の照準位置のX座標を取得する
         * @return  光線銃の照準位置のX座標
         */
        s32 getGunX();
        /**
         * @brief 光線銃の照準位置のY座標を取得する
         * @return  光線銃の照準位置のY座標
         */
        s32 getGunY();

        /**
         * @brief 光線銃を撃ったかどうか
         * @return  光線銃を撃ったらtrueを返す
         */
        bool isFire();

        /**
         * @brief 的に当たったかどうか
         * @return  的に当たったらtrueを返す
         */
        bool isHit() const { return hit; }
        void setHit(const bool isHit) { this->hit = isHit; }
    };
    /**
     * @brief 光線銃
     */
    Gun gun;

    /**
     * @brief コーススイッチ（1～5）
     */
    u8 courseSwitch = 3;
    /**
     * @brief コーススイッチを取得する
     * @return コーススイッチ（1～5）
     */
    u8 getCourseSwitch() const { return courseSwitch; }
    void updateKey();
public:
    WinPD777();

    bool setup(const std::optional<std::vector<u8>>& codeData, const std::optional<std::vector<u8>>& cgData);
    bool targetDependentSetup(HWND hwnd);

    void onPaint(HWND hwnd);
    void onKey();
    bool isFinish() const { return finished; }
    void setFinish() { finished = true; }
    /**
     * @brief コーススイッチを設定する
     * @param[in]   courseSwitch    コーススイッチ（1～5）
     */
    void setCourseSwitch(const u8 courseSwitch) { this->courseSwitch = courseSwitch; }
protected:
    // grah
    virtual void present() override;
    // sound
    virtual void setFLS(const s64 clockCounter, const u8 value, const bool reverberatedSoundEffect) override;
    virtual void setFRS(const s64 clockCounter, const u8 value, const bool reverberatedSoundEffect) override;
    // input
    virtual bool isPD1(u8& value) override;
    virtual bool isPD2(u8& value) override;
    virtual bool isPD3(u8& value) override;
    virtual bool isPD4(u8& value) override;
    virtual bool isGunPortLatch(u8& value) override;
    virtual void readKIN(KeyStatus& key) override;
public:
    virtual void registerDump() override;
};
