module;
#include <cmath>
module entity.bug;
import collider.aabb;
import console;
import sound;

void entity::Bug::collide_x(aabb::Infoc our, aabb::Infoc other) {
    if (m_parent) return;
    if (!sprite() || !our.owner || !other.owner || !aabb::Set::at(other.id)) return;
    if (aabb::Set::at(our.id)->points().size() < 4 || aabb::Set::at(other.id)->points().size() < 4) return;

    entity::Typec other_type = other.owner->type();

    aabb::Name other_aabb_name = aabb::Set::at(other.id)->name;

    Rectfc our_rect = { aabb::Set::at(our.id)->points().at(0).x, aabb::Set::at(our.id)->points().at(0).y,
                        aabb::Set::at(our.id)->points().at(3).x, aabb::Set::at(our.id)->points().at(3).y };
    Rectfc other_rect = { aabb::Set::at(other.id)->points().at(0).x, aabb::Set::at(other.id)->points().at(0).y,
                        aabb::Set::at(other.id)->points().at(3).x, aabb::Set::at(other.id)->points().at(3).y };
    
    f32c overlap_x = our_rect.x < other_rect.x ? our_rect.w - other_rect.x : -(other_rect.w - our_rect.x);

    if (other_type == entity::Type::bug) {
        transform()->position.x -= overlap_x;
        transform()->velocity.x *= -1.0f;
        transform()->moved_velocity.x = 0.0f;
        sprite()->is_leftward = !sprite()->is_leftward;
    } else if (other_type == entity::Type::clip || other_type == entity::Type::clip_duck || other_type == entity::Type::clip_ledge) {
        transform()->position.x -= overlap_x;

        if (m_time_left_turning > 0 || !m_is_on_ground) return;
        
        transform()->moved_velocity.x = 0.0f;
        if (transform()->velocity.x > 0.0f && our_rect.x < other_rect.x) {
            transform()->velocity.x = -1.0f;
            transform()->position.x -= 2.0f;
            m_time_left_turning = 30;            
        } else if (transform()->velocity.x < 0.0f && our_rect.x > other_rect.x) {
            transform()->velocity.x = 1.0f;
            transform()->position.x += 2.0f;
            m_time_left_turning = 30;
        }
        sprite()->is_leftward = transform()->velocity.x < 0.0f;

        if (m_state == entity::State::tossed) {
            Vec2f our_center = Vec2f{ our_rect.x + (our_rect.w - our_rect.x) / 2.0f,     our_rect.y + (our_rect.h - our_rect.y) / 2.0f };
            Vec2f other_center = Vec2f{ other_rect.x + (other_rect.w - other_rect.x) / 2.0f, other_rect.y + (other_rect.h - other_rect.y) / 2.0f };

            m_particles_to_spawn.push_back({ .type     = ParticleType::hit,
                                             .position = Vec2f{ (our_center.x + other_center.x) / 2.0f, (our_center.y + other_center.y) / 2.0f },
                                             .velocity = Vec2f{ 0.0f, 0.0f } });
            
            hurt();
        }
    } else if (other_type == entity::Type::clip_L || other_type == entity::Type::clip_D_L) {
        if (transform()->velocity.x + transform()->moved_velocity.x <= 0.0f) return;
        transform()->position.x -= overlap_x;
        transform()->velocity.x *= -1.0f;
        transform()->moved_velocity.x = 0.0f;
        sprite()->is_leftward = !sprite()->is_leftward;

        if (m_state == entity::State::tossed) {
            Vec2f our_center = Vec2f{ our_rect.x + (our_rect.w - our_rect.x) / 2.0f,     our_rect.y + (our_rect.h - our_rect.y) / 2.0f };
            Vec2f other_center = Vec2f{ other_rect.x + (other_rect.w - other_rect.x) / 2.0f, other_rect.y + (other_rect.h - other_rect.y) / 2.0f };

            m_particles_to_spawn.push_back({ .type     = ParticleType::hit,
                                             .position = Vec2f{ (our_center.x + other_center.x) / 2.0f, (our_center.y + other_center.y) / 2.0f },
                                             .velocity = Vec2f{ 0.0f, 0.0f } });
            hurt();
        }
    } else if (other_type == entity::Type::clip_R || other_type == entity::Type::clip_D_R) {
        if (transform()->velocity.x + transform()->moved_velocity.x >= 0.0f) return;
        transform()->position.x -= overlap_x;
        transform()->velocity.x *= -1.0f;
        transform()->moved_velocity.x = 0.0f;
        sprite()->is_leftward = !sprite()->is_leftward;

        //Console::log("hello ", entity::state_to_string(m_state), "\n");

        if (m_state == entity::State::tossed) {
            Vec2f our_center = Vec2f{ our_rect.x + (our_rect.w - our_rect.x) / 2.0f,     our_rect.y + (our_rect.h - our_rect.y) / 2.0f };
            Vec2f other_center = Vec2f{ other_rect.x + (other_rect.w - other_rect.x) / 2.0f, other_rect.y + (other_rect.h - other_rect.y) / 2.0f };
                        
            m_particles_to_spawn.push_back({ .type     = ParticleType::hit,
                                             .position = Vec2f{ (our_center.x + other_center.x) / 2.0f, (our_center.y + other_center.y) / 2.0f },
                                             .velocity = Vec2f{ 0.0f, 0.0f } });

            hurt();            
        }
    } else if (other_type == entity::Type::frog) {
        
    } else if (other_type == entity::Type::player) {
        if (m_state == entity::State::tossed) return;
        if (other_aabb_name == aabb::Name::interact) {
            m_parent = other.owner;
            m_time_left_interacting = m_time_to_interact;
            next_state(entity::State::walk);
            other.owner->is_carrying(true);

            /*if (sound::Set::at(m_bounce_sound_id)) {
                sound::Set::at(m_bounce_sound_id)->position({ position().x / 160.0f, position().y / 90.0f });
                sound::Set::at(m_bounce_sound_id)->play();
            }*/
            return;
        }
        if (other_aabb_name == aabb::Name::melee_L || other_aabb_name == aabb::Name::melee_R) {
            if (!is_hurting()) {                
                Vec2f our_center   = Vec2f{ our_rect.x   + (our_rect.w - our_rect.x) / 2.0f,     our_rect.y   + (our_rect.h - our_rect.y) / 2.0f     };
                Vec2f other_center = Vec2f{ other_rect.x + (other_rect.w - other_rect.x) / 2.0f, other_rect.y + (other_rect.h - other_rect.y) / 2.0f };

                m_particles_to_spawn.push_back({ .type     = ParticleType::hit,
                                                 .position = Vec2f{ (our_center.x + other_center.x) / 2.0f, (our_center.y + other_center.y) / 2.0f },
                                                 .velocity = Vec2f{ 0.0f, 0.0f } });

                hurt();
            }
            return;
        }
        //Console::log(collider::type_as_string(aabb::Set::at(other.id)->type), "\n");
        transform()->position.x -= overlap_x;
        if (m_state == entity::State::walk) {
            //transform()->velocity.x *= -1.0f;
            //transform()->velocity.x = other.owner->transform()->velocity.x;


            //if (std::abs(other.owner->transform()->velocity.x) > std::abs(transform()->velocity.x)) {
                //sprite()->is_leftward = !sprite()->is_leftward;
            //}

            transform()->velocity.x = other.owner->transform()->velocity.x;
            if (our_rect.x < other_rect.x) {
                transform()->velocity.x = -1.0f;
            } else {
                transform()->velocity.x = 1.0f;
            }
            
            sprite()->is_leftward = transform()->velocity.x < 0.0f;

            transform()->velocity.y = -1.0f;

            transform()->moved_velocity.x = 0.0f;

        }
    } else if (other_type == entity::Type::slope_L_1x1) {
        transform()->velocity.y = -transform()->velocity.x;
        if (transform()->velocity.x > transform()->velocity_limit.x * 0.7f) {
            transform()->velocity.x = transform()->velocity_limit.x * 0.7f;
        }
        m_is_on_ground = true;
        m_is_on_slope = true;
    } else if (other_type == entity::Type::slope_L_2x1_0 || other_type == entity::Type::slope_L_2x1_1) {
        transform()->velocity.y = -transform()->velocity.x / 2.0f;
        if (transform()->velocity.x > transform()->velocity_limit.x * 0.9f) {
            transform()->velocity.x = transform()->velocity_limit.x * 0.9f;
        }
        m_is_on_ground = true;
        m_is_on_slope = true;
    } else if (other_type == entity::Type::slope_R_1x1) {
        transform()->velocity.y = transform()->velocity.x;
        if (transform()->velocity.x < -transform()->velocity_limit.x * 0.7f) {
            transform()->velocity.x = -transform()->velocity_limit.x * 0.7f;
        }
        m_is_on_ground = true;
        m_is_on_slope = true;
    } else if (other_type == entity::Type::slope_R_2x1_0 || other_type == entity::Type::slope_R_2x1_1) {
        transform()->velocity.y = transform()->velocity.x / 2.0f;
        if (transform()->velocity.x < -transform()->velocity_limit.x * 0.9f) {
            transform()->velocity.x = -transform()->velocity_limit.x * 0.9f;
        }
        m_is_on_ground = true;
        m_is_on_slope = true;
    }
};


