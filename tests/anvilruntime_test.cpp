#include "../include/runtime.hpp"
#include <cstdint>
#include <iostream>
#include <thread>

int main() {
    anvil::game game;
    game.create(true, false, 4);

    anvil::renderer_2d renderer(&game);

    anvil::asset_manager asset_manager;
    std::shared_ptr<anvil::font> font = std::make_shared<anvil::font>("/usr/share/fonts/TTF/JetBrainsMonoNerdFont-Bold.ttf", 32);
    int id = asset_manager.add_font(font);
    auto _font = *asset_manager.get_font(id);
     
    while (game.is_running()) {
        renderer.begin_frame();
        renderer.clear({0, 0, 0, 255});
        {
            renderer.draw_rect({100, 100}, {100, 100}, {255, 0, 0, 255}, 0);
            renderer.draw_text("Hello, world!", _font, {100, 100}, {141, 255, 0, 255}, 0);

            renderer.draw_circle({ 500, 500 }, 64, { 255, 255, 255, 255 }, 65536);
            renderer.draw_pixel({ 550, 620 }, { 255, 255, 255, 255 });
        }
        renderer.end_frame();
        game.poll_events();
    }

    return 0;
}
