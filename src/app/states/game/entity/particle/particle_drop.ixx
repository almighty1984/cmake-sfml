module;
#include <cmath>

export module entity.particle.drop;
import console;
import entity.particle;
import collider.aabb;

export namespace entity {
    class ParticleDrop : public Particle {
    public:
        ParticleDrop() {
            m_type = Type::particle_drop;
            m_state = m_next_state = State::idle;
        }
        void update_idle() override {
            if (!sprite()) return;
            m_parent = nullptr;

            if (m_is_first_state_update) {
                m_is_first_state_update = false;
                for (auto& i : m_aabb_ids) {
                    if (aabb::Manager::get(i)) {
                        aabb::Manager::get(i)->is_active(false);
                    }
                }
            }

            if (m_time_in_state <= 5) {
                ++m_time_in_state;                
            }
            if (m_time_in_state == 5) {
                for (auto& i : m_aabb_ids) {
                    if (aabb::Manager::get(i)) {
                        aabb::Manager::get(i)->is_active(true);
                    }
                }
            }
            transform()->velocity.y += transform()->acceleration.y;

            F32 radians = std::atan2(transform()->velocity.y, transform()->velocity.x);
            if (radians < 0.0F) radians += 3.1415926535f * 2.0F;

            cF32 degrees = radians * 180.0F / 3.1415926535F;
                
            sprite()->set_rotation(degrees);
        }
        void update_dead() override {
            if (!sprite()) return;

            if (m_is_first_state_update) {
                m_is_first_state_update = false;
                m_time_left_alive = 0;
                m_time_left_dead = m_time_to_be_dead;

                for (auto& i : m_aabb_ids) {
                    if (aabb::Manager::get(i)) {
                        aabb::Manager::get(i)->is_active(false);
                    }
                }
                //Console::log("ParticleDrop update_dead ", (int)sprite()->layer, "\n");
            }
            transform()->velocity = {};
            if (m_parent && entity::is_water_line(m_parent->type())) {
                transform()->position.y = m_parent->transform()->position.y - 8.0F;
            }
        }

