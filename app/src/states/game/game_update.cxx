module;
#include <sstream>

module state.game;

namespace state {
    void Game::update() {
        if (!is_all_valid()) return;

        if (Transforms::at(m_transform_id)->position.x < 0.0f) {
            Transforms::at(m_transform_id)->position.x = 0.0f;
            Transforms::at(m_transform_id)->velocity.x *= -0.9f;
            Transforms::at(m_transform_id)->velocity.x += m_player.velocity().x;
        } else if (Transforms::at(m_transform_id)->position.x > 320.0f - m_fps_text.get_text().size() * m_fps_text.font_size) {
            Transforms::at(m_transform_id)->position.x = 320.0f - m_fps_text.get_text().size() * m_fps_text.font_size;
            Transforms::at(m_transform_id)->velocity.x *= -0.9f;
            Transforms::at(m_transform_id)->velocity.x += m_player.velocity().x;
        }
        if (Transforms::at(m_transform_id)->position.y < 0.0f) {
            Transforms::at(m_transform_id)->position.y = 0.0f;
            Transforms::at(m_transform_id)->velocity.y *= -0.9f;
            Transforms::at(m_transform_id)->velocity.y += m_player.velocity().y;
        } else if (Transforms::at(m_transform_id)->position.y > 180.0f - m_fps_text.font_size) {
            Transforms::at(m_transform_id)->position.y = 180.0f - m_fps_text.font_size;
            Transforms::at(m_transform_id)->velocity.y *= -0.9f;
            Transforms::at(m_transform_id)->velocity.y += m_player.velocity().y;
        }

        if (Inputs::at(m_input_id)->is_pressed(input::Key::f1)) {
            Inputs::at(m_input_id)->release(input::Key::f1);
            set_next(state::Type::Edit);
            return;
        }

        Lines::at(m_line_0)->start = Inputs::mouse_prev;
        Lines::at(m_line_0)->end = Inputs::mouse;
        Lines::at(m_line_0)->layer = NUM_VISIBLE_LAYERS - 1;

        m_fps_text.set_text(std::to_string(current_fps));

        /*if (Inputs::at(m_input_id)->is_pressed(input::Key::up)) {
            Inputs::at(m_input_id)->release(input::Key::up);
            Console::log("Game::update pressed up\n");
        }*/

        //Console::log("player position: ", m_player.x(), " ", m_player.y(), "\n");


        //Console::log("level position: ", level_position().x, " ", level_position().y, "\n");
        //Console::log("quad_node position: ", m_quad_node.rect.x, " ", m_quad_node.rect.y, "\n");

        Vec2f scroll_limit = { 160.0f - m_player.source_rect().w / 2.0f, 90.0f - m_player.source_rect().h / 2.0f };

        if (m_player.x() > scroll_limit.x || m_player.x() < scroll_limit.x) {
            f32c diff_x = m_player.position().x - scroll_limit.x;
            level_x(level_x() - diff_x);
            m_player.x(m_player.x() - diff_x);
            m_player2.x(m_player2.x() - diff_x);

            Transforms::at(m_transform_id)->position.x -= diff_x;
        }
        if (m_player.y() > scroll_limit.y || m_player.y() < scroll_limit.y) {
            f32c diff_y = m_player.position().y - scroll_limit.y;
            level_y(level_y() - diff_y);
            m_player.y(m_player.y() - diff_y);
            m_player2.y(m_player2.y() - diff_y);

            Transforms::at(m_transform_id)->position.y -= diff_y;
        }

        m_player.update();

        m_player2.update();

        m_quad_node.clear();
        m_quad_node.init(NUM_VISIBLE_LAYERS - 1, { 0, 0, 256, 256 });

        //m_quad_node.insert(Inputs::mouse);
        /*m_quad_node.insert(Inputs::mouse + Vec2f{2.0f, 2.0f});
        m_quad_node.insert(Inputs::mouse + Vec2f{ 4.0f, 4.0f });
        m_quad_node.insert(Inputs::mouse + Vec2f{ 6.0f, 6.0f });*/

        //Console::log("state::Game::update mouse: ", Inputs::mouse.x, " ", Inputs::mouse.y, "\n");                
        //Console::log("state::Game::update player position: ", m_player.x(), " ", m_player.y(), "\n");

        Rectic player_rect = Sprites::at(m_player.get_sprite_id())->source_rect;

        m_quad_node.insert(m_player.position());
        m_quad_node.insert({ m_player.x() + player_rect.w, m_player.y() });
        m_quad_node.insert({ m_player.x(),  m_player.y() + player_rect.h });
        m_quad_node.insert({ m_player.x() + player_rect.w, m_player.y() + player_rect.h });

        m_quad_node.insert(m_player2.position());
        m_quad_node.insert({ m_player2.x() + player_rect.w, m_player2.y() });
        m_quad_node.insert({ m_player2.x(),  m_player2.y() + player_rect.h });
        m_quad_node.insert({ m_player2.x() + player_rect.w, m_player2.y() + player_rect.h });

        //Console::log("id: ", m_quad_node.id_at(player_position), "\n");

        //for (auto& i : m_quad_node.points_at(m_player.position())) {
            //Console::log("point: ", i.x, " ", i.y, " size: ", m_quad_node.points_at(m_player.position()).size(), "\n");
        //}
    }
}