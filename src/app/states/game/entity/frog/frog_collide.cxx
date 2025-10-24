module;
#include <utility>
module entity.frog;
import collider.aabb;
import console;
import sound;
import particle;
import random;

void entity::Frog::collide_x(aabb::cInfo our, aabb::cInfo other) {
    if (!sprite() || !our.owner || !other.owner || !aabb::Manager::get(our.id) || !aabb::Manager::get(other.id)) return;
    if (!aabb::Manager::get(our.id)->is_active() || !aabb::Manager::get(other.id)->is_active()) return;
    if (aabb::Manager::get(our.id)->points().size() < 4 || aabb::Manager::get(other.id)->points().size() < 4) return;

    entity::cType other_type = other.owner->type();

    aabb::cName other_aabb_name = aabb::Manager::get(other.id)->name;

    cRectF our_rect   = { aabb::Manager::get(our.id)->points().at(0).x,   aabb::Manager::get(our.id)->points().at(0).y,
                          aabb::Manager::get(our.id)->points().at(3).x,   aabb::Manager::get(our.id)->points().at(3).y };
    cRectF other_rect = { aabb::Manager::get(other.id)->points().at(0).x, aabb::Manager::get(other.id)->points().at(0).y,
                          aabb::Manager::get(other.id)->points().at(3).x, aabb::Manager::get(other.id)->points().at(3).y };

    Vec2F our_dimensions = { aabb::Manager::get(our.id)->points().at(1).x - aabb::Manager::get(our.id)->points().at(0).x,
                             aabb::Manager::get(our.id)->points().at(3).y - aabb::Manager::get(our.id)->points().at(1).y };

    aabb::cName our_aabb_name = aabb::Manager::get(our.id)->name;
    
    cF32 overlap_x = our_rect.x < other_rect.x ? our_rect.w - other_rect.x : -(other_rect.w - our_rect.x);

    cVec2F our_velocity = transform()->velocity;
    cVec2F other_velocity = other.owner->transform()->velocity;

    if (other_type == entity::Type::brick) {
        Console::log("entity::Frog::collide_x brick\n");
    } else if (other_type == entity::Type::bug) {
        transform()->position.x -= overlap_x;        
        if (other.owner->state() == entity::State::tossed) {
            return;
        }
        
        transform()->velocity.x = other_velocity.x * 0.5F;
        other.owner->transform()->velocity.x = our_velocity.x * 0.25F;

        if (our_rect.x < other_rect.x) {
            //transform()->velocity.x -= 1.0F;
            other.owner->transform()->velocity.x += 1.0F;
            other.owner->sprite()->is_leftward = false;
        } else {
            //transform()->velocity.x += 1.0F;
            other.owner->transform()->velocity.x -= 1.0F;
            other.owner->sprite()->is_leftward = true;
        }
    } else if (other_type == entity::Type::clip || other_type == entity::Type::clip_duck || other_type == entity::Type::clip_ledge) {
        transform()->position.x -= overlap_x;
        if (m_state == entity::State::swim) {
            if (transform()->velocity.x < 0.0F) {
                sprite()->is_leftward = false;
                transform()->velocity.x *= -1.0F;

                transform()->position.x += 1.0F;
            } else if (transform()->velocity.x > 0.0F) {
                sprite()->is_leftward = true;
                transform()->velocity.x *= -1.0F;

                transform()->position.x -= 1.0F;
            }
            
        }
        transform()->velocity.x = 0.0F;
        transform()->moved_velocity.x = 0.0F;        
    } else if (other_type == entity::Type::clip_L || other_type == entity::Type::clip_LD) {
        if (transform()->velocity.x + transform()->moved_velocity.x <= 0.0F) return;
        if (m_state == entity::State::swim) {
            sprite()->is_leftward = true;
        }
        transform()->position.x -= overlap_x;
        transform()->velocity.x = 0.0F;
        transform()->moved_velocity.x = 0.0F;
    } else if (other_type == entity::Type::clip_R || other_type == entity::Type::clip_RD) {
        if (transform()->velocity.x + transform()->moved_velocity.x >= 0.0F) return;
        if (m_state == entity::State::swim) {
            sprite()->is_leftward = false;
        }
        transform()->position.x -= overlap_x;
        transform()->velocity.x = 0.0F;
        transform()->moved_velocity.x = 0.0F;
    } else if (other_type == entity::Type::frog) {
        transform()->position.x -= overlap_x;
        if (std::abs(transform()->velocity.x) > std::abs(other.owner->transform()->velocity.x)) {
            return;
        }
        transform()->velocity.x = other_velocity.x * 0.5F;
        other.owner->transform()->velocity.x = our_velocity.x * 0.25f;
        if (our_rect.x < other_rect.x) {
            transform()->velocity.x -= 1.0F;
            other.owner->transform()->velocity.x += 1.0F;
        } else {
            transform()->velocity.x += 1.0F;
            other.owner->transform()->velocity.x -= 1.0F;
        }
    } else if (other_type == entity::Type::particle_melee) {
        
    } else if (other_type == entity::Type::player) {
        transform()->position.x -= overlap_x;

        /*if (std::abs(transform()->velocity.x) > std::abs(other.owner->transform()->velocity.x)) {
            return;
        }*/

        //if (m_state == entity::State::swim) return;

        cF32 force_x = other.owner->is_ducking() ? 4.0F : 1.0F;

        transform()->velocity.x = other_velocity.x * 0.5F;
        other.owner->transform()->velocity.x = our_velocity.x * 0.25f;
        if (our_rect.x < other_rect.x) {
            transform()->velocity.x -= 1.0F;
            other.owner->transform()->velocity.x += force_x;
        } else {
            transform()->velocity.x += 1.0F;
            other.owner->transform()->velocity.x -= force_x;
        }
        transform()->moved_velocity.x = 0.0F;

    } else if (other_type == entity::Type::slope_L_1x1) {
        transform()->velocity.y = -transform()->velocity.x;
        if (transform()->velocity.x > transform()->max_velocity.x * 0.7F) {
            transform()->velocity.x = transform()->max_velocity.x * 0.7F;
        }
        m_is_on_ground = true;
        m_is_on_slope = true;
    } else if (other_type == entity::Type::slope_L_2x1_0 || other_type == entity::Type::slope_L_2x1_1) {
        transform()->velocity.y = -transform()->velocity.x / 2.0F;
        if (transform()->velocity.x > transform()->max_velocity.x * 0.9F) {
            transform()->velocity.x = transform()->max_velocity.x * 0.9F;
        }
        m_is_on_ground = true;
        m_is_on_slope = true;
    } else if (other_type == entity::Type::slope_R_1x1) {
        transform()->velocity.y = transform()->velocity.x;
        if (transform()->velocity.x < -transform()->max_velocity.x * 0.7F) {
            transform()->velocity.x = -transform()->max_velocity.x * 0.7F;
        }
        m_is_on_ground = true;
        m_is_on_slope = true;
    } else if (other_type == entity::Type::slope_R_2x1_0 || other_type == entity::Type::slope_R_2x1_1) {
        transform()->velocity.y = transform()->velocity.x / 2.0F;
        if (transform()->velocity.x < -transform()->max_velocity.x * 0.9F) {
            transform()->velocity.x = -transform()->max_velocity.x * 0.9F;
        }
        m_is_on_ground = true;
        m_is_on_slope = true;
    }
};




