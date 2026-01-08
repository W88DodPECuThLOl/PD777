#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>
#include <optional>
#include <regex>

#include <stdio.h>
#if defined(_WIN32) && !defined(_XBOX)
#include <windows.h>
#endif
#include "libretro.h"

#include "LibretroPD777.h"
#include "../core/catLowBasicTypes.h"
#include "cat/catAudio.h"

#define VIDEO_WIDTH 375
#define VIDEO_HEIGHT 240
#define VIDEO_PIXELS VIDEO_WIDTH * VIDEO_HEIGHT

static uint8_t *frame_buf;
static struct retro_log_callback logging;
static retro_log_printf_t log_cb;
static bool use_audio_cb;
static float last_aspect;
static float last_sample_rate;
char retro_base_directory[4096];
char retro_game_path[4096];

/**
 * K1  K2  K3  K4  K5  K6  K7
 * ----------------------------------------
 * STA L1L L1R SEL AUX 6   7    | [A08]
 * 1   L2L L2R 4   5   6   7    | [A09]
 * 1   2   3   4   5   P4  P3   | [A10]
 * 1   2   3   4   5   P2  P1   | [A11]
 * LL  L   C   R   RR  6   7    | [A12]
 */
enum class KeyStateIndex : s32 {
    A08 = 0,
    A09 = 1,
    A10 = 2,
    A11 = 3,
    A12 = 4,

    KeyStateIndexMax
};

enum class KeyMappingIndex : s32 {
    A8  = 0,
    A9,
    A10,
    A11,
    A12,

    B9,
    B10,
    B11,
    B12,
    B13,
    B14,
    B15,

    KeyMappingIndexMax
};

namespace {
    LibretroPD777* cpu = nullptr;
    CatAudio* catAudio = nullptr;
} // namespace

extern "C" {
    void initialize();
    void terminate();
    void reset();
    /**
     * @brief CPUを実行する
     * @param[in] clock 実行するクロック数
     */
    void execute(const s32 clock);
    bool setupCode(const void* data, const size_t dataSize);
    bool setupPattern(const void* data, const size_t dataSize);
    bool setupAuto(const void* data, const size_t dataSize);
    bool isVRAMDirty();
    void setVRAMDirty();
    const void* getVRAMImage();

    void clearKeyStatus();
    void setKeyStatus(const KeyStateIndex keyStateIndex, const u8 value);
    void setKeyMapping(const KeyMappingIndex keyMappingIndex, const u8 value);

    void* memoryAllocate(const s32 size);
    void memoryFree(u8* ptr);
} // extern "C"

static void fallback_log(enum retro_log_level level, const char *fmt, ...)
{
    (void)level;
    va_list va;
    va_start(va, fmt);
    vfprintf(stderr, fmt, va);
    va_end(va);
}


static retro_environment_t environ_cb;

void initialize()
{
    if(catAudio) [[unlikely]] {
        delete catAudio;
        catAudio = nullptr;
    }
    catAudio = new CatAudio();
    catAudio->init();

    if(cpu) [[unlikely]] {
        terminate();
    }
    cpu = new LibretroPD777();
    cpu->init();
    cpu->catAudio = catAudio; // TODO (mittonk): Not public

}

void retro_init(void)
{
    frame_buf = (uint8_t*)malloc(VIDEO_PIXELS * sizeof(uint32_t));
    const char *dir = NULL;

    initialize();

    if (environ_cb(RETRO_ENVIRONMENT_GET_SYSTEM_DIRECTORY, &dir) && dir)
    {
        snprintf(retro_base_directory, sizeof(retro_base_directory), "%s", dir);
    }

}

void terminate()
{
    if(cpu) [[likely]] {
        cpu->term();
        delete cpu;
        cpu = nullptr;
    }

    if (catAudio) [[likely]] {
        delete catAudio;
        catAudio = nullptr;
    }
}

void retro_deinit(void)
{
    free(frame_buf);
    frame_buf = NULL;

    terminate();
}

unsigned retro_api_version(void)
{
    return RETRO_API_VERSION;
}

void retro_set_controller_port_device(unsigned port, unsigned device)
{
    log_cb(RETRO_LOG_INFO, "Plugging device %u into port %u.\n", device, port);
}

