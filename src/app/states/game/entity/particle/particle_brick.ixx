module;
#include <cmath>

export module entity.particle.brick;
import console;
import entity.particle;
import collider.aabb;
import random;
import sound;

export namespace entity {
    class ParticleBrick : public Particle {
    public:
        ParticleBrick() {
            m_type = Type::particle_brick;
            m_state = m_next_state = State::idle;
        }
        void update_idle() override {
            if (!transform()) return;

            if (m_is_first_state_update) {
                m_is_first_state_update = false;
                m_time_left_alive = m_time_to_be_alive + random::Generator::number(0, 50);

                Console::log("m_time_left_alive: ", m_time_left_alive, "\n");
                
                for (auto& i : m_aabb_ids) {
                    if (aabb::Manager::get(i)) {
                        aabb::Manager::get(i)->is_active(false);
                    }
                }
            }
            m_parent = nullptr;

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

            if (transform()->velocity.x > transform()->acceleration.x || transform()->velocity.x < -transform()->acceleration.x ||
                transform()->velocity.y > transform()->acceleration.y || transform()->velocity.y < -transform()->acceleration.y) {
                F32 radians = std::atan2(transform()->velocity.y, transform()->velocity.x);
                if (radians < 0.0F) radians += (3.1415926535F * 2.0F);

                cF32 degrees = radians * 180.0F / 3.1415926535F;

                sprite()->set_rotation(degrees);
            }
        }
        
