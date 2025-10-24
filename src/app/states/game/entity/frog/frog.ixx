module;
#include <cmath>
#include <sstream>

export module entity.frog;
import anim;
import camera;
import console;
import entity;
import collider.aabb;
import sound;
import transform;
import line;
import particle;
import random;


export namespace entity {
    class Frog : public Object {
        I32 m_tounge_line_id = -1;

        Vec2F m_tounge_start,
              m_tounge_end,
              m_sensed_position;

        State m_sensed_state;
        
    public:
        //void create_tile_line() {

        //    Vec2F position_on_level = Camera::position + transform()->position;

        //    Console::log("position_on_level: ", position_on_level.x, " ", position_on_level.y, "\n");
        //    //Console::log("Camera::position: ", Camera::position.x, " ", Camera::position.y, "\n");


        //    Vec2F real_position = position_on_level;

        //    //Console::log("real: ", real_position.x, " ", real_position.y, "\n");

        //    Vec2F distance_to_grid = { std::fmodf(real_position.x, 16.0F), std::fmodf(real_position.y, 16.0F) };


        //    Vec2F position_on_grid = { std::fmodf(transform()->position.x, 16.0F), std::fmodf(transform()->position.y, 16.0F) };

        //    m_particles_to_spawn.emplace_back({ .type = ParticleType::tile_line,
        //                                     .position = position_on_level - distance_to_grid + Camera::position,
        //                                     .velocity = Vec2F{ 0.0F, 0.0F } });
        //}


        Frog() {            
            m_state = m_next_state = entity::State::idle;
            m_time_to_spawn_sense = 5;
            m_tounge_line_id = line::Manager::make(m_tounge_start, m_tounge_end);
            line::Manager::get(m_tounge_line_id)->is_hidden = true;
        }
        ~Frog() {            
            line::Manager::erase(m_tounge_line_id);
        }
        bool is_all_valid() override {
            for (auto& i : m_aabb_ids) {
                if (!aabb::Manager::get(i)) return false;
            }
            return anim::Manager::get(m_current_anim_id) &&
                   transform::Manager::is_valid(m_transform_id) &&
                   sprite::Manager::is_valid(m_sprite_id);
        }
        void collide_x(aabb::cInfo our, aabb::cInfo other) override;
        void collide_y(aabb::cInfo our, aabb::cInfo other) override;
        void hurt(entity::Object* culprit) override {
            if (m_time_left_hurt > 0) return;
            m_time_left_hurt = m_time_to_hurt;

            if (!culprit || !culprit->transform()) return;

            if (culprit->type() == entity::Type::brick || culprit->type() == entity::Type::bug) {
                m_sensed_objects.clear();
                m_sensed_objects.emplace_back(culprit);
                
                //transform()->velocity = culprit->transform()->velocity * 0.25F;
                cF32 amount = std::abs(culprit->transform()->velocity.x) * 4.0F;
                health()->amount -= amount;
                //health()->amount -= 32.0F;
                m_next_state = entity::State::hurt;
            } else if (culprit->type() == entity::Type::particle_brick) {
                m_sensed_objects.clear();
                m_sensed_objects.emplace_back(culprit);

                cF32 amount = std::sqrtf(std::abs(culprit->velocity().x * culprit->velocity().y));
                //Console::log("amount: ", amount, "\n");
                
                health()->amount -= amount;
                m_next_state = entity::State::hurt;                
            } else  if (culprit->type() == entity::Type::particle_down_thrust) {
                transform()->velocity.x = culprit->transform()->velocity.x * 0.7F;
                if (m_is_on_ground) {
                    transform()->velocity.y = -2.0f;
                }
                m_next_state = entity::State::stunned;
            } else if (culprit->type() == entity::Type::particle_melee) {
                m_sensed_objects.clear();
                m_sensed_objects.emplace_back(culprit);

                transform()->velocity = culprit->transform()->velocity * 0.25F;
                health()->amount -= 8.0f;
                m_next_state = entity::State::hurt;
            } else if (culprit->type() == entity::Type::player) {
                health()->amount -= 8.0f;
                m_next_state = entity::State::hurt;
            }
        }
        void update() override {
            if (!is_all_valid()) return;

            if (m_time_left_blocked > 0) --m_time_left_blocked;
            if (m_time_left_hurt    > 0) --m_time_left_hurt;

            /*if (transform()->velocity.y < 0.0F && !m_is_on_slope) {
                m_is_on_ground = false;
            }*/

            transform()->deceleration.x = m_is_on_ground ? 0.2F : 0.0F;

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
            case entity::State::blocked: update_blocked(); break;
            case entity::State::dead:    update_dead();    break;
            case entity::State::hurt:    update_hurt();    break;
            case entity::State::idle:    update_idle();    break;
            case entity::State::jump:    update_jump();    break;
            case entity::State::melee:   update_melee();   break;
            case entity::State::stunned: update_stunned(); break;
            case entity::State::swim:    update_swim();    break;
            default:                                       break; }