void entity::Bug::collide_y(aabb::Infoc our, aabb::Infoc other) {
    if (m_parent) return;
    if (!sprite() || !our.owner || !other.owner || !aabb::Set::at(other.id)) return;
    if (aabb::Set::at(our.id)->points().size() < 4 || aabb::Set::at(other.id)->points().size() < 4) return;

    Rectfc our_rect = { aabb::Set::at(our.id)->points().at(0).x, aabb::Set::at(our.id)->points().at(0).y,
                        aabb::Set::at(our.id)->points().at(3).x, aabb::Set::at(our.id)->points().at(3).y };

    entity::Typec other_type = other.owner->type();
    aabb::Name    other_aabb_name = aabb::Set::at(other.id)->name;    
    Rectfc        other_rect = { aabb::Set::at(other.id)->points().at(0).x, aabb::Set::at(other.id)->points().at(0).y,
                                 aabb::Set::at(other.id)->points().at(3).x, aabb::Set::at(other.id)->points().at(3).y };

    f32c overlap_y = our_rect.y < other_rect.y ? our_rect.h - other_rect.y : -(other_rect.h - our_rect.y);

    if (other_type == entity::Type::clip || other_type == entity::Type::clip_duck || other_type == entity::Type::clip_ledge) {
        transform()->position.y -= overlap_y;
        transform()->velocity.y = transform()->moved_velocity.y = 0.0f;
        if (our_rect.y < other_rect.y) {
            m_is_on_ground = true;
            m_is_on_slope = false;
        }

        //if (!m_is_on_ground) return;
        if (other_type == entity::Type::clip_ledge) {
            //Console::log("our: ", our_rect.y, " ", our_rect.h, "\n");
            if (our_rect.h > other_rect.y + transform()->acceleration.y) return;

            if (m_time_left_turning > 0 || m_state == entity::State::upended || !m_is_on_ground) return;
            if (transform()->velocity.x > 0.0f && our_rect.x > other_rect.x) {
                if (our_rect.x + our_rect.w > other_rect.x + other_rect.w) {
                    transform()->velocity.x = -1.0f;
                    transform()->position.x -= 2.0f;
                    m_time_left_turning = 30;
                }
            } else if (transform()->velocity.x < 0.0f && our_rect.x < other_rect.x) {
                if (our_rect.x < other_rect.x) {
                    transform()->velocity.x = 1.0f;
                    transform()->position.x += 2.0f;
                    m_time_left_turning = 30;
                }
            }
            sprite()->is_leftward = transform()->velocity.x < 0.0f;
        }
    } else if (other_type == entity::Type::clip_U || other_type == entity::Type::slope_U) {
        if (transform()->velocity.y < 0.0f) return;
        transform()->position.y -= overlap_y;

        transform()->velocity.y = transform()->moved_velocity.y = 0.0f;
        m_is_on_ground = true;
        m_is_on_slope = other_type == entity::Type::slope_U;
    } else if (other_type == entity::Type::frog) {
        if (m_state == entity::State::upended || m_state == entity::State::bounce) {            
            transform()->position.y -= overlap_y;
            next_state(entity::State::bounce);
            return;
        }
    } else if (other_type == entity::Type::slope_L_2x1_0 || other_type == entity::Type::slope_L_2x1_1) {
        //if (transform()->velocity.y < 0.0f) return;
        transform()->position.y -= overlap_y;

        transform()->velocity.y = 0.0f;
        transform()->moved_velocity.y = 0.0f;
        //if (transform()->velocity.x < 0.0f) {
        transform()->velocity.y = transform()->velocity.x / -2.0f;
        //}
        m_is_on_ground = true;
        m_is_on_slope = true;
        /*if (transform()->velocity.x > transform()->velocity_limit.x * 0.9f) {
            transform()->velocity.x = transform()->velocity_limit.x * 0.9f;
        }*/
    } else if (other_type == entity::Type::slope_R_1x1) {
        //if (transform()->velocity.y < 0.0f) return;
        transform()->position.y -= overlap_y;

        transform()->velocity.y = 0.0f;
        transform()->moved_velocity.y = 0.0f;
        //if (transform()->velocity.x > 0.0f) {
        transform()->velocity.y = transform()->velocity.x;
        //}
        m_is_on_ground = true;
        m_is_on_slope = true;
        /*if (transform()->velocity.x < -transform()->velocity_limit.x * 0.7f) {
            transform()->velocity.x = -transform()->velocity_limit.x * 0.7f;
        }*/
    } else if (other_type == entity::Type::slope_R_2x1_0 || other_type == entity::Type::slope_R_2x1_1) {
        //if (transform()->velocity.y < 0.0f) return;
        transform()->position.y -= overlap_y;

        transform()->velocity.y = 0.0f;
        transform()->moved_velocity.y = 0.0f;
        if (transform()->velocity.x > 0.0f) {
            transform()->velocity.y = transform()->velocity.x / 2.0f;
        }
        m_is_on_ground = true;
        m_is_on_slope = true;
        /*if (transform()->velocity.x < -transform()->velocity_limit.x * 0.9f) {
            transform()->velocity.x = -transform()->velocity_limit.x * 0.9f;
        }*/
    } else if (other_type == entity::Type::player) {
        if (other_aabb_name == aabb::Name::melee_L || other_aabb_name == aabb::Name::melee_R) {
            collide_x(our, other);
            return;
        }
        if (our_rect.h <= other_rect.y) {
            /*Vec2f vel = transform()->velocity + transform()->moved_velocity;
            transform()->velocity.x = (other.owner->transform()->velocity.x + other.owner->transform()->moved_velocity.x) / 1.0f;
            transform()->velocity.y = -6.0f;

            f32c v = other.owner->transform()->velocity.y;

            Console::log("entity::Bug::collide_y vel y: ", v, "\n");*/
            //transform()->position.y -= 8.0f;
            //transform()->position.y -= overlap_y;
            //other.owner->transform()->velocity = vel / 2.0f;

            //other.owner->hurt();

            Vec2f our_center = Vec2f{ our_rect.x + (our_rect.w - our_rect.x) / 2.0f,     our_rect.y + (our_rect.h - our_rect.y) / 2.0f };
            Vec2f other_center = Vec2f{ other_rect.x + (other_rect.w - other_rect.x) / 2.0f, other_rect.y + (other_rect.h - other_rect.y) / 2.0f };
                        
            m_particles_to_spawn.push_back({ .type     = ParticleType::hit,
                                             .position = Vec2f{ (our_center.x + other_center.x) / 2.0f, (our_center.y + other_center.y) / 2.0f },
                                             .velocity = Vec2f{ 0.0f, 0.0f } });
        } else {
            if (m_state == entity::State::upended) {
                next_state(entity::State::bounce);
            }
        }
    }
};