        void collide_x(aabb::cInfo our, aabb::cInfo other) override {
            if (!other.owner || other.owner == this || !other.owner->transform() || !transform() || !sprite()) return;

            cRectF our_rect = { aabb::Manager::get(our.id)->points().at(0).x, aabb::Manager::get(our.id)->points().at(0).y,
                        aabb::Manager::get(our.id)->points().at(3).x, aabb::Manager::get(our.id)->points().at(3).y };

            entity::cType other_type = other.owner->type();
            aabb::cName   other_aabb_name = aabb::Manager::get(other.id)->name;
            cRectF        other_rect = { aabb::Manager::get(other.id)->points().at(0).x, aabb::Manager::get(other.id)->points().at(0).y,
                                         aabb::Manager::get(other.id)->points().at(3).x, aabb::Manager::get(other.id)->points().at(3).y };

            cF32 overlap_x = our_rect.x < other_rect.x ? our_rect.w - other_rect.x : -(other_rect.w - our_rect.x);

            cVec2F other_size = { other_rect.w - other_rect.x, other_rect.h - other_rect.y };

            cVec2F other_velocity = other.owner->transform()->velocity;
            cVec2F our_velocity   = transform()->velocity;
                        
            if (other_type == entity::Type::arch_L_1x1 || other_type == entity::Type::arch_R_1x1) {
                transform()->velocity.x *= 0.5F;                
            }
            else if (other_type == entity::Type::arch_L_2x1_0 || other_type == entity::Type::arch_L_2x1_1 ||
                     other_type == entity::Type::arch_R_2x1_0 || other_type == entity::Type::arch_R_2x1_1) {
                transform()->velocity.x *= 0.75F;
                //transform()->velocity.y *= -0.9F;
            }
            else if (other_type == entity::Type::brick ||
                other_type == entity::Type::clip  ||
               (other_type == entity::Type::clip_L && transform()->velocity.x > 0.0F) ||
               (other_type == entity::Type::clip_R && transform()->velocity.x < 0.0F) ||
                other_type == entity::Type::clip_ledge
                ) {

                if (other_rect.x < our_rect.x && transform()->velocity.x > 0.0F ||
                    other_rect.w > our_rect.w && transform()->velocity.x < 0.0F) return;

                transform()->position.x -= overlap_x;
                transform()->velocity.x *= -0.8F;

                //transform()->velocity.y = 0.0F;

                /*if (other_rect.x < our_rect.x) {
                    sprite()->set_rotation(180.0f);
                } else {
                    sprite()->set_rotation(0.0f);
                }*/
            }
            else if (other_type == entity::Type::frog) {
                if (std::abs(transform()->velocity.x) >= 2.0F) {
                    other.owner->hurt(this);
                    cVec2F hit_pos = position() + Vec2F{ -8.0F, -8.0F };

                    if (m_melee_sound_id != -1 && sound::Manager::get(m_melee_sound_id)) {
                        sound::Manager::get(m_melee_sound_id)->position({ hit_pos.x / 160.0F, hit_pos.y / 90.0F });
                        sound::Manager::get(m_melee_sound_id)->play();
                    }
                    spawn(particle::Type::hit, hit_pos, {});
                }
                transform()->position.x -= overlap_x;
                transform()->velocity.x *= -1.0F;
            } else if (other_type == entity::Type::slope_L_1x1) {
                //transform()->position.x -= overlap_x;
                collide_y(our, other);
            } else if (other_type == entity::Type::slope_R_1x1) {
                //transform()->position.x -= overlap_x;
                collide_y(our, other);
            } else if (other_type == entity::Type::slope_L_2x1_0 || other_type == entity::Type::slope_L_2x1_1) {
                //transform()->position.x -= overlap_x;
                collide_y(our, other);
            } else if (other_type == entity::Type::slope_R_2x1_0 || other_type == entity::Type::slope_R_2x1_1) {
                //transform()->position.x -= overlap_x;
                collide_y(our, other);
            } else if (other_type == entity::Type::particle_brick) {
                //if (is_hurting() || std::abs(other_velocity.x) < 1.0F) return;

                //transform()->position.x -= overlap_x;
                //transform()->velocity.x = our_velocity.x * 0.4F + other_velocity.x * 0.4F;
                //other.owner->transform()->velocity.x = our_velocity.x * 0.5F + other_velocity.x * 0.5F;

                //transform()->velocity.x = other_velocity.x * 0.8F;
                //other.owner->transform()->velocity.x = our_velocity.x * 0.8F;

                /*if (std::abs(transform()->velocity.x) > std::abs(other.owner->transform()->velocity.x)) {
                    other.owner->transform()->velocity.x = transform()->velocity.x;
                } else {
                    transform()->velocity.x = other.owner->transform()->velocity.x;
                }*/               
            } else if (other_type == entity::Type::player) {
                if (is_hurting() || std::abs(other_velocity.x) < 1.0F) return;

                m_time_left_hurt = m_time_to_hurt;

                //transform()->position.x -= overlap_x;
                //transform()->position.y -= 2.0F;
                transform()->velocity.y = other_velocity.y - 2.0F;

                transform()->velocity.x = other_velocity.x * 1.2F;
                //other.owner->transform()->velocity.x = other_velocity.x * 0.8F + our_velocity.x * 0.2F;

                //other.owner->transform()->velocity.x *= 0.95F;
            } else if (entity::is_water_line(other.owner->type())) {
                collide_y(our, other);
            }
        }
        void collide_y(aabb::cInfo our, aabb::cInfo other) override {
            if (!other.owner || other.owner == this || !other.owner->transform() || !transform() || !sprite()) return;

            cRectF our_rect = { aabb::Manager::get(our.id)->points().at(0).x, aabb::Manager::get(our.id)->points().at(0).y,
                        aabb::Manager::get(our.id)->points().at(3).x, aabb::Manager::get(our.id)->points().at(3).y };

            entity::cType other_type = other.owner->type();
            aabb::cName   other_aabb_name = aabb::Manager::get(other.id)->name;
            cRectF        other_rect = { aabb::Manager::get(other.id)->points().at(0).x, aabb::Manager::get(other.id)->points().at(0).y,
                                         aabb::Manager::get(other.id)->points().at(3).x, aabb::Manager::get(other.id)->points().at(3).y };

            cF32 overlap_y = our_rect.y < other_rect.y ? our_rect.h - other_rect.y : -(other_rect.h - our_rect.y);

            cVec2F other_velocity = other.owner->transform()->velocity;
            cVec2F our_velocity = transform()->velocity;
                               
            if (entity::is_arch(other_type)) {                
                if (transform()->velocity.y < 0.0F) {
                    transform()->position.y -= overlap_y;
                    transform()->velocity.y *= -1.0F;
                }
            }
            else if (other_type == entity::Type::brick || other_type == entity::Type::clip || other_type == entity::Type::clip_duck || other_type == entity::Type::clip_ledge) {
                if (transform()->velocity.y < 0.0F && our_rect.h < other_rect.h) return;
                if (transform()->velocity.y > 0.0F && our_rect.y > other_rect.y) return;

                transform()->position.y -= overlap_y;
                transform()->velocity.y *= -0.5F;
                if (transform()->velocity.y >= -transform()->acceleration.y && transform()->velocity.y <= transform()->acceleration.y) {
                    transform()->velocity.y = 0.0F;
                }
                transform()->moved_velocity = 0.0F;

                transform()->velocity.x *= 0.5F;
            } else if (other_type == entity::Type::clip_U || other_type == entity::Type::slope_U) {
                if (transform()->velocity.y < 0.0F) return;
                if (transform()->velocity.y > 0.0F && our_rect.y > other_rect.y) return;

                transform()->position.y -= overlap_y;
                transform()->velocity.y *= -0.5F;
                if (transform()->velocity.y >= -transform()->acceleration.y && transform()->velocity.y <= transform()->acceleration.y) {
                    transform()->velocity.y = 0.0F;
                }
                transform()->moved_velocity = 0.0F;

                transform()->velocity.x *= 0.5F;

                //sprite()->set_rotation(90.0f);
            }
            else if (other_type == entity::Type::clip_D || other_type == entity::Type::clip_LD || other_type == entity::Type::clip_RD) {
                if (transform()->velocity.y > 0.0F) return;
                //if (transform()->velocity.y < 0.0F && our_rect.h < other_rect.h) return;

                transform()->position.y -= overlap_y;
                transform()->velocity.y *= -0.5F;
                transform()->moved_velocity = 0.0F;

                transform()->velocity.x *= 0.5F;
            }
            else if (other_type == entity::Type::frog) {
                if (std::abs(transform()->velocity.y) >= 2.0F) {
                    other.owner->hurt(this);
                    cVec2F hit_pos = position() + Vec2F{ -8.0F, -8.0F };
                    if (m_melee_sound_id != -1 && sound::Manager::get(m_melee_sound_id)) {
                        sound::Manager::get(m_melee_sound_id)->position({ hit_pos.x / 160.0F, hit_pos.y / 90.0F });
                        sound::Manager::get(m_melee_sound_id)->play();
                    }
                    spawn(particle::Type::hit, hit_pos, {});
                }
                if (our_rect.y < other_rect.y) {
                    transform()->position.y -= overlap_y;
                    transform()->velocity.y *= -1.0F;
                }                
            }
            else if (other_type == entity::Type::slope_L_1x1) {
                transform()->position.y -= overlap_y;
                transform()->velocity.y *= -0.5F;
                if (transform()->velocity.y >= -transform()->acceleration.y && transform()->velocity.y <= transform()->acceleration.y) {
                    transform()->velocity.y = 0.0F;
                }
                sprite()->set_rotation(45.0f);

                transform()->velocity.x *= 0.5F;
            } else if (other_type == entity::Type::slope_R_1x1) {
                transform()->position.y -= overlap_y;
                transform()->velocity.y *= -0.5F;
                if (transform()->velocity.y >= -transform()->acceleration.y && transform()->velocity.y <= transform()->acceleration.y) {
                    transform()->velocity.y = 0.0F;
                }
                sprite()->set_rotation(135.0f);

                transform()->velocity.x *= 0.5F;
            } else if (other_type == entity::Type::slope_L_2x1_0 || other_type == entity::Type::slope_L_2x1_1) {
                transform()->position.y -= overlap_y;
                transform()->velocity.y *= -0.5F;
                if (transform()->velocity.y >= -transform()->acceleration.y && transform()->velocity.y <= transform()->acceleration.y) {
                    transform()->velocity.y = 0.0F;
                }
                sprite()->set_rotation(67.5f);

                transform()->velocity.x *= 0.5F;
            } else if (other_type == entity::Type::slope_R_2x1_0 || other_type == entity::Type::slope_R_2x1_1) {
                transform()->position.y -= overlap_y;
                transform()->velocity.y *= -0.5F;
                if (transform()->velocity.y >= -transform()->acceleration.y && transform()->velocity.y <= transform()->acceleration.y) {
                    transform()->velocity.y = 0.0F;
                }
                sprite()->set_rotation(112.5f);

                transform()->velocity.x *= 0.5F;
            }
            else if (other_type == entity::Type::player) {
                if (is_hurting() || std::abs(other_velocity.x) < 1.0F) return;
                m_time_left_hurt = m_time_to_hurt;

                transform()->position.y -= 2.0F;
                transform()->velocity.x = other_velocity.x * 1.2F;
                transform()->velocity.y = other_velocity.y - 1.0F;
            } else if (other_type == entity::Type::particle_brick) {
                //if (is_hurting() || std::abs(other_velocity.x) < 1.0F) return;
                ////if (other_rect.y > our_rect.y) return;

                ////transform()->position.y -= overlap_y;
                ////transform()->velocity.x = other_velocity.y * 0.5F;
                ////other.owner->transform()->velocity.x = our_velocity.y * 0.5F;

                ////transform()->velocity.y = our_velocity.y * 0.4F + other_velocity.y * 0.4F;
                ////other.owner->transform()->velocity.y = our_velocity.y * 0.4F + other_velocity.y * 0.4F;
                //transform()->velocity.y = 0.0F;

                //sprite()->set_rotation(90.0f);

                //transform()->velocity.x *= -0.9F;
            } else if (entity::is_water_line(other_type)) {                
                transform()->acceleration = { 0.05F, 0.05F };
                transform()->max_velocity = { 0.4F, 0.7F };

                if (transform()->velocity.y > 0.0F) {
                    if (m_water_enter_sound_id != -1 && !sound::Manager::get(m_water_enter_sound_id)->is_playing()) {                        
                        sound::Manager::get(m_water_enter_sound_id)->position({ position().x / 160.0F, position().y / 90.0F });
                        sound::Manager::get(m_water_enter_sound_id)->play();
                        spawn_fan(235.0F, 305.0F, 3, particle::Type::drop, transform()->position + Vec2F{ -4.0F, 0.0F }, Vec2F{ transform()->velocity.x * 0.1F, -transform()->velocity.y * 0.1F }, 1.5F);
                    }
                }

            }
        }
    };
}