            sprite()->source_rect.x = anim::Manager::get(m_current_anim_id)->source.x;
            sprite()->source_rect.y = anim::Manager::get(m_current_anim_id)->source.y;            
        }
        virtual void draw(std::unique_ptr<Window>& window) {
            window->draw(sprite::Manager::get(m_sprite_id));
            window->draw(line::Manager::get(m_tounge_line_id));
        }
        void update_melee() {
            if (!m_is_on_ground) return;

            set_anim(m_melee_anim_id);
            
            sprite()->is_leftward = m_sensed_position.x < position().x;

            m_tounge_start = transform()->position + Vec2F{ 8.0F, 6.0F };
            m_tounge_end = m_sensed_position + Vec2F{ 8.0F, 8.0F };
            //Console::log("tounge start: ", m_tounge_start.x, " ", m_tounge_start.y, "\n");
            //Console::log("tounge end: ", m_tounge_end.x, " ", m_tounge_end.y, "\n");

            line::Manager::get(m_tounge_line_id)->size = 2;
            line::Manager::get(m_tounge_line_id)->max_length = 48.0F - anim()->source.x / 16.0F;
                        
            //Console::log("max_length: ", line::Manager::get(m_tounge_line_id)->max_length, "\n");

            if (line::Manager::get(m_tounge_line_id)->max_length < 16.0F) {
                line::Manager::get(m_tounge_line_id)->max_length = 16.0F;
            }

            //line::Manager::get(m_tounge_line_id)->max_length = 64.0F;

            line::Manager::get(m_tounge_line_id)->is_hidden = false;
            line::Manager::get(m_tounge_line_id)->layer = sprite()->layer + 1;
            line::Manager::get(m_tounge_line_id)->color = { 191, 0, 0 };
            line::Manager::get(m_tounge_line_id)->set(m_tounge_start, m_tounge_end);
            line::Manager::get(m_tounge_line_id)->update();

            if (m_is_first_state_update) {
                m_is_first_state_update = false;
                reset_anim(m_melee_anim_id);

                sound::Manager::get(m_melee_sound_id)->position({ position().x / 160.0F, position().y / 90.0F });
                sound::Manager::get(m_melee_sound_id)->play();

                cVec2F tounge_vector = (m_tounge_end - m_tounge_start);
                cF32 tounge_length = line::length(tounge_vector);

                //Console::log("length: ", tounge_length, "\n");

                Vec2F melee_velocity = (tounge_vector / tounge_length) * 6.0f;

                particle::Manager::spawn({ .parent   = this,
                                           .type     = particle::Type::melee,
                                           .position = { position().x + 4.0f, position().y + 4.0f },
                                           .velocity = melee_velocity });
            }
            

            if (anim()->is_last_frame()) {
                //Console::log("is last frame: ", anim()->is_last_frame(), "\n");                
                //line::Manager::get(m_tounge_line_id)->is_hidden = true;
                m_next_state = entity::State::jump;
            }

        }
        void update_idle() {
            if (m_is_first_state_update) {
                m_is_first_state_update = false;
                transform()->max_velocity = transform()->start_max_velocity;
                m_time_left_to_spawn_sense = m_time_to_spawn_sense;
                reset_anim(m_idle_anim_id);
                aabb_object(aabb::Name::body)->is_active(true);
                aabb_object(aabb::Name::body_swim)->is_active(false);

                m_sensed_objects.clear();
            }

            if (m_time_left_to_spawn_sense > 0) {
                --m_time_left_to_spawn_sense;
                if (m_time_left_to_spawn_sense == 0) {
                    //Console::log("spawn sense\n");

                    m_time_left_to_spawn_sense = m_time_to_spawn_sense;

                    spawn_fan(0.0F, 360.0F, 16, particle::Type::sense, transform()->position + Vec2F{ 4.0F, 0.0F }, {}, 3.0F);
                }
            }

            set_anim(m_idle_anim_id);

            line::Manager::get(m_tounge_line_id)->is_hidden = true;

            transform()->deceleration.x = m_is_on_ground ? 0.2F : 0.0F;

            transform()->velocity.y += transform()->acceleration.y;
                        
            if (!m_sensed_objects.empty() && m_time_left_to_react == 0) {
                for (auto& i : m_sensed_objects) {
                    if (m_is_on_ground &&
                        i->type() == entity::Type::player
                        &&
                        !(i->state() == entity::State::swim && i->num_jumps() == 0)
                        ) {
                        if (i->is_ducking() && (i->velocity().x < -1.5F || i->velocity().x > 1.5F)) {
                            //Console::log("velocity x: ", i->velocity().x, "\n");
                            //Console::log("player sliding ground\n");
                            m_time_left_to_react = 1;
                            m_sensed_state = entity::State::jump;

                            m_sensed_position = i->position() + Vec2F{ 0.0F, -32.0F };
                        } else {
                            m_time_left_to_react = 5 + random::Generator::number(0, 2);
                            m_sensed_state = entity::State::melee;

                            m_sensed_position = i->position();
                        }
                        m_sensed_objects.clear();
                        break;
                    }
                }
            }
            if (m_time_left_to_react > 0) {
                --m_time_left_to_react;
                if (m_time_left_to_react == 0) {
                    m_next_state = m_sensed_state;
                }
                return;
            }            
        }

