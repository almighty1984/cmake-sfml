module;
#include <filesystem>
#include <fstream>
#include <vector>
#include <sstream>
module state.game;
import camera;
import collider.aabb;
import console;
import input;
import line;
import quad_node;
import sprite;
import transform;
import types;

namespace state {    
    Game::Game(u16c window_w, u16c window_h, std::filesystem::path level_path, start::Info start) {
        //Console::log("Game::Game level: ", level_path, " start: ", start_position.x, " ", start_position.y, "\n");
        current_state(state::Type::game);
        m_window_w = window_w, m_window_h = window_h;
        m_level_path = level_path;

        start_info(start);

        m_transform_id = transform::Set::make();
        m_fps_text.transform_id = m_transform_id;
        m_fps_text.layer = NUM_VISIBLE_LAYERS - 1;

        //Console::log("state::Game fps_text transform id: ", m_transform_id, "\n");

        m_background_plane.create(3, 1);

        m_level_transform_id = transform::Set::make();
        m_player.level_transform_id = m_level_transform_id;

        //Console::log("level transform_id: ", m_level_transform_id, "\n");

        //m_entity_objects.push_back(std::make_unique<player::Object>());
        //m_entity_objects.back()->level_transform_id = m_level_transform_id;
        //m_entity_objects.back()->position(m_start_position);

        load_level(m_level_path);


        /*for (u16 i = 0; i < m_num_quad_nodes.x * m_num_quad_nodes.y; ++i) {
            m_quad_nodes.push_back(std::make_unique<QuadNode>());
            m_quad_nodes.back().get()->transform_id = m_level_transform_id;
        }*/

        for (u8 i = 0; i < NUM_VISIBLE_LAYERS; ++i) {
            add_visible_layer(i);
        }

        

        //m_player.start_offset(m_start_position);
        //m_player.load_config("res/entity/player/player.cfg");
        m_player.position(m_start_position);
        
        //m_player.id = 0;

        //m_player2.position(m_start_position - Vec2f{ 16.0f, 0.0f });


        //m_player2.id = 1;

        /*m_player2.key_up = input::Key::w;
        m_player2.key_down = input::Key::s;
        m_player2.key_left = input::Key::a;
        m_player2.key_right = input::Key::d;
        m_player2.key_jump = input::Key::g;
        m_player2.key_sprint = input::Key::h;
        m_player2.key_melee = input::Key::f;*/

        /*m_player3.set_layer(NUM_VISIBLE_LAYERS - 1);
        m_player3.position(m_start_position + Vec2f{ 16.0f, 0.0f });

        m_player3.key_up = input::Key::i;
        m_player3.key_down = input::Key::k;
        m_player3.key_left = input::Key::j;
        m_player3.key_right = input::Key::l;
        m_player3.key_jump = input::Key::r;
        m_player3.key_sprint = input::Key::e;
        m_player3.key_melee = input::Key::t;*/

        //m_player3.id = 2;

        //transform::Set::at(m_transform_id)->velocity = { 2.0f, 1.0f };
        //transform::Set::at(m_transform_id)->deceleration = { 0.001f, 0.001f };
        //Console::log("Game transform_id: ", m_transform_id, "\n");

        m_input_id = input::Set::make();

        //Console::log("sprite::Objects size: ", sprite::Set::size(), "\n");


       /* for (auto& i : m_background_plane.get_transform_ids()) {
            m_camera.add_transform_id(i);
        }*/

        Camera::focus_transform = m_player.get_transform_id();
        Camera::add_transform_id(m_level_transform_id);

        Camera::add_transform_id(m_player.get_transform_id());
        //m_camera.add_transform_id(m_player2.get_transform_id());
        //m_camera.add_transform_id(m_player3.get_transform_id());

        //Vec2f camera_position = m_player.position() - Vec2f{ window_w / 2.0f, window_h / 2.0f };
        Vec2f camera_position = m_start_position - Vec2f{ window_w / 2.0f, window_h / 2.0f + 8.0f };


        Console::log("state::Game::Game camera position: ", camera_position.x, " ", camera_position.y, "\n");

        Camera::set_position(camera_position);

        //m_camera.add_transform_id(m_transform_id);

    }

