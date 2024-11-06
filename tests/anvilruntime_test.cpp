#include <anvil/runtime.hpp>

int main() {
    anvil::game game("Test!", { 1280, 720 });
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