        void update_blocked() {
            if (m_is_first_state_update) {
                m_is_first_state_update = false;
                m_sensed_objects.clear();
                reset_anim(m_hurt_anim_id);                
                aabb_object(aabb::Name::body)->is_active(true);
                aabb_object(aabb::Name::body_swim)->is_active(false);
                m_time_in_state = 0;
                transform()->velocity.y -= 1.5f;
                transform()->velocity.x = sprite()->is_leftward ? 1.0f : -1.0f;
                m_is_on_ground = false;

                if (m_block_sound_id != -1 && sound::Manager::get(m_block_sound_id)) {
                    sound::Manager::get(m_block_sound_id)->position({ (position().x + 8.0f) / 160.0F, (position().y + 8.0f) / 90.0F });
                    sound::Manager::get(m_block_sound_id)->play();
                }
            }
            //Console::log("entity::Frog::update_blocked\n");

            set_anim(m_hurt_anim_id);
            m_tounge_start = transform()->position + Vec2F{ 8.0F, 6.0F };
            m_tounge_end = m_sensed_position + Vec2F{ 8.0F, 8.0F };

            line::Manager::get(m_tounge_line_id)->max_length = 32.0F - m_time_in_state * 4.0f;
            if (line::Manager::get(m_tounge_line_id)->max_length < 1.0f) {
                line::Manager::get(m_tounge_line_id)->max_length = 1.0f;
            }

            line::Manager::get(m_tounge_line_id)->set(m_tounge_start, m_tounge_end);
            line::Manager::get(m_tounge_line_id)->update();
                        
            transform()->velocity.y += transform()->acceleration.y;
            ++m_time_in_state;
            if (m_is_on_ground && m_time_in_state > 100) {
                m_time_in_state = 0;
                m_next_state = entity::State::idle;
            }
        }
        void update_dead() {
            if (m_is_first_state_update) {
                m_is_first_state_update = false;
                m_time_left_dead = U16_MAX;
                reset_anim(m_dead_anim_id);
                line::Manager::get(m_tounge_line_id)->is_hidden = true;
                for (auto& i : m_aabb_ids) {
                    if (aabb::Manager::get(i)) {
                        aabb::Manager::get(i)->is_active(false);
                    }
                }
                spawn_fan(0.0F, 360.0F, 8, particle::Type::drop_blood, transform()->position + Vec2F{4.0F, 0.0F}, transform()->velocity, 3.0F);
                spawn(particle::Type::health, transform()->position + Vec2F{4.0F, 0.0F}, {});

                if (m_dead_sound_id != -1 && sound::Manager::get(m_dead_sound_id)) {
                    sound::Manager::get(m_dead_sound_id)->position({ position().x / 160.0F, position().y / 90.0F });
                    sound::Manager::get(m_dead_sound_id)->play();
                }
            }
            transform()->velocity.x *= 0.9F;
            transform()->velocity.y *= 0.9F;
            transform()->moved_velocity = {};
        }
        void update_hurt() {
            if (m_is_first_state_update) {
                m_is_first_state_update = false;

                reset_anim(m_hurt_anim_id);
                aabb_object(aabb::Name::body)->is_active(true);
                aabb_object(aabb::Name::body_swim)->is_active(false);

                m_sensed_state = entity::State::idle;
                m_time_left_to_react = 0;
            }

            set_anim(m_hurt_anim_id);
            line::Manager::get(m_tounge_line_id)->is_hidden = true;

            transform()->velocity.y += transform()->acceleration.y;
            if (/*anim()->is_last_frame() &&*/ m_is_on_ground) {
                //Console::log("is last frame: ", anim()->is_last_frame(), "\n");                

                if (!m_sensed_objects.empty() && m_time_left_to_react == 0) {
                    for (auto& i : m_sensed_objects) {
                        if (m_is_on_ground && (i->type() == entity::Type::brick ||
                                               i->type() == entity::Type::particle_brick ||
                                               i->type() == entity::Type::particle_melee ||
                                               i->type() == entity::Type::bug)
                            ) {
                            m_time_left_to_react = 10;
                            Console::log("entity::Frog::update_hurt() sensed: ", entity::to_string(i->type()), "\n");
                            m_sensed_state = entity::State::melee;
                            m_sensed_position = i->position();
                            m_sensed_objects.clear();
                            break;
                        }
                    }
                }
                
            }
            //Console::log("time left to react: ", (int)m_time_left_to_react, "\n");
            if (m_time_left_to_react > 0) {                
                --m_time_left_to_react;
                if (m_time_left_to_react == 0) {
                    m_next_state = m_sensed_state;
                }
            }
        }

