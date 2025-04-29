module;
#include <sstream>
module state.game;
import collider;
import console;
import transform;

namespace state {
    void Game::update() {
        if (!is_all_valid()) return;
        
        if (input::Set::at(m_input_id)->is_pressed(input::Key::f1)) {
            input::Set::at(m_input_id)->release(input::Key::f1);
            set_next(state::Type::Edit);
            return;
        }
        if (input::Set::at(m_input_id)->is_pressed(input::Key::g)) {
            input::Set::at(m_input_id)->release(input::Key::g);
            is_drawing_debug_lines = !is_drawing_debug_lines;
        }
        if (input::Set::at(m_input_id)->is_pressed(input::Key::enter)) {
            input::Set::at(m_input_id)->release(input::Key::enter);
            if (m_camera.focus_transform == m_player.get_transform_id()) {
                m_camera.focus_transform = m_player2.get_transform_id();
            } else if (m_camera.focus_transform == m_player2.get_transform_id()) {
                m_camera.focus_transform = m_player3.get_transform_id();
            } else if (m_camera.focus_transform == m_player3.get_transform_id()) {
                m_camera.focus_transform = m_player.get_transform_id();
            }
        }
        
        line::Set::at(m_line_0)->start = input::Set::mouse_prev;
        line::Set::at(m_line_0)->end = input::Set::mouse;
        line::Set::at(m_line_0)->layer = NUM_VISIBLE_LAYERS - 1;
        //line::Set::at(m_line_0)->transform_id = m_transform_id;

        m_fps_text.set_text(std::to_string(current_fps));

        //Console::log("state::Game::update mouse: ", input::Set::mouse.x, " ", input::Set::mouse.y, "\n");

        /*if (input::Set::at(m_input_id)->is_pressed(input::Key::up)) {
            input::Set::at(m_input_id)->release(input::Key::up);
            Console::log("Game::update pressed up\n");
        }*/

        m_player.update();
        m_player2.update();
        m_player3.update();
        m_camera.update();

        //Console::log("quadnode x:", m_level_quad_nodes.front().second->x(), "\n");

        for (auto& i : m_level_quad_nodes) {
            i.second->clear();
            i.second->init(m_window_w, m_window_h,
                            NUM_VISIBLE_LAYERS - 1,
                           { (i.first.x - 1) * 256.0f + transform::Set::at(m_level_transform_id)->position.x,
                             (i.first.y - 1) * 256.0f + transform::Set::at(m_level_transform_id)->position.y,
                           256.0f, 256.0f });

            for (auto& aabb : collider::aabb::Set::get_objects()) {
                if (!aabb) continue;
                //aabb->update();

                aabb->quad_node_ids.clear();
                i.second->insert(aabb);
            }
            i.second->check_collision();
        }
        //line::Set::update();
        //Console::log("position: ", m_player.position().x, " ", m_player.position().y, "\n");
        //Console::log("position: ", m_player.position().x + m_camera.position.x, " ", m_player.position().y + m_camera.position.y, "\n");
    }
}