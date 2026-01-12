#include <cstdio>
#include <string>

#include "LibretroPD777.h"
#include "libretro.h"

extern retro_video_refresh_t video_cb;
extern retro_input_state_t input_state_cb;
extern retro_environment_t environ_cb;

namespace {

    inline f32 clamp(f32 value, const f32 minValue, const f32 maxValue)
    {
        if(value < minValue) { return minValue; }
        if(value > maxValue) { return maxValue; }
        return value;
    }

} // namespace

LibretroPD777::LibretroPD777()
    : PD777()
      , bVRAMDirty(true)
{
    PD777::init();
    keyStatus.clear();
};


void LibretroPD777::present()
{
    const auto BACKGROUND = 0xFF000000;
    // フレームバッファからビットマップにコピー
    {
        // (10,0)-(84,59)の範囲をコピーして描画している
        const s32 offsetX = 10 * dotWidth;
        for(int y = 0; y < HEIGHT; y++) {
            for(int x = 0; x < WIDTH; x++) {
                const auto bgr = frameBuffer[x + offsetX + (y*frameBufferWidth)];
                const auto r = (bgr >> 16) & 0x0000FF;
                const auto b = (bgr << 16) & 0xFF0000;
                const auto g = (bgr      ) & 0x00FF00;
                const auto argb  = BACKGROUND | r | g | b;
                image[x + y * WIDTH] = argb;
                //image[x + y * WIDTH] = 0xFF44FF88;

            }
        }
    }

    // 光線銃の照準
    if(gun.isEnableGun()) {
        const s32 gunX = gun.getGunX();
        const s32 gunY = gun.getGunY();
        gun.setHit(image[gunX + gunY * WIDTH] != BACKGROUND);

        if(gun.isDrawGunTarget()) {
            for(int x = -2; x < 3; x++) {
                image[gunX + x + gunY * WIDTH] = 0xFF0000;
            }
            for(int y = -2; y < 3; y++) {
                image[gunX + (gunY + y) * WIDTH] = 0xFF0000;
            }
        }
    }
    int VIDEO_PITCH = WIDTH * 4; // TODO (mittonk)

    video_cb(reinterpret_cast<uint8_t*>(image), WIDTH, HEIGHT, VIDEO_PITCH);

    setVRAMDirty();
}

void LibretroPD777::readKIN(KeyStatus& key)
{
    key = keyStatus;
}