    void Game::check_entities_to_add_input_from(std::shared_ptr<entity::Object> trigger_entity) {
        if (!entity::is_conduit(trigger_entity->type()) && !entity::is_logic(trigger_entity->type()) && !entity::is_track(trigger_entity->type())
            && trigger_entity->type() != entity::Type::trigger
            ) {
            return;
        }
        for (auto& entity : m_entity_objects) {
            if (!entity::is_conduit(entity->type()) && !entity::is_logic(entity->type()) && !entity::is_track(entity->type())
                //&& entity->type() != entity::Type::trigger
                ) {
                continue;
            }
            //bool is_to_add_input = false;
            //Console::log("check_entities_to_add_input_from: ", entity::type_to_string(entity->type()), "\n");
            std::vector<Vec2f> offsets_from_trigger_to_check;
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
                entity->type() == entity::Type::track_UR_1x2_0) {
                offsets_from_trigger_to_check.push_back({ 0.0f,-16.0f });
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
                offsets_from_trigger_to_check.push_back({ 0.0f, 16.0f });
            } else if (entity->type() == entity::Type::logic_not_L ||
                entity->type() == entity::Type::conduit_L ||
                entity->type() == entity::Type::conduit_LU ||
                entity->type() == entity::Type::conduit_LD ||
                entity->type() == entity::Type::track_L ||
                entity->type() == entity::Type::track_LU ||
                entity->type() == entity::Type::track_LD ||
                entity->type() == entity::Type::track_LU_1x1 ||
                entity->type() == entity::Type::track_LD_1x1) {
                offsets_from_trigger_to_check.push_back({ -16.0f, 0.0f });
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
                offsets_from_trigger_to_check.push_back({ 16.0f, 0.0f });
            } else if (entity->type() == entity::Type::logic_and_UD || entity->type() == entity::Type::logic_or_UD) {
                offsets_from_trigger_to_check.push_back({ 0.0f, 16.0f });
                offsets_from_trigger_to_check.push_back({ 0.0f,-16.0f });
            } else if (entity->type() == entity::Type::logic_and_LR || entity->type() == entity::Type::logic_or_LR) {
                offsets_from_trigger_to_check.push_back({ 16.0f, 0.0f });
                offsets_from_trigger_to_check.push_back({ -16.0f, 0.0f });
            } else if (entity->type() == entity::Type::track_RU_2x1_1) {
                offsets_from_trigger_to_check.push_back({ 16.0f, -16.0f });
            } else if (entity->type() == entity::Type::track_RD_2x1_1) {
                offsets_from_trigger_to_check.push_back({ 16.0f, 16.0f });
            } else if (entity->type() == entity::Type::track_UL_1x2_1) {
                offsets_from_trigger_to_check.push_back({ -16.0f, -16.0f });
                offsets_from_trigger_to_check.push_back({ 0.0f, -16.0f });
            } else if (entity->type() == entity::Type::track_UR_1x2_1) {
                offsets_from_trigger_to_check.push_back({ 16.0f, -16.0f });
                //offsets_from_trigger_to_check.push_back({  0.0f, -16.0f });
            } else if (entity->type() == entity::Type::track_DL_1x2_1) {
                offsets_from_trigger_to_check.push_back({ -16.0f, 16.0f });
                offsets_from_trigger_to_check.push_back({ 0.0f, 16.0f });
            } else if (entity->type() == entity::Type::track_DR_1x2_1) {
                offsets_from_trigger_to_check.push_back({ 16.0f, 16.0f });
                offsets_from_trigger_to_check.push_back({ 0.0f, 16.0f });
            } else if (entity->type() == entity::Type::track_LD_2x1_1) {
                offsets_from_trigger_to_check.push_back({ -16.0f, 0.0f });
                offsets_from_trigger_to_check.push_back({ -16.0f, 16.0f });
            }

            for (auto& offset : offsets_from_trigger_to_check) {
                if (entity->start_offset() == trigger_entity->start_offset() + offset) {
                    //entity->direction(entity->start_offset() - trigger_entity->start_offset());
                    entity->add_input_object(trigger_entity);
                }
            }

        }
    }
}