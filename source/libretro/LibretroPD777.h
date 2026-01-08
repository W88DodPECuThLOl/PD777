#pragma once

#include "../core/catLowBasicTypes.h"
#include "../core/PD777.h"
#include "cat/catAudio.h"

class AnalogStatus {
    public:
        static const auto NUM_CONTROLLERS = 2;
        int32_t input_analog_left_x[NUM_CONTROLLERS];
        int32_t input_analog_left_y[NUM_CONTROLLERS];
};

/**
 * @brief Libretro用のμPD777
 */
class LibretroPD777 : public PD777 {
    public: // TODO (mittonk)
        /**
         * @brief 画面イメージの横幅
         */
        static constexpr s32 WIDTH = 75 * PD777::dotWidth;
        /**
         * @brief 画面イメージの高さ
         */
        static constexpr s32 HEIGHT = 60 * PD777::dotHeight;

        CatAudio* catAudio; // TODO (mittonk): Private and initializer.

        AnalogStatus analogStatus; // TODO (mittonk): Dynamically allocated?
        f32 padValue[4] = {};

        // TODO (mittonk): Tunable?  Core option?
        static constexpr float analogscale = 32768.0f * 20.0f;

        /**
         * @brief 画面イメージが更新されているかどうか
         *      @arg true:  更新されている
         *      @arg false: 更新されてない
         */

        /**
         * @brief コーススイッチ（1～5）
         */
        u8 courseSwitch = 3;
        u8 getCourseSwitch() const { return courseSwitch; }
        /**
         * @brief コーススイッチを設定する
         * @param[in]   courseSwitch    コーススイッチ（1～5）
         */
        void setCourseSwitch(const u8 courseSwitch) { this->courseSwitch = courseSwitch; }
    private:
        bool bVRAMDirty;
        /**
         * @brief 画面イメージ(A8R8G8B8フォーマット)
         */
        u32 image[WIDTH * HEIGHT];

        /**
         * @brief キー入力の状態
         */
        KeyStatus keyStatus;

    public:
        LibretroPD777();
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
        //    virtual bool isGunPortLatch(u8& value) override;
        virtual void readKIN(KeyStatus& key) override;
    public:
        //    virtual void registerDump() override;

        /**
         * @brief リセットする
         */
        void reset();
        /**
         * @brief 画面イメージの更新状態をセットする
         */
        void setVRAMDirty() noexcept { bVRAMDirty = true; }
        /**
         * @brief 画面イメージの更新状態をリセットする
         */
        void resetVRAMDirty() noexcept { bVRAMDirty = false; }
        /**
         * @brief 画面イメージが更新されているかどうか
         * @return 画面イメージの更新状態
         * @retval  true:  更新されている
         * @retval  false: 更新されてない
         */
        bool isVRAMDirty() const noexcept { return bVRAMDirty; }
        /**
         * @brief 画面イメージを取得する
         * @return 画面イメージ(A8R8G8B8フォーマット)
         */
        const void* getImage() const noexcept { return (void*)image; }

        /**
         * @brief キー入力の状態を取得する
         * @return キー入力の状態
         */
        KeyStatus* getKeyStatus() noexcept { return &keyStatus; }
        /**
         * @brief キーマッピングを取得する
         * @return キーマッピング
         */
        KeyMapping* getkeyMapping() noexcept { return &keyMapping; }
};
