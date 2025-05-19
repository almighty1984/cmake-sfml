module;
#include <cmath>
module player;
import console;

void player::Object::update_swim() {
    if (is_pressed(key_melee)) {
        release(key_melee);
        m_state = player::State::run;
        return;
    }
    //Console::log("player::Object::update_swim\n");
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
        sprite()->is_leftward = true;
        transform()->velocity.x -= transform()->acceleration.x;
    }
    if (is_pressed(key_right)) {
        sprite()->is_leftward = false;
        transform()->velocity.x += transform()->acceleration.x;
    }
}