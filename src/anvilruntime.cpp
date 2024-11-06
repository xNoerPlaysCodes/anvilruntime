#include <AL/al.h>
#include <AL/alc.h>
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glext.h>
#include <GL/glu.h>
#include <GLFW/glfw3.h>
#include <chrono>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <iterator>
#include <memory>
#include <mutex>
#include <sstream>
#include <string>
#include <thread>
#include <utility>
#include <vector>

#define STB_TRUETYPE_IMPLEMENTATION
#define STB_VORBIS_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include "stb_vorbis.h"
#undef L
#include "stb_image.h"

#include "../include/runtime.hpp"

namespace util {

std::string format_error(std::string error, int error_id, std::string error_source, std::string level) {
    std::stringstream sstream;
    sstream << "[" << level << "]\n";
    sstream << "source: " << error_source << '\n';
    sstream << "description: " << error << '\n';
    if (error_id >= 0) {
        sstream << "error id: " << "ERR-" << error_source << "-" << error_id << '\n';
    } else if (error_id < 0) {
        sstream << "error id: " << "ERR-ANVILRUNTIME-" << -error_id << '\n';
    }
    return sstream.str();
}

std::vector<std::function<void()>> on_close_listeners;

void gl_setup_ortho(anvil::vec2i_t size) {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glOrtho(0, size.x, size.y, 0, -1, 1);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void close_callback(GLFWwindow *) {
    for (auto l : on_close_listeners) {
        l();
    }
}

}

std::vector<anvil::io::key_listener_t> key_listeners;
std::vector<anvil::io::mouse_listener_t> mouse_listeners;
std::vector<anvil::io::mouse_move_listener_t> mouse_move_listeners;

namespace anvil {

game::game() : title("game window"), window_size({ 800, 600 }) {
    int ret = glfwInit();
    if (!ret) {
        std::cout << util::format_error("glfwInit()", ret, "glfw", "fatal");
        std::exit(1);
    }
}
game::game(std::string title, anvil::vec2i_t size) : title(title), window_size(size) {
    int ret = glfwInit();
    if (!ret) {
        std::cout << util::format_error("glfwInit()", ret, "glfw", "fatal");
        std::exit(1);
    }
}

bool game::is_running() { return !glfwWindowShouldClose(this->glfw_window); }

void game::create(bool fs, bool rz, int sp) {
    if (fs) {
        GLFWmonitor *monitor = glfwGetPrimaryMonitor();
        if (monitor == NULL) {
            std::cout << util::format_error("glfwGetPrimaryMonitor() == NULL", 1, "glfw", "fatal");
            std::exit(1);
        }
        const GLFWvidmode *mode = glfwGetVideoMode(monitor);
        window_size.x = mode->width;
        window_size.y = mode->height;
    }

    glfwSetErrorCallback([](int error, const char* description) { std::cout << util::format_error(description, error, "glfw", "fatal"); std::exit(1); });

#ifdef ANVIL_RUNTIME_OPENGL_SUPPORT_COMPUTE_SHADER
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
#else
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
#endif
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
    if (rz) {
        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    } else {
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    }
    glfwWindowHint(GLFW_SAMPLES, sp);

    this->glfw_window = glfwCreateWindow(window_size.x, window_size.y, title.c_str(), nullptr, nullptr);
    glfwMakeContextCurrent(this->glfw_window);

    glfwSetFramebufferSizeCallback(this->glfw_window, [](GLFWwindow* window, int width, int height) {
        glViewport(0, 0, width, height);
        util::gl_setup_ortho({width, height});
    });

    glfwSetKeyCallback(this->glfw_window, [](GLFWwindow *window, int key, int scancode, int action, int mods) {
        io::key_event event;
        event.key = static_cast<io::keyboard_key>(key);
        event.scancode = scancode;
        event.action = static_cast<io::action>(action);
        if (mods & GLFW_MOD_SHIFT) {
            event.modifiers.push_back(anvil::io::modifier::shift);
        }
        if (mods & GLFW_MOD_CONTROL) {
            event.modifiers.push_back(anvil::io::modifier::control);
        }
        if (mods & GLFW_MOD_ALT) {
            event.modifiers.push_back(anvil::io::modifier::alt);
        }
        if (mods & GLFW_MOD_SUPER) {
            event.modifiers.push_back(anvil::io::modifier::super);
        }
        if (mods & GLFW_MOD_CAPS_LOCK) {
            event.modifiers.push_back(anvil::io::modifier::caps_lock);
        }
        if (mods & GLFW_MOD_NUM_LOCK) {
            event.modifiers.push_back(anvil::io::modifier::num_lock);
        }
        for (auto listener : key_listeners) {
            listener(event);
        }
    });

    glfwSetMouseButtonCallback(this->glfw_window, [](GLFWwindow *window, int button, int action, int mods) {
        io::mouse_event event;
        event.button = static_cast<io::mouse_button>(button);
        event.action = static_cast<io::action>(action);
        if (mods & GLFW_MOD_SHIFT) {
            event.modifiers.push_back(anvil::io::modifier::shift);
        }
        if (mods & GLFW_MOD_CONTROL) {
            event.modifiers.push_back(anvil::io::modifier::control);
        }
        if (mods & GLFW_MOD_ALT) {
            event.modifiers.push_back(anvil::io::modifier::alt);
        }
        if (mods & GLFW_MOD_SUPER) {
            event.modifiers.push_back(anvil::io::modifier::super);
        }
        if (mods & GLFW_MOD_CAPS_LOCK) {
            event.modifiers.push_back(anvil::io::modifier::caps_lock);
        }
        if (mods & GLFW_MOD_NUM_LOCK) {
            event.modifiers.push_back(anvil::io::modifier::num_lock);
        }
        for (auto listener : mouse_listeners) {
            listener(event);
        }
    });

    glfwSetCursorPosCallback(this->glfw_window, [](GLFWwindow *window, double xpos, double ypos) {
        io::mouse_move_event event;
        event.position.x = xpos;
        event.position.y = ypos;
        for (auto listener : mouse_move_listeners) {
            listener(event);
        }
    });

    glfwSetWindowCloseCallback(this->glfw_window, util::close_callback);
}

anvil::vec2i_t game::get_window_size() {
    if (window_size.x == 0 || window_size.y == 0) {
        int x;
        int y;
        glfwGetWindowSize(this->glfw_window, &x, &y);
        window_size = { x, y };
    }
    return window_size;
}

std::string game::get_window_title() {
    return title;
}

void game::poll_events() {
    glfwPollEvents();
    int x;
    int y;
    glfwGetWindowSize(this->glfw_window, &x, &y);
    window_size = { x, y };

    title = glfwGetWindowTitle(this->glfw_window);
}

void game::register_on_close(std::function<void()> func) {
    util::on_close_listeners.push_back(func);
}

void game::close() {
    glfwTerminate();
}
game::~game() { close(); }

struct __compiledshaderobj {
    int original_shader_id;