void LibretroPD777::updateKey(void) {
    /**
     * K1  K2  K3  K4  K5  K6  K7
     * ----------------------------------------
     * STA L1L L1R SEL AUX 6   7    | [A08]
     * 1   L2L L2R 4   5   6   7    | [A09]
     * 1   2   3   4   5   P4  P3   | [A10]
     * 1   2   3   4   5   P2  P1   | [A11]
     * LL  L   C   R   RR  6   7    | [A12]
     */
    keyStatus.clear();
    keyStatus.setCourseSwitch(getCourseSwitch());

    // The Cassette Vision had a bunch of central controls, rather than
    // per-player pads --- some games would asymmetrically assign most buttons
    // to Player 2, for example.  The following mapping should allow playing
    // pretty much all 1-player games with 1 pad, and all 2-player games with
    // 2 pads.
    auto NUM_CONTROLLERS = 2;
    // Both pads get access to most of the simple buttons.
    unsigned pad = 0;
    for (pad=0; pad<NUM_CONTROLLERS; pad++) {
        if (input_state_cb((pad), RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_START))
            keyStatus.setGameStartKey();
        if (input_state_cb((pad), RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_SELECT))
            keyStatus.setGameSelectKey();
        if (input_state_cb((pad), RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_R))
            keyStatus.setAux();
        if (input_state_cb((pad), RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_X))
            keyStatus.setPush1();
        if (input_state_cb((pad), RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_B))
            keyStatus.setPush2();
        if (input_state_cb((pad), RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_Y))
            keyStatus.setPush3();
        if (input_state_cb((pad), RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_A))
            keyStatus.setPush4();

        // Up and Down do not exist on the actual device; they get remapped for convenience.
        if (input_state_cb((pad), RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_UP))
            keyStatus.setUp();
        if (input_state_cb((pad), RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_DOWN))
            keyStatus.setDown();

        // Use D-pad Up and Down to control the Course Switch, used for things
        // like aiming pitching in New Baseball.
        // メモ）コーススイッチをデジタルパッドの上下で切り替える
        {
            u8 courseSwitch = getCourseSwitch();
            if (input_state_cb((pad), RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_UP)) {
                if(courseSwitch < 5) {
                    courseSwitch++;
                    setCourseSwitch(courseSwitch);
                    announceCourseSwitch(courseSwitch);
                }
            }
            if (input_state_cb((pad), RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_DOWN)) {
                if(courseSwitch > 1) {
                    courseSwitch--;
                    setCourseSwitch(courseSwitch);
                    announceCourseSwitch(courseSwitch);
                }
            }
        }
    }

    // First controller gets left two paddles, for 1-player analog games.  Also
    // lever switch 1, heavily used in 1-player games and for player 1 of
    // 2-player games.  Also lightgun.
    pad = 0;
    analogStatus.input_analog_left_x[pad] = input_state_cb( (pad), RETRO_DEVICE_ANALOG, RETRO_DEVICE_INDEX_ANALOG_LEFT,
            RETRO_DEVICE_ID_ANALOG_X);

    analogStatus.input_analog_left_y[pad] = input_state_cb( (pad), RETRO_DEVICE_ANALOG, RETRO_DEVICE_INDEX_ANALOG_LEFT,
            RETRO_DEVICE_ID_ANALOG_Y);

    if (input_state_cb((pad), RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_LEFT))
        keyStatus.setLeverSwitch1Left();
    if (input_state_cb((pad), RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_RIGHT))
        keyStatus.setLeverSwitch1Right();

    // Lightgun, controlled by a mouse / pointer / physical lightgun.
    {
        // TODO (mittonk): Tune offsets.
        /*
        int crop_overscan_h_left = 15;
        int crop_overscan_v_top = 0;
        int offset_x = (crop_overscan_h_left * 0x120) - 1;
        int offset_y = (crop_overscan_v_top * 0x133) + 1;
        */

        int offset_x = 0;
        int offset_y = 0;
        int offscreen;
        int offscreen_shot;
        int trigger;
        int mousedata[4] = {};

        offscreen = input_state_cb( pad, RETRO_DEVICE_LIGHTGUN, 0, RETRO_DEVICE_ID_LIGHTGUN_IS_OFFSCREEN );
        offscreen_shot = input_state_cb( pad, RETRO_DEVICE_LIGHTGUN, 0, RETRO_DEVICE_ID_LIGHTGUN_RELOAD );
        trigger = input_state_cb( pad, RETRO_DEVICE_LIGHTGUN, 0, RETRO_DEVICE_ID_LIGHTGUN_TRIGGER );

        if ( offscreen || offscreen_shot )
        {
            mousedata[0] = 0;
            mousedata[1] = 0;
        }
        else
        {
            int _x = input_state_cb( pad, RETRO_DEVICE_LIGHTGUN, 0, RETRO_DEVICE_ID_LIGHTGUN_SCREEN_X );
            int _y = input_state_cb( pad, RETRO_DEVICE_LIGHTGUN, 0, RETRO_DEVICE_ID_LIGHTGUN_SCREEN_Y );

            mousedata[0] = (_x + (0x7FFF + offset_x)) * LibretroPD777::WIDTH  / ((0x7FFF + offset_x) * 2);
            mousedata[1] = (_y + (0x7FFF + offset_y)) * LibretroPD777::HEIGHT  / ((0x7FFF + offset_y) * 2);
        }

        gun.gunX = mousedata[0];
        gun.gunY = mousedata[1];
        gun.fire = (trigger || offscreen_shot);

    }

    // Second controller gets right two paddles, for most 2-player analog games.
    // Also lever switch 2.
    pad = 1;
    analogStatus.input_analog_left_x[pad] = input_state_cb( (pad), RETRO_DEVICE_ANALOG, RETRO_DEVICE_INDEX_ANALOG_LEFT,
            RETRO_DEVICE_ID_ANALOG_X);

    analogStatus.input_analog_left_y[pad] = input_state_cb( (pad), RETRO_DEVICE_ANALOG, RETRO_DEVICE_INDEX_ANALOG_LEFT,
            RETRO_DEVICE_ID_ANALOG_Y);

    if (input_state_cb((pad), RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_LEFT))
        keyStatus.setLeverSwitch2Left();
    if (input_state_cb((pad), RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_RIGHT))
        keyStatus.setLeverSwitch2Right();
};

