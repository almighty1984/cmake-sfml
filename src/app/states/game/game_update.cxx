module;
#include <sstream>

module state.game;
import camera;
import collider.aabb;
import console;
import transform;
import random;
import entity.particle.bubble;
import entity.particle.drop;
import entity.particle.sense;
import entity.water_line;
import sound;
import particle;

namespace state {
     void Game::update_unlocked() {
         particle::Manager::check_to_erase();
         particle::Manager::check_to_spawn();

        for (auto& i : m_unlocked_entity_objects) {
            if (i) i->update();
        }
    }
    void Game::update(cF32 ts) {
        if (!is_all_valid()) return;

        //Console::log("visible layers: ", m_visible_layers.size(), "\n");
        if (input()->is_pressed(input::Key::t) && !is_to_transition) {
            input()->release(input::Key::t);
            Console::log("state::Game pressed t\n");
            is_to_transition = true;
            
            m_start_info = { .type = start::Type::center,
                             .number = 0 };

            
            return;
        }
        //Console::log("level transform: ", m_level_transform_id, " position: ", transform::Manager::get(m_level_transform_id)->position.x, "\n");
        //Console::log("transforms: ", transform::Manager::size(), " unused: ", transform::Manager::unused_size(), "\n");
        particle::Manager::update();

        Camera::update();
        m_background_plane.set_velocity(-Camera::difference / 2.0F);

        //line::Manager::get(m_line_id)->set({ 160.0F, 90.0F }, input::Manager::mouse);
        //line::Manager::get(m_line_id)->update();

        m_player.update();

        m_background_plane.update();
        //Console::log("focus transform: ", Camera::focus_transform, " ", m_player.get_transform_id(), "\n");

        for (auto& i : m_entity_objects) {            
            if (i && !entity::is_water_line(i->type()) && !entity::is_track(i->type())) {
                i->update();
            }
        }
        for (auto& i : m_water_entity_objects) {
            if (i) i->update();
        }


        if (Camera::focus_transform == m_player.get_transform_id() &&
            (m_player.position().x < -8.0F || m_player.position().x > m_window_w - 8.0F)) {
            m_start_info = m_player.next_start();
            is_to_transition = true;
            m_next_level = m_player.next_level();
            Console::log("state::Game next level: ", start::to_string(m_next_level), "\n");
            return;
            //restart();
        }
        
        
        if (input()->is_pressed(input::Key::f1)) {
            input()->release(input::Key::f1);
            //Console::log("level path: ", m_level_path, "\n");
            
            //m_next_start = start::Type::none;
            
            is_to_transition = true;

            m_start_info = { .type = start::Type::none,
                             .number = 0 };            

            m_next_state = state::Type::edit;


            return;
        }

        //next_start(m_player.next_start().type);

        //anim::Manager::update();

        if (input()->is_pressed(input::Key::g)) {
            input()->release(input::Key::g);
            is_drawing_debug = !is_drawing_debug;
        }
        /*if (input()->is_pressed(input::Key::enter)) {
            input()->release(input::Key::enter);
            if (m_camera.focus_transform == m_player.get_transform_id()) {
                m_camera.focus_transform = m_player2.get_transform_id();
            } else if (m_camera.focus_transform == m_player2.get_transform_id()) {
                m_camera.focus_transform = m_player3.get_transform_id();
            } else if (m_camera.focus_transform == m_player3.get_transform_id()) {
                m_camera.focus_transform = m_player.get_transform_id();
            }
        }*/

        if (input()->is_pressed(input::Key::space)) {
            input()->release(input::Key::space);
            //m_sound.play();

        }

        //line::Manager::get(m_line_0)->transform_id = m_transform_id;

        //Console::log("state::Game::update mouse: ", input::Manager::mouse.x, " ", input::Manager::mouse.y, "\n");

        /*if (input()->is_pressed(input::Key::up)) {
            input()->release(input::Key::up);
            Console::log("Game::update pressed up\n");
        }*/
                

        if (m_player.is_to_write_save()) {
            is_to_write_save = true;
            m_player.is_to_write_save(false);
        }

        
        //m_player2.update();
        //m_player3.update();

        
       /* for (auto& i : m_entity_objects) {
            if (i && i->time_left_alive() > 0) i->time_left_alive(i->time_left_alive() - 1);
        }*/



        //Console::log("entity objects size: ", m_entity_objects.size(), "\n");

        //m_sound.position({ (m_player.position().x + sprite::Manager::get(m_player.aabb_ids().front())->source_rect.w / 2.0F) / m_window_w,
        //                   (m_player.position().y + sprite::Manager::get(m_player.aabb_ids().front())->source_rect.h / 2.0F) / m_window_h });

        //Console::log("state::Game::update sound position: ", m_sound.position().x, " ", m_sound.position().y, "\n");

        //Console::log("quadnode x:", m_level_quad_nodes.front().second->x(), "\n");






        //line::Manager::update();
        //Console::log("position: ", m_player.position().x, " ", m_player.position().y, "\n");
        //Console::log("position: ", m_player.position().x + m_camera.position.x, " ", m_player.position().y + m_camera.position.y, "\n");

        //Console::log("level position: ", transform::Manager::get(m_level_transform_id)->position.x, " ", transform::Manager::get(m_level_transform_id)->position.y, "\n");


        //Console::log("camera position: ", m_camera.position.x, " ", m_camera.position.y, "\n");

        //if (m_player.position().x < -collider::aabb::Manager::get(m_player.aabb_ids().front())->get_rect().w || m_player.position().x > m_window_w) {

        
        /*else if (m_camera.focus_transform == m_player2.get_transform_id() &&
            (m_player2.position().x < -8.0F || m_player2.position().x > m_window_w)) {
            start_info(m_player2.next_start());
            restart();
        } else if (m_camera.focus_transform == m_player3.get_transform_id() &&
            (m_player3.position().x < -8.0F || m_player3.position().x > m_window_w)) {
            start_info(m_player3.next_start());
            restart();
        }*/


        if (input()->is_pressed(input::Key::grave)) {
            input()->release(input::Key::grave);
            is_to_change_view = true;
            if (view() == RectF{0.0F, 0.0F, 480.0F, 270.0F}) {
                view(RectF{ 0.0F, 0.0F, 320.0F, 180.0F });
            } else {
                view(RectF{ 0.0F, 0.0F, 480.0F, 270.0F });
            }
        }


        quad_node_check_collision();
    }
}