        void collide_x(aabb::cInfo our, aabb::cInfo other) override {
            if (!other.owner || !other.owner->transform() || is_dead()) return;

            cRectF our_rect = { aabb::Manager::get(our.id)->points().at(0).x, aabb::Manager::get(our.id)->points().at(0).y,
                        aabb::Manager::get(our.id)->points().at(3).x, aabb::Manager::get(our.id)->points().at(3).y };

            entity::cType other_type = other.owner->type();
            aabb::cName   other_aabb_name = aabb::Manager::get(other.id)->name;
            cRectF        other_rect = { aabb::Manager::get(other.id)->points().at(0).x, aabb::Manager::get(other.id)->points().at(0).y,
                                         aabb::Manager::get(other.id)->points().at(3).x, aabb::Manager::get(other.id)->points().at(3).y };

            cF32 overlap_x = our_rect.x < other_rect.x ? our_rect.w - other_rect.x : -(other_rect.w - our_rect.x);

            cVec2F other_size = { other_rect.w - other_rect.x, other_rect.h - other_rect.y };

            if (other_type == entity::Type::clip_ledge
                &&
                (our_rect.h < other_rect.y + other_size.y / 4.0F || our_rect.y > other_rect.h - other_size.y / 4.0F)) {
                
                collide_y(our, other);
                return;
            }

            if (entity::is_water_line(other.owner->type())) {
                collide_y(our, other);
            }
            else if (other_type == entity::Type::clip       ||
                     other_type == entity::Type::clip_duck  ||
                     other_type == entity::Type::clip_ledge ||
                     (other_type == entity::Type::clip_L  && transform()->velocity.x > 0.0F) ||
                     (other_type == entity::Type::clip_LD && transform()->velocity.x > 0.0F) ||
                     (other_type == entity::Type::clip_R  && transform()->velocity.x < 0.0F) ||
                     (other_type == entity::Type::clip_RD && transform()->velocity.x < 0.0F)
                ) {
                transform()->position.x -= overlap_x;
                transform()->velocity.x = 0.0F;
                transform()->velocity.y = 0.0F;

                if (other_rect.x < our_rect.x) {
                    sprite()->set_rotation(180.0f);
                } else {
                    sprite()->set_rotation(0.0f);
                }
                m_next_state = entity::State::dead;
            }            
        }
        void collide_y(aabb::cInfo our, aabb::cInfo other) override {
            if (!other.owner || !other.owner->transform() || is_dead()) return;

            cRectF our_rect = { aabb::Manager::get(our.id)->points().at(0).x, aabb::Manager::get(our.id)->points().at(0).y,
                        aabb::Manager::get(our.id)->points().at(3).x, aabb::Manager::get(our.id)->points().at(3).y };

            entity::cType other_type = other.owner->type();
            aabb::cName   other_aabb_name = aabb::Manager::get(other.id)->name;
            cRectF        other_rect = { aabb::Manager::get(other.id)->points().at(0).x, aabb::Manager::get(other.id)->points().at(0).y,
                                         aabb::Manager::get(other.id)->points().at(3).x, aabb::Manager::get(other.id)->points().at(3).y };

            cF32 overlap_y = our_rect.y < other_rect.y ? our_rect.h - other_rect.y : -(other_rect.h - our_rect.y);

            if (other_type == entity::Type::clip_L || other_type == entity::Type::clip_R) {
                collide_x(our, other);
                return;
            }

            if (other_rect.y > our_rect.y && transform()->velocity.y > 0.0f) {
                if (entity::is_water_line(other_type)) {
                    transform()->position.y -= overlap_y;
                    transform()->velocity.x = 0.0F;
                    transform()->velocity.y = 0.0F;
                    //transform()->moved_velocity = other.owner->transform()->velocity;
                    m_next_state = entity::State::dead;
                    m_time_left_dead = m_time_to_be_dead;

                    m_parent = other.owner;
                }                
                else if (other_type == entity::Type::clip       ||
                         other_type == entity::Type::clip_U     ||
                         other_type == entity::Type::clip_ledge ||
                         other_type == entity::Type::slope_U    ||
                         entity::is_slope(other_type)                    
                    ) {
                    transform()->position.y -= overlap_y;
                    transform()->position.y += 2.0f;

                    transform()->velocity.y = 0.0F;
                    transform()->acceleration.y = 0.0F;

                    transform()->velocity.x = 0.0F;

                    sprite()->set_rotation(90.0f);

                    if (other_type == entity::Type::slope_L_1x1) {
                        sprite()->set_rotation(45.0f);
                    } else if (other_type == entity::Type::slope_R_1x1) {
                        sprite()->set_rotation(135.0f);
                    } else if (other_type == entity::Type::slope_L_2x1_0 || other_type == entity::Type::slope_L_2x1_1) {
                        sprite()->set_rotation(67.5f);
                    } else if (other_type == entity::Type::slope_R_2x1_0 || other_type == entity::Type::slope_R_2x1_1) {
                        sprite()->set_rotation(112.5f);
                    }

                    m_next_state = entity::State::dead;
                    m_time_left_dead = m_time_to_be_dead;

                    m_parent = nullptr;
                }                
            } else if (other_rect.h < our_rect.h && transform()->velocity.y < 0.0f) {
                if (entity::is_arch(other_type)) {
                    if (other_type == entity::Type::arch_L_1x1) {
                        sprite()->set_rotation(45.0f);
                    } else if (other_type == entity::Type::arch_R_1x1) {
                        sprite()->set_rotation(135.0f);
                    } else if (other_type == entity::Type::arch_L_2x1_0 || other_type == entity::Type::arch_L_2x1_1) {
                        sprite()->set_rotation(112.5f);
                    } else if (other_type == entity::Type::arch_R_2x1_0 || other_type == entity::Type::arch_R_2x1_1) {
                        
                        sprite()->set_rotation(67.5f);
                    }
                    transform()->position.y -= overlap_y;
                    transform()->position.y -= 4.0F;
                    transform()->velocity.y = 0.0F;
                    transform()->acceleration.y = 0.0F;
                    m_next_state = entity::State::dead;
                } else if (other_type == entity::Type::clip ||
                    other_type == entity::Type::clip_D      ||
                    other_type == entity::Type::clip_LD     ||
                    other_type == entity::Type::clip_RD     ||
                    other_type == entity::Type::clip_duck   ||
                    other_type == entity::Type::clip_ledge) {
                    transform()->position.y -= overlap_y;
                    //transform()->position.y -= 2.0F;
                    transform()->velocity.y = 0.0F;
                    transform()->acceleration.y = 0.0F;
                    transform()->velocity.x = 0.0F;
                    //sprite()->is_upended = true;
                    sprite()->set_rotation(270.0f);
                    m_next_state = entity::State::dead;

                    m_parent = nullptr;
                }                
            }
        }
    };
}