void entity::Frog::collide_y(aabb::cInfo our, aabb::cInfo other) {    
    if (!sprite() || !our.owner || !other.owner || !aabb::Manager::get(other.id)) return;
    
    if (aabb::Manager::get(our.id)->points().size() < 4 || aabb::Manager::get(other.id)->points().size() < 4) return;
    
    aabb::cName our_aabb_name = aabb::Manager::get(our.id)->name;    

    cRectF our_rect = { aabb::Manager::get(our.id)->points().at(0).x, aabb::Manager::get(our.id)->points().at(0).y,
                        aabb::Manager::get(our.id)->points().at(3).x, aabb::Manager::get(our.id)->points().at(3).y };

    entity::cType other_type = other.owner->type();
    aabb::cName   other_aabb_name = aabb::Manager::get(other.id)->name;
    cRectF        other_rect = { aabb::Manager::get(other.id)->points().at(0).x, aabb::Manager::get(other.id)->points().at(0).y,
                                 aabb::Manager::get(other.id)->points().at(3).x, aabb::Manager::get(other.id)->points().at(3).y };

    cF32 overlap_y = our_rect.y < other_rect.y ? our_rect.h - other_rect.y : -(other_rect.h - our_rect.y);

    cVec2F other_velocity = other.owner->transform()->velocity;
    cVec2F our_velocity = transform()->velocity;

    if (m_state == entity::State::swim) {
        if (position_on_level().y < m_water_line_y) {
            if (entity::is_clip(other_type) || entity::is_slope(other_type)) {
                m_next_state = entity::State::idle;
                m_water_line_y = 0;
            }
        }
    }
    if (other_type == entity::Type::brick) {
        Console::log("entity::Frog::collide_y brick\n");
    }
    else if (other_type == entity::Type::bug) {
        Console::log("entity::Frog::collide_y bug\n");
        if (other.owner->state() == entity::State::tossed) {
            return;
        }
        transform()->position.y -= overlap_y;

        if (our_rect.y > other_rect.y) {       
            hurt(other.owner);
            m_next_state = entity::State::hurt;
            /*Vec2F our_center = Vec2F{ our_rect.x + (our_rect.w - our_rect.x) / 2.0F,     our_rect.y + (our_rect.h - our_rect.y) / 2.0F };
            Vec2F other_center = Vec2F{ other_rect.x + (other_rect.w - other_rect.x) / 2.0F, other_rect.y + (other_rect.h - other_rect.y) / 2.0F };
            
            spawn(particle::Type::hit, { (our_center.x + other_center.x) / 2.0F, (our_center.y + other_center.y) / 2.0F }, {});
            hurt(other_type);*/
            return;
        }
        if (other.owner->state() == entity::State::upended || other.owner->state() == entity::State::bounce) {
            if (transform()->velocity.y < 0.0F) return;
            transform()->position.y -= overlap_y;
            transform()->max_velocity.y = 10.0F;
            transform()->velocity.y = -5.0F - transform()->velocity.y / 4.0F;
            m_is_on_ground = false;
            m_is_on_slope = false;
            //m_time_left_bouncing = m_time_to_bounce;
            return;
        }
        transform()->moved_velocity = other_velocity;
        //transform()->velocity.x *= 0.5F;
        transform()->velocity.y = 0.0F;
        m_is_on_ground = true;
    }    
    else if (other_type == entity::Type::clip || other_type == entity::Type::clip_duck || other_type == entity::Type::clip_ledge) {
        transform()->position.y -= overlap_y;
        transform()->velocity.y = transform()->moved_velocity.y = 0.0F;
        if (our_rect.y < other_rect.y) {
            m_is_on_ground = true;
            m_is_on_slope = false;
            transform()->moved_velocity.x = 0.0F;
        }
    } else if (other_type == entity::Type::clip_U || other_type == entity::Type::slope_U) {
        if (transform()->velocity.y < 0.0F) return;
        transform()->position.y -= overlap_y;

        transform()->velocity.y = transform()->moved_velocity.y = 0.0F;
        transform()->moved_velocity.x = 0.0F;
        m_is_on_ground = true;
        m_is_on_slope = other_type == entity::Type::slope_U;
    } else if (other_type == entity::Type::clip_D || other_type == entity::Type::clip_LD || other_type == entity::Type::clip_RD) {
        if (transform()->velocity.y > 0.0F) return;
        transform()->position.y -= overlap_y;
        m_is_on_ground = false;
        m_is_on_slope = false;
        transform()->velocity.y = std::abs(transform()->velocity.y) * 0.75F;

        spawn(particle::Type::hit, { other_rect.x + 8.0F, other_rect.h }, {});
        //, m_melee_sound_id
    } else if (other_type == entity::Type::frog) {
        if (our_rect.h <= other_rect.y) {
            transform()->position.y -= overlap_y;
            transform()->velocity.y = -4.0F;
            if (other.owner->transform()) {
                transform()->velocity.x = other.owner->transform()->velocity.x + other.owner->transform()->moved_velocity.x;
                transform()->velocity.y += (other.owner->transform()->velocity.y + other.owner->transform()->moved_velocity.y);
            }

            Vec2F our_center = Vec2F{ our_rect.x + (our_rect.w - our_rect.x) / 2.0F,     our_rect.y + (our_rect.h - our_rect.y) / 2.0F };
            Vec2F other_center = Vec2F{ other_rect.x + (other_rect.w - other_rect.x) / 2.0F, other_rect.y + (other_rect.h - other_rect.y) / 2.0F };

            spawn(particle::Type::hit, { (our_center.x + other_center.x) / 2.0F, (our_center.y + other_center.y) / 2.0F }, {});
            //, m_melee_sound_id
        }
    } else if (other_type == entity::Type::slope_L_2x1_0 || other_type == entity::Type::slope_L_2x1_1) {
        //if (transform()->velocity.y < 0.0F) return;
        transform()->position.y -= overlap_y;

        transform()->velocity.y = transform()->moved_velocity.y = 0.0F;        
        //if (transform()->velocity.x < 0.0F) {
        transform()->velocity.y = transform()->velocity.x / -2.0F;
        //}        
        m_is_on_ground = true;
        m_is_on_slope = true;
        if (transform()->velocity.x > transform()->max_velocity.x * 0.9F) {
            transform()->velocity.x = transform()->max_velocity.x * 0.9F;
        }
        transform()->moved_velocity.x = 0.0F;
    } else if (other_type == entity::Type::slope_R_1x1) {
        //if (transform()->velocity.y < 0.0F) return;
        transform()->position.y -= overlap_y;

        transform()->velocity.y = transform()->moved_velocity.y = 0.0F;
        //if (transform()->velocity.x > 0.0F) {
        transform()->velocity.y = transform()->velocity.x;
        //}
        m_is_on_ground = true;
        m_is_on_slope = true;        
        if (transform()->velocity.x < -transform()->max_velocity.x * 0.7F) {
            transform()->velocity.x = -transform()->max_velocity.x * 0.7F;
        }
        transform()->moved_velocity.x = 0.0F;
    } else if (other_type == entity::Type::slope_R_2x1_0 || other_type == entity::Type::slope_R_2x1_1) {
        //if (transform()->velocity.y < 0.0F) return;
        transform()->position.y -= overlap_y;

        transform()->velocity.y = transform()->moved_velocity.y = 0.0F;
        if (transform()->velocity.x > 0.0F) {
            transform()->velocity.y = transform()->velocity.x / 2.0F;
        }
        m_is_on_ground = true;
        m_is_on_slope = true;
        if (transform()->velocity.x < -transform()->max_velocity.x * 0.9F) {
            transform()->velocity.x = -transform()->max_velocity.x * 0.9F;
        }
        transform()->moved_velocity.x = 0.0F;
    } else if (other_type == entity::Type::player) {        
        if (our_rect.y <= other_rect.y) {

            transform()->position.y -= overlap_y;

            cI32 rand_num = random::Generator::number(-1, 1);

            //Console::log("rand_num: ", rand_num, "\n");

            transform()->velocity.x = other_velocity.x * 0.5F + our_velocity.x * 0.5F + (F32)rand_num;
            transform()->velocity.y = other_velocity.y * 0.5F - 3.0F + (F32)rand_num;

            /*if (other.owner->transform()) {
                transform()->velocity.x = other.owner->transform()->velocity.x + other.owner->transform()->moved_velocity.x;
                transform()->velocity.y += (other.owner->transform()->velocity.y + other.owner->transform()->moved_velocity.y);
            }*/            

            if (other_velocity.y < 0.0F) {
                hurt(other.owner);
                m_next_state = entity::State::hurt;
            }

            //cVec2F our_center = Vec2F{ our_rect.x + (our_rect.w - our_rect.x) / 2.0F,     our_rect.y + (our_rect.h - our_rect.y) / 2.0F };
            cVec2F other_center = Vec2F{ other_rect.x + (other_rect.w - other_rect.x) / 2.0F, other_rect.y + (other_rect.h - other_rect.y) / 2.0F };

            cVec2F hit_pos = { other_center.x - 8.0F, other_rect.y - 12.0F };

            spawn(particle::Type::hit, hit_pos, {});

            other.owner->transform()->velocity.y = -other.owner->transform()->velocity.y * 0.9F;
            
            if (m_bump_head_sound_id != -1 && sound::Manager::get(m_bump_head_sound_id)) {
                sound::Manager::get(m_bump_head_sound_id)->position({ hit_pos.x / 160.0F, hit_pos.y / 90.0F });
                sound::Manager::get(m_bump_head_sound_id)->play();
            }

            //next_state(entity::State::hurt);
            //m_time_left_in_state = 200;
        } else {
            if (other.owner->transform()->velocity.y >= 5.0F) {
                cVec2F player_vel = other.owner->transform()->velocity;

                other.owner->transform()->velocity.y *= -0.7F;
                other.owner->transform()->velocity.x = transform()->velocity.x;
                other.owner->transform()->moved_velocity = 0.0F;

                transform()->velocity = player_vel * 0.5F;

                m_next_state = entity::State::stunned;

                cF32 our_w = our_rect.w - our_rect.x;
                cF32 our_h = our_rect.h - our_rect.y;
                cF32 other_w = other_rect.w - other_rect.x;
                cF32 other_h = other_rect.h - other_rect.y;
                cVec2F our_mid_point = transform()->position + Vec2F{ our_w / 2.0F, our_h / 2.0F };
                cVec2F other_mid_point = other.owner->transform()->position + Vec2F{ other_w / 2.0F, other_h / 2.0F };

                cVec2F hit_pos = Vec2F(our_mid_point + other_mid_point) / 2.0F - Vec2F{ our_w / 2.0F, our_h / 2.0F };

                if (m_bounce_sound_id != -1 && sound::Manager::get(m_bounce_sound_id)) {
                    sound::Manager::get(m_bounce_sound_id)->position({ hit_pos.x / 160.0F, hit_pos.y / 90.0F });
                    sound::Manager::get(m_bounce_sound_id)->play();
                }

                spawn(particle::Type::hit, hit_pos, {});
            }
        }
    } else if (other_type == entity::Type::water_line) {
        if (transform()->velocity.y > 0.0F && m_state != entity::State::swim) {
            m_next_state = entity::State::swim;
            if (other.owner->transform())
                m_water_line_y = other.owner->transform()->start_position.y;

        }
        if (m_state == entity::State::swim) {
            if (transform()->position.y > m_water_line_y) {
                transform()->velocity.y = other_velocity.y;
            }
        }
    }
};