    GLuint id;
    GLuint program;
};

void renderer_2d::draw_texture(anvil::texture texture, anvil::vec2f_t pos, anvil::vec2i_t size) {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture.tid);
    glColor4f(1, 1, 1, 1);
    glBegin(GL_QUADS);
    {
        glTexCoord2f(0.0f, 0.0f); glVertex2f(pos.x, pos.y);
        glTexCoord2f(1.0f, 0.0f); glVertex2f(pos.x + size.x, pos.y);
        glTexCoord2f(1.0f, 1.0f); glVertex2f(pos.x + size.x, pos.y + size.y);
        glTexCoord2f(0.0f, 1.0f); glVertex2f(pos.x, pos.y + size.y);
    }
    glEnd();

    triangle_count += 2;

    glDisable(GL_TEXTURE_2D);
}

void renderer_2d::run_shader(anvil::shader shader) {
    for (auto &os : compiled_shaders) {
        if (os.original_shader_id == shader.id) {
            glUseProgram(os.program);
            return;
        }
    }
    __compiledshaderobj compiled;

    compiled.original_shader_id = shader.id;
    
    GLenum shader_type;
    if (shader.type == anvil::shader_type::vertex) {
        shader_type = GL_VERTEX_SHADER;
    } else if (shader.type == anvil::shader_type::fragment) {
        shader_type = GL_FRAGMENT_SHADER;
    } else if (shader.type == anvil::shader_type::tesselation_control) {
        shader_type = GL_TESS_CONTROL_SHADER;
    } else if (shader.type == anvil::shader_type::tesselation_evaluation) {
        shader_type = GL_TESS_EVALUATION_SHADER;
    } else if (shader.type == anvil::shader_type::geometry) {
        shader_type = GL_GEOMETRY_SHADER;
    }
#ifdef ANVIL_RUNTIME_OPENGL_SUPPORT_COMPUTE_SHADER
    else if (shader.type == anvil::shader_type::compute) {
        shader_type = GL_COMPUTE_SHADER;
    }
#endif
    else {
        std::cout << util::format_error("unknown shader type", -1, "anvil::renderer_2d::run_shader()", "warning") << '\n';
    }

    compiled.id = glCreateShader(shader_type);

    const char *source = shader.glsl_code.c_str();
    glShaderSource(compiled.id, 1, &source, nullptr);
    glCompileShader(compiled.id);

    GLint success;
    glGetShaderiv(compiled.id, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLchar info_log[512];
        glGetShaderInfoLog(compiled.id, 512, nullptr, info_log);
        std::cout << util::format_error(info_log, -1, "anvil::renderer_2d::run_shader()", "fatal") << '\n';
        std::exit(1);
    }

    compiled.program = glCreateProgram();
    glAttachShader(compiled.program, compiled.id);
    glLinkProgram(compiled.program);
    
    GLint isLinked = 0;
    glGetProgramiv(compiled.program, GL_LINK_STATUS, &isLinked);
    if (isLinked == GL_FALSE) {
        GLint maxLength = 0;
        glGetProgramiv(compiled.program, GL_INFO_LOG_LENGTH, &maxLength);

        // Retrieve and print the error log
        std::string errorLog(maxLength, ' ');
        glGetProgramInfoLog(compiled.program, maxLength, &maxLength, &errorLog[0]);
        std::cout << util::format_error(errorLog, -1, "anvil::renderer_2d::run_shader()", "fatal") << '\n';

        // Clean up
        glDeleteProgram(compiled.program);
        glDeleteShader(compiled.id);
        std::exit(1);
    }

    // Shaders can be detached and deleted after linking
    glDetachShader(compiled.program, compiled.id);
    glDeleteShader(compiled.id);

    compiled_shaders.push_back(compiled);
    glUseProgram(compiled.program);
}

void renderer_2d::draw_pixel(anvil::vec2f_t position, anvil::rgba_color color) {
    glBegin(GL_POINTS);
    {
        glColor4f(static_cast<float>(color.x) / 255, static_cast<float>(color.y) / 255, static_cast<float>(color.z) / 255, static_cast<float>(color.a) / 255);
        glVertex2f(position.x, position.y);
    }
    glEnd();
}

renderer_2d::renderer_2d(anvil::game *g) {
    vsync(true);
    this->game = g;
    glinit();
}

renderer_2d::renderer_2d(anvil::game *g, int fps_) {
    vsync(false);
    fps(fps_);
    this->game = g;
    glinit();
}

int renderer_2d::fps() {
    return target_fps;
}

uint64_t renderer_2d::get_frame_counter() {
    return frame_counter;
}

bool renderer_2d::vsync() {
    return is_vsync;
}

double renderer_2d::deltatime() {
    return delta_time;
}

int renderer_2d::tri_count() {
    return triangle_count;
}

void renderer_2d::glinit() {
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        const GLubyte *err_str = glewGetErrorString(err);
        const char* errstr = (char*) (err_str); // We have to do C-style string casting :(
        std::cout << util::format_error(errstr, err, "glew", "fatal");
        std::exit(1);
    }

