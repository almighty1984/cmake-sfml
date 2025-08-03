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
        u8 m_time_left_turning = 0;
    public:
        Bug() {
            /*m_transform_id = transform::Set::make();
            transform::Set::at(m_transform_id)->acceleration = { 0.2f, 0.2f };
            transform::Set::at(m_transform_id)->deceleration = { 0.0f, 0.0f };
            transform::Set::at(m_transform_id)->velocity_limit = { 2.0f, 4.0f };
            Camera::add_transform_id(m_transform_id);*/

            //transform::Set::at(m_transform_id)->position = m_start_offset;
            m_state = entity::State::walk;
            //load_config("res/entity/bug.cfg");

        }
        ~Bug() {
            //Camera::remove_transform_id(m_transform_id);
            //transform::Set::erase(m_transform_id);
            //sprite::Set::erase(m_sprite_id);
        }
        bool is_all_valid() override {
            return anim::Set::at(m_current_anim_id)         &&
                   transform::Set::is_valid(m_transform_id) &&
                   sprite::Set::is_valid(m_sprite_id);
        }
        void collide_x(aabb::Infoc our, aabb::Infoc other) override;
        void collide_y(aabb::Infoc our, aabb::Infoc other) override;

        void update() override {            
            if (!is_all_valid()) return;

            if (m_is_first_update) {
                m_is_first_update = false;
                //transform()->position = m_start_offset;

                //Console::log("bug start_offset: ", m_start_offset.x, " ", m_start_offset.y, "\n");

                //Console::log("bug transform_id: ", m_transform_id, "\n");
            }

            //Console::log("bug position: ", transform()->position.x, " ", transform()->position.y, "\n");

            if (m_parent) {
                m_is_on_ground = false;

                if (m_parent->is_ducking()) {                    
                    m_parent->is_carrying(false);
                    transform()->velocity.x = m_parent->transform()->velocity.x;
                    if (m_parent->sprite()->is_leftward) {
                        transform()->velocity.x -= 0.5f;
                        transform()->position.x -= 8.0f;
                    } else {
                        transform()->velocity.x += 0.5f;
                        transform()->position.x += 8.0f;
                    }                    
                    transform()->velocity.y = -1.0f;
                    m_parent = nullptr;                    
                }
                //Console::log(" ", m_is_on_ground, "\n");
            }

            if (!m_is_on_slope && (transform()->velocity.y < 0.0f || transform()->velocity.y > transform()->acceleration.y)) {
                m_is_on_ground = false;
            }
            //Console::log("state: ", entity::state_to_string(m_state), " ", m_is_on_ground, "\n");

            if (m_time_left_hurt > 0) {
                if (m_time_left_hurt == m_time_to_hurt) {
                    //m_is_to_spawn_particle = true;
                }
                /*if (m_time_left_hurt == m_time_to_hurt) {
                    if (sound::Set::at(m_hurt_sound_id)) {
                        sound::Set::at(m_hurt_sound_id)->position({ position().x / 160.0f, position().y / 90.0f });
                        sound::Set::at(m_hurt_sound_id)->play();
                    }
                }*/
                --m_time_left_hurt;
                //m_state = m_time_left_hurt == 0 ? entity::State::upended : entity::State::hurt;                
            }
            if (m_time_left_turning > 0) {
                --m_time_left_turning;
            }

            if (m_time_left_interacting > 0) {
                --m_time_left_interacting;
            }

            switch (m_state) {
            case entity::State::bounce:  update_bounce();  break;
            case entity::State::hurt:    update_hurt();    break;
            case entity::State::tossed:  update_tossed();  break;
            case entity::State::upended: update_upended(); break;
            case entity::State::walk:    update_walk();    break; }

            sprite()->source_rect.x = anim::Set::at(m_current_anim_id)->source.x;
            sprite()->source_rect.y = anim::Set::at(m_current_anim_id)->source.y;

            if (m_parent) {
                //transform()->position = m_parent->transform()->position;
                //sprite()->is_leftward = m_parent->sprite()->is_leftward;

                

                transform()->velocity = m_parent->transform()->velocity;
                if (m_parent->sprite()->is_leftward) {
                    if (transform()->position.x < m_parent->transform()->position.x - 12.0f) {
                        transform()->position.x = m_parent->transform()->position.x - 12.0f;
                    }

                    if (transform()->position.x > m_parent->transform()->position.x - 12.0f) {
                        f32c diff_x = (m_parent->transform()->position.x - 12.0f) - transform()->position.x;
                        transform()->velocity.x += diff_x / 4.0f;
                    } else {                        
                        transform()->position.x = m_parent->transform()->position.x - 12.0f;
                    }
                } else {
                    if (transform()->position.x > m_parent->transform()->position.x + 12.0f) {
                        transform()->position.x = m_parent->transform()->position.x + 12.0f;
                    }

                    if (transform()->position.x < m_parent->transform()->position.x + 12.0f) {
                        f32c diff_x = (m_parent->transform()->position.x + 12.0f) - transform()->position.x;
                        transform()->velocity.x += diff_x / 4.0f;
                    } else {                        
                        transform()->position.x = m_parent->transform()->position.x + 12.0f;
                    }
                }

                sprite()->is_leftward = transform()->position.x + 16.0f < m_parent->transform()->position.x + 8.0f;
                //sprite()->is_leftward = m_parent->sprite()->is_leftward;

                transform()->position.y = m_parent->transform()->position.y + 6.0f;
                if (m_parent->is_ducking()) {
                    transform()->position.y += 4.0f;
                }
                return;
            }
            transform()->velocity.y += transform()->acceleration.y;
            transform()->moved_velocity = { 0.0f, 0.0f };
        }
        void update_walk() {
            if (m_is_first_state_update) {
                m_is_first_state_update = false;
            }

            transform::Set::at(m_transform_id)->deceleration = { 0.0f, 0.0f };

            if (m_is_on_ground) {
                transform()->velocity.x = sprite()->is_leftward ? -0.5f : 0.5f;
            }

            m_current_anim_id = m_idle_anim_id;
        }
        void update_hurt() {
            if (m_is_first_state_update) {
                m_is_first_state_update = false;
            }

            transform::Set::at(m_transform_id)->deceleration = { 0.2f, 0.0f };

            set_anim(m_hurt_anim_id);

            if (anim()->is_last_frame()) {
                next_state(entity::State::upended);
            }
        }
        void update_tossed() {
            if (m_is_first_state_update) {
                m_is_first_state_update = false;
            }

            if (m_is_on_ground) {
                next_state(entity::State::walk);
            }
        }
        void update_upended() {
            if (m_is_first_state_update) {
                m_is_first_state_update = false;
            }

            set_anim(m_upended_anim_id);

            if (m_is_on_ground) {
                transform::Set::at(m_transform_id)->deceleration = { 0.2f, 0.0f };
            } else {
                transform::Set::at(m_transform_id)->deceleration = { 0.0f, 0.0f };
            }

            if (m_time_left_in_state > 0) {
                --m_time_left_in_state;
                if (m_time_left_in_state == 0) {
                    next_state(entity::State::walk);
                }
            }
        }
        void update_bounce() {
            if (m_is_first_state_update) {
                m_is_first_state_update = false;
                if (sound::Set::at(m_bounce_sound_id)) {
                    sound::Set::at(m_bounce_sound_id)->position({ position().x / 160.0f, position().y / 90.0f });
                    sound::Set::at(m_bounce_sound_id)->play();
                }
                reset_anim(m_bounce_anim_id);
            }


            //int num_frames = anim()->texture_size.x / anim()->source.w;

            //Console::log("num_frames: ", anim()->current_frame(), "\n");

            if (anim()->current_frame() + 1 == anim()->num_frames()) {
                next_state(entity::State::upended);
            }

            //Console::log(anim::Set::at(m_bounce_anim_id)->source.x, " ", anim::Set::at(m_bounce_anim_id)->source.w, "\n");
        }
    };
}