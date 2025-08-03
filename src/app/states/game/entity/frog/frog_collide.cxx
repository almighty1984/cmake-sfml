module;
#include <utility>
module entity.frog;
import collider.aabb;
import console;
import sound;

void entity::Frog::collide_x(aabb::Infoc our, aabb::Infoc other) {    
    if (!sprite() || !our.owner || !other.owner || !aabb::Set::at(our.id) || !aabb::Set::at(other.id)) return;
    if (aabb::Set::at(our.id)->is_inactive() || aabb::Set::at(other.id)->is_inactive()) return;
    if (aabb::Set::at(our.id)->points().size() < 4 || aabb::Set::at(other.id)->points().size() < 4) return;

    entity::Typec other_type = other.owner->type();

    aabb::Name other_aabb_name = aabb::Set::at(other.id)->name;

    Rectfc our_rect = { aabb::Set::at(our.id)->points().at(0).x, aabb::Set::at(our.id)->points().at(0).y,
                        aabb::Set::at(our.id)->points().at(3).x, aabb::Set::at(our.id)->points().at(3).y };
    Rectfc other_rect = { aabb::Set::at(other.id)->points().at(0).x, aabb::Set::at(other.id)->points().at(0).y,
                        aabb::Set::at(other.id)->points().at(3).x, aabb::Set::at(other.id)->points().at(3).y };

    f32c overlap_x = our_rect.x < other_rect.x ? our_rect.w - other_rect.x : -(other_rect.w - our_rect.x);

    Vec2fc our_velocity = transform()->velocity;
    Vec2fc other_velocity = other.owner->transform()->velocity;

    if (other_type == entity::Type::bug) {
        //if (other_rect.y < our_rect.y) return;
        if (other.owner->state() == entity::State::tossed && !is_hurting() && !other.owner->parent()) {
            if (other.owner->transform()) {
                transform()->velocity.x = other.owner->transform()->velocity.x + other.owner->transform()->moved_velocity.x;
                transform()->velocity.y = (other.owner->transform()->velocity.y + other.owner->transform()->moved_velocity.y) * 0.5f;
            }
            transform()->velocity.y -= 1.0f;
            hurt();

            Vec2f our_center = Vec2f{ our_rect.x + (our_rect.w - our_rect.x) / 2.0f,     our_rect.y + (our_rect.h - our_rect.y) / 2.0f };
            Vec2f other_center = Vec2f{ other_rect.x + (other_rect.w - other_rect.x) / 2.0f, other_rect.y + (other_rect.h - other_rect.y) / 2.0f };

            m_particles_to_spawn.push_back({ .type = ParticleType::hit,
                                             .position = Vec2f{ (our_center.x + other_center.x) / 2.0f, (our_center.y + other_center.y) / 2.0f },
                                             .velocity = Vec2f{ 0.0f, 0.0f } });
        }

        transform()->position.x -= overlap_x;
                
        transform()->velocity.x = other_velocity.x * 0.5f;
        other.owner->transform()->velocity.x = our_velocity.x * 0.25f;

        //transform()->moved_velocity = other_velocity;

        /*if (other.owner->sprite()) {
            other.owner->sprite()->is_leftward = !other.owner->sprite()->is_leftward;
        }*/
        if (our_rect.x < other_rect.x) {
            //transform()->velocity.x -= 1.0f;
            other.owner->transform()->velocity.x += 1.0f;
            other.owner->sprite()->is_leftward = false;
        } else {
            //transform()->velocity.x += 1.0f;
            other.owner->transform()->velocity.x -= 1.0f;
            other.owner->sprite()->is_leftward = true;
        }
    } else if (other_type == entity::Type::clip || other_type == entity::Type::clip_duck || other_type == entity::Type::clip_ledge) {
        transform()->position.x -= overlap_x;
        transform()->velocity.x = 0.0f;
        transform()->moved_velocity.x = 0.0f;
    } else if (other_type == entity::Type::clip_L || other_type == entity::Type::clip_D_L) {
        if (transform()->velocity.x + transform()->moved_velocity.x <= 0.0f) return;
        transform()->position.x -= overlap_x;
        transform()->velocity.x = 0.0f;
        transform()->moved_velocity.x = 0.0f;
    } else if (other_type == entity::Type::clip_R || other_type == entity::Type::clip_D_R) {
        if (transform()->velocity.x + transform()->moved_velocity.x >= 0.0f) return;
        transform()->position.x -= overlap_x;
        transform()->velocity.x = 0.0f;
        transform()->moved_velocity.x = 0.0f;
    } else if (other_type == entity::Type::player) {
        if (other_aabb_name == aabb::Name::melee_L || other_aabb_name == aabb::Name::melee_R) {
            if (!is_hurting()) {
                if (other.owner->transform()) {
                    transform()->velocity.x = other.owner->transform()->velocity.x + other.owner->transform()->moved_velocity.x;
                    transform()->velocity.y = (other.owner->transform()->velocity.y + other.owner->transform()->moved_velocity.y) * 0.5f;
                }
                transform()->velocity.y -= 1.0f;
                hurt();

                Vec2f our_center = Vec2f{ our_rect.x + (our_rect.w - our_rect.x) / 2.0f,     our_rect.y + (our_rect.h - our_rect.y) / 2.0f };
                Vec2f other_center = Vec2f{ other_rect.x + (other_rect.w - other_rect.x) / 2.0f, other_rect.y + (other_rect.h - other_rect.y) / 2.0f };
                                
                m_particles_to_spawn.push_back({ .type     = ParticleType::hit,
                                                 .position = Vec2f{ (our_center.x + other_center.x) / 2.0f, (our_center.y + other_center.y) / 2.0f },
                                                 .velocity = Vec2f{ 0.0f, 0.0f } });       
            }
            return;
        }
        transform()->position.x -= overlap_x;

        if (std::abs(transform()->velocity.x) > std::abs(other.owner->transform()->velocity.x)) {
            return;
        }

        transform()->velocity.x = other_velocity.x * 0.5f;
        other.owner->transform()->velocity.x = our_velocity.x * 0.25f;
        if (our_rect.x < other_rect.x) {
            transform()->velocity.x -= 1.0f;
            other.owner->transform()->velocity.x += 1.0f;
        } else {
            transform()->velocity.x += 1.0f;
            other.owner->transform()->velocity.x -= 1.0f;
        }
        //transform()->moved_velocity.x = 0.0f;

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


void entity::Frog::collide_y(aabb::Infoc our, aabb::Infoc other) {    
    if (!sprite() || !our.owner || !other.owner || !aabb::Set::at(other.id)) return;
    if (aabb::Set::at(our.id)->is_inactive() || aabb::Set::at(other.id)->is_inactive()) return;
    if (aabb::Set::at(our.id)->points().size() < 4 || aabb::Set::at(other.id)->points().size() < 4) return;

    Rectfc our_rect = { aabb::Set::at(our.id)->points().at(0).x, aabb::Set::at(our.id)->points().at(0).y,
                        aabb::Set::at(our.id)->points().at(3).x, aabb::Set::at(our.id)->points().at(3).y };

    entity::Typec other_type = other.owner->type();
    aabb::Name    other_aabb_name = aabb::Set::at(other.id)->name;
    Rectfc        other_rect = { aabb::Set::at(other.id)->points().at(0).x, aabb::Set::at(other.id)->points().at(0).y,
                                 aabb::Set::at(other.id)->points().at(3).x, aabb::Set::at(other.id)->points().at(3).y };

    f32c overlap_y = our_rect.y < other_rect.y ? our_rect.h - other_rect.y : -(other_rect.h - our_rect.y);

    Vec2fc other_velocity = other.owner->transform()->velocity;

    if (other_type == entity::Type::bug) {
        transform()->position.y -= overlap_y;
        if (our_rect.y >= other_rect.y) {
            if (other.owner->transform()) {
                transform()->velocity.x = other.owner->transform()->velocity.x + other.owner->transform()->moved_velocity.x;
                transform()->velocity.y = (other.owner->transform()->velocity.y + other.owner->transform()->moved_velocity.y) * 0.5f;
            }
            transform()->velocity.y -= 1.0f;
            hurt();

            Vec2f our_center = Vec2f{ our_rect.x + (our_rect.w - our_rect.x) / 2.0f,     our_rect.y + (our_rect.h - our_rect.y) / 2.0f };
            Vec2f other_center = Vec2f{ other_rect.x + (other_rect.w - other_rect.x) / 2.0f, other_rect.y + (other_rect.h - other_rect.y) / 2.0f };

            m_particles_to_spawn.push_back({ .type = ParticleType::hit,
                                             .position = Vec2f{ (our_center.x + other_center.x) / 2.0f, (our_center.y + other_center.y) / 2.0f },
                                             .velocity = Vec2f{ 0.0f, 0.0f } });
            return;
        }
        if (other.owner->state() == entity::State::upended || other.owner->state() == entity::State::bounce) {
            if (transform()->velocity.y < 0.0f) return;
            transform()->position.y -= overlap_y;
            transform()->velocity_limit.y = 10.0f;
            transform()->velocity.y = -5.0f - transform()->velocity.y / 4.0f;
            m_is_on_ground = false;
            m_is_on_slope = false;
            //m_time_left_bouncing = m_time_to_bounce;
            return;
        }
        transform()->moved_velocity = other_velocity;
        //transform()->velocity.x *= 0.5f;
        transform()->velocity.y = 0.0f;
        m_is_on_ground = true;
    }    
    else if (other_type == entity::Type::clip || other_type == entity::Type::clip_duck || other_type == entity::Type::clip_ledge) {
        transform()->position.y -= overlap_y;
        transform()->velocity.y = transform()->moved_velocity.y = 0.0f;
        if (our_rect.y < other_rect.y) {
            m_is_on_ground = true;
            m_is_on_slope = false;
            transform()->moved_velocity.x = 0.0f;
        }
    } else if (other_type == entity::Type::clip_U || other_type == entity::Type::slope_U) {
        if (transform()->velocity.y < 0.0f) return;
        transform()->position.y -= overlap_y;

        transform()->velocity.y = transform()->moved_velocity.y = 0.0f;
        m_is_on_ground = true;
        m_is_on_slope = other_type == entity::Type::slope_U;
    } else if (other_type == entity::Type::slope_L_2x1_0 || other_type == entity::Type::slope_L_2x1_1) {
        //if (transform()->velocity.y < 0.0f) return;
        transform()->position.y -= overlap_y;

        transform()->velocity.y = transform()->moved_velocity.y = 0.0f;        
        //if (transform()->velocity.x < 0.0f) {
        transform()->velocity.y = transform()->velocity.x / -2.0f;
        //}        
        m_is_on_ground = true;
        m_is_on_slope = true;
        if (transform()->velocity.x > transform()->velocity_limit.x * 0.9f) {
            transform()->velocity.x = transform()->velocity_limit.x * 0.9f;
        }
        transform()->moved_velocity.x = 0.0f;
    } else if (other_type == entity::Type::slope_R_1x1) {
        //if (transform()->velocity.y < 0.0f) return;
        transform()->position.y -= overlap_y;

        transform()->velocity.y = transform()->moved_velocity.y = 0.0f;
        //if (transform()->velocity.x > 0.0f) {
        transform()->velocity.y = transform()->velocity.x;
        //}
        m_is_on_ground = true;
        m_is_on_slope = true;        
        if (transform()->velocity.x < -transform()->velocity_limit.x * 0.7f) {
            transform()->velocity.x = -transform()->velocity_limit.x * 0.7f;
        }
        transform()->moved_velocity.x = 0.0f;
    } else if (other_type == entity::Type::slope_R_2x1_0 || other_type == entity::Type::slope_R_2x1_1) {
        //if (transform()->velocity.y < 0.0f) return;
        transform()->position.y -= overlap_y;

        transform()->velocity.y = transform()->moved_velocity.y = 0.0f;
        if (transform()->velocity.x > 0.0f) {
            transform()->velocity.y = transform()->velocity.x / 2.0f;
        }
        m_is_on_ground = true;
        m_is_on_slope = true;
        if (transform()->velocity.x < -transform()->velocity_limit.x * 0.9f) {
            transform()->velocity.x = -transform()->velocity_limit.x * 0.9f;
        }
        transform()->moved_velocity.x = 0.0f;
    } else if (other_type == entity::Type::player) {
        if (other_aabb_name == aabb::Name::melee_L || other_aabb_name == aabb::Name::melee_R) {
            collide_x(our, other);
            return;
        }
        if (our_rect.h <= other_rect.y) {
            transform()->position.y -= overlap_y;
            transform()->velocity.y = -4.0f;

            if (other.owner->transform()) {
                transform()->velocity.x = other.owner->transform()->velocity.x + other.owner->transform()->moved_velocity.x;
                transform()->velocity.y += (other.owner->transform()->velocity.y + other.owner->transform()->moved_velocity.y);
            }

            Vec2f our_center = Vec2f{ our_rect.x + (our_rect.w - our_rect.x) / 2.0f,     our_rect.y + (our_rect.h - our_rect.y) / 2.0f };
            Vec2f other_center = Vec2f{ other_rect.x + (other_rect.w - other_rect.x) / 2.0f, other_rect.y + (other_rect.h - other_rect.y) / 2.0f };

            m_particles_to_spawn.push_back({ .type     = ParticleType::hit,
                                             .position = Vec2f{ (our_center.x + other_center.x) / 2.0f, (our_center.y + other_center.y) / 2.0f },
                                             .velocity = Vec2f{ 0.0f, 0.0f } });
        }
    }
};