    glViewport(0, 0, game->window_size.x, game->window_size.y);
    util::gl_setup_ortho(game->window_size);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_FRAMEBUFFER_SRGB);
}

void renderer_2d::begin_frame() {
    start_time = std::chrono::high_resolution_clock::now();
    frame_start_time = glfwGetTime();
    delta_time = frame_start_time - last_time;
    last_time = frame_start_time;
}

void renderer_2d::clear(anvil::rgba_color color) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(color.x, color.y, color.z, color.a);
}

void renderer_2d::draw_rect(anvil::vec2f_t pos, anvil::vec2f_t size, anvil::rgba_color color, float rotation) {
    glLoadIdentity(); 

    // Rotate the rectangle around the Z-axis (for 2D rotation)
    glRotatef(rotation, 0.0f, 0.0f, 1.0f); // Rotate around Z-axis

    glBegin(GL_QUADS);
    {
        glColor4f((float) color.x / 255, (float) color.y / 255, (float) color.z / 255, (float) color.a / 255);
        glVertex2f(pos.x, pos.y);
        glVertex2f(pos.x + size.x, pos.y);
        glVertex2f(pos.x + size.x, pos.y + size.y);
        glVertex2f(pos.x, pos.y + size.y);
    }
    glEnd();

    util::gl_setup_ortho(game->window_size);
    triangle_count += 2;
}

