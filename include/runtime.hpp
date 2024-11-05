#ifndef ANVIL_GAMEENGINE_RUNTIME_HPP_INCLUDE_HEADER
#define ANVIL_GAMEENGINE_RUNTIME_HPP_INCLUDE_HEADER

#include <GLFW/glfw3.h>
#include <AL/al.h>
#include <AL/alc.h>
#include <chrono>
#include <cstdint>
#include <functional>
#include <map>
#include <mutex>
#include <memory>
#include <string>
#include <sys/types.h>
#include <thread>
#include <unordered_map>
#include <vector>
#include "stb_truetype.h"

/// @brief enables support for compute shaders
/// @note requires opengl 4.3 or newer which is not supported on some legacy systems
/*#define ANVIL_RUNTIME_OPENGL_SUPPORT_COMPUTE_SHADER*/

/// @brief enables support for advanced circle drawing method
/// @note currently not implemented
/*#define ANVIL_RUNTIME_OPENGL_SUPPORT_ADVANCED_CIRCLE_DRAWING_METHOD*/

/// @brief enables anvil-custom-ecs
/// @note modified version of ecs
#define ANVIL_RUNTIME_USE_CUSTOM_ECS

namespace anvil {

template<typename T>
struct vec2 {
    T x;
    T y;
};

template<typename T>
struct vec3 {
    T x;
    T y;
    T z;
};

template<typename T>
struct vec4 {
    T x;
    T y;
    T z;
    T a;
};

// Types


using rgba_color    =       vec4<uint8_t>;
using rgb_color     =       vec3<uint8_t>;

using vec2i_t       =       vec2<int32_t>;
using vec2f_t       =       vec2<float>;
using vec2d_t       =       vec2<double>;

using vec3i_t       =       vec3<int32_t>;
using vec3f_t       =       vec3<float>;
using vec3d_t       =       vec3<double>;

using ldouble_t     =       long double;

}

namespace anvil {

class game {
private:
    GLFWwindow *glfw_window;

    anvil::vec2i_t window_size;
    std::string title;

    friend class renderer_2d;
public:
    /// @brief poll events
    void poll_events();

    /// @brief actually creates the window and makes it show on screen
    /// @note fullscreen is overriding
    void create(bool fullscreen);

    /// @brief returns if the current window is running
    bool is_running();
public: 
    /// @brief default constructor for game (does not create the window)
    game();

    /// @brief creates a game with given title and size (does not create the window)
    game(std::string title, anvil::vec2i_t size);
public:
    /// @brief closes the game window
    void close();

    /// @brief closes the game window
    ~game();
};

class shader;
class font;

// for renderer_2d
struct __compiledshaderobj;

class renderer_2d {
private:
    bool is_vsync = false;
    int target_fps = 60;
    anvil::game *game;

    // frame stuff
    std::chrono::time_point<std::chrono::high_resolution_clock> start_time;
    double last_time;
    double frame_start_time;
    double delta_time;

    uint64_t frame_counter;

    std::vector<__compiledshaderobj> compiled_shaders;
private:
    void glinit();
public:
    /// @brief starts drawing a new frame
    void begin_frame();

    /// @brief clears background
    void clear(anvil::rgba_color color);

    /// @brief draws a rectangle
    /// @param rotation spans 0-180
    void draw_rect(anvil::vec2f_t pos, anvil::vec2f_t size, anvil::rgba_color color, float rotation);

    /// @brief draws some text with specified font
    /// @param rotation spans 0-180
    void draw_text(std::string text, anvil::font font, anvil::vec2f_t pos, anvil::rgba_color color, float rotation);

