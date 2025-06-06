module;
#include <cmath>
module player;
import console;

void player::Object::update_swing() {
    Vec2f offset_from_swing = position_difference(m_transform_id, level_transform_id) - m_swing_level_offset;
    


    Vec2f level_pos = transform::Set::at(level_transform_id)->position;

    Vec2f prev_pos = transform()->position;
    if (m_prev_state != State::swing) {
        m_prev_state = State::swing;
        m_next_state = State::swing;
        //for (auto& i : m_melee_collider_ids) aabb::Set::at(i)->is_inactive(true);
        aabb::Set::at(m_melee_left_collider_id)->is_inactive(true);
        aabb::Set::at(m_melee_right_collider_id)->is_inactive(true);

        m_rotation_speed = -(transform()->velocity.x * transform()->velocity.y);

        transform()->velocity = { 0.0f, 0.0f };

        f32 angle = std::atan2f(offset_from_swing.y, offset_from_swing.x) * (180.0f / 3.14159265359f) - 90.0f;

        sprite()->set_rotation(angle);
        Console::log("angle: ", angle, "\n");

        

        reset_anim(m_swing_anim_id);

        transform::Set::at(m_transform_id)->velocity_limit = m_swing_velocity_limit;

        sound::Set::at(m_swing_attach_sound_id)->position({ position().x / 160.0f, position().y / 90.0f });
        sound::Set::at(m_swing_attach_sound_id)->play();
    }

    is_on_ground = false;
    anim::Set::at(m_current_anim_id)->speed = 0.0f;


    transform()->acceleration = { 0.1f, 0.1f };
    transform()->deceleration = { 0.0f, 0.0f };

    sprite()->set_origin({ 32.0f, 20.0f });
    


    


    Vec2f pos_on_level = position_difference(m_transform_id, level_transform_id);

    



    //transform()->velocity.x -= offset_from_swing.x / 100.0f;


    f32 rotate_dec = offset_from_swing.x / 80.0f;
    //f32 rotate_dec = offset_from_swing.x / 8.0f;
    //f32 rotate_acc = 1.0f - std::abs(offset_from_swing.x) / -100.0f;
    f32 rotate_acc = 0.25f;

    /*if (rotate_acc < -1.0f) {
        rotate_acc = -1.0f;
    }
    if (rotate_acc > 1.0f) {
        rotate_acc = 1.0f;
    }*/

    /*if (sprite()->rotation < 0.0f) {
        sprite()->rotate(1.0f);
    }
    if (sprite()->rotation > 0.0f) {
        sprite()->rotate(-1.0f);
    }*/

    //if (sprite()->rotation > 10.0f || sprite()->rotation < -10.0f) {
        //sprite()->rotate(rotate_dec);
    //}


    /*if (sprite()->rotation < 0.0f) {
        m_rotation_speed += rotate_dec;
    }

    if (sprite()->rotation > 0.0f) {
        m_rotation_speed -= rotate_dec;
    }*/

    m_rotation_speed += rotate_dec;

    
    sprite()->rotate(m_rotation_speed);

    

    //Console::log("rotate_acc: ", rotate_dec, "\n");


    if (is_pressed(key_left)) {
        m_rotation_speed += rotate_acc;
        //sprite()->rotate(rotate_acc);
        //transform()->velocity.x -= transform()->acceleration.x;
    }
    if (is_pressed(key_right)) {
        m_rotation_speed -= rotate_acc;
        //sprite()->rotate(-rotate_acc);
        //transform()->velocity.x += transform()->acceleration.x;
    }

    //Console::log("rotation: ", sprite()->rotation, "\n");

    //transform()->position.y = std::cosf(pos_diff / 40.0f) * 56.0f;


    //Console::log("collider pos: ", aabb::Set::at(m_run_collider_id)->transform()->position.x, " ", aabb::Set::at(m_run_collider_id)->transform()->position.y, "\n");
    
    //Console::log("transform id: ", m_transform_id, " ", aabb::Set::at(m_run_collider_id)->transform_id(), "\n");

    //sprite()->set_origin({pos_diff, 0.0f});

    //sprite()->set_rotation(pos_diff);

    //sprite()->rotate(2.0f);

    //aabb::Set::at(m_run_collider_id)->set_offset({ 0.0f,std::sinf(sprite()->radians())});


    
    

    /*if (!is_pressed(key_left) && !is_pressed(key_right)) {
        if (position_difference(level_transform_id).x < m_swing_position.x) {
            transform()->velocity.x += 0.1f;
        } else if (position_difference(level_transform_id).x > m_swing_position.x) {
            transform()->velocity.x -= 0.1f;
        }
    }*/


    transform()->position.x = std::cosf(sprite()->radians() + 1.57079632679f) * 28.0f + level_pos.x + m_swing_level_offset.x;
    transform()->position.y = std::sinf(sprite()->radians() + 1.57079632679f) * 28.0f + level_pos.y + m_swing_level_offset.y;

    offset_from_swing = position_difference(m_transform_id, level_transform_id) - m_swing_level_offset;

    sprite()->offset = { -24.0f - offset_from_swing.x, -10.0f - offset_from_swing.y };

    if (!is_pressed(key_melee) /*|| sprite()->rotation < -80.0f || sprite()->rotation > 80.0f*/) {
        //release(key_melee);
        m_prev_state = m_state;
        m_state = player::State::run;
        sprite()->offset = { -24.0f,-40.0f };
        
        //transform()->velocity_limit = m_air_velocity_limit;
        
        //transform()->moved_velocity = (transform()->position - prev_pos) * 1.0f;

        //Vec2f over_limit = (transform()->position - prev_pos) - transform()->velocity_limit;

        //Console::log("overshoot: ", over_limit.x, "\n");

        //transform()->velocity.x = -sprite()->rotation;
        //transform()->velocity.y = (transform()->position - prev_pos).y;

        //transform()->moved_velocity = over_limit * 0.5f;

        transform()->velocity = (transform()->position - prev_pos) * 2.0f;        
        transform()->update();


        //m_velocity_at_jump = transform()->velocity.x;

        //Console::log("moved_velocity: ", transform()->moved_velocity.x, " ", transform()->moved_velocity.y, "\n");
        Console::log("velocity: ", transform()->velocity.x, " ", transform()->velocity.y, "\n");

        reset_anim(m_jump_anim_id);

        sprite()->set_rotation(0.0f);
        
        sprite()->is_leftward = velocity().x < 0.0f;

        sound::Set::at(m_swing_detach_sound_id)->position({ position().x / 160.0f, position().y / 90.0f });
        sound::Set::at(m_swing_detach_sound_id)->play();
    }

    m_rotation_speed *= 0.99f;

    if (m_rotation_speed < -12.0f) {
        m_rotation_speed = -12.0f;
    }
    if (m_rotation_speed > 12.0f) {
        m_rotation_speed = 12.0f;
    }

    //Console::log("player::Object::update_swing() rotation_speed: ", m_rotation_speed, "\n");
}