void renderer_2d::draw_circle(anvil::vec2f_t pos, float radius, anvil::rgba_color color, int segments) {
    glBegin(GL_TRIANGLE_FAN);
    {
        glColor4f((float) color.x / 255, (float) color.y / 255, (float) color.z / 255, (float) color.a / 255);
        for (int i = 0; i <= segments; i++) {
            float angle = 2 * M_PI * i / segments;
            glVertex2f(pos.x + radius * cos(angle), pos.y + radius * sin(angle));
            triangle_count++;
        }
    }
    glEnd();
}

void renderer_2d::wireframe(bool w) {
    if (w) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    } else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
}

void renderer_2d::end_frame() {
    glFlush();
    glfwSwapBuffers(this->game->glfw_window);
    if (this->is_vsync) {
        return;
    }
    double frametime_limit = 1.0 / target_fps;
    // SET_PHYSICS_DELTATIME(delta_time);
    
    frame_counter++;

    double frame_end_time = glfwGetTime();
    double frame_duration = frame_end_time - frame_start_time;

    auto err = glGetError();
    if (err != GL_NO_ERROR) {
        std::cout << util::format_error("unknown", err, "opengl", "fatal") << '\n';
        game->close();
        std::exit(1);
    }

    if (frame_duration < frametime_limit) {
        double sleep_time = frametime_limit - frame_duration;
        std::this_thread::sleep_for(std::chrono::duration<double>(sleep_time));
    }
}

void renderer_2d::vsync(bool b) {
    if (b) {
        glfwSwapInterval(1);
    } else {
        glfwSwapInterval(0);
    }
    this->is_vsync = b;
}

void renderer_2d::fps(int fps) {
    this->target_fps = fps;
}

void renderer_2d::cleanup() {}

renderer_2d::~renderer_2d() {
    cleanup();
}

}