    // @brief draws a circle with the triangle fan drawing method
    // @note this is extremely inefficient at high segment count
    // @param segments amount of triangles to use
    void draw_circle(anvil::vec2f_t pos, float radius, anvil::rgba_color color, int segments);

#ifdef ANVIL_RUNTIME_OPENGL_SUPPORT_ADVANCED_CIRCLE_DRAWING_METHOD
    /// @brief draws a circle with the more efficient circle drawing method
    /// @note requires ANVIL_RUNTIME_OPENGL_SUPPORT_ADVANCED_CIRCLE_DRAWING_METHOD to be defined
    void draw_circle(anvil::vec2f pos, float radius, anvil::rgba_color color);
#endif // !ANVIL_RUNTIME_OPENGL_SUPPORT_ADVANCED_CIRCLE_DRAWING_METHOD

    /// @brief sets wireframe to true | false
    void wireframe(bool);

    /// @brief ends drawing a new frame
    void end_frame();

    /// @brief sets vsync to true | false
    void vsync(bool);

    /// @brief sets target fps
    void fps(int);

    /// @brief runs a shader
    void run_shader(anvil::shader shader);

    /// @brief draws a pixel with specified color and position
    void draw_pixel(anvil::vec2f_t position, anvil::rgba_color);
public:
    /// @brief construct a 2d renderer with a set target fps
    renderer_2d(anvil::game*, int fps);

    /// @brief construct a 2d renderer with vsync
    renderer_2d(anvil::game*);
public:
    /// @brief clean up resource that the renderer created
    /// @note does not close the window
    /// @note do not use renderer after calling cleanup()
    void cleanup();

    /// @brief destructor for 2d renderer_2d
    /// @note simply calls cleanup() under the hood
    ~renderer_2d();
};

}

namespace anvil {

enum class shader_type {
    fragment,
    vertex,
    tesselation_control,
    tesselation_evaluation,
    geometry,
#ifdef ANVIL_RUNTIME_OPENGL_SUPPORT_COMPUTE_SHADER
    compute
#endif // !ANVIL_RUNTIME_OPENGL_SUPPORT_COMPUTE_SHADER
};

/// @brief a custom glsl shader
/// @note does not get lazy loaded
class shader {
private:
    int id;
    anvil::shader_type type;
    friend class asset_manager;
    friend class renderer_2d;
private:
    std::string glsl_code;
public:
    /// @brief constructor for shader
    /// @param glsl_code the code of the shader (glsl)
    /// @note glsl code must conform to OpenGL 3.3 (or 4.3 if ANVIL_RUNTIME_OPENGL_SUPPORT_COMPUTE_SHADER is defined) standards
    shader(std::string glsl_code, anvil::shader_type type);
};

class texture;

/// @brief a custom sprite
/// @note not a valid asset, convert to texture first
class sprite {
private:
    anvil::vec2i_t size;
    int channels;

    std::vector<uint8_t> data;
public:
    /// @brief save sprite to a file
    /// @note saves in png format
    /// @note not implemented
    void save_to_file(std::string file) const;

    /// @brief resize the sprite
    void resize(anvil::vec2i_t new_size);

    /// @brief crop the sprite
    void crop(anvil::vec2i_t pos, anvil::vec2i_t size);

    /// @brief flip the sprite horizontally
    void fliph();

    /// @brief flip the sprite vertically
    void flipv();

    /// @brief convert the sprite to a texture
    /// @note does not free the data
    /// @note the sprite must be in RGBA format
    /// @note does not set the id of the texture, use asset_manager::add_texture(...)
    std::shared_ptr<anvil::texture> convert_to_texture();
public:
    /// @brief constructor for a sprite
    sprite(std::string filename);
public:
};

/// @brief a custom font
class font {
private:
    int id;
    friend class asset_manager;
private:
    uint8_t *ttf_buffer = new uint8_t[1 << 20];
    uint8_t *temp_bitmap = new uint8_t[512 * 512];
    stbtt_bakedchar *cdata = new stbtt_bakedchar[96];

    friend class renderer_2d;
public:
public:
    /// @brief constructor for font
    /// @param filepath the path to the .ttf file
    /// @param font_size the size of the font
    font(std::string filepath, int font_size);
};

/// @brief a custom texture uploaded to the gpu
class texture {
private:
    int id;
    anvil::vec2i_t size;

