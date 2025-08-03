module;
#include <sstream>
#include <thread>
#include <mutex>
module state.game;
import camera;
import collider.aabb;
import console;
import transform;
import random;
import entity.particle.sense;

std::mutex quad_node_mutex;

namespace state {
    void Game::update() {
        if (!is_all_valid()) return;

        m_player.update();

        for (auto& i : m_entity_objects) {
            if (i) {
                for (auto& p : i->particles_to_spawn()) {
                    if (p.type == entity::ParticleType::hit) {
                        m_particle_entity_objects.push_back(std::make_unique<entity::Particle>());
                        m_particle_entity_objects.back()->parent(i.get());

                        m_particle_entity_objects.back()->load_config("res/entity/particle/hit.cfg");       
                        m_particle_entity_objects.back()->sprite()->layer = NUM_VISIBLE_LAYERS - 1;
                        m_particle_entity_objects.back()->sprite()->set_rotation(random::Generator::real_number(0.0f, 359.0f));
                        //Console::log("rotation: ", m_particle_entity_objects.back()->sprite()->rotation, "\n");

                        m_player.melee_hit();
                    } else if (p.type == entity::ParticleType::sense_frog) {
                        m_particle_entity_objects.push_back(std::make_unique<entity::ParticleSense>());
                        m_particle_entity_objects.back()->parent(i.get());

                        m_particle_entity_objects.back()->load_config("res/entity/particle/sense_frog.cfg");
                    }
                    if (m_particle_entity_objects.back()->transform()) {
                        m_particle_entity_objects.back()->transform()->position = p.position;
                        m_particle_entity_objects.back()->transform()->velocity = p.velocity;
                    }
                }
                i->particles_to_spawn().clear();
                
                i->update();
            }
        }

        for (auto it = m_particle_entity_objects.begin(); it != m_particle_entity_objects.end(); ++it) {
            if ((*it)) {
                (*it)->update();
                if ((*it)->is_to_erase()) {
                    //Console::log("erasing particle\n");
                    m_particle_entity_objects.erase(it);
                }
            }
        }   
        
        if (input::Set::at(m_input_id)->is_pressed(input::Key::f1)) {
            input::Set::at(m_input_id)->release(input::Key::f1);
            next_start(start::Type::center);
            next_state(state::Type::edit);
            return;
        }

        next_start(m_player.next_start().type);

        //anim::Set::update();

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


        if (m_player.is_to_write_save()) {
            is_to_write_save(true);
            m_player.is_to_write_save(false);
        }

        
        //m_player2.update();
        //m_player3.update();

        
       /* for (auto& i : m_entity_objects) {
            if (i && i->time_left_alive() > 0) i->time_left_alive(i->time_left_alive() - 1);
        }*/

        Camera::update();


        m_background_plane.set_velocity(-Camera::difference / 2.0f);

        m_background_plane.update();

        //Console::log("entity objects size: ", m_entity_objects.size(), "\n");

        //m_sound.position({ (m_player.position().x + sprite::Set::at(m_player.aabb_ids().front())->source_rect.w / 2.0f) / m_window_w,
        //                   (m_player.position().y + sprite::Set::at(m_player.aabb_ids().front())->source_rect.h / 2.0f) / m_window_h });

        //Console::log("state::Game::update sound position: ", m_sound.position().x, " ", m_sound.position().y, "\n");

        //Console::log("quadnode x:", m_level_quad_nodes.front().second->x(), "\n");




        std::vector<std::thread> threads;
        for (std::pair<Vec2i, std::unique_ptr<QuadNode>>& quad_node : m_level_quad_nodes) {
            if (!quad_node.second || !transform::Set::at(m_level_transform_id)) continue;

            auto quad_node_check_collision = [&]() {
                std::unique_lock<std::mutex> quad_node_lock(quad_node_mutex);

                quad_node.second->clear();
                quad_node.second->init(m_window_w, m_window_h,
                    NUM_VISIBLE_LAYERS - 1,
                    { (quad_node.first.x - 1) * 256.0f + transform::Set::at(m_level_transform_id)->position.x,
                    (quad_node.first.y - 1) * 256.0f + transform::Set::at(m_level_transform_id)->position.y,
                    256.0f, 256.0f });

                for (aabb::Object* aabb : aabb::Set::get_objects()) {
                    if (!aabb || aabb->is_inactive()) continue;
                    aabb->quad_node_ids.clear();

                    for (auto& point : aabb->points()) {
                        //if (!aabb->parent) continue;
                        quad_node.second->insert_point(aabb, point);
                    }
                }

                quad_node_lock.unlock();

                quad_node.second->check_collision();                
            };
            threads.push_back(std::thread(quad_node_check_collision));
        }
        //Console::log("threads size: ", threads.size(), "\n");
        for (auto& i : threads) {            
            i.join();
        }


        //line::Set::update();
        //Console::log("position: ", m_player.position().x, " ", m_player.position().y, "\n");
        //Console::log("position: ", m_player.position().x + m_camera.position.x, " ", m_player.position().y + m_camera.position.y, "\n");

        //Console::log("level position: ", transform::Set::at(m_level_transform_id)->position.x, " ", transform::Set::at(m_level_transform_id)->position.y, "\n");


        //Console::log("camera position: ", m_camera.position.x, " ", m_camera.position.y, "\n");

        //if (m_player.position().x < -collider::aabb::Set::at(m_player.aabb_ids().front())->get_rect().w || m_player.position().x > m_window_w) {

        if (Camera::focus_transform == m_player.get_transform_id() &&
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