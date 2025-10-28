module;
#include <sstream>

export module entity.bug;
import anim;
//import camera;
import console;
import entity;
import collider.aabb;
import sound;
import transform;

export namespace entity {
    class Bug : public Object {
        //U8 m_time_left_turning = 0;
    public:
        Bug() {
            m_state = m_next_state = entity::State::walk;
            m_time_to_interact = 15;
        }       
        bool is_all_valid() override {
            return anim::Manager::get(m_current_anim_id)         &&
                   transform::Manager::is_valid(m_transform_id) &&
                   sprite::Manager::is_valid(m_sprite_id);
        }
        void collide_x(aabb::cInfo our, aabb::cInfo other) override;
        void collide_y(aabb::cInfo our, aabb::cInfo other) override;
        //void hurt(entity::cType type, cVec2F velocity) override {
        void hurt(entity::Object* culprit) override {
            if (!culprit) return;
            //m_state = entity::State::upended;
            m_next_state = entity::State::hurt;
                        
            if (m_time_left_hurt > 0) return;
            m_time_left_hurt = m_time_to_hurt;

            if (culprit->type() == entity::Type::particle_melee) {
                health()->amount -= 16.0f;
            } else {
                health()->amount -= 8.0f;
            }
            sprite()->is_leftward = !sprite()->is_leftward;
        }

        void update() override {
            if (!is_all_valid()) return;

            if (m_is_first_update) {
                m_is_first_update = false;
            }
            
            if (m_parent) {
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
                    m_next_state = entity::State::walk;
                }
                //Console::log(" ", m_is_on_ground, "\n");
            }

            if (!m_is_on_slope && (transform()->velocity.y < 0.0F || transform()->velocity.y > transform()->acceleration.y)) {
                m_is_on_ground = false;
            }

            if (m_state == entity::State::upended) {
                //Console::log("entity::Bug::update is_near_wall: ", m_is_near_wall_L, " ", m_is_near_wall_R, "\n");
            }
            if (transform()->velocity.x < 0.0F) {
                m_is_near_wall_R = false;
            }
            if (transform()->velocity.x > 0.0F) {
                m_is_near_wall_L = false;
            }
            //Console::log("state: ", entity::to_string(m_state), " ", m_is_on_ground, "\n");

            if (m_time_left_hurt        > 0) --m_time_left_hurt;
            //if (m_time_left_turning     > 0) --m_time_left_turning;
            if (m_time_left_interacting > 0) --m_time_left_interacting;

            if (health()->amount <= 0.0f) {
                m_next_state = entity::State::dead;
            }
            health()->layer = m_start_layer;

            if (m_next_state != m_state) {
                m_prev_state = m_state;
                m_state = m_next_state;
                m_is_first_state_update = true;
            }

            switch (m_state) {
            case entity::State::bounce:  update_bounce();  break;
            case entity::State::dead:    update_dead();    break;
            case entity::State::hurt:    update_hurt();    break;
            case entity::State::swim:    update_swim();    break;
            case entity::State::tossed:  update_tossed();  break;
            case entity::State::upended: update_upended(); break;
            case entity::State::walk:    update_walk();    break; }

            sprite()->source_rect.x = anim::Manager::get(m_current_anim_id)->source.x;
            sprite()->source_rect.y = anim::Manager::get(m_current_anim_id)->source.y;