void retro_get_system_info(struct retro_system_info *info)
{
    memset(info, 0, sizeof(*info));
    info->library_name     = "PD777";
    info->library_version  = "1.00.00";
    info->need_fullpath    = true;
    info->valid_extensions = "";
}

retro_video_refresh_t video_cb;
retro_audio_sample_t audio_cb;
static retro_audio_sample_batch_t audio_batch_cb;
static retro_input_poll_t input_poll_cb;
static retro_input_state_t input_state_cb;

void retro_get_system_av_info(struct retro_system_av_info *info)
{
    float aspect                = 0.0f;
    float sampling_rate         = 30000.0f;


    info->geometry.base_width   = VIDEO_WIDTH;
    info->geometry.base_height  = VIDEO_HEIGHT;
    info->geometry.max_width    = VIDEO_WIDTH;
    info->geometry.max_height   = VIDEO_HEIGHT;
    info->geometry.aspect_ratio = aspect;

    last_aspect                 = aspect;
    last_sample_rate            = sampling_rate;
}

void retro_set_environment(retro_environment_t cb)
{
    environ_cb = cb;

    if (cb(RETRO_ENVIRONMENT_GET_LOG_INTERFACE, &logging))
        log_cb = logging.log;
    else
        log_cb = fallback_log;

    static const struct retro_controller_description controllers[] = {
        { "Cassette Vision controller", RETRO_DEVICE_SUBCLASS(RETRO_DEVICE_ANALOG, 0) },
    };

    static const struct retro_controller_info ports[] = {
        { controllers, 1 },
        { NULL, 0 },
    };

    cb(RETRO_ENVIRONMENT_SET_CONTROLLER_INFO, (void*)ports);
}

void retro_set_audio_sample(retro_audio_sample_t cb)
{
    audio_cb = cb;
}

void retro_set_audio_sample_batch(retro_audio_sample_batch_t cb)
{
    audio_batch_cb = cb;
}

void retro_set_input_poll(retro_input_poll_t cb)
{
    input_poll_cb = cb;
}

void retro_set_input_state(retro_input_state_t cb)
{
    input_state_cb = cb;
}

void retro_set_video_refresh(retro_video_refresh_t cb)
{
    video_cb = cb;
}

// TODO (mittonk): Push down to catAudio.
unsigned phase;

void reset()
{
    if(cpu) [[likely]] {
        cpu->reset();
    }
}

void retro_reset(void)
{
    reset();
}