    GLuint tid;

    friend class asset_manager;
    friend class renderer_2d;
    friend class sprite;
};

/// @brief context for audio
class audio_context {
private:
    ALCdevice *device;
    ALCcontext *context;
    friend class audio;
    friend class asset_manager;
public:
    /// @brief constructor for audio_context
    /// @note there is no point constructing this as this is used internally
    audio_context();
};

/// @brief a custom audio
class audio {
private:
    int id;
    std::shared_ptr<audio_context> context;
    ALuint *buffer;
    friend class asset_manager;
private:
    void set_audio_context(std::shared_ptr<audio_context>);
public:
    /// @brief plays the audio asynchronously
    void play();
public:
    /// @brief constructor for audio
    /// @param path the path to the .ogg file
    /// @note only takes in .ogg files
    audio(std::string path);
};

/// @brief a lazy-loading asset manager
/// @note lazy loading timeout is customizable and can be turned off in the constructor
class asset_manager {
private:
    std::chrono::seconds timeout;
    bool lazy_loading;

    std::vector<std::shared_ptr<shader>> shaders;
    
    std::unordered_map<std::shared_ptr<font>,    std::chrono::time_point<std::chrono::high_resolution_clock>> fonts;
    std::unordered_map<std::shared_ptr<texture>, std::chrono::time_point<std::chrono::high_resolution_clock>> textures;
    std::unordered_map<std::shared_ptr<audio>,   std::chrono::time_point<std::chrono::high_resolution_clock>> audios;

    int shader_id = 0;

    int font_id = 0;
    int texture_id = 0;
    int audio_id = 0;

    std::shared_ptr<audio_context> audio_context;

    std::thread cleanup_thread;
    std::mutex asset_mutex;
public:
    /// @brief get an audio by its id
    /// @note returns nullptr if doesn't exist
    std::shared_ptr<audio> get_audio(int id);

    /// @brief add an audio
    /// @note allocates memory for it and returns the id of the audio
    int add_audio(std::shared_ptr<audio>);

    /// @brief get a texture by its id
    /// @note returns nullptr if doesn't exist
    std::shared_ptr<texture> get_texture(int id);

    /// @brief add a texture
    /// @note allocates memory for it and returns the id of the texture
    int add_texture(std::shared_ptr<texture>);

    /// @brief get a font by its id
    /// @note returns nullptr if doesn't exist
    std::shared_ptr<font> get_font(int id);

    /// @brief add a font
    /// @note allocates memory for it and returns the id of the font
    int add_font(std::shared_ptr<font>);

    /// @brief get a shader by its id
    /// @note shader_id is the id of the shader and not the id of glsl shader it contains
    /// @note returns nullptr if doesn't exist
    std::shared_ptr<shader> get_shader(int shader_id);

    /// @brief add a shader
    /// @note allocates memory for it and returns the id of the shader
    int add_shader(std::shared_ptr<shader> shader);

    /// @brief creates the audio context
    void init_audio_context();
public:
    /// @brief constructor, constructs without any lazy loading
    asset_manager();

    /// @brief constructor, constructs with lazy loading
    asset_manager(std::chrono::seconds timeout);
public:
    /// @brief destructor, cleans up everything including audio_context
    void cleanup();

    /// @brief destructor, calls cleanup()
    ~asset_manager();
};

}

namespace anvil {
namespace io {

enum class keyboard_key {
    // Alphanumeric keys
    a = GLFW_KEY_A, b = GLFW_KEY_B, c = GLFW_KEY_C, d = GLFW_KEY_D, e = GLFW_KEY_E,
    f = GLFW_KEY_F, g = GLFW_KEY_G, h = GLFW_KEY_H, i = GLFW_KEY_I, j = GLFW_KEY_J,
    k = GLFW_KEY_K, l = GLFW_KEY_L, m = GLFW_KEY_M, n = GLFW_KEY_N, o = GLFW_KEY_O,
    p = GLFW_KEY_P, q = GLFW_KEY_Q, r = GLFW_KEY_R, s = GLFW_KEY_S, t = GLFW_KEY_T,
    u = GLFW_KEY_U, v = GLFW_KEY_V, w = GLFW_KEY_W, x = GLFW_KEY_X, y = GLFW_KEY_Y,
    z = GLFW_KEY_Z,

