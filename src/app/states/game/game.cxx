module;
#include <filesystem>
#include <fstream>
#include <vector>
#include <sstream>
#include <thread>
#include <mutex>

module state.game;
import camera;
import collider.aabb;
import console;
import input;
import line;
import quad_node;
import sound;
import sprite;
import transform;
import types;

std::mutex quad_node_mutex;

namespace state {
    Game::Game(cU16 window_w, cU16 window_h, std::filesystem::path level_path, start::Info start) {
        //Console::log("Game::Game level: ", level_path, " start: ", start_position.x, " ", start_position.y, "\n");
        m_current_state = m_next_state = state::Type::game;
        m_window_w = window_w, m_window_h = window_h;

        start_info(start);

        Console::log("transform::Manager::size: ", transform::Manager::size(), "\n");

        Console::log("transform::Manager::unused_size: ", transform::Manager::unused_size(), "\n");

        m_background_plane.create(3, 1);

        for (auto& i : m_background_plane.get_sprite_ids()) {
            m_visible_layers.insert(sprite::Manager::get(i)->layer);
        }

        //m_visible_layers.insert(m_player.sprite()->layer);
        



        m_level_transform_id = transform::Manager::make();

        Console::log("m_level_transform_id: ", m_level_transform_id, "\n");

        m_player.level_transform_id = m_level_transform_id;

        //Console::log("level transform_id: ", m_level_transform_id, "\n");

        //m_entity_objects.emplace_back(std::make_unique<player::Object>());
        //m_entity_objects.back()->level_transform_id = m_level_transform_id;
        //m_entity_objects.back()->position(m_start_position);

        load_level(level_path);
        m_level_path = level_path;


        /*for (U16 i = 0; i < m_num_quad_nodes.x * m_num_quad_nodes.y; ++i) {
            m_quad_nodes.emplace_back(std::make_unique<QuadNode>());
            m_quad_nodes.back().get()->transform_id = m_level_transform_id;
        }*/

        /*for (U8 i = 0; i < NUM_VISIBLE_LAYERS; ++i) {
            add_visible_layer(i);
        }*/

        m_visible_layers.insert(NUM_VISIBLE_LAYERS - 1);  // for particles like blood

        

        //m_player.start_offset(m_start_position);
        //m_player.load_config("res/entity/player/player.cfg");
        
        m_player.transform()->position = m_start_position;

        Console::log("start position: ", m_start_position.x, " ", m_start_position.y, "\n");

        //m_player.transform()->position = Vec2F{ 320.0F, 0.0F };
        
        //m_player.id = 0;

        //m_player2.position(m_start_position - Vec2F{ 16.0F, 0.0F });


        //m_player2.id = 1;

        /*m_player2.key_up = input::Key::w;
        m_player2.key_down = input::Key::s;
        m_player2.key_left = input::Key::a;
        m_player2.key_right = input::Key::d;
        m_player2.key_jump = input::Key::g;
        m_player2.key_sprint = input::Key::h;
        m_player2.key_melee = input::Key::f;*/

        /*m_player3.set_layer(NUM_VISIBLE_LAYERS - 1);
        m_player3.position(m_start_position + Vec2F{ 16.0F, 0.0F });

        m_player3.key_up = input::Key::i;
        m_player3.key_down = input::Key::k;
        m_player3.key_left = input::Key::j;
        m_player3.key_right = input::Key::l;
        m_player3.key_jump = input::Key::r;
        m_player3.key_sprint = input::Key::e;
        m_player3.key_melee = input::Key::t;*/

        //m_player3.id = 2;

        //transform::Manager::get(m_transform_id)->velocity = { 2.0F, 1.0F };
        //transform::Manager::get(m_transform_id)->deceleration = { 0.001f, 0.001f };
        //Console::log("Game transform_id: ", m_transform_id, "\n");

        m_input_id = input::Manager::make();

        //Console::log("sprite::Objects size: ", sprite::Manager::size(), "\n");



       /* for (auto& i : m_background_plane.get_transform_ids()) {
            m_camera.add_transform_id(i);
        }*/

        //Camera::clear();
        Camera::add_transform_id(m_level_transform_id);
        Camera::add_transform_id(m_player.get_transform_id());

        Camera::focus_transform = m_player.get_transform_id();
        //m_camera.add_transform_id(m_player2.get_transform_id());
        //m_camera.add_transform_id(m_player3.get_transform_id());

        //Vec2F camera_position = m_player.position() - Vec2F{ window_w / 2.0F, window_h / 2.0F };
        cVec2F camera_position = m_start_position - Vec2F{ window_w / 2.0F, window_h / 2.0F + 8.0F };


        //Console::log("\nstate::Game::Game camera position: ", camera_position.x, " ", camera_position.y, "\n\n");

        Camera::set_position(camera_position);

        //m_camera.add_transform_id(m_transform_id);

        //m_line_id = line::Manager::make({ 160.0F,90.0F }, { 320.0F, 180.0F });
        //line::Manager::get(m_line_id)->size = 3;
    }

