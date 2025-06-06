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
            next_start(start::Type::center);
            next(state::Type::edit);
            return;
        }

        next_start(m_player.next_start().type);

        if (input::Set::at(m_input_id)->is_pressed(input::Key::g)) {
            input::Set::at(m_input_id)->release(input::Key::g);
            is_drawing_debug_lines(!is_drawing_debug_lines());
        }
        /*if (input::Set::at(m_input_id)->is_pressed(input::Key::enter)) {
            input::Set::at(m_input_id)->release(input::Key::enter);
            if (m_camera.focus_transform == m_player.get_transform_id()) {
                m_camera.focus_transform = m_player2.get_transform_id();
            } else if (m_camera.focus_transform == m_player2.get_transform_id()) {
                m_camera.focus_transform = m_player3.get_transform_id();
            } else if (m_camera.focus_transform == m_player3.get_transform_id()) {
                m_camera.focus_transform = m_player.get_transform_id();
            }
        }*/

        if (input::Set::at(m_input_id)->is_pressed(input::Key::space)) {
            input::Set::at(m_input_id)->release(input::Key::space);
            //m_sound.play();

        }

        //line::Set::at(m_line_0)->transform_id = m_transform_id;

        m_fps_text.set_text(std::to_string(current_fps));

        //Console::log("state::Game::update mouse: ", input::Set::mouse.x, " ", input::Set::mouse.y, "\n");

        /*if (input::Set::at(m_input_id)->is_pressed(input::Key::up)) {
            input::Set::at(m_input_id)->release(input::Key::up);
            Console::log("Game::update pressed up\n");
        }*/


        m_player.update();

        if (m_player.is_to_write_save()) {
            is_to_write_save(true);
            m_player.is_to_write_save(false);
        }

        
        //m_player2.update();
        //m_player3.update();

        for (auto& i : m_tile_objects) {
            if (i) i->update();
        }


        m_camera.update();

        m_background_plane.set_velocity(-m_camera.difference / 2.0f);

        m_background_plane.update();

        //m_sound.position({ (m_player.position().x + sprite::Set::at(m_player.aabb_ids().front())->source_rect.w / 2.0f) / m_window_w,
        //                   (m_player.position().y + sprite::Set::at(m_player.aabb_ids().front())->source_rect.h / 2.0f) / m_window_h });

        //Console::log("state::Game::update sound position: ", m_sound.position().x, " ", m_sound.position().y, "\n");

        //Console::log("quadnode x:", m_level_quad_nodes.front().second->x(), "\n");

        for (auto& i : m_level_quad_nodes) {
            i.second->clear();
            i.second->init(m_window_w, m_window_h,
                            NUM_VISIBLE_LAYERS - 1,
                           { (i.first.x - 1) * 256.0f + transform::Set::at(m_level_transform_id)->position.x,
                             (i.first.y - 1) * 256.0f + transform::Set::at(m_level_transform_id)->position.y,
                           256.0f, 256.0f });

            for (auto& aabb : aabb::Set::get_objects()) {
                if (!aabb || aabb->is_inactive()) continue;
                //aabb->update();

                aabb->quad_node_ids.clear();
                i.second->insert(aabb);
            }
            i.second->check_collision();
        }
        //line::Set::update();
        //Console::log("position: ", m_player.position().x, " ", m_player.position().y, "\n");
        //Console::log("position: ", m_player.position().x + m_camera.position.x, " ", m_player.position().y + m_camera.position.y, "\n");

        //Console::log("level position: ", transform::Set::at(m_level_transform_id)->position.x, " ", transform::Set::at(m_level_transform_id)->position.y, "\n");


        //Console::log("camera position: ", m_camera.position.x, " ", m_camera.position.y, "\n");

        //if (m_player.position().x < -collider::aabb::Set::at(m_player.aabb_ids().front())->get_rect().w || m_player.position().x > m_window_w) {

        if (m_camera.focus_transform == m_player.get_transform_id() &&
            (m_player.position().x < -8.0f || m_player.position().x > m_window_w - 8.0f)) {
            start_info(m_player.next_start());
            restart();
        }
        /*else if (m_camera.focus_transform == m_player2.get_transform_id() &&
            (m_player2.position().x < -8.0f || m_player2.position().x > m_window_w)) {
            start_info(m_player2.next_start());
            restart();
        } else if (m_camera.focus_transform == m_player3.get_transform_id() &&
            (m_player3.position().x < -8.0f || m_player3.position().x > m_window_w)) {
            start_info(m_player3.next_start());
            restart();
        }*/


        if (input::Set::at(m_input_id)->is_pressed(input::Key::grave)) {
            input::Set::at(m_input_id)->release(input::Key::grave);
            if (view() == Rectf{0.0f, 0.0f, 480.0f, 270.0f}) {
                view(Rectf{ 0.0f, 0.0f, 320.0f, 180.0f });
            } else {
                view(Rectf{ 0.0f, 0.0f, 480.0f, 270.0f });
            }
        }
    }
}