    zero = GLFW_KEY_0, one = GLFW_KEY_1, two = GLFW_KEY_2, three = GLFW_KEY_3,
    four = GLFW_KEY_4, five = GLFW_KEY_5, six = GLFW_KEY_6, seven = GLFW_KEY_7,
    eight = GLFW_KEY_8, nine = GLFW_KEY_9,

    // Function keys
    f1 = GLFW_KEY_F1, f2 = GLFW_KEY_F2, f3 = GLFW_KEY_F3, f4 = GLFW_KEY_F4,
    f5 = GLFW_KEY_F5, f6 = GLFW_KEY_F6, f7 = GLFW_KEY_F7, f8 = GLFW_KEY_F8,
    f9 = GLFW_KEY_F9, f10 = GLFW_KEY_F10, f11 = GLFW_KEY_F11, f12 = GLFW_KEY_F12,

    // Control keys
    up = GLFW_KEY_UP, down = GLFW_KEY_DOWN, left = GLFW_KEY_LEFT, right = GLFW_KEY_RIGHT,
    enter = GLFW_KEY_ENTER, backspace = GLFW_KEY_BACKSPACE, tab = GLFW_KEY_TAB,
    space = GLFW_KEY_SPACE, escape = GLFW_KEY_ESCAPE, del = GLFW_KEY_DELETE,
    insert = GLFW_KEY_INSERT, home = GLFW_KEY_HOME, end = GLFW_KEY_END, page_up = GLFW_KEY_PAGE_UP,
    page_down = GLFW_KEY_PAGE_DOWN,

    apostrophe = GLFW_KEY_APOSTROPHE,

    // Modifier keys
    left_shift = GLFW_KEY_LEFT_SHIFT, right_shift = GLFW_KEY_RIGHT_SHIFT,
    left_control = GLFW_KEY_LEFT_CONTROL, right_control = GLFW_KEY_RIGHT_CONTROL,
    left_alt = GLFW_KEY_LEFT_ALT, right_alt = GLFW_KEY_RIGHT_ALT,

    // Special keys
    caps_lock = GLFW_KEY_CAPS_LOCK, scroll_lock = GLFW_KEY_SCROLL_LOCK,
    num_lock = GLFW_KEY_NUM_LOCK, print_screen = GLFW_KEY_PRINT_SCREEN,
    pause = GLFW_KEY_PAUSE,

    // Numeric keypad keys
    kp_0 = GLFW_KEY_KP_0, kp_1 = GLFW_KEY_KP_1, kp_2 = GLFW_KEY_KP_2, kp_3 = GLFW_KEY_KP_3,
    kp_4 = GLFW_KEY_KP_4, kp_5 = GLFW_KEY_KP_5, kp_6 = GLFW_KEY_KP_6, kp_7 = GLFW_KEY_KP_7,
    kp_8 = GLFW_KEY_KP_8, kp_9 = GLFW_KEY_KP_9,
    kp_add = GLFW_KEY_KP_ADD, kp_subtract = GLFW_KEY_KP_SUBTRACT,
    kp_multiply = GLFW_KEY_KP_MULTIPLY, kp_divide = GLFW_KEY_KP_DIVIDE,
    kp_decimal = GLFW_KEY_KP_DECIMAL, kp_enter = GLFW_KEY_KP_ENTER,