    void Game::check_entities_to_add_input_from(entity::Object* trigger_entity) {
        if (!entity::is_conduit(trigger_entity->type()) &&
            !entity::is_logic(trigger_entity->type()) &&
            !entity::is_track(trigger_entity->type()) &&
            trigger_entity->type() != entity::Type::trigger &&
            trigger_entity->type() != entity::Type::track_trigger_UL && trigger_entity->type() != entity::Type::track_trigger_UR
            ) {
            return;
        }
        for (auto& entity : m_unlocked_entity_objects) {
            if (!entity::is_conduit(entity->type()) &&
                !entity::is_logic(entity->type()) &&
                !entity::is_track(entity->type()) &&
                entity->type() != entity::Type::track_trigger_UL &&
                entity->type() != entity::Type::track_trigger_UR
                ) {
                continue;
            }
            //bool is_to_add_input = false;
            //Console::log("check_entities_to_add_input_from: ", entity::to_string(entity->type()), "\n");
            std::vector<Vec2F> offsets_from_trigger_to_check;
            if (entity->type() == entity::Type::logic_not_U ||
                entity->type() == entity::Type::conduit_U ||
                entity->type() == entity::Type::conduit_UL ||
                entity->type() == entity::Type::conduit_UR ||
                entity->type() == entity::Type::track_U ||
                entity->type() == entity::Type::track_UL ||
                entity->type() == entity::Type::track_UR ||
                entity->type() == entity::Type::track_UL_1x1 ||
                entity->type() == entity::Type::track_UR_1x1 ||
                entity->type() == entity::Type::track_UL_1x2_0 ||
                entity->type() == entity::Type::track_UR_1x2_0 ||
                entity->type() == entity::Type::track_trigger_UL || entity->type() == entity::Type::track_trigger_UR) {
                offsets_from_trigger_to_check.push_back({ 0.0F,-16.0F });
            } else if (entity->type() == entity::Type::logic_not_D ||
                entity->type() == entity::Type::conduit_D ||
                entity->type() == entity::Type::conduit_DL ||
                entity->type() == entity::Type::conduit_DR ||
                entity->type() == entity::Type::track_D ||
                entity->type() == entity::Type::track_DL ||
                entity->type() == entity::Type::track_DR ||
                entity->type() == entity::Type::track_DL_1x1 ||
                entity->type() == entity::Type::track_DR_1x1 ||
                entity->type() == entity::Type::track_DL_1x2_0 ||
                entity->type() == entity::Type::track_DR_1x2_0) {
                offsets_from_trigger_to_check.push_back({ 0.0F, 16.0F });
            } else if (entity->type() == entity::Type::logic_not_L ||
                entity->type() == entity::Type::conduit_L ||
                entity->type() == entity::Type::conduit_LU ||
                entity->type() == entity::Type::conduit_LD ||
                entity->type() == entity::Type::track_L ||
                entity->type() == entity::Type::track_LU ||
                entity->type() == entity::Type::track_LD ||
                entity->type() == entity::Type::track_LU_1x1 ||
                entity->type() == entity::Type::track_LD_1x1) {
                offsets_from_trigger_to_check.push_back({ -16.0F, 0.0F });
            } else if (entity->type() == entity::Type::logic_not_R ||
                entity->type() == entity::Type::conduit_R ||
                entity->type() == entity::Type::conduit_RU ||
                entity->type() == entity::Type::conduit_RD ||
                entity->type() == entity::Type::track_R ||
                entity->type() == entity::Type::track_RU ||
                entity->type() == entity::Type::track_RD ||
                entity->type() == entity::Type::track_RU_1x1 ||
                entity->type() == entity::Type::track_RU_2x1_0 ||
                entity->type() == entity::Type::track_RD_1x1 ||
                entity->type() == entity::Type::track_RD_2x1_0) {
                offsets_from_trigger_to_check.push_back({ 16.0F, 0.0F });
            } else if (entity->type() == entity::Type::logic_and_UD || entity->type() == entity::Type::logic_or_UD) {
                offsets_from_trigger_to_check.push_back({ 0.0F, 16.0F });
                offsets_from_trigger_to_check.push_back({ 0.0F,-16.0F });
            } else if (entity->type() == entity::Type::logic_and_LR || entity->type() == entity::Type::logic_or_LR) {
                offsets_from_trigger_to_check.push_back({ 16.0F, 0.0F });
                offsets_from_trigger_to_check.push_back({ -16.0F, 0.0F });
            } else if (entity->type() == entity::Type::track_RU_2x1_1) {
                offsets_from_trigger_to_check.push_back({ 16.0F, -16.0F });
            } else if (entity->type() == entity::Type::track_RD_2x1_1) {
                offsets_from_trigger_to_check.push_back({ 16.0F, 16.0F });
            } else if (entity->type() == entity::Type::track_UL_1x2_1) {
                offsets_from_trigger_to_check.push_back({ -16.0F, -16.0F });
                offsets_from_trigger_to_check.push_back({ 0.0F, -16.0F });
            } else if (entity->type() == entity::Type::track_UR_1x2_1) {
                offsets_from_trigger_to_check.push_back({ 16.0F, -16.0F });
                //offsets_from_trigger_to_check.emplace_back({  0.0F, -16.0F });
            } else if (entity->type() == entity::Type::track_DL_1x2_1) {
                offsets_from_trigger_to_check.push_back({ -16.0F, 16.0F });
                offsets_from_trigger_to_check.push_back({ 0.0F, 16.0F });
            } else if (entity->type() == entity::Type::track_DR_1x2_1) {
                offsets_from_trigger_to_check.push_back({ 16.0F, 16.0F });
                offsets_from_trigger_to_check.push_back({ 0.0F, 16.0F });
            } else if (entity->type() == entity::Type::track_LD_2x1_1) {
                offsets_from_trigger_to_check.push_back({ -16.0F, 0.0F });
                offsets_from_trigger_to_check.push_back({ -16.0F, 16.0F });
            }

            for (auto& offset : offsets_from_trigger_to_check) {
                if (entity->start_offset() == trigger_entity->start_offset() + offset) {
                    //entity->direction(entity->start_offset() - trigger_entity->start_offset());
                    entity->add_input_object(trigger_entity);
                }
            }

        }
    }

