export module entity.frog;
import anim;
//import camera;
import console;
import entity;
import collider.aabb;
import sound;
import transform;
import line;

export namespace entity {
    class Frog : public Object {
        i32 m_tounge_line_id = -1;

        Vec2f m_tounge_start, m_tounge_end;
    public:
        Frog() {
            /*m_transform_id = transform::Set::make();
            transform::Set::at(m_transform_id)->acceleration = { 0.2f, 0.2f };
            transform::Set::at(m_transform_id)->deceleration = { 0.0f, 0.0f };
            Camera::add_transform_id(m_transform_id);*/

            m_state = entity::State::idle;

            m_time_to_spawn_sense = 5;

            m_tounge_line_id = line::Set::make(m_tounge_start, m_tounge_end);
            line::Set::at(m_tounge_line_id)->is_hidden = true;
            //line::Set::at(m_tounge_line_id)->transform_id = m_transform_id;

            //Camera::add_transform_id(line::Set::at(m_tounge_line_id)->transform_id);
        }
        ~Frog() {
            //Camera::remove_transform_id(m_transform_id);
            //transform::Set::erase(m_transform_id);

            //sprite::Set::erase(m_sprite_id);

            line::Set::erase(m_tounge_line_id);
        }
        bool is_all_valid() override {
            return anim::Set::at(m_current_anim_id) &&
                   transform::Set::is_valid(m_transform_id) &&
                   sprite::Set::is_valid(m_sprite_id);
        }
        void collide_x(aabb::Infoc our, aabb::Infoc other) override;
        void collide_y(aabb::Infoc our, aabb::Infoc other) override;
        
        void update() override {
            if (!is_all_valid()) return;

            if (m_time_left_hurt > 0) {
                if (m_time_left_hurt == m_time_to_hurt) {
                    
                }
                --m_time_left_hurt;
            }

            /*if (transform()->velocity.y < 0.0f && !m_is_on_slope) {
                m_is_on_ground = false;
            }*/
            if (m_is_on_ground) {
                //Console::log("frog on ground\n");
                //Console::log("moved velocity: ", transform()->moved_velocity.x, "\n");
                transform()->deceleration.x = 0.2f;
            } else {
                transform()->deceleration.x = 0.0f;
            }

            transform()->velocity.y += transform()->acceleration.y;

            switch (m_state) {
            case entity::State::hurt:    update_hurt();    break;
            case entity::State::idle:    update_idle();    break;
            case entity::State::jump:    update_jump();    break;
            case entity::State::melee:   update_melee();   break; }

            sprite()->source_rect.x = anim::Set::at(m_current_anim_id)->source.x;
            sprite()->source_rect.y = anim::Set::at(m_current_anim_id)->source.y;
            
        }
        void update_melee() {
            if (m_is_first_state_update) {
                m_is_first_state_update = false;
                reset_anim(m_melee_anim_id);
                sound::Set::at(m_melee_sound_id)->position({ position().x / 160.0f, position().y / 90.0f });
                sound::Set::at(m_melee_sound_id)->play();
            }

            set_anim(m_melee_anim_id);

            if (!m_sensed_objects.empty()) {
                for (auto& i : m_sensed_objects) {
                    if (i->type() == entity::Type::player && m_is_on_ground) {
                        sprite()->is_leftward = i->position().x < position().x;

                        m_tounge_start = transform()->position + Vec2f{ 8.0f, 6.0f };
                        m_tounge_end = i->position() + Vec2f{ 8.0f, 8.0f };

                        line::Set::at(m_tounge_line_id)->size = 2;
                        line::Set::at(m_tounge_line_id)->max_length = 48.0f - anim()->source.x / 8.0f;
                        line::Set::at(m_tounge_line_id)->is_hidden = false;
                        line::Set::at(m_tounge_line_id)->layer = NUM_VISIBLE_LAYERS - 1;
                        line::Set::at(m_tounge_line_id)->color = { 191, 0, 0 };
                        line::Set::at(m_tounge_line_id)->set(m_tounge_start, m_tounge_end);
                        line::Set::at(m_tounge_line_id)->update();
                    }
                }
            }




            if (anim()->is_last_frame()) {
                //Console::log("is last frame: ", anim()->is_last_frame(), "\n");                
                line::Set::at(m_tounge_line_id)->is_hidden = true;
                next_state(entity::State::jump);
            }

        }
        void update_idle() {
            if (m_is_first_state_update) {
                m_is_first_state_update = false;
                m_time_left_to_spawn_sense = m_time_to_spawn_sense;
                reset_anim(m_idle_anim_id);
            }

            if (m_time_left_to_spawn_sense > 0) {
                --m_time_left_to_spawn_sense;
                if (m_time_left_to_spawn_sense == 0) {
                    //Console::log("spawn sense\n");

                    m_time_left_to_spawn_sense = m_time_to_spawn_sense;

                    m_particles_to_spawn.push_back({ .type     = ParticleType::sense_frog,
                                                     .position = transform()->position,
                                                     .velocity = Vec2f{ -3.0f, 0.0f } });

                    m_particles_to_spawn.push_back({ .type = ParticleType::sense_frog,
                                                     .position = { transform()->position.x + 0.0f, transform()->position.y },
                                                     .velocity = Vec2f{  3.0f, 0.0f } });
                }
            }

            set_anim(m_idle_anim_id);


            //Console::log("idle\n");

            //Console::log("sensed size: ", m_sensed_objects.size(), "\n");
            line::Set::at(m_tounge_line_id)->is_hidden = true;

            if (!m_sensed_objects.empty()) {
                for (auto& i : m_sensed_objects) {
                    if (i->type() == entity::Type::player && m_is_on_ground) {                        
                        next_state(entity::State::melee);
                        break;
                    }
                }
            }
            
        }
        void update_hurt() {
            if (m_is_first_state_update) {
                m_is_first_state_update = false;
                reset_anim(m_hurt_anim_id);
            }

            set_anim(m_hurt_anim_id);
            line::Set::at(m_tounge_line_id)->is_hidden = true;

            if (anim()->is_last_frame()) {
                //Console::log("is last frame: ", anim()->is_last_frame(), "\n");
                next_state(entity::State::idle);
            }
        }

        void update_jump() {
            if (m_is_first_state_update) {
                m_is_first_state_update = false;
                
                f32c diff_x = m_tounge_end.x - position().x;
                f32c diff_y = m_tounge_end.y - position().y < 0.0f ? m_tounge_end.y - position().y : 0.0f;

                //Console::log("diff_x: ", diff_x, "\n");
                transform()->velocity.x = diff_x / 32.0f;
                transform()->velocity.y = diff_y / 32.0f;

                transform()->position.y -= transform()->acceleration.y;
                transform()->velocity.y -= 2.0f;
                m_is_on_ground = false;

                reset_anim(m_jump_anim_id);
            }
            
            line::Set::at(m_tounge_line_id)->is_hidden = true;

            set_anim(m_jump_anim_id);

            if (m_is_on_ground && transform()->velocity.y > 0.0f) {
                m_sensed_objects.clear();
                next_state(entity::State::idle);
                reset_anim(m_idle_anim_id);
            }
        }
    };
}