# AnvilRuntime
<center><img src="https://raw.githubusercontent.com/xNoerPlaysCodes/anvilruntime/refs/heads/master/anvilge.nobg.png"></img></center>
<center>Library for Making Games in C++ <b>Easy Fast & Free</b></center>

## Examples

```cpp
#include <anvil/anvilruntime.hpp>

int main() {
    anvil::game game;
    game.create(false, false, 4);
    
    anvil::renderer_2d renderer(&game, 60);
    
    while (game.is_running()) {
        renderer.begin_frame();
        renderer.clear({ 0, 0, 0, 255 });
        {
            renderer.draw_rect({ 100, 100 }, { 100, 100 }, { 255, 0, 0, 255 }, 0);
        }
        renderer.end_frame();
        game.poll_events();
    }
}
```
<img src="https://raw.githubusercontent.com/xNoerPlaysCodes/anvilruntime/refs/heads/master/image.png"></img>
<i>Create a basic window with a renderer and draw a rectangle.</i>

## Features
- Well Documented
- Simple API

## Bindings
Currently there are only bindings for `C++`<br>
If you'd like to create a binding, put your github link here and submit a pull request!

## Documentation
- All functions, classes, structs etc.. are documented in the include header
- Additional documentation can be reached in the Github "Wiki" page

## Technical Info
Distributed as a `static library`, written in `C++17`<br>
Compiled with `clang++`<br>
Libraries used:
- `OpenGL   - Graphics`
- `GLFW     - Windowing`
- `GLU      - Orthographic and Perspective Projection`
- `GLEW     - Loading OpenGL Functions`
- `OpenAL   - Audio`