    void Game::quad_node_check_collision() {
        std::vector<std::thread> threads;
        for (std::pair<Vec2I, std::unique_ptr<QuadNode>>& quad_node : m_level_quad_nodes) {
            if (!quad_node.second || !transform::Manager::get(m_level_transform_id)) continue;

            auto quad_node_check_collision = [&]() {
                std::unique_lock<std::mutex> quad_node_lock(quad_node_mutex);

                quad_node.second->clear();
                quad_node.second->init(m_window_w, m_window_h,
                    NUM_VISIBLE_LAYERS - 1,
                    { (quad_node.first.x - 1) * 256.0F + transform::Manager::get(m_level_transform_id)->position.x,
                    (quad_node.first.y - 1) * 256.0F + transform::Manager::get(m_level_transform_id)->position.y,
                    256.0F, 256.0F });

                for (aabb::Object* aabb : aabb::Manager::get_objects()) {
                    if (!aabb || !aabb->is_active()) continue;

                    aabb->quad_node_ids.clear();

                    for (auto& point : aabb->points()) {
                        //if (!aabb->parent) continue;
                        quad_node.second->insert_point(aabb, point);
                    }
                }
                quad_node_lock.unlock();

                quad_node.second->check_collision();
                };
            threads.emplace_back(std::thread(quad_node_check_collision));
        }
        //Console::log("threads size: ", threads.size(), "\n");
        for (auto& i : threads) {
            i.join();
        }
    }
}