namespace anvil {

shader::shader(std::string glsl_code, anvil::shader_type type) : glsl_code(glsl_code), type(type) {}

audio::audio(std::string path) {
    alGenBuffers(1, buffer);

    // Variables to hold audio data
    int channels, sampleRate;
    short* output;
    int samples;

    // Decode the Vorbis file
    stb_vorbis* vorbis = stb_vorbis_open_filename(path.c_str(), nullptr, nullptr);
    if (!vorbis) {
        std::cout << util::format_error("failed to load vorbis file", -1, "stb_vorbis.h - stb_vorbis_open_filename(...)", "fatal");
        std::exit(1);
    }

    // Get the format info
    stb_vorbis_info info = stb_vorbis_get_info(vorbis);
    channels = info.channels; // Get channel count
    sampleRate = info.sample_rate; // Get sample rate

    // Calculate total samples
    samples = stb_vorbis_stream_length_in_samples(vorbis);
    output = new short[samples * channels]; // Allocate for stereo or mono

    // Decode the audio data
    int numSamples = stb_vorbis_get_samples_short_interleaved(vorbis, channels, output, samples * channels);
    stb_vorbis_close(vorbis);

    // Determine the OpenAL format
    ALenum format = (channels == 1) ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;

    // Fill the OpenAL buffer
    alBufferData(*buffer, format, output, numSamples * sizeof(short) * channels, sampleRate);

    // Check for OpenAL errors
    ALenum error = alGetError();
    if (error != AL_NO_ERROR) {
        std::cout << util::format_error(alGetString(error), error, "openal", "fatal");
        std::exit(1);
    }

    this->path = path;
}

void audio::set_audio_context(std::shared_ptr<audio_context> ac) {
    this->context = ac;
}

void audio::play() {
    ALenum error = alGetError();
    if (error != AL_NO_ERROR) {
        std::cout << util::format_error(alGetString(error), error, "openal", "fatal");
        std::exit(1);
    }
    alSourcePlay(*buffer);
    ALuint source;
    alGenSources(1, &source);
    alSourcei(source, AL_BUFFER, *buffer);
    alSourcePlay(source);
}

void audio::cleanup() {
    if (buffer) {
        alDeleteBuffers(1, buffer);
        delete buffer;
        buffer = nullptr;
    }
}

audio::~audio() {
    cleanup();
}

audio_context::audio_context() {
    this->device = alcOpenDevice(nullptr);
}

void asset_manager::init_audio_context() {
    this->audio_context = std::make_shared<anvil::audio_context>();
    audio_context->device = alcOpenDevice(nullptr);
    if (!audio_context->device) {
        util::format_error("alcOpenDevice()", alcGetError(audio_context->device), "openal", "fatal");
        std::exit(1);
    }

    audio_context->context = alcCreateContext(audio_context->device, nullptr);
    if (!audio_context->context) {
        std::cout << util::format_error("alcCreateContext()", alcGetError(audio_context->device), "openal", "fatal");
        std::exit(1);
    }
    alcMakeContextCurrent(audio_context->context);
}

asset_manager::asset_manager(std::chrono::seconds t) : timeout(t), lazy_loading(true) {
    cleanup_thread = std::thread([this]{
        while (lazy_loading) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            std::chrono::time_point<std::chrono::high_resolution_clock> now = std::chrono::high_resolution_clock::now();

            std::vector<std::shared_ptr<audio>> audio_removes;
            std::vector<std::shared_ptr<font>> font_removes;
            std::vector<std::shared_ptr<texture>> texture_removes;

            std::lock_guard<std::mutex> x(asset_mutex);
            for (auto &[k, v] : fonts) {
                if (std::chrono::high_resolution_clock::to_time_t(now) - std::chrono::high_resolution_clock::to_time_t(v) > timeout.count()) {
                    font_removes.push_back(k);
                }
            }

            for (auto &[k, v] : textures) {
                if (std::chrono::high_resolution_clock::to_time_t(now) - std::chrono::high_resolution_clock::to_time_t(v) > timeout.count()) {
                    texture_removes.push_back(k);
                }
            }

            for (auto &[k, v] : audios) {
                if (std::chrono::high_resolution_clock::to_time_t(now) - std::chrono::high_resolution_clock::to_time_t(v) > timeout.count()) {
                    audio_removes.push_back(k);
                }
            }

            for (auto v : font_removes) {
                fonts.erase(v);
                audios_removed.insert({v->path, v->id});
                v.reset();
            }

            for (auto v : texture_removes) {
                textures.erase(v);
                v.reset();
            }

            for (auto v : audio_removes) {
                audios.erase(v);
                audios_removed.insert({v->path, v->id});
                v.reset();
            }
        }
    });
    cleanup_thread.detach();
}

std::shared_ptr<audio> asset_manager::get_audio(int id) {
    asset_mutex.lock();
    for (auto &[k, v] : audios) {
        if (k->id == id) {
            v = std::chrono::high_resolution_clock::now();
            asset_mutex.unlock();
            return k;
        }
    }
    std::string path;
    int removed_id;
    bool found = false;
    for (auto &[k, v] : audios_removed) {
        if (v == id) {
            path = k;
            removed_id = v;
            found = true;
            break;
        }
    }
    if (!found) {
        return nullptr;
    }
    int prev_audio_id = audio_id;
    audio_id = removed_id;

    std::shared_ptr<audio> a = std::make_shared<audio>(path);
    add_audio(a);
    audio_id = prev_audio_id;
    asset_mutex.unlock();
    return a;
}

int asset_manager::add_audio(std::shared_ptr<audio> audio) {
    asset_mutex.lock();
    audio->id = audio_id++; // use the current one and increment
    audio->set_audio_context(this->audio_context);
    audios[audio] = std::chrono::high_resolution_clock::now();
    asset_mutex.unlock();
    return audio->id;
}

std::shared_ptr<texture> asset_manager::get_texture(int id) {
    asset_mutex.lock();
    for (auto &[k, v] : textures) {
        if (k->id == id) {
            v = std::chrono::high_resolution_clock::now();
            asset_mutex.unlock();
            return k;
        }
    }
    return nullptr;
}

