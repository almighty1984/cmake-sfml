module;
#include <cmath>
#include <sstream>
export module entity.brick;
import console;
import collider.aabb;
import entity;
import sound;

export namespace entity {
    class Brick : public Object {
    public:
        Brick() {
            Console::log("Brick()\n");
            m_state = m_next_state = entity::State::idle;
            //load_config("res/entity/level_center.cfg");
        }
        void hurt(Object* culprit) override {
            m_next_state = entity::State::dead;
            if (m_melee_sound_id != -1 && sound::Manager::get(m_melee_sound_id)) {
                sound::Manager::get(m_melee_sound_id)->position({ position().x / 160.0F, position().y / 90.0F});
                sound::Manager::get(m_melee_sound_id)->play();
            }
        }
        void collide_x(aabb::cInfo our, aabb::cInfo other) override {
            if ((m_parent && other.owner == m_parent) || is_dead()) return;
            if (!sprite() || !our.owner || !other.owner || !other.owner->transform() || !aabb::Manager::get(other.id) || other.owner->is_dead()) return;
            if (aabb::Manager::get(our.id)->points().size() < 4 || aabb::Manager::get(other.id)->points().size() < 4) return;

            entity::cType other_type = other.owner->type();

            aabb::cName other_aabb_name = aabb::Manager::get(other.id)->name;

            cRectF our_rect = { aabb::Manager::get(our.id)->points().at(0).x,   aabb::Manager::get(our.id)->points().at(0).y,
                                  aabb::Manager::get(our.id)->points().at(3).x,   aabb::Manager::get(our.id)->points().at(3).y };
            cRectF other_rect = { aabb::Manager::get(other.id)->points().at(0).x, aabb::Manager::get(other.id)->points().at(0).y,
                                  aabb::Manager::get(other.id)->points().at(3).x, aabb::Manager::get(other.id)->points().at(3).y };

            Vec2F our_dimensions = { aabb::Manager::get(our.id)->points().at(1).x - aabb::Manager::get(our.id)->points().at(0).x,
                                     aabb::Manager::get(our.id)->points().at(3).y - aabb::Manager::get(our.id)->points().at(1).y };

            aabb::cName our_aabb_name = aabb::Manager::get(our.id)->name;

            cF32 overlap_x = our_rect.x < other_rect.x ? our_rect.w - other_rect.x : -(other_rect.w - our_rect.x);

            cVec2F our_velocity = transform()->velocity;
            cVec2F other_velocity = other.owner->transform()->velocity;

            if (m_parent && entity::is_clip(other_type)) {
                if (other_rect.x < our_rect.x) {
                    m_is_near_wall_L = true;
                }
                if (other_rect.w > our_rect.w) {
                    m_is_near_wall_R = true;
                }
                return;
            }
            m_is_near_wall_L = false;
            m_is_near_wall_R = false;

            if (other_type == entity::Type::brick) {
                transform()->position.x -= overlap_x;
                transform()->moved_velocity.x = 0.0F;
                transform()->velocity.x *= -0.9F;
                if (std::abs(transform()->velocity.x >= 1.5F) &&
                    (m_state == entity::State::tossed || other.owner->state() == entity::State::tossed)) {                    
                    hurt(other.owner);
                    other.owner->hurt(this);
                    return;
                }
            } else if (other_type == entity::Type::bug) {
                transform()->position.x -= overlap_x;
                transform()->moved_velocity.x = 0.0F;
                transform()->velocity.x *= -0.9F;
                if (std::abs(transform()->velocity.x >= 1.5F) &&
                    (m_state == entity::State::tossed || other.owner->state() == entity::State::tossed)) {
                    hurt(other.owner);
                    other.owner->hurt(this);
                    return;
                }
            }
            else if (other_type == entity::Type::clip       ||
                     other_type == entity::Type::clip_duck  ||
                     other_type == entity::Type::clip_ledge ||
                    (other_type == entity::Type::clip_L || other_type == entity::Type::clip_LD && transform()->velocity.x > 0.0F) ||
                    (other_type == entity::Type::clip_R || other_type == entity::Type::clip_RD && transform()->velocity.x < 0.0F)
                    ) {                
                transform()->position.x -= overlap_x;
                transform()->moved_velocity.x = 0.0F;

                if (m_state == entity::State::tossed && std::abs(transform()->velocity.x >= 1.5F)) {
                    transform()->velocity.x *= -0.9F;
                    hurt(other.owner);
                    return;
                }
            } else if (other_type == entity::Type::frog) {
                transform()->position.x -= overlap_x;
                transform()->velocity.x *= -0.9F;
                if (m_state == entity::State::tossed && std::abs(transform()->velocity.x) >= 1.5F) {
                    hurt(other.owner);
                    other.owner->hurt(this);
                }
            }
            else if (other_type == entity::Type::player) {
                transform()->velocity.x = other_velocity.x;
                transform()->position.x -= overlap_x;
                if (other.owner->state() == entity::State::swing) {
                    transform()->velocity.x = -other.owner->rotation_speed();
                    hurt(other.owner);
                }
            } else if (other_type == entity::Type::slope_L_1x1) {
                if (m_state == entity::State::tossed && std::abs(transform()->velocity.x) >= 1.5F) {
                    transform()->velocity.x *= -0.9F;
                    hurt(other.owner);
                    return;
                }                
                if (transform()->velocity.x > 0.0F) {
                    transform()->velocity.y = -transform()->velocity.x;
                }
                transform()->moved_velocity = {};                
                m_is_on_ground = true;
                m_is_on_slope = true;
            } else if (other_type == entity::Type::slope_R_1x1) {
                if (m_state == entity::State::tossed && std::abs(transform()->velocity.x) >= 1.5F) {
                    transform()->velocity.x *= -0.9F;
                    hurt(other.owner);
                    return;
                }                
                if (transform()->velocity.x < 0.0F) {
                    transform()->velocity.y = transform()->velocity.x;
                }
                transform()->moved_velocity = {};
                m_is_on_ground = true;
                m_is_on_slope = true;
            } else if (other_type == entity::Type::slope_L_2x1_0 || other_type == entity::Type::slope_L_2x1_1) {
                if (m_state == entity::State::tossed && std::abs(transform()->velocity.x) >= 1.5F) {
                    transform()->velocity.x *= -0.9F;
                    hurt(other.owner);
                    return;
                }                
                transform()->moved_velocity = {};                
                if (transform()->velocity.x > 0.0F) {
                    transform()->velocity.y = -transform()->velocity.x / 2.0F;
                }
                m_is_on_ground = true;
                m_is_on_slope = true;
            } else if (other_type == entity::Type::slope_R_2x1_0 || other_type == entity::Type::slope_R_2x1_1) {
                if (m_state == entity::State::tossed && std::abs(transform()->velocity.x) >= 1.5F) {
                    transform()->velocity.x *= -0.9F;
                    hurt(other.owner);
                    return;
                }                
                transform()->moved_velocity = {};
                if (transform()->velocity.x < 0.0F) {
                    transform()->velocity.y = transform()->velocity.x / 2.0F;
                }
                m_is_on_ground = true;
                m_is_on_slope = true;
            } else if (other_type == entity::Type::spring_U) {
                collide_y(our, other);
            } else if (entity::is_water_line(other_type)) {
                collide_y(our, other);
            }
        };
        void collide_y(aabb::cInfo our, aabb::cInfo other) override {
            if ((m_parent && other.owner == m_parent) || is_dead()) return;
            if (!sprite() || !our.owner || !other.owner || !other.owner->transform() || !aabb::Manager::get(other.id) || other.owner->is_dead()) return;
            if (aabb::Manager::get(our.id)->points().size() < 4 || aabb::Manager::get(other.id)->points().size() < 4) return;

            cRectF our_rect = { aabb::Manager::get(our.id)->points().at(0).x, aabb::Manager::get(our.id)->points().at(0).y,
                                aabb::Manager::get(our.id)->points().at(3).x, aabb::Manager::get(our.id)->points().at(3).y };

            entity::cType other_type = other.owner->type();
            aabb::Name    other_aabb_name = aabb::Manager::get(other.id)->name;
            cRectF        other_rect = { aabb::Manager::get(other.id)->points().at(0).x, aabb::Manager::get(other.id)->points().at(0).y,
                                         aabb::Manager::get(other.id)->points().at(3).x, aabb::Manager::get(other.id)->points().at(3).y };

            cF32 overlap_y = our_rect.y < other_rect.y ? our_rect.h - other_rect.y : -(other_rect.h - our_rect.y);

            cVec2F our_velocity = transform()->velocity;
            cVec2F other_velocity = other.owner->transform()->velocity;

            if (other_type == entity::Type::brick) {
                transform()->position.y -= overlap_y;                
                if (std::abs(transform()->velocity.y - other.owner->transform()->velocity.y) >= 1.5F &&
                    (m_state == entity::State::tossed || other.owner->state() == entity::State::tossed)) {
                    transform()->velocity.y *= -0.9F;
                    hurt(other.owner);
                    return;
                }
                transform()->velocity.y = 0.0F;
                transform()->moved_velocity = other_velocity;
            }
            else if (other_type == entity::Type::clip       ||
                     other_type == entity::Type::clip_duck  ||
                     other_type == entity::Type::clip_ledge ||
                     other_type == entity::Type::clip_D || other_type == entity::Type::clip_LD || other_type == entity::Type::clip_RD) {

                /*if (our_rect.y > other_rect.h - 2.0F) {
                    return;
                }*/

                if (other_type == entity::Type::clip_D && transform()->velocity.y + transform()->moved_velocity.y > 0.0F) {
                    return;
                }
                if (other_type == entity::Type::clip_LD && transform()->velocity.x + transform()->moved_velocity.x < 0.0F
                    && transform()->velocity.y + transform()->moved_velocity.y > 0.0F) {
                    return;
                }
                if (other_type == entity::Type::clip_RD && transform()->velocity.x + transform()->moved_velocity.x > 0.0F
                    && transform()->velocity.y + transform()->moved_velocity.y > 0.0F) {
                    return;
                }

                if (m_state == entity::State::tossed && std::abs(transform()->velocity.y) >= 1.5F) {
                    transform()->velocity.y *= -0.9F;
                    hurt(other.owner);
                    return;
                }
                transform()->position.y -= overlap_y;
                
                transform()->velocity.y = 0.0F;
                if (other_rect.h < our_rect.h) {
                    //transform()->velocity.y *= -1.0F;
                } else {                    
                    transform()->moved_velocity = other_velocity;
                    m_is_on_ground = true;
                }
            }            
            else if (other_type == entity::Type::clip_U || other_type == entity::Type::slope_U) {
                if (transform()->velocity.y < 0.0F) return;
                if (m_state == entity::State::tossed && std::abs(transform()->velocity.y) >= 1.5F) {
                    transform()->velocity.y *= -0.9F;
                    hurt(other.owner);
                    return;
                }
                transform()->position.y -= overlap_y;
                transform()->velocity.y = transform()->moved_velocity.y = 0.0F;
                m_is_on_ground = true;
                m_is_on_slope = other_type == entity::Type::slope_U;
            } else if (other_type == entity::Type::frog) {
                if (our_rect.h < other_rect.y + 4.0F) {
                    transform()->position.y -= overlap_y;

                    if (m_state == entity::State::tossed && std::abs(transform()->velocity.y) >= 1.5F) {
                        transform()->velocity.y *= -0.9F;
                        hurt(other.owner);
                        return;
                    }
                    transform()->velocity.y = 0.0F;
                }
            } else if (other_type == entity::Type::player) {
                if (our_rect.h < other_rect.y + 4.0F) {
                    if (m_state == entity::State::tossed && std::abs(transform()->velocity.y) >= 1.5F) {
                        transform()->velocity.y *= -0.9F;
                        hurt(other.owner);
                        return;
                    }
                    transform()->position.y -= overlap_y;
                    transform()->velocity.y = -std::abs(other_velocity.y);                    
                    return;
                }
                //else if (our_rect.y > other_rect.h - 4.0F) {
                    //other.owner->is_on_ground(true);
                //}
                if (other.owner->state() == entity::State::swing) {
                    transform()->velocity.x = -other.owner->rotation_speed();                    
                    hurt(other.owner);
                    return;                    
                }
            } else if (other_type == entity::Type::slope_L_1x1) {                
                if (m_state == entity::State::tossed && std::abs(transform()->velocity.y) >= 1.5F) {
                    transform()->velocity.y *= -0.9F;
                    hurt(other.owner);
                    return;
                }
                transform()->position.y -= overlap_y;
                //transform()->velocity.y = -transform()->velocity.x;
                if (transform()->velocity.y > 0.0F) {
                    transform()->velocity.x = -transform()->velocity.y;
                }
                transform()->moved_velocity.x = 0.0F;
                transform()->moved_velocity.y = 0.0F;
                m_is_on_ground = true;
                m_is_on_slope = true;
            } else if (other_type == entity::Type::slope_R_1x1) {
                if (m_state == entity::State::tossed && std::abs(transform()->velocity.y) >= 1.5F) {
                    transform()->velocity.y *= -0.9F;
                    hurt(other.owner);
                    return;
                }
                transform()->position.y -= overlap_y;
                //transform()->velocity.y = transform()->velocity.x;
                if (transform()->velocity.y > 0.0F) {
                    transform()->velocity.x = transform()->velocity.y;
                }
                transform()->moved_velocity.x = 0.0F;
                transform()->moved_velocity.y = 0.0F;
                m_is_on_ground = true;
                m_is_on_slope = true;
            } else if (other_type == entity::Type::slope_L_2x1_0 || other_type == entity::Type::slope_L_2x1_1) {
                if (m_state == entity::State::tossed && std::abs(transform()->velocity.y) >= 1.5F) {
                    transform()->velocity.y *= -0.9F;
                    hurt(other.owner);
                    return;
                }
                transform()->position.y -= overlap_y;
                //transform()->velocity.y = 0.0F;
                transform()->moved_velocity.y = 0.0F;
                //if (transform()->velocity.x < 0.0F) {

                //transform()->velocity.y = transform()->velocity.x / -2.0F;
                //}
                if (transform()->velocity.y > 0.0F) {
                    transform()->velocity.x = -transform()->velocity.y / 2.0F;
                }
                m_is_on_ground = true;
                m_is_on_slope = true;
            } else if (other_type == entity::Type::slope_R_2x1_0 || other_type == entity::Type::slope_R_2x1_1) {
                if (m_state == entity::State::tossed && std::abs(transform()->velocity.y) >= 1.5F) {
                    transform()->velocity.y *= -0.9F;
                    hurt(other.owner);
                    return;
                }
                transform()->position.y -= overlap_y;

                transform()->velocity.y = 0.0F;
                transform()->moved_velocity.y = 0.0F;
                //if (transform()->velocity.x > 0.0F) {
                    //transform()->velocity.y = transform()->velocity.x / 2.0F;
                //}
                if (transform()->velocity.y > 0.0F) {
                    transform()->velocity.x = transform()->velocity.y / 2.0F;
                }
                m_is_on_ground = true;
                m_is_on_slope = true;
            } else if (other_type == entity::Type::spring_U) {
                if (m_parent) return;
                transform()->position.y -= overlap_y;
                transform()->max_velocity.y = 10.0F;
                transform()->velocity.y = -5.0F - transform()->velocity.y / 4.0F;                
                m_is_on_ground = false;
                m_is_on_slope = false;                
            } else if (entity::is_water_line(other_type)) {
                
                transform()->acceleration = { 0.05F, 0.05F };
                transform()->max_velocity = { 1.0F, 1.0F };

                if (transform()->velocity.y > 0.0F) {
                    if (m_water_enter_sound_id != -1 && !sound::Manager::get(m_water_enter_sound_id)->is_playing()) {
                        Console::log("entity::Brick::collide_y water_line\n");
                        sound::Manager::get(m_water_enter_sound_id)->position({ position().x / 160.0F, position().y / 90.0F });
                        sound::Manager::get(m_water_enter_sound_id)->play();
                        spawn_fan(235.0F, 305.0F, 6, particle::Type::drop, transform()->position + Vec2F{ -4.0F, 0.0F }, Vec2F{ transform()->velocity.x * 0.1F, -transform()->velocity.y * 0.1F }, 2.0F);
                    }
                }
            }
        };