static void update_input(void)
{
    /**
     * K1  K2  K3  K4  K5  K6  K7
     * ----------------------------------------
     * STA L1L L1R SEL AUX 6   7    | [A08]
     * 1   L2L L2R 4   5   6   7    | [A09]
     * 1   2   3   4   5   P4  P3   | [A10]
     * 1   2   3   4   5   P2  P1   | [A11]
     * LL  L   C   R   RR  6   7    | [A12]
     */
    KeyStatus *keyStatus = cpu->getKeyStatus();
    keyStatus->clear();
    keyStatus->setCourseSwitch(cpu->getCourseSwitch());

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
            keyStatus->setGameStartKey();
        if (input_state_cb((pad), RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_SELECT))
            keyStatus->setGameSelectKey();
        if (input_state_cb((pad), RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_R))
            keyStatus->setAux();
        if (input_state_cb((pad), RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_X))
            keyStatus->setPush1();
        if (input_state_cb((pad), RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_B))
            keyStatus->setPush2();
        if (input_state_cb((pad), RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_Y))
            keyStatus->setPush3();
        if (input_state_cb((pad), RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_A))
            keyStatus->setPush4();

        // Up and Down do not exist on the actual device; they get remapped for convenience.
        if (input_state_cb((pad), RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_UP))
            keyStatus->setUp();
        if (input_state_cb((pad), RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_DOWN))
            keyStatus->setDown();

        // Use D-pad Up and Down to control the Course Switch, used for things
        // like aiming pitching in New Baseball.
        // メモ）コーススイッチをデジタルパッドの上下で切り替える
        {
            u8 courseSwitch = cpu->getCourseSwitch();
            if (input_state_cb((pad), RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_UP)) {
                if(courseSwitch < 5) {
                    courseSwitch++;
                    cpu->setCourseSwitch(courseSwitch);
                }
            }
            if (input_state_cb((pad), RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_DOWN)) {
                if(courseSwitch > 1) {
                    courseSwitch--;
                    cpu->setCourseSwitch(courseSwitch);
                }
            }
        }
    }

    // First controller gets left two paddles, for 1-player analog games.  Also
    // lever switch 1, heavily used in 1-player games and for player 1 of
    // 2-player games.
    pad = 0;
    cpu->analogStatus.input_analog_left_x[pad] = input_state_cb( (pad), RETRO_DEVICE_ANALOG, RETRO_DEVICE_INDEX_ANALOG_LEFT,
            RETRO_DEVICE_ID_ANALOG_X);

    cpu->analogStatus.input_analog_left_y[pad] = input_state_cb( (pad), RETRO_DEVICE_ANALOG, RETRO_DEVICE_INDEX_ANALOG_LEFT,
            RETRO_DEVICE_ID_ANALOG_Y);

    if (input_state_cb((pad), RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_LEFT))
        keyStatus->setLeverSwitch1Left();
    if (input_state_cb((pad), RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_RIGHT))
        keyStatus->setLeverSwitch1Right();

    // Second controller gets right two paddles, for most 2-player analog games.
    // Also lever switch 2.
    pad = 1;
    cpu->analogStatus.input_analog_left_x[pad] = input_state_cb( (pad), RETRO_DEVICE_ANALOG, RETRO_DEVICE_INDEX_ANALOG_LEFT,
            RETRO_DEVICE_ID_ANALOG_X);

    cpu->analogStatus.input_analog_left_y[pad] = input_state_cb( (pad), RETRO_DEVICE_ANALOG, RETRO_DEVICE_INDEX_ANALOG_LEFT,
            RETRO_DEVICE_ID_ANALOG_Y);

    if (input_state_cb((pad), RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_LEFT))
        keyStatus->setLeverSwitch2Left();
    if (input_state_cb((pad), RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_RIGHT))
        keyStatus->setLeverSwitch2Right();

}


static void check_variables(void)
{

}

static void audio_callback(void)
{
    catAudio->present();

}

static void audio_set_state(bool enable)
{
    (void)enable;
}

void retro_run(void)
{
    update_input();

    s32 clock = 100000;  // TODO (mittonk)
    if(cpu) [[likely]] {
        for(s32 i = 0; i < clock; ++i) {
            cpu->execute();
        }
    }

    bool updated = false;
    if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE_UPDATE, &updated) && updated)
        check_variables();
}

bool setupAuto(const void* data, const size_t dataSize)
{
    if(setupPattern(data, dataSize)) {
        return true; // 成功
    }
    if(setupCode(data, dataSize)) {
        reset();
        return true; // 成功
    }
    return false;
}

bool setupCode(const void* data, const size_t dataSize)
{
    return cpu && cpu->setupCode(data, dataSize);
}

bool setupPattern(const void* data, const size_t dataSize)
{
    return cpu && cpu->setupPattern(data, dataSize);
}

bool isVRAMDirty()
{
    return cpu && cpu->isVRAMDirty();
}

void setVRAMDirty()
{
    if(cpu) [[likely]] {
        cpu->setVRAMDirty();
    }
}

const void* getVRAMImage()
{
    if(!cpu->isVRAMDirty()) {
        return nullptr;
    }
    cpu->resetVRAMDirty();
    return cpu->getImage();
}


std::optional<std::vector<u8>> loadBinaryFile(const std::string& filename)
{
    if(filename.empty()) {
        return std::nullopt;
    }
    if(std::unique_ptr<FILE, decltype(&fclose)> fp(fopen(filename.c_str(), "rb"), &fclose); fp) [[likely]] {
        if(fseek(fp.get(), 0, SEEK_END) == 0) [[likely]] {
            if(auto fileSize = ftell(fp.get()); fileSize > 0) [[likely]] {
                if(fseek(fp.get(), 0, SEEK_SET) == 0) [[likely]] {
                    std::vector<u8> data(fileSize);
                    if(fread(data.data(), 1, data.size(), fp.get()) == data.size()) [[likely]] {
                        return std::move(data);
                    }
                }
            }
        }
    }
    return std::nullopt;
}