    // Other symbols
    semicolon = GLFW_KEY_SEMICOLON, equal = GLFW_KEY_EQUAL, comma = GLFW_KEY_COMMA,
    minus = GLFW_KEY_MINUS, period = GLFW_KEY_PERIOD, slash = GLFW_KEY_SLASH,
    left_bracket = GLFW_KEY_LEFT_BRACKET, backslash = GLFW_KEY_BACKSLASH,
    right_bracket = GLFW_KEY_RIGHT_BRACKET, grave = GLFW_KEY_GRAVE_ACCENT
};

enum class mouse_button {
    button_1 = GLFW_MOUSE_BUTTON_1, button_2 = GLFW_MOUSE_BUTTON_2, button_3 = GLFW_MOUSE_BUTTON_3,
    button_4 = GLFW_MOUSE_BUTTON_4, button_5 = GLFW_MOUSE_BUTTON_5, button_6 = GLFW_MOUSE_BUTTON_6,
    button_7 = GLFW_MOUSE_BUTTON_7, button_8 = GLFW_MOUSE_BUTTON_8,
    left = GLFW_MOUSE_BUTTON_LEFT, right = GLFW_MOUSE_BUTTON_RIGHT,
    middle = GLFW_MOUSE_BUTTON_MIDDLE
};

enum class modifier {
    shift = GLFW_MOD_SHIFT,
    control = GLFW_MOD_CONTROL,
    alt = GLFW_MOD_ALT,
    super = GLFW_MOD_SUPER,
    caps_lock = GLFW_MOD_CAPS_LOCK,
    num_lock = GLFW_MOD_NUM_LOCK
};

enum class action {
    press = GLFW_PRESS,
    repeat = GLFW_REPEAT,
    release = GLFW_RELEASE,
};

struct key_event {
    keyboard_key key;
    io::action action;
    std::vector<modifier> modifiers;
    int scancode;
};

struct mouse_event {
    mouse_button button;
    io::action action;
    std::vector<modifier> modifiers;
};

struct mouse_move_event {
    vec2d_t position;
};

using key_listener_t        = std::function<void(const key_event &)>;
using mouse_listener_t      = std::function<void(const mouse_event &)>;
using mouse_move_listener_t = std::function<void(const mouse_move_event &)>;

/// @brief add a key listener
/// @note the listener will be called on every key event
void add_listener(key_listener_t);

/// @brief add a mouse listener
/// @note the listener will be called on every mouse event
void add_listener(mouse_listener_t);

/// @brief add a mouse move listener
/// @note the listener will be called on every mouse move event
void add_listener(mouse_move_listener_t);

}
}

#ifdef ANVIL_RUNTIME_USE_CUSTOM_ECS
namespace anvil {
namespace ecs {

/// @brief extend for velocity (2d)
template<typename T>
struct velocity2d {
    virtual anvil::vec2<T> get_velocity() const = 0;
    virtual void set_velocity(anvil::vec2<T>) = 0;
    virtual ~velocity2d() = default;
};

/// @brief extend for velocity (3d)
template<typename T>
struct velocity3d {
    virtual anvil::vec3<T> get_velocity() const = 0;
    virtual void set_velocity(anvil::vec3<T>) = 0;
    virtual ~velocity3d() = default;
};

/// @brief extend for position (2d)
template<typename T>
struct position2d {
    virtual anvil::vec2<T> get_position() const = 0;
    virtual void set_position(anvil::vec2<T>) = 0;
    virtual ~position2d() = default;
};

/// @brief extend for position (3d)
template<typename T>
struct position3d {
    virtual anvil::vec3<T> get_position() const = 0;
    virtual void set_position(anvil::vec3<T>) = 0;
    virtual ~position3d() = default;
};

/// @brief extend for health
template<typename T>
struct health {
    virtual T get_health() const = 0;
    virtual void set_health(T) = 0;
    virtual ~health() = default;
};

/// @brief extend for durability
template<typename T>
struct durability {
    virtual T get_durability() const = 0;
    virtual void set_durability(T) = 0;
    virtual ~durability() = default;
};

}
}
#endif // !ANVIL_RUNTIME_USE_CUSTOM_ECS

#endif // !ANVIL_GAMEENGINE_RUNTIME_HPP_INCLUDE_HEADER