        void update() override {
            if (!transform() || !sprite()) return;
            if (m_is_first_update) {
                m_is_first_update = false;
            }
            if (m_parent) {                
                Console::log("entity::Brick::update is near wall: ", m_is_near_wall_L, " ", m_is_near_wall_R, "\n");

                m_is_near_wall_L = false;
                m_is_near_wall_R = false;

                m_is_on_ground = false;

                if (m_parent->is_ducking()) {
                    m_parent->is_carrying(false);
                    transform()->velocity.x = m_parent->transform()->velocity.x;
                    if (m_parent->sprite()->is_leftward) {
                        transform()->velocity.x -= 0.5F;
                        transform()->position.x -= 8.0F;
                    } else {
                        transform()->velocity.x += 0.5F;
                        transform()->position.x += 8.0F;
                    }
                    transform()->velocity.y = -1.0F;
                    m_parent = nullptr;
                    m_is_first_state_update = true;
                    m_next_state = entity::State::idle;
                }
                //Console::log(" ", m_is_on_ground, "\n");
            }

            if (!m_is_on_slope && (transform()->velocity.y < 0.0F || transform()->velocity.y > transform()->acceleration.y)) {
                m_is_on_ground = false;
            }
            //Console::log("state: ", entity::to_string(m_state), " ", m_is_on_ground, "\n");

            if (m_time_left_hurt > 0) --m_time_left_hurt;
            if (m_time_left_interacting > 0) --m_time_left_interacting;

            if (m_next_state != m_state) {
                m_prev_state = m_state;
                m_state = m_next_state;
                m_is_first_state_update = true;
            }

            switch (m_state) {
            case entity::State::dead:   update_dead();   break;
            case entity::State::idle:   update_idle();   break;
            case entity::State::tossed: update_tossed(); break; }

            sprite()->source_rect.x = anim::Manager::get(m_current_anim_id)->source.x;
            sprite()->source_rect.y = anim::Manager::get(m_current_anim_id)->source.y;

            if (m_parent) {
                transform()->velocity = m_parent->transform()->velocity;
                if (m_parent->sprite()->is_leftward) {
                    if (transform()->position.x < m_parent->transform()->position.x - 12.0F) {
                        transform()->position.x = m_parent->transform()->position.x - 12.0F;
                    }

                    if (transform()->position.x > m_parent->transform()->position.x - 12.0F) {
                        cF32 diff_x = (m_parent->transform()->position.x - 12.0F) - transform()->position.x;
                        transform()->velocity.x += diff_x / 4.0F;
                    } else {
                        transform()->position.x = m_parent->transform()->position.x - 12.0F;
                    }
                } else {
                    if (transform()->position.x > m_parent->transform()->position.x + 12.0F) {
                        transform()->position.x = m_parent->transform()->position.x + 12.0F;
                    }

                    if (transform()->position.x < m_parent->transform()->position.x + 12.0F) {
                        cF32 diff_x = (m_parent->transform()->position.x + 12.0F) - transform()->position.x;
                        transform()->velocity.x += diff_x / 4.0F;
                    } else {
                        transform()->position.x = m_parent->transform()->position.x + 12.0F;
                    }
                }

                sprite()->is_leftward = transform()->position.x + 16.0F < m_parent->transform()->position.x + 8.0F;
                //sprite()->is_leftward = m_parent->sprite()->is_leftward;

                transform()->position.y = m_parent->transform()->position.y + 4.0F;
                if (m_parent->is_ducking()) {
                    transform()->position.y += 4.0F;
                }
                return;
            }
            if (!is_dead()) {
                transform()->velocity.y += transform()->acceleration.y;
            }
        }

