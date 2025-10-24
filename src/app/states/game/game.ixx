module;
#include <filesystem>
#include <sstream>
#include <vector>
#include <map>
#include <list>

export module state.game;

import state;
import bitmap_text;
import console;
import input;
import line;
import sprite;
import transform;
import types;

import camera;
import collider.aabb;
import entity;
import entity.particle;
import plane;
import player;
import quad_node;

import particle;

export namespace state {
    class Game : public Object {
    private:        
        U16 m_window_w = 0,
            m_window_h = 0;

        I32 m_level_transform_id = -1;

        U32 m_timer = 0;
        player::Object m_player;
        //player::Object m_player2;
        //player::Object m_player3;

        Vec2F m_start_position = { 0.0F, 0.0F };
        //entity::Type m_level_start = entity::Type::level_start;

        std::vector<I32> m_level_sprite_ids;

        std::vector<entity::Object*> m_entity_objects,
                                     m_trigger_entity_objects,
                                     m_water_entity_objects,
                                     m_unlocked_entity_objects;
                                
        std::vector<std::pair<Vec2I, std::unique_ptr<QuadNode>>> m_level_quad_nodes;

        std::filesystem::path m_level_path = "res/level/test.bin";
        
        multiplane::Object m_background_plane;

    public:
        Game() = delete;
        Game(cU16 window_w, cU16 window_h, const std::filesystem::path level_path, start::Info start_info);
        ~Game() {
            clear();
        }        
        void clear() {
            Console::log("state::Game::clear()\n");
            Camera::remove_transform_id(m_level_transform_id);
            Camera::remove_transform_id(m_player.get_transform_id());
            Camera::difference = 0.0F;
            Camera::clear();

            m_visible_layers.clear();
            //m_background_plane.clear();

            m_level_quad_nodes.clear();
            
            for (auto& i : m_entity_objects) {
                delete i;
                i = nullptr;
            }
            m_entity_objects.clear();
            m_water_entity_objects.clear();
            m_unlocked_entity_objects.clear();
            m_trigger_entity_objects.clear();

            input::Manager::erase(m_input_id);
            

            Console::log("sprite::Manager::size: ", sprite::Manager::size(), " unused size: ", sprite::Manager::unused_size(), "\n");
            for (auto& i : m_level_sprite_ids) {
                sprite::Manager::erase(i);
            }
            m_level_sprite_ids.clear();
            //sprite::Manager::clear();       // FIXME: Why the need ??

            aabb::Manager::clear();
            
            particle::Manager::clear();

            transform::Manager::erase(m_level_transform_id);

        }
        void check_entities_to_add_input_from(entity::Object* trigger_entity);

        virtual void update(cU64 dt) override;
        virtual void update_unlocked() override;

        void draw(std::unique_ptr<Window>& window, cU8 layer) override;

        bool is_all_valid() const {
            return transform::Manager::is_valid(m_level_transform_id) &&                 
                   input::Manager::is_valid(m_input_id);
        }

        std::filesystem::path level_path_to_save() override {
            return m_level_path;
        }

        std::map<U16, std::string> load_types_from_bin();

        Vec2F level_velocity() { return transform::Manager::get(m_level_transform_id) ? transform::Manager::get(m_level_transform_id)->velocity : Vec2F{}; }
        void level_velocity(cVec2F v) {
            if (transform::Manager::get(m_level_transform_id)) transform::Manager::get(m_level_transform_id)->velocity = v;
        }
        cF32 level_x() { return transform::Manager::get(m_level_transform_id) ? transform::Manager::get(m_level_transform_id)->position.x : 0.0F; }
        void level_x(cF32 x) { if (transform::Manager::get(m_level_transform_id)) transform::Manager::get(m_level_transform_id)->position.x = x; }

        cF32 level_y() { return transform::Manager::get(m_level_transform_id) ? transform::Manager::get(m_level_transform_id)->position.y : 0.0F; }
        void level_y(cF32 y) { if (transform::Manager::get(m_level_transform_id)) transform::Manager::get(m_level_transform_id)->position.y = y; }

        Vec2F level_position() { return transform::Manager::get(m_level_transform_id) ? transform::Manager::get(m_level_transform_id)->position : Vec2F{}; }
        void level_position(cVec2F p) {
            if (transform::Manager::get(m_level_transform_id)) transform::Manager::get(m_level_transform_id)->position = p;
        }

        void load_level(const std::filesystem::path& path);

        void quad_node_check_collision();
    };
}