            if (m_parent) {
                //transform()->position = m_parent->transform()->position;
                //sprite()->is_leftward = m_parent->sprite()->is_leftward;

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

                transform()->position.y = m_parent->transform()->position.y + 6.0F;
                if (m_parent->is_ducking()) {
                    transform()->position.y += 4.0F;
                }
                return;
            }
            if (!is_dead()) {
                transform()->velocity.y += transform()->acceleration.y;                
            }
        }
        void update_bounce() {
            if (m_is_first_state_update) {
                m_is_first_state_update = false;
                if (sound::Manager::get(m_bounce_sound_id)) {
                    sound::Manager::get(m_bounce_sound_id)->position({ position().x / 160.0F, position().y / 90.0F });
                    sound::Manager::get(m_bounce_sound_id)->play();
                }
                reset_anim(m_bounce_anim_id);
            }
            //int num_frames = anim()->texture_size.x / anim()->source.w;
            //Console::log("num_frames: ", anim()->current_frame(), "\n");
            if (anim()->current_frame() + 1 == anim()->num_frames()) {
                m_next_state = entity::State::upended;
            }
            //Console::log(anim::Manager::get(m_bounce_anim_id)->source.x, " ", anim::Manager::get(m_bounce_anim_id)->source.w, "\n");
        }
        void update_dead() {
            if (m_is_first_state_update) {
                m_is_first_state_update = false;
                m_time_left_dead = U16_MAX;           
                reset_anim(m_dead_anim_id);
                //Console::log("prev state: ", entity::to_string(m_prev_state), "\n");
                if (m_prev_state == entity::State::upended || m_prev_state == entity::State::bounce) {
                    sprite()->is_upended = true;
                }
                for (auto& i : m_aabb_ids) {
                    if (aabb::Manager::get(i)) {
                        aabb::Manager::get(i)->is_active(false);
                    }
                }
                spawn_fan(0.0F, 360.0F, 8, particle::Type::drop_blood, transform()->position - Vec2F{ 16.0F, 8.0F }, transform()->velocity, 3.0F);
                spawn(particle::Type::health, transform()->position + Vec2F{ 0.0F, 0.0F }, {});

                if (m_dead_sound_id != -1 && sound::Manager::get(m_dead_sound_id)) {
                    sound::Manager::get(m_dead_sound_id)->position({ position().x / 160.0F, position().y / 90.0F });
                    sound::Manager::get(m_dead_sound_id)->play();
                }
                if (m_parent) {
                    m_parent->is_carrying(false);
                    m_parent = nullptr;
                }
            }
            transform()->velocity = {};
            transform()->moved_velocity = {};
        }
        void update_walk() {
            if (m_is_first_state_update) {
                m_is_first_state_update = false;
            }
            transform::Manager::get(m_transform_id)->deceleration = { 0.0F, 0.0F };

            if (m_is_on_ground) {
                //Console::log("entity::Bug walk on ground\n");
                transform()->moved_velocity = { 0.0F, 0.0F };
                transform()->velocity.x = sprite()->is_leftward ? -0.5F : 0.5F;
            } else {
                //Console::log("entity::Bug walk not on ground\n");
            }

            m_current_anim_id = m_idle_anim_id;            
        }
        void update_hurt() {
            if (m_is_first_state_update) {
                m_is_first_state_update = false;                
                reset_anim(m_hurt_anim_id);
            }
            transform()->deceleration = { 0.0F, 0.0F };
            set_anim(m_hurt_anim_id);
            if (anim()->is_last_frame()) {
                m_next_state = entity::State::upended;
            }            
        }
        void update_swim() {
            if (m_is_first_state_update) {
                m_is_first_state_update = false;
                m_sensed_objects.clear();
                m_is_on_ground = false;
                //reset_anim(m_swim_anim_id);
            }            
            set_anim(m_hurt_anim_id);
            //transform()->deceleration = { 0.0F, 0.0F };            

            transform()->velocity.x *= 0.9F;
            //transform()->velocity.y *= 0.9F;
            //transform()->moved_velocity = 0.0F;

            if (position_on_level().y > m_water_line_y) {
                transform()->velocity.y -= 0.1F;
            } else {
                transform()->velocity.y += 0.1F;
            }

        }
        void update_tossed() {
            if (m_is_first_state_update) {
                m_is_first_state_update = false;
                m_is_on_ground = false;
            }
            if (m_is_on_ground) {
                //Console::log("entity::Bug toss on ground\n");
                //m_next_state = entity::State::walk;
                //m_next_state = entity::State::upended;
                m_next_state = m_prev_state;
            } else {
                //Console::log("entity::Bug toss not on ground\n");
            }
        }
        void update_upended() {
            if (m_is_first_state_update) {
                m_is_first_state_update = false;
            }
            set_anim(m_upended_anim_id);

            if (m_is_on_ground) {
                transform::Manager::get(m_transform_id)->deceleration = { 0.2F, 0.0F };
            } else {
                transform::Manager::get(m_transform_id)->deceleration = { 0.0F, 0.0F };
            }

            if (m_time_left_in_state > 0) {
                --m_time_left_in_state;
                if (m_time_left_in_state == 0) {
                    m_next_state = entity::State::walk;
                }
            }
        }        
    };
}