        void update_dead() {
            if (m_is_first_state_update) {
                m_is_first_state_update = false;
                m_time_left_dead = 2;

                Console::log("entity::Brick::update_dead()\n");
                m_parent = nullptr;
                reset_anim(m_dead_anim_id);

                transform()->acceleration = {};                
                for (auto& i : m_aabb_ids) {
                    if (aabb::Manager::get(i)) {
                        aabb::Manager::get(i)->is_active(false);
                    }
                }
            }
            set_anim(m_dead_anim_id);
                        
            if (m_time_left_dead > 0) {
                --m_time_left_dead;
                if (m_time_left_dead == 0) {
                    sprite()->is_hidden = true;

                    //spawn_fan(0.0F, 360.0F, 8, particle::Type::brick, transform()->position - Vec2F{ 0.0F, 8.0F }, transform()->velocity / 1.0F, 2.0F);
                    //spawn(particle::Type::health, transform()->position + Vec2F{ 0.0F, -8.0F }, {});

                    if (m_dead_sound_id != -1 && sound::Manager::get(m_dead_sound_id)) {
                        sound::Manager::get(m_dead_sound_id)->position({ position().x / 160.0F, position().y / 90.0F });
                        sound::Manager::get(m_dead_sound_id)->play();
                    }
                    spawn(particle::Type::hit, transform()->position, {});

                    spawn_fan(0.0F, 360.0F, 8, particle::Type::brick, transform()->position - Vec2F{ 0.0F, 8.0F }, transform()->velocity, 2.0F);

                    transform()->velocity = {};
                    transform()->moved_velocity = {};
                }
            }
        }
        void update_idle() {
            if (m_is_first_state_update) {
                m_is_first_state_update = false;
                m_time_left_alive = U16_MAX;
                reset_anim(m_idle_anim_id);
                m_parent = nullptr;
            }
            if (m_is_on_ground) {
                transform()->deceleration = { 0.1F, 0.0F };
            }
        }
        void update_tossed() {
            if (m_is_first_state_update) {
                m_is_first_state_update = false;
                m_is_on_ground = false;

                transform()->deceleration = { 0.0F, 0.0F };
            }
            if (m_is_on_ground) {
                transform()->deceleration = { 0.1F, 0.0F };
                m_next_state = entity::State::idle;
            } else {
                //Console::log("entity::Bug toss not on ground\n");
            }
        }
    };
}