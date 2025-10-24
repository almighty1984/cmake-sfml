module;
#include <cmath>
module player;
import console;
import random;
import particle;

void player::Object::update_swim() {    
    if (m_is_first_state_update) {
        m_is_first_state_update = false;
        m_num_jumps = 0;
        Console::log("player::Object::update_swim\n");
        sound::Manager::get(m_hover_sound_id)->stop();
        sound::Manager::get(m_slide_wall_sound_id)->stop();
    }

    m_current_anim_id = m_swim_anim_id;

    transform()->moved_velocity = 0.0F;
    transform()->max_velocity = is_pressed(key_sprint) || m_num_jumps == 1 ? Vec2F{ 3.0F, 3.0F } : Vec2F{ 2.0F, 2.0F };

    cVec2F swim_acc = { 0.15f, 0.15f };
    cVec2F jump_acc = { 0.05f, 0.01f };

    transform()->acceleration = m_num_jumps == 1 ? jump_acc : swim_acc;
    transform()->deceleration = { 0.02f, 0.02f };

    if (m_num_jumps == 0) {
        for (auto& i : m_aabb_ids) {
            aabb::Manager::get(i)->is_active(false);
            if (!aabb::Manager::get(i)->is_active() && aabb::Manager::get(i)->name == aabb::Name::body_swim) {
                aabb::Manager::get(i)->is_active(true);
                break;
            }
        }
        cF32 hypotenuse = std::sqrtf(transform()->velocity.x * transform()->velocity.x + transform()->velocity.y * transform()->velocity.y);
        anim::Manager::get(m_current_anim_id)->speed = hypotenuse / 5.0F;
        if (anim::Manager::get(m_current_anim_id)->speed < 0.1F) {
            anim::Manager::get(m_current_anim_id)->speed = 0.1F;
        }


        if (m_time_in_state > 50) {
            cU32 random_num = random::Generator::number(0, 10);
            cU32 speed = (I32)std::sqrtf(velocity().x * velocity().x + velocity().y * velocity().y);
            //Console::log("speed: ", speed, "\n");
            m_time_in_state = random_num + speed * 8;

            //Console::log("m_time_in_state: ", m_time_in_state, "\n");
            
            //Console::log("spawn bubble\n");

            F32 vel_x = sprite()->is_upended ? -0.5F : 0.5F;

            //Console::log("is upended: ", sprite()->is_upended, "\n");
            //Console::log("vel_x: ", vel_x, "\n");

            particle::Manager::spawn({ .parent = this,
                                     .type = particle::Type::bubble,
                                     .position = position() + Vec2F{sprite()->is_upended ? -6.0F : 6.0F, 0.0F},
                                     .velocity = Vec2F{transform()->velocity.x / 10.0F + vel_x, -0.5F}
                                  }        
            );
        } else {
            ++m_time_in_state;
        }

    } else if (m_num_jumps == 1) {
        m_time_in_state = 0;
        for (auto& i : m_aabb_ids) {
            aabb::Manager::get(i)->is_active(false);
            if (!aabb::Manager::get(i)->is_active() && aabb::Manager::get(i)->name == aabb::Name::body) {
                aabb::Manager::get(i)->is_active(true);
                break;
            }
        }
        transform()->velocity.y += 0.2F;

        anim::Manager::get(m_current_anim_id)->speed = 0.0F;
        anim::Manager::get(m_current_anim_id)->source.x = anim::Manager::get(m_current_anim_id)->source.w * 7;
                
        if ((m_is_on_ground || is_pressed(key_jump) || is_pressed(key_melee)) && m_next_state != entity::State::run) {
            m_next_state = entity::State::run;
            return;
            //next_state(entity::State::run);
            //Console::log("is_on_ground: ", m_is_on_ground, "\n");
        }
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


    F32 radians = std::atan2(transform()->velocity.y, transform()->velocity.x);
    if (radians < 0.0F) {
        radians += 3.1415926535F * 2.0F;
    }
    F32 degrees = radians * 180.0F / 3.1415926535F;
    //Console::log("degrees: ", degrees, "\n");
    sprite()->set_origin({ sprite()->source_rect.w / 2.0F, 48.0F });


    //Console::log("origin: ", sprite()->origin.x, " ", sprite()->origin.y, "\n");

    //F32 degree_diff = sprite()->rotation - degrees;
    //Console::log("degree diff: ", degree_diff, "\n");

    if (is_pressed(key_jump) && m_num_jumps == 0) {
        return;
    }

    if (velocity().x < -transform()->deceleration.x || velocity().x > transform()->deceleration.x ||
        velocity().y < -transform()->deceleration.y || velocity().y > transform()->deceleration.y
        ||
        is_pressed(key_left) || is_pressed(key_right) || is_pressed(key_up) || is_pressed(key_down)
        ) {
        
        //if (degree_diff > 10.0F) {
            //sprite()->rotate(degree_diff * 0.1F);
        //} else {
            sprite()->set_rotation(degrees);
        //}
    }

    sprite()->is_leftward = false;
    if (velocity().x < 0.0F) { 
        sprite()->is_upended = true;
    } else if (velocity().x > 0.0F) {
        sprite()->is_upended = false;
    }
    
    if (!is_pressed(key_up) && !is_pressed(key_down) && !is_pressed(key_left) && !is_pressed(key_right)) {
        if (sprite()->is_upended) {
            cF32 from_target_dir = std::abs(180.0F - sprite()->rotation);
            //Console::log("from_target_dir: ", from_target_dir, "\n");

            if (sprite()->rotation >= 90.0F && sprite()->rotation < 180.0F) {
                sprite()->set_rotation(sprite()->rotation + 1.0F * from_target_dir * 0.1F);
            } else if (sprite()->rotation <= 270.0F && sprite()->rotation > 180.0F) {
                sprite()->set_rotation(sprite()->rotation - 1.0F * from_target_dir * 0.1F);
            }
        } else {
            cF32 target_dir = sprite()->rotation >= 270.0F && sprite()->rotation <= 360.0F ? 360.0F : 0.0F;

            cF32 from_target_dir = std::abs(target_dir - sprite()->rotation);

            //Console::log("from_target_dir: ", from_target_dir, "\n");

            if (sprite()->rotation >= 270.0F && sprite()->rotation < 360.0F) {
                sprite()->set_rotation(sprite()->rotation + 1.0F * from_target_dir * 0.1F);
            } else if (sprite()->rotation <= 90.0F && sprite()->rotation > 0.0F) {
                sprite()->set_rotation(sprite()->rotation - 1.0F * from_target_dir * 0.1F);
            }
        }
    }

    //Console::log("player::Object::update_swim is_upended: ", sprite()->is_upended, "\n");

    if (m_next_state == entity::State::run) {
        if (m_time_left_in_state > 0) --m_time_left_in_state;
        if (m_time_left_in_state == 0) {
            m_is_first_state_update = true;
            m_is_locked_melee = false;
            m_is_ducking = false;
            m_time_left_melee = 0;
            m_state = entity::State::run;
            sprite()->is_leftward = sprite()->is_upended;
            sprite()->is_upended = false;
            sprite()->set_rotation(0.0F);
        }
    }
}