int asset_manager::add_texture(std::shared_ptr<texture> texture) {
    asset_mutex.lock();
    texture->id = texture_id++; // use the current one and increment
    textures[texture] = std::chrono::high_resolution_clock::now();
    asset_mutex.unlock();
    return texture->id;
}

std::shared_ptr<font> asset_manager::get_font(int id) {
    asset_mutex.lock();
    for (auto &[k, v] : fonts) {
        if (k->id == id) {
            v = std::chrono::high_resolution_clock::now();
            asset_mutex.unlock();
            return k;
        }
    }
    std::string path;
    int removed_id;
    int font_size;
    bool found = false;
    for (auto &[k, v] : fonts_removed) {
        if (std::get<0>(v) == id) {
            path = k;
            removed_id = std::get<0>(v);
            font_size = std::get<1>(v);
            found = true;
            break;
        }
    }
    if (!found) {
        return nullptr;
    }
    int prev_font_id = font_id;
    font_id = removed_id;

    std::shared_ptr<font> f = std::make_shared<font>(path, font_size);
    add_font(f);
    font_id = prev_font_id;
    asset_mutex.unlock();
    return f;
}

int asset_manager::add_font(std::shared_ptr<font> font) {
    asset_mutex.lock();
    font->id = font_id++; // use the current one and increment
    fonts[font] = std::chrono::high_resolution_clock::now();
    asset_mutex.unlock();
    return font->id;
}

std::shared_ptr<shader> asset_manager::get_shader(int id) {
    asset_mutex.lock();
    for (auto &k : shaders) {
        if (k->id == id) {
            asset_mutex.unlock();
            return k;
        }
    }
    return nullptr;
}

int asset_manager::add_shader(std::shared_ptr<shader> shader) {
    asset_mutex.lock();
    shader->id = shader_id++; // use the current one and increment
    shaders.push_back(shader);
    asset_mutex.unlock();
    return shader->id;
}

asset_manager::asset_manager() : timeout(0), lazy_loading(false) {}

void asset_manager::cleanup() {
    lazy_loading = false;
    if (cleanup_thread.joinable()) {
        cleanup_thread.join();
    }
}

asset_manager::~asset_manager() {
    cleanup();
}

// font

font::font(std::string filepath, int font_size) {
    FILE *f = fopen(filepath.c_str(), "rb");
    if (!f) {
        std::cout << util::format_error("could not open font file", -1, "fopen() - stdio.h", "error");
        std::exit(1);
    }

    fread(ttf_buffer, 1, 1 << 20, f);
    fclose(f);

    if (stbtt_BakeFontBitmap(ttf_buffer, 0, static_cast<float>(font_size), temp_bitmap, 512, 512, 32, 96, cdata) <= 0) {
        std::cout << util::format_error("could not bake font bitmap", -1, "stbtt_BakeFontBitmap() - stb_truetype.h", "warning");
    }

    GLuint font_texture;
    glGenTextures(1, &font_texture);
    glBindTexture(GL_TEXTURE_2D, font_texture);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, 512, 512, 0, GL_ALPHA, GL_UNSIGNED_BYTE, temp_bitmap);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

// renderer_2d-extension
void renderer_2d::draw_text(std::string text, anvil::font font, anvil::vec2f_t pos, anvil::rgba_color color, float rotation) {
    glLoadIdentity();
    glRotatef(rotation, 0.0, 0.0, 1.0);

    glEnable(GL_TEXTURE_2D);
    glColor4f((float) color.x / 255, (float) color.y / 255, (float) color.z / 255, (float) color.a / 255);
    glBegin(GL_QUADS);
    {
        for (char c : text) {
            if (c < 32 || c > 126) continue;
            stbtt_aligned_quad q;
            stbtt_GetBakedQuad(font.cdata, 512, 512, c - 32, &pos.x, &pos.y, &q, 1);

            glTexCoord2f(q.s0, q.t0); glVertex2f(q.x0, q.y0);
            glTexCoord2f(q.s1, q.t0); glVertex2f(q.x1, q.y0);
            glTexCoord2f(q.s1, q.t1); glVertex2f(q.x1, q.y1);
            glTexCoord2f(q.s0, q.t1); glVertex2f(q.x0, q.y1);
        }
    }
    glEnd();
    glDisable(GL_TEXTURE_2D);
    util::gl_setup_ortho(game->window_size);
}