void LibretroPD777::announceCourseSwitch(u8 courseSwitch) {
    if (not announce_course_switch) {
        return;
    }
    struct retro_message message;
    char courseMessage[256];
    std::string courseFormatted;
    switch (courseSwitch) {
        case 1: courseFormatted = "LL"; break;
        case 2: courseFormatted = "L"; break;
        case 3: courseFormatted = "C"; break;
        case 4: courseFormatted = "R"; break;
        case 5: courseFormatted = "RR"; break;
        default: courseFormatted = "unknown"; break;
    }
    snprintf(courseMessage, sizeof(courseMessage), "Course Switch: %s", courseFormatted.c_str());
    message.msg = courseMessage;
    message.frames = 15;
    environ_cb(RETRO_ENVIRONMENT_SET_MESSAGE, &message);
};

void LibretroPD777::reset()
{
    crt.reset();
    stack.reset();
    regs.reset();
    sound.reset();
    gun.reset();
    calledVBLK = false;
} 

bool LibretroPD777::isPD1(u8& value) {
    padValue[0] = clamp(padValue[0] + float(analogStatus.input_analog_left_y[0]) / analogscale, KeyStatus::PAD_MIN_VALUE, KeyStatus::PAD_MAX_VALUE);
    value = (u8)((u8)padValue[0] & 0x7F);
    return true;
}

bool LibretroPD777::isPD2(u8& value)
{
    padValue[1] = clamp(padValue[1] + float(analogStatus.input_analog_left_x[0]) / analogscale, KeyStatus::PAD_MIN_VALUE, KeyStatus::PAD_MAX_VALUE);
    value = (u8)((u8)padValue[1] & 0x7F);
    return true;
}

bool LibretroPD777::isPD3(u8& value)
{
    padValue[2] = clamp(padValue[2] + float(analogStatus.input_analog_left_x[1]) / analogscale, KeyStatus::PAD_MIN_VALUE, KeyStatus::PAD_MAX_VALUE);
    value = (u8)((u8)padValue[2] & 0x7F);
    return true;
}

bool LibretroPD777::isPD4(u8& value)
{
    padValue[3] = clamp(padValue[3] + float(analogStatus.input_analog_left_y[1]) / analogscale, KeyStatus::PAD_MIN_VALUE, KeyStatus::PAD_MAX_VALUE);
    value = (u8)((u8)padValue[3] & 0x7F);
    return true;
}

s32 LibretroPD777::Gun::getGunX()
{
    /*s32 gamePadIndex = 0;
    cat::core::pad::GamePadState gamePadState;
    cat::core::pad::getPadState(gamePadIndex, &gamePadState);
    return clamp(gamePadState.analogs[0].x * WinImage::WIDTH * 0.5 + WinImage::WIDTH * 0.5, 5, WinImage::WIDTH - 5); */
    return gunX;
}

s32 LibretroPD777::Gun::getGunY()
{
    /*s32 gamePadIndex = 0;
    cat::core::pad::GamePadState gamePadState;
    cat::core::pad::getPadState(gamePadIndex, &gamePadState);
    return clamp(-gamePadState.analogs[0].y * WinImage::HEIGHT * 0.5 + WinImage::HEIGHT * 0.5, 5, WinImage::HEIGHT - 5); */
    return gunY;
}

bool LibretroPD777::Gun::isFire()
{
    /* s32 gamePadIndex = 0;
    cat::core::pad::GamePadState gamePadState;
    cat::core::pad::getPadState(gamePadIndex, &gamePadState);
    return (gamePadState.buttons & (cat::core::pad::ButtonMask::A | cat::core::pad::ButtonMask::B | cat::core::pad::ButtonMask::X | cat::core::pad::ButtonMask::Y)); */
    return fire;
}

bool LibretroPD777::isGunPortLatch(u8& value)
{
    // 呼び出されたら、光線銃を有効にする
    gun.setEnableGun();

    if(gun.isFire()) {
        // 撃った
        value = 0x02;
        if(gun.isHit()) {
            return true;    // 的に当たっていた
        } else {
            return false;   // 外れていた
        }
    } else {
        // 撃ってない
        value = 0x00;
        return false;
    }
}

void LibretroPD777::setFLS(const s64 clockCounter, const u8 value, const bool reverberatedSoundEffect)
{
    // 再生する音の周波数は「15734.0 / (FLS - 1)」(2以上) で計算できる。

    catAudio->writeFLS(value, reverberatedSoundEffect);
    // TODO (mittonk): clockCounter?
}

void LibretroPD777::setFRS(const s64 clockCounter, const u8 value, const bool reverberatedSoundEffect)
{
    // TODO (mittonk): Audio processor.
    catAudio->writeFRS(value, reverberatedSoundEffect);
}
