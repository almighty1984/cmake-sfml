module;
#include <cmath>
module player;
import console;

void player::Object::update_swing() {
    if (!m_parent || !m_parent->transform()) {
        m_next_state = entity::State::run;
        return;
    }
    
    Vec2F swing_position = m_parent->transform()->position + m_parent->start_offset();  // level transform + start offset
    Vec2F offset_from_swing = transform()->position - swing_position;

    cVec2F prev_pos = transform()->position;
    cVec2F level_pos = transform::Manager::get(level_transform_id)->position;
    
    cVec2F pos_on_level = transform::Manager::get(m_transform_id)->position - transform::Manager::get(level_transform_id)->position;
    if (m_is_first_state_update) {
        m_is_first_state_update = false;
        m_next_state = entity::State::swing;

        for (auto& i : m_aabb_ids) {
            aabb::Manager::get(i)->is_active(false);
            if (!aabb::Manager::get(i)->is_active() && aabb::Manager::get(i)->name == aabb::Name::body) {
                aabb::Manager::get(i)->is_active(true);
            }
        }

        m_rotation_speed = -transform()->velocity.y * transform()->velocity.x * 0.75F;
        //m_rotation_speed = -(transform()->velocity.x * transform()->velocity.y);
        //m_rotation_speed = offset_from_swing.x / 4.0F + offset_from_swing.y / 4.0F/* + transform()->velocity.y / 2.0F*/;

        cF32 lower_limit = 3.0F,
             upper_limit = 8.0F;

        if (!sprite()->is_leftward) {
            if (m_rotation_speed > -lower_limit) {
                m_rotation_speed = -lower_limit;
            } else if (m_rotation_speed < -upper_limit) {
                m_rotation_speed = -upper_limit;
            }
        } else {
            if (m_rotation_speed < lower_limit) {
                m_rotation_speed = lower_limit;
            } else if (m_rotation_speed > upper_limit) {
                m_rotation_speed = upper_limit;
            }
        }
        Console::log("rotation speed: ", m_rotation_speed, "\n");

        //m_rotation_speed = transform()->velocity.y;

        //m_rotation_speed = 0.0F;

        Console::log("m_rotation_speed: ", m_rotation_speed, "\n");

        cF32 radians = std::atan2f(offset_from_swing.y, offset_from_swing.x);
        cF32 angle = radians * 180.0F / 3.1415926535F;


        Console::log("player::Object::update_swing angle: ", angle, "\n");

        sprite()->set_origin({ 32.0F, 20.0F });
        sprite()->set_rotation(angle - 90.0F);

        //Console::log("player::Object::update_swing offset from swing: ", offset_from_swing.x, " ", " ", offset_from_swing.y, "\n");

        transform()->velocity = { 0.0F, 0.0F };
        transform()->acceleration = { 0.1F, 0.1F };
        transform()->deceleration = { 0.0F, 0.0F };

        reset_anim(m_swing_anim_id);

        transform::Manager::get(m_transform_id)->max_velocity = m_swing_velocity_limit;
    }
    m_is_on_ground = false;
    anim::Manager::get(m_current_anim_id)->speed = 0.0F;
    

    cF32 rotate_dec = offset_from_swing.x / 80.0F;
    cF32 rotate_acc = 0.25f;
    m_rotation_speed += rotate_dec;
    
    sprite()->rotate(m_rotation_speed);

    if (is_pressed(key_left)) {
        m_rotation_speed += rotate_acc;
    }
    if (is_pressed(key_right)) {
        m_rotation_speed -= rotate_acc;        
    }    

    transform()->position.x = std::cosf(sprite()->radians() + 3.1415956535F / 2.0F) * 28.0F + level_pos.x + m_parent->start_offset().x;
    transform()->position.y = std::sinf(sprite()->radians() + 3.1415956535F / 2.0F) * 28.0F + level_pos.y + m_parent->start_offset().y;

    offset_from_swing = transform()->position - swing_position;
    sprite()->offset = { -24.0F - offset_from_swing.x, -10.0F - offset_from_swing.y };

    if (!is_pressed(key_melee) /*|| sprite()->rotation < -80.0F || sprite()->rotation > 80.0F*/) {
        //release(key_melee);        
        m_next_state = entity::State::run;
        //sprite()->offset = { -24.0F,-40.0F };

        sprite()->offset = m_start_sprite_offset;
        
        m_time_left_jump_again = m_time_to_jump_again;
        m_num_jumps = 0;
        reset_anim(m_jump_spin_anim_id);
        
        sound::Manager::get(m_jump_sound_id)->pitch(1.122454545454545f);

        transform()->velocity = (transform()->position - prev_pos) * 1.5F;        
        transform()->update();

        Console::log("player_update_swing() velocity: ", transform()->velocity.x, " ", transform()->velocity.y, "\n");

        sprite()->set_rotation(0.0F);
        
        sprite()->is_leftward = velocity().x < 0.0F;

        sound::Manager::get(m_swing_detach_sound_id)->position({ position().x / 160.0F, position().y / 90.0F });
        sound::Manager::get(m_swing_detach_sound_id)->play();
    }

    m_rotation_speed *= 0.99f;

    if (m_rotation_speed < -13.0F) {
        m_rotation_speed = -13.0F;
    }
    if (m_rotation_speed > 13.0F) {
        m_rotation_speed = 13.0F;
    }

    //Console::log("player::Object::update_swing() rotation_speed: ", rotation_speed, "\n");
}