sprite::sprite(std::string filename) {
    uint8_t *img = stbi_load(filename.c_str(), &size.x, &size.y, &channels, 0);

    if (!img) {
        std::cout << util::format_error("could not load sprite", -1, "stbi_load() - stb_image.h", "fatal");
        std::exit(1);
    }

    data.assign(img, img + size.x * size.y * channels);
    stbi_image_free(img);
}

void sprite::save_to_file(std::string file) const {
    std::cout << util::format_error("not implemented", -1, "save_to_file()", "warning");
}

void sprite::resize(anvil::vec2i_t new_size) {
    std::vector<uint8_t> new_data(new_size.x * new_size.y * channels);
    float xratio = static_cast<float>(size.x) / static_cast<float>(new_size.x);
    float yratio = static_cast<float>(size.y) / static_cast<float>(new_size.y);

    for (int y = 0; y < new_size.y; y++) {
        for (int x = 0; x < new_size.x; x++) {
            int sx = static_cast<int>(x * xratio);
            int sy = static_cast<int>(y * yratio);
            for (int c = 0; c < channels; c++) {
                new_data[(y * new_size.x * channels + x) * channels + c] = data[(sy * size.x + sx) * channels + c];
            }
        }
    }
    this->size = new_size;
    this->data = std::move(new_data);
}

void sprite::crop(anvil::vec2i_t pos, anvil::vec2i_t size) {
    std::vector<uint8_t> new_data(size.x * size.y * channels);
    for (int y = 0; y < size.y; y++) {
        for (int x = 0; x < size.x; x++) {
            for (int c = 0; c < channels; c++) {
                new_data[(y * size.x + x) * channels + c] =
                    data[((pos.y + y) * size.x + (pos.x + x)) * channels + c];
            }
        }
    }
    this->size = size;
    this->data = std::move(new_data);
}

void sprite::fliph() {
    for (int y = 0; y < size.y; y++) {
        for (int x = 0; x < size.x; x++) {
            for (int c = 0; c < channels; c++) {
                std::swap(data[(y * size.x + x) * channels + c],
                          data[(y * size.x + (size.x - x - 1)) * channels +c]);
            }
        }
    }
}

void sprite::flipv() {
    for (int y = 0; y < size.y; y++) {
        for (int x = 0; x < size.x; x++) {
            for (int c = 0; c < channels; c++) {
                std::swap(data[(y * size.x + x) * channels + c],
                          data[((size.y - y - 1) * size.x + x) * channels + c]);
            }
        }
    }
}

std::shared_ptr<texture> sprite::convert_to_texture() {
    std::shared_ptr<texture> t = std::make_shared<texture>();
    t->size = this->size;

    glGenTextures(1, &t->tid);
    glBindTexture(GL_TEXTURE_2D, t->tid);
    GLenum type;
    if (channels == 4) {
        type = GL_RGBA;
    } else if (channels == 3) {
        type = GL_RGB;
    } else {
        std::cout << util::format_error("channels=" + std::to_string(channels), -1, "convert_to_texture()", "fatal");
        std::exit(1);
    }
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, size.x, size.y, 0,
                 type, GL_UNSIGNED_BYTE, data.data());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    return t;
}

}

namespace anvil {
namespace io {

void add_listener(key_listener_t listener) {
    key_listeners.push_back(listener);
}

void add_listener(mouse_listener_t listener) {
    mouse_listeners.push_back(listener);
}

void add_listener(mouse_move_listener_t listener) {
    mouse_move_listeners.push_back(listener);
}

bool is_down(keyboard_key key) {
    return ((glfwGetKey(glfwGetCurrentContext(), static_cast<int>(key)) == GLFW_PRESS
    || glfwGetKey(glfwGetCurrentContext(), static_cast<int>(key)) == GLFW_REPEAT)
    || glfwGetKey(glfwGetCurrentContext(), static_cast<int>(key)) != GLFW_RELEASE);
}

}
}

#ifdef ANVIL_RUNTIME_USE_CUSTOM_ECS
namespace anvil {
namespace ecs {


}
}
#endif
