module;
#include <cmath>
module player;
import console;

void player::Object::update_swim() { 
    if (m_prev_state != entity::State::swim) {
        m_prev_state = entity::State::swim;
        m_next_state = entity::State::swim;
        Console::log("player::Object::update_swim\n");
    }

    for (auto& i : m_aabb_ids) {
        aabb::Set::at(i)->is_inactive(true);
        if (aabb::Set::at(i)->is_inactive() && aabb::Set::at(i)->name == aabb::Name::body_swim) {
            aabb::Set::at(i)->is_inactive(false);
        }
    }
    
    m_current_anim_id = m_swim_anim_id;

    transform()->velocity_limit = is_pressed(key_sprint) ? Vec2f{ 2.0f, 2.0f } : Vec2f{ 1.5f, 1.5f };
    transform()->acceleration = { 0.2f, 0.2f };
    transform()->deceleration = { 0.01f, 0.01f };

    f32c hypotenuse = std::sqrtf(transform()->velocity.x * transform()->velocity.x + transform()->velocity.y * transform()->velocity.y);

    anim::Set::at(m_current_anim_id)->speed = hypotenuse / 5.0f;
    if (anim::Set::at(m_current_anim_id)->speed < 0.1f) {
        anim::Set::at(m_current_anim_id)->speed = 0.1f;
    }


    if (is_pressed(key_up)) {
        transform()->velocity.y -= transform()->acceleration.y;
    }
    if (is_pressed(key_down)) {
        transform()->velocity.y += transform()->acceleration.y;
    }
    if (is_pressed(key_left)) {
        //sprite()->is_leftward = true;
        transform()->velocity.x -= transform()->acceleration.x;
    }
    if (is_pressed(key_right)) {
        //sprite()->is_leftward = false;
        transform()->velocity.x += transform()->acceleration.x;
    }


    f32 radians = std::atan2(transform()->velocity.y, transform()->velocity.x);
    if (radians < 0.0f) {
        radians += 3.14159265f * 2.0f;
    }
    f32 degrees = radians * 180.0f / 3.14159265f;

    sprite()->set_origin({ sprite()->source_rect.w / 2.0f, 48.0f });


    //Console::log("origin: ", sprite()->origin.x, " ", sprite()->origin.y, "\n");

    if (velocity().x < 0.0f || velocity().x > 0.0f || velocity().y < 0.0f || velocity().y > 0.0f ||
        is_pressed(key_left) || is_pressed(key_right) || is_pressed(key_up) || is_pressed(key_down)) { 
        sprite()->set_rotation(degrees);
    }

    sprite()->is_leftward = false;
    if (velocity().x < 0.0f) { 
        sprite()->is_upended = true;
    } else if (velocity().x > 0.0f) {
        sprite()->is_upended = false;
    }

    if (!is_pressed(key_up) && !is_pressed(key_down) && !is_pressed(key_left) && !is_pressed(key_right)) {
        if (sprite()->rotation > 270.0f && sprite()->rotation < 360.0f)   sprite()->set_rotation(sprite()->rotation + 1.0f);
        if (sprite()->rotation >   0.0f && sprite()->rotation <  90.0f)   sprite()->set_rotation(sprite()->rotation - 1.0f);
        if (sprite()->rotation < 180.0f && sprite()->rotation >  90.0f)   sprite()->set_rotation(sprite()->rotation + 1.0f);
        if (sprite()->rotation < 270.0f && sprite()->rotation > 180.0f)   sprite()->set_rotation(sprite()->rotation - 1.0f);
        if (sprite()->rotation == 90.0f) {
            sprite()->is_upended ? sprite()->set_rotation(sprite()->rotation + 1.0f) : sprite()->set_rotation(sprite()->rotation - 1.0f);
        }
    }

    //Console::log("player::Object::update_swim is_upended: ", sprite()->is_upended, "\n");

    if (is_pressed(input::Key::num_5)) {
        release(input::Key::num_5);
        m_prev_state = m_state;
        m_next_state = entity::State::run;
        m_time_left_in_state = 20;
    }

    if (m_next_state == entity::State::run && m_time_left_in_state == 0) {
        m_state = entity::State::run;
        sprite()->is_leftward = sprite()->is_upended;
        sprite()->is_upended = false;
        sprite()->set_rotation(0.0f);

        //for (auto& i : m_run_aabb_ids)  aabb::Set::at(i)->is_inactive(false);
        //for (auto& i : m_swim_aabb_ids) aabb::Set::at(i)->is_inactive(true);
    }
}