bool retro_load_game(const struct retro_game_info *info)
{
    struct retro_input_descriptor desc[] = {
        // Controller 2 gets Levers Switch 2 left/right, not easy to summarize here.
        { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_LEFT,  "Lever Switch 1 Left" },
        { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_UP,    "Course Switch Up" },
        { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_DOWN,  "Course Switch Down" },
        { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_RIGHT, "Lever Switch 1 Right" },
        { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_SELECT, "Select" },
        { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_START,  "Start" },
        { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_A,  "Push4" },
        { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_B,  "Push2" },
        { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_X,  "Push1" },
        { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_Y,  "Push3" },
        { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_R,  "AUX" },

        // Controller 2 gets Paddle 3 and 4, not easy to summarize here.
        { 0, RETRO_DEVICE_ANALOG, RETRO_DEVICE_INDEX_ANALOG_LEFT, RETRO_DEVICE_ID_ANALOG_X, "Paddle 2" },
        { 0, RETRO_DEVICE_ANALOG, RETRO_DEVICE_INDEX_ANALOG_LEFT, RETRO_DEVICE_ID_ANALOG_Y, "Paddle 1" },

        { 0 },
    };

    environ_cb(RETRO_ENVIRONMENT_SET_INPUT_DESCRIPTORS, desc);

    enum retro_pixel_format fmt = RETRO_PIXEL_FORMAT_XRGB8888;
    if (!environ_cb(RETRO_ENVIRONMENT_SET_PIXEL_FORMAT, &fmt))
    {
        log_cb(RETRO_LOG_INFO, "XRGB8888 is not supported.\n");
        return false;
    }

    // TODO (mittonk): Support zip-files containing code and pattern together.

    snprintf(retro_game_path, sizeof(retro_game_path), "%s", info->path);

    std::optional<std::vector<u8>> code_data = loadBinaryFile(info->path);
    if (code_data.has_value())
        setupCode(code_data.value().data(), code_data.value().size());
    else {
        struct retro_message message;
        message.msg = "ROM file not found or damaged.  Using data compiled from rom.cpp instead!";
        message.frames = 60;
        environ_cb(RETRO_ENVIRONMENT_SET_MESSAGE, &message);
    }

    // Try to load a similarly-named pattern file.
    std::string pattern_path = info->path;
    std::string toReplace = "bin777";
    std::string replaceWith = "ptn777";
    std::size_t pos = pattern_path.find(toReplace);
    if (pos != std::string::npos) { // Check if the substring was found
        pattern_path.replace(pos, toReplace.length(), replaceWith);
    }
    std::optional<std::vector<u8>> pattern_data = loadBinaryFile(pattern_path);
    if (pattern_data.has_value())
        setupPattern(pattern_data.value().data(), pattern_data.value().size());
    else {
        struct retro_message message;
        message.msg = "Pattern file not found or damaged.  Using data compiled from rom.cpp instead!";
        message.frames = 60;
        environ_cb(RETRO_ENVIRONMENT_SET_MESSAGE, &message);
    }


    struct retro_audio_callback audio_cb = { audio_callback, audio_set_state };
    use_audio_cb = environ_cb(RETRO_ENVIRONMENT_SET_AUDIO_CALLBACK, &audio_cb);


    check_variables();

    (void)info;
    return true;
}

void retro_unload_game(void)
{

}

unsigned retro_get_region(void)
{
    return RETRO_REGION_NTSC;
}

bool retro_load_game_special(unsigned type, const struct retro_game_info *info, size_t num)
{
    return false;
}

size_t retro_serialize_size(void)
{
    return 0;
}

bool retro_serialize(void *data_, size_t size)
{
    return false;
}

bool retro_unserialize(const void *data_, size_t size)
{
    return false;
}

void* memoryAllocate(const s32 size)
{
    return new u8[size];
}


void memoryFree(u8* ptr)
{
    delete[] ptr;
}

void *retro_get_memory_data(unsigned id)
{
    (void)id;
    return NULL;
}

size_t retro_get_memory_size(unsigned id)
{
    (void)id;
    return 0;
}

void retro_cheat_reset(void)
{}

void retro_cheat_set(unsigned index, bool enabled, const char *code)
{
    (void)index;
    (void)enabled;
    (void)code;
}

