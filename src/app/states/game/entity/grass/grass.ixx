module;
#include <cmath>
#include <sstream>

export module entity.grass;
import anim;
import camera;
import collider.aabb;
import console;
import entity;
import random;

export namespace entity {
    class Grass : public Object {
        I32 m_prev_source_x     = 0;

        U8 m_time_limit         = 8,
           m_random_time_offset = 0;

        bool m_is_in_view = false;
    public:        
        void collide_x(aabb::cInfo our, aabb::cInfo other) override {
            if (!sprite() || !our.owner || !other.owner || !other.owner->anim() || !other.owner->sprite()) return;

            entity::cType other_type = other.owner->type();

            cRectF our_rect = { aabb::Manager::get(our.id)->points().at(0).x,   aabb::Manager::get(our.id)->points().at(0).y,
                                aabb::Manager::get(our.id)->points().at(3).x,   aabb::Manager::get(our.id)->points().at(3).y };
            cRectF other_rect = { aabb::Manager::get(other.id)->points().at(0).x, aabb::Manager::get(other.id)->points().at(0).y,
                                  aabb::Manager::get(other.id)->points().at(3).x, aabb::Manager::get(other.id)->points().at(3).y };

            cF32 our_w = our_rect.w - our_rect.x;
            cF32 our_mid_x = our_rect.x + our_w / 2.0f;

            cF32 other_w = other_rect.w - other_rect.x;
            cF32 other_mid_x = other_rect.x + other_w / 2.0f;

            if (other_type == entity::Type::player &&
                (other.owner->anim()->type == anim::Type::run || other.owner->anim()->type == anim::Type::walk) &&
                (other.owner->sprite()->source_rect.x == 64 || other.owner->sprite()->source_rect.x == 320)) {
                return;
            }

            if (other_type == entity::Type::bug  ||
                other_type == entity::Type::frog ||
                other_type == entity::Type::player) {
                //Console::log("source x: ", other.owner->sprite()->source_rect.x, "\n");
                //Console::log("anim type: ", anim::to_string(other.owner->anim()->type), "\n");                

                int num = 0;
                if (our_mid_x > other_mid_x) {
                    //sprite()->is_leftward = false;
                    num = (int)std::abs(our_mid_x - other_mid_x);
                } else {
                    //sprite()->is_leftward = true;
                    num = (int)std::abs(our_mid_x - other_mid_x);
                }
                int tile_x = 32 * 7 - ((num / 4) * 32);
                tile_x -= tile_x % 32;

                sprite()->source_rect.x = tile_x;
                //m_time_left_dead = m_time_to_be_dead;
            }
        };
        void collide_y(aabb::cInfo our, aabb::cInfo other) override {
            collide_x(our, other);
        };

        void update() override {
            if (m_is_first_update) {
                m_is_first_update = false;
                m_random_time_offset = random::Generator::number(0, m_time_limit / 4);
                //Console::log("m_random_time_offset: ", (I16)m_random_time_offset, "\n");
                //Console::log("grass origin: ", sprite()->origin.x, " ", sprite()->origin.y, "\n");
            }
            //Console::log("pos: ", transform()->position.x, " ", m_start_offset.x, "\n");            
            if (Camera::position.x          > m_start_offset.x ||
                Camera::position.x + 320.0f < m_start_offset.x
                ) {
                m_is_in_view = false;
                for (auto& i : m_aabb_ids) {
                    if (aabb::Manager::get(i)) {
                        aabb::Manager::get(i)->is_active(false);
                    }
                }
                //return;
            } else {
                m_is_in_view = true;
                for (auto& i : m_aabb_ids) {
                    if (aabb::Manager::get(i)) {
                        aabb::Manager::get(i)->is_active(true);
                    }
                }
            }
            
            ++m_time_in_state; 
            if (m_time_in_state > m_time_limit + m_random_time_offset) {
                m_time_in_state = 0;
                                
                if (m_is_in_view) {
                    m_is_first_update = true;
                }

                if (sprite()->source_rect.x == 0) {
                    if (m_prev_source_x == 32) {
                        sprite()->is_leftward = !sprite()->is_leftward;
                    }
                    sprite()->source_rect.x = 32;
                    m_prev_source_x = 0;
                } else if (sprite()->source_rect.x == 32) {
                    m_prev_source_x == 0 ? sprite()->source_rect.x = 64 : sprite()->source_rect.x = 0;
                    m_prev_source_x = 32;
                } else if (sprite()->source_rect.x == 64) {
                    m_prev_source_x == 32 ? sprite()->source_rect.x = 96 : sprite()->source_rect.x = 32;
                    m_prev_source_x = 64;
                } else if (sprite()->source_rect.x >= 96) {
                    sprite()->source_rect.x = 64;
                    m_prev_source_x = 96;
                }
            }
        }        
    };
}