        void update_jump() {
            if (m_is_first_state_update) {
                m_is_first_state_update = false;
                
                m_sensed_objects.clear();

                cF32 diff_x = m_sensed_position.x - position().x;
                cF32 diff_y = m_sensed_position.y - position().y;

                //Console::log("diff_y: ", diff_y, "\n");
                transform()->max_velocity = transform()->start_max_velocity;

                transform()->velocity.x = diff_x / 32.0F;
                transform()->velocity.y = diff_y / 10.0F;

                transform()->position.y -= 2.0F;

                if (transform()->velocity.y < -4.0F) {
                    transform()->velocity.y = -4.0F;
                }
                m_is_on_ground = false;

                reset_anim(m_jump_anim_id);
            }
            
            line::Manager::get(m_tounge_line_id)->is_hidden = true;

            set_anim(m_jump_anim_id);

            
            transform()->velocity.y += transform()->acceleration.y;

            if (m_is_on_ground && transform()->velocity.y > 0.0F) {                
                m_next_state = entity::State::idle;
                reset_anim(m_idle_anim_id);
            }
        }
        void update_stunned() {
            if (m_is_first_state_update) {
                m_is_first_state_update = false;
                m_sensed_objects.clear();
                reset_anim(m_stunned_anim_id);
                aabb_object(aabb::Name::body)->is_active(true);
                aabb_object(aabb::Name::body_swim)->is_active(false);
                m_time_in_state = 0;
            }
            set_anim(m_stunned_anim_id);

            line::Manager::get(m_tounge_line_id)->is_hidden = true;
            

            transform()->velocity.y += transform()->acceleration.y;

            ++m_time_in_state;
            if (m_is_on_ground && m_time_in_state > 100) {
                m_time_in_state = 0;
                m_next_state = entity::State::idle;
            }
        }
        void update_swim() {
            if (m_is_first_state_update) {
                m_is_first_state_update = false;                
                m_sensed_objects.clear();
                m_is_on_ground = false;
                //reset_anim(m_swim_anim_id);

                aabb::Object* body_aabb_object = aabb_object(aabb::Name::body);
                if (body_aabb_object) {
                    body_aabb_object->is_active(false);
                }
                aabb::Object* body_swim_aabb_object = aabb_object(aabb::Name::body_swim);
                if (body_swim_aabb_object) {
                    body_swim_aabb_object->is_active(true);
                }
            }
            line::Manager::get(m_tounge_line_id)->is_hidden = true;
            set_anim(m_hurt_anim_id);
            
            transform()->deceleration = { 0.0F, 0.0F };

            if (position_on_level().y + 16 < m_water_line_y) {
                Console::log("frog swim water line y: ", m_water_line_y, "\n");
                //transform()->max_velocity = transform()->start_max_velocity;
                transform()->max_velocity = { 0.0F, 4.0F };
            } else {
                transform()->max_velocity = { 0.0F, 2.0F };
                transform()->velocity.x = sprite()->is_leftward ? -0.8F : 0.8F;
            }

            transform()->velocity.y *= 0.9F;
            //transform()->moved_velocity = 0.0F;

            if (position_on_level().y > m_water_line_y) {
                transform()->velocity.y -= 0.3F;
            } else {
                transform()->velocity.y += 0.3F;
            }

        }
    };
}