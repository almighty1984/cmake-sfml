module;
#include <cmath>
module player;
import console;

void player::Object::update_swing() {
    if (m_is_first_state_update) {
        m_is_first_state_update = false;
        for (auto& i : m_aabb_ids) {
            aabb::Set::at(i)->is_inactive(true);
            if (aabb::Set::at(i)->is_inactive() && aabb::Set::at(i)->name == aabb::Name::body) {
                aabb::Set::at(i)->is_inactive(false);
            }
        }
    }

    Vec2f offset_from_swing = transform::Set::at(m_transform_id)->position - transform::Set::at(level_transform_id)->position - m_swing_level_offset;

    Vec2f level_pos = transform::Set::at(level_transform_id)->position;

    Vec2f prev_pos = transform()->position;
    if (m_prev_state != entity::State::swing) {
        m_prev_state = entity::State::swing;
        m_next_state = entity::State::swing;

        m_rotation_speed = -(transform()->velocity.x * transform()->velocity.y);

        transform()->velocity = { 0.0f, 0.0f };

        f32 angle = std::atan2f(offset_from_swing.y, offset_from_swing.x) * (180.0f / 3.14159265359f) - 90.0f;

        sprite()->set_rotation(angle);        

        reset_anim(m_swing_anim_id);

        transform::Set::at(m_transform_id)->velocity_limit = m_swing_velocity_limit;

        sound::Set::at(m_swing_attach_sound_id)->position({ position().x / 160.0f, position().y / 90.0f });
        sound::Set::at(m_swing_attach_sound_id)->play();
    }

    m_is_on_ground = false;
    anim::Set::at(m_current_anim_id)->speed = 0.0f;

    transform()->acceleration = { 0.1f, 0.1f };
    transform()->deceleration = { 0.0f, 0.0f };

    sprite()->set_origin({ 32.0f, 20.0f });

    Vec2f pos_on_level = transform::Set::at(m_transform_id)->position - transform::Set::at(level_transform_id)->position;    

    f32 rotate_dec = offset_from_swing.x / 80.0f;
    f32 rotate_acc = 0.25f;

    m_rotation_speed += rotate_dec;
    
    sprite()->rotate(m_rotation_speed);

    if (is_pressed(key_left)) {
        m_rotation_speed += rotate_acc;        
    }
    if (is_pressed(key_right)) {
        m_rotation_speed -= rotate_acc;        
    }
    
    transform()->position.x = std::cosf(sprite()->radians() + 1.57079632679f) * 28.0f + level_pos.x + m_swing_level_offset.x;
    transform()->position.y = std::sinf(sprite()->radians() + 1.57079632679f) * 28.0f + level_pos.y + m_swing_level_offset.y;

    offset_from_swing = transform::Set::at(m_transform_id)->position - transform::Set::at(level_transform_id)->position - m_swing_level_offset;

    sprite()->offset = { -24.0f - offset_from_swing.x, -10.0f - offset_from_swing.y };

    if (!is_pressed(key_melee) /*|| sprite()->rotation < -80.0f || sprite()->rotation > 80.0f*/) {
        //release(key_melee);
        m_prev_state = m_state;
        m_state = entity::State::run;
        //sprite()->offset = { -24.0f,-40.0f };

        sprite()->offset = m_start_sprite_offset;
                
        m_time_left_jump_again = m_time_to_jump_again;
        m_num_jumps = 0;
        reset_anim(m_jump_spin_anim_id);
        
        sound::Set::at(m_jump_sound_id)->pitch(1.122454545454545f);

        transform()->velocity = (transform()->position - prev_pos) * 1.5f;        
        transform()->update();

        Console::log("player_update_swing() velocity: ", transform()->velocity.x, " ", transform()->velocity.y, "\n");

        sprite()->set_rotation(0.0f);
        
        sprite()->is_leftward = velocity().x < 0.0f;

        sound::Set::at(m_swing_detach_sound_id)->position({ position().x / 160.0f, position().y / 90.0f });
        sound::Set::at(m_swing_detach_sound_id)->play();
    }

    m_rotation_speed *= 0.99f;

    if (m_rotation_speed < -13.0f) {
        m_rotation_speed = -13.0f;
    }
    if (m_rotation_speed > 13.0f) {
        m_rotation_speed = 13.0f;
    }

    //Console::log("player::Object::update_swing() rotation_speed: ", m_rotation_speed, "\n");
}