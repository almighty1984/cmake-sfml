module;
#include <cmath>
#include <sstream>
module entity.bug;
import collider.aabb;
import console;
import sound;
import particle;

void entity::Bug::collide_x(aabb::cInfo our, aabb::cInfo other) {
    if (m_parent || is_dead()) return;
    if (!sprite() || !our.owner || !other.owner || !other.owner->transform() || !aabb::Manager::get(other.id) || other.owner->is_dead()) return;
    if (aabb::Manager::get(our.id)->points().size() < 4 || aabb::Manager::get(other.id)->points().size() < 4) return;

    entity::cType other_type = other.owner->type();

    aabb::Name other_aabb_name = aabb::Manager::get(other.id)->name;

    cRectF our_rect = { aabb::Manager::get(our.id)->points().at(0).x, aabb::Manager::get(our.id)->points().at(0).y,
                        aabb::Manager::get(our.id)->points().at(3).x, aabb::Manager::get(our.id)->points().at(3).y };
    cRectF other_rect = { aabb::Manager::get(other.id)->points().at(0).x, aabb::Manager::get(other.id)->points().at(0).y,
                        aabb::Manager::get(other.id)->points().at(3).x, aabb::Manager::get(other.id)->points().at(3).y };

    cVec2F other_velocity = other.owner->transform()->velocity;

    cF32 overlap_x = our_rect.x < other_rect.x ? our_rect.w - other_rect.x : -(other_rect.w - our_rect.x);

    cF32 overlap_y = our_rect.y < other_rect.y ? our_rect.h - other_rect.y : -(other_rect.h - our_rect.y);

    if (other_type == entity::Type::bug) {
        transform()->position.x -= overlap_x;
        transform()->velocity.x *= -1.0F;
        transform()->moved_velocity.x = 0.0F;
        sprite()->is_leftward = !sprite()->is_leftward;
    } else if (other_type == entity::Type::brick) {
        if (m_state == entity::State::tossed ||
            m_state == entity::State::upended && (transform()->velocity.x < -1.0F || transform()->velocity.x > 1.0F)) {

            cVec2F hit_pos = { (our_rect.x < other_rect.x ? other_rect.x : other_rect.w) - 8.0F,
                               (our_rect.y < other_rect.y ? other_rect.y : other_rect.h) - 8.0F };

            transform()->velocity.x *= -0.9F;
            spawn(particle::Type::hit, hit_pos, transform()->velocity);
            hurt(other.owner);
            m_next_state = entity::State::hurt;
            //other.owner->next_state(entity::State::dead);
            if (m_melee_sound_id != -1 && sound::Manager::get(m_melee_sound_id)) {
                sound::Manager::get(m_melee_sound_id)->position({ hit_pos.x / 160.0F, hit_pos.y / 90.0F });
                sound::Manager::get(m_melee_sound_id)->play();
            }
            return;
        }
        transform()->position.x -= overlap_x;
        transform()->velocity.x *= -1.0F;
        transform()->moved_velocity.x = 0.0F;
        sprite()->is_leftward = !sprite()->is_leftward;
    } else if (other_type == entity::Type::clip || other_type == entity::Type::clip_duck || other_type == entity::Type::clip_ledge) {
        transform()->position.x -= overlap_x;
        transform()->moved_velocity.x = 0.0F;
        //if (m_time_left_turning > 0 || !m_is_on_ground) return;

        if (m_state == entity::State::tossed ||
            m_state == entity::State::upended && (transform()->velocity.x < -1.0F || transform()->velocity.x > 1.0F)) {

            cVec2F hit_pos = { (our_rect.x < other_rect.x ? other_rect.x : other_rect.w) - 8.0F,
                               (our_rect.y < other_rect.y ? other_rect.y : other_rect.h) - 8.0F };

            spawn(particle::Type::hit, hit_pos, {});
            hurt(other.owner);
            m_next_state = entity::State::hurt;
            if (m_melee_sound_id != -1 && sound::Manager::get(m_melee_sound_id)) {
                sound::Manager::get(m_melee_sound_id)->position({ hit_pos.x / 160.0F, hit_pos.y / 90.0F });
                sound::Manager::get(m_melee_sound_id)->play();
            }
            return;
        }        
        
        if (transform()->velocity.x > 0.0F && our_rect.x < other_rect.x) {
            transform()->velocity.x = -1.0F;
            transform()->position.x -= 2.0F;
            m_time_left_turning = 30;            
        } else if (transform()->velocity.x < 0.0F && our_rect.x > other_rect.x) {
            transform()->velocity.x = 1.0F;
            transform()->position.x += 2.0F;
            m_time_left_turning = 30;
        }
        sprite()->is_leftward = transform()->velocity.x < 0.0F;
    } else if (other_type == entity::Type::clip_L || other_type == entity::Type::clip_LD) {
        if (transform()->velocity.x + transform()->moved_velocity.x <= 0.0F) return;
        transform()->position.x -= overlap_x;
        transform()->moved_velocity.x = 0.0F;        

        if (m_state == entity::State::tossed ||
            m_state == entity::State::upended && (transform()->velocity.x > 1.0F)) {

            cVec2F hit_pos = { (our_rect.x < other_rect.x ? other_rect.x : other_rect.w) - 8.0F,
                                    (our_rect.y < other_rect.y ? other_rect.y : other_rect.h) - 8.0F };

            spawn(particle::Type::hit, hit_pos, {});
            hurt(other.owner);
            m_next_state = entity::State::hurt;
            if (m_melee_sound_id != -1 && sound::Manager::get(m_melee_sound_id)) {
                sound::Manager::get(m_melee_sound_id)->position({ hit_pos.x / 160.0F, hit_pos.y / 90.0F });
                sound::Manager::get(m_melee_sound_id)->play();
            }
            return;
        }
        transform()->velocity.x *= -1.0F;
        sprite()->is_leftward = !sprite()->is_leftward;

    } else if (other_type == entity::Type::clip_R || other_type == entity::Type::clip_RD) {
        if (transform()->velocity.x + transform()->moved_velocity.x >= 0.0F) return;

        transform()->position.x -= overlap_x;
        transform()->moved_velocity.x = 0.0F;

        if (m_state == entity::State::tossed ||
            m_state == entity::State::upended && (transform()->velocity.x < -1.0F)) {

            cVec2F hit_pos = { (our_rect.x < other_rect.x ? other_rect.x : other_rect.w) - 8.0F,
                               (our_rect.y < other_rect.y ? other_rect.y : other_rect.h) - 8.0F };
            spawn(particle::Type::hit, hit_pos, {});
            hurt(other.owner);
            m_next_state = entity::State::hurt;
            if (m_melee_sound_id != -1 && sound::Manager::get(m_melee_sound_id)) {
                sound::Manager::get(m_melee_sound_id)->position({ hit_pos.x / 160.0F, hit_pos.y / 90.0F });
                sound::Manager::get(m_melee_sound_id)->play();
            }
            return;
        }
        transform()->velocity.x *= -1.0F;
        sprite()->is_leftward = !sprite()->is_leftward;

    } else if (other_type == entity::Type::frog) {
        transform()->position.x -= overlap_x;
        if (m_state == entity::State::tossed) {
            cVec2F hit_pos = { (our_rect.x < other_rect.x ? other_rect.x : other_rect.w) - 8.0F,
                               (our_rect.y < other_rect.y ? other_rect.y : other_rect.h) - 8.0F };

            transform()->velocity.x *= -0.9F;
            spawn(particle::Type::hit, hit_pos, transform()->velocity);
            hurt(other.owner);
            other.owner->hurt(this);
            m_next_state = entity::State::hurt;
            
            //transform()->velocity.y *= -1.0f;

            if (m_melee_sound_id != -1 && sound::Manager::get(m_melee_sound_id)) {
                sound::Manager::get(m_melee_sound_id)->position({ hit_pos.x / 160.0F, hit_pos.y / 90.0F });
                sound::Manager::get(m_melee_sound_id)->play();
            }
            return;
        }

    } else if (other_type == entity::Type::particle_interact) {                
    } else if (other_type == entity::Type::player) {
        //Console::log("entity::Bug::collide_x player overlap_x: ", overlap_x, "\n");
        if (m_state == entity::State::tossed) return;
        //Console::log("aabb name: ", aabb::to_string(other_aabb_name), "\n");

        transform()->position.x -= overlap_x;
        if (m_state == entity::State::walk) {
            //transform()->velocity.x *= -1.0F;
            //transform()->velocity.x = other.owner->transform()->velocity.x;

            if (std::abs(other_velocity.x) > std::abs(transform()->velocity.x)) {
                //sprite()->is_leftward = !sprite()->is_leftward;
                Console::log("change direction ", other_velocity.x, " ", transform()->velocity.x, "\n");

                if (other_velocity.x < 0.0f) {
                    sprite()->is_leftward = true;
                    transform()->velocity.x = -2.0F;
                } else if (other_velocity.x > 0.0f) {
                    sprite()->is_leftward = false;
                    transform()->velocity.x = 2.0F;
                }

                transform()->velocity.y = -1.0F;

                other.owner->transform()->velocity.x *= -1.0f;
                
            } else {
                other.owner->transform()->velocity.x += (transform()->velocity.x * 4.0F);
                other.owner->transform()->velocity.y -= 2.0f;
            }
        }
    } else if (other_type == entity::Type::slope_L_1x1) {
        //if (our_rect.h > other_rect.h) return;

        //transform()->velocity.y = -transform()->velocity.x;
        transform()->position.y -= transform()->velocity.x;
        /*if (transform()->velocity.x > transform()->max_velocity.x * 0.7F) {
            transform()->velocity.x = transform()->max_velocity.x * 0.7F;
        }*/
        m_is_on_ground = true;
        m_is_on_slope = true;
    } else if (other_type == entity::Type::slope_R_1x1) {
        //if (our_rect.h > other_rect.h) return;

        //transform()->velocity.y = transform()->velocity.x;
        transform()->position.y += transform()->velocity.x;
        /*if (transform()->velocity.x < -transform()->max_velocity.x * 0.7F) {
            transform()->velocity.x = -transform()->max_velocity.x * 0.7F;
        }*/
        m_is_on_ground = true;
        m_is_on_slope = true;
    } else if (other_type == entity::Type::slope_L_2x1_0 || other_type == entity::Type::slope_L_2x1_1) {
        return;
        //if (our_rect.h > other_rect.h) return;

        transform()->position.y -= overlap_y;
        transform()->velocity.y = -transform()->velocity.x / 2.0F;
        //transform()->position.y -= transform()->velocity.x / 2.0F;

        /*if (transform()->velocity.x > transform()->max_velocity.x * 0.9F) {
            transform()->velocity.x = transform()->max_velocity.x * 0.9F;
        }*/
        m_is_on_ground = true;
        m_is_on_slope = true;

    } else if (other_type == entity::Type::slope_R_2x1_0 || other_type == entity::Type::slope_R_2x1_1) {
        return;

        //if (our_rect.h > other_rect.h) return;

        transform()->position.y -= overlap_y;
        transform()->velocity.y = transform()->velocity.x / 2.0F;
        //transform()->position.y += transform()->velocity.x / 2.0F;

        /*if (transform()->velocity.x < -transform()->max_velocity.x * 0.9F) {
            transform()->velocity.x = -transform()->max_velocity.x * 0.9F;
        }*/
        m_is_on_ground = true;
        m_is_on_slope = true;

    } else if (other_type == entity::Type::water_line) {
        if (other.owner->transform())
            m_water_line_y = other.owner->transform()->start_position.y;

        if (transform()->velocity.y > 0.0F && m_state != entity::State::swim) {
            m_next_state = entity::State::swim;
        }
        if (m_state == entity::State::swim) {  
            transform()->velocity.y = other_velocity.y * 0.9F + transform()->velocity.y * 0.1F;
        }
    }
};




void entity::Bug::collide_y(aabb::cInfo our, aabb::cInfo other) {    
    if (m_parent || is_dead()) return;
    if (!sprite() || !our.owner || !other.owner || !other.owner->transform() || !aabb::Manager::get(other.id) || other.owner->is_dead()) return;
    if (aabb::Manager::get(our.id)->points().size() < 4 || aabb::Manager::get(other.id)->points().size() < 4) return;

    cRectF our_rect = { aabb::Manager::get(our.id)->points().at(0).x, aabb::Manager::get(our.id)->points().at(0).y,
                        aabb::Manager::get(our.id)->points().at(3).x, aabb::Manager::get(our.id)->points().at(3).y };

    entity::cType other_type = other.owner->type();
    aabb::Name    other_aabb_name = aabb::Manager::get(other.id)->name;    
    cRectF        other_rect = { aabb::Manager::get(other.id)->points().at(0).x, aabb::Manager::get(other.id)->points().at(0).y,
                                 aabb::Manager::get(other.id)->points().at(3).x, aabb::Manager::get(other.id)->points().at(3).y };

    cF32 overlap_y = our_rect.y < other_rect.y ? our_rect.h - other_rect.y : -(other_rect.h - our_rect.y);

    cVec2F our_velocity = transform()->velocity;
    cVec2F other_velocity = other.owner->transform()->velocity;

    //Console::log("type: ", entity::to_string(other_type), "\n");

    if (other_type == entity::Type::clip || other_type == entity::Type::clip_duck || other_type == entity::Type::clip_ledge) {
        transform()->position.y -= overlap_y;
        transform()->velocity.y = transform()->moved_velocity.y = 0.0F;
        if (our_rect.y < other_rect.y) {
            m_is_on_ground = true;
            m_is_on_slope = false;
        }

        //if (!m_is_on_ground) return;
        if (other_type == entity::Type::clip_ledge) {
            //Console::log("our: ", our_rect.y, " ", our_rect.h, "\n");
            if (our_rect.h > other_rect.y + transform()->acceleration.y) return;

            if (m_time_left_turning > 0 || m_state == entity::State::upended || !m_is_on_ground) return;
            if (transform()->velocity.x > 0.0F && our_rect.x > other_rect.x) {
                if (our_rect.x + our_rect.w > other_rect.x + other_rect.w) {
                    transform()->velocity.x = -1.0F;
                    transform()->position.x -= 2.0F;
                    m_time_left_turning = 30;
                }
            } else if (transform()->velocity.x < 0.0F && our_rect.x < other_rect.x) {
                if (our_rect.x < other_rect.x) {
                    transform()->velocity.x = 1.0F;
                    transform()->position.x += 2.0F;
                    m_time_left_turning = 30;
                }
            }
            sprite()->is_leftward = transform()->velocity.x < 0.0F;
        }
    } else if (other_type == entity::Type::clip_U || other_type == entity::Type::slope_U) {
        if (transform()->velocity.y < 0.0F) return;
        transform()->position.y -= overlap_y;

        transform()->velocity.y = transform()->moved_velocity.y = 0.0F;
        m_is_on_ground = true;
        m_is_on_slope = other_type == entity::Type::slope_U;
    } else if (other_type == entity::Type::clip_D || other_type == entity::Type::clip_LD || other_type == entity::Type::clip_RD) {
        if (transform()->velocity.y > 0.0F) return;
        transform()->position.y -= overlap_y;
        m_is_on_ground = false;
        m_is_on_slope = false;
        transform()->velocity.y = std::abs(transform()->velocity.y) * 0.75F;
        
        spawn(particle::Type::hit, { other_rect.x + 8.0f, other_rect.h }, {});

        m_next_state = entity::State::hurt;
    } else if (other_type == entity::Type::frog) {
        Console::log("entity::Bug::collide_y frog\n");
        if (m_state == entity::State::tossed || (m_state == entity::State::upended && our_rect.y < other_rect.y)) {
            cVec2F hit_pos = { (our_rect.x < other_rect.x ? other_rect.x : other_rect.w) - 8.0F,
                               (our_rect.y < other_rect.y ? other_rect.y : other_rect.h) - 8.0F };
            spawn(particle::Type::hit, hit_pos, {});
            hurt(other.owner);
            m_next_state = entity::State::hurt;
            other.owner->hurt(this);
            //transform()->velocity.x *= -1.0f;
            transform()->velocity.y *= -1.0f;
            if (m_melee_sound_id != -1 && sound::Manager::get(m_melee_sound_id)) {
                sound::Manager::get(m_melee_sound_id)->position({ hit_pos.x / 160.0F, hit_pos.y / 90.0F });
                sound::Manager::get(m_melee_sound_id)->play();
            }
            return;
        }        

        if (our_rect.y < other_rect.h) {
            transform()->position.y -= overlap_y;
            if (m_state == entity::State::upended) {
                m_next_state = entity::State::bounce;
            }
        }
        //if (other_rect.y > our_rect.y) {            
            //if (!is_hurting()) {
            //    transform()->velocity.y = other.owner->transform()->velocity.y + transform()->velocity.y - 10.0f;
            //}
        //} else {
            //transform()->position.y -= overlap_y;            
        //}
    } else if (other_type == entity::Type::particle_interact) {
        collide_x(our, other);
    } else if (other_type == entity::Type::slope_L_1x1) {
        //if (transform()->velocity.y < 0.0F) return;
        transform()->position.y -= overlap_y;

        transform()->velocity.y = -transform()->velocity.x;
        transform()->moved_velocity.x = 0.0F;
        transform()->moved_velocity.y = 0.0F;

        m_is_on_ground = true;
        m_is_on_slope = true;
    } else if (other_type == entity::Type::slope_R_1x1) {
        //if (transform()->velocity.y < 0.0F) return;
        transform()->position.y -= overlap_y;

        transform()->velocity.y = transform()->velocity.x;
        transform()->moved_velocity.x = 0.0F;
        transform()->moved_velocity.y = 0.0F;
        
        m_is_on_ground = true;
        m_is_on_slope = true;        
    } else if (other_type == entity::Type::slope_L_2x1_0 || other_type == entity::Type::slope_L_2x1_1) {
        //if (transform()->velocity.y < 0.0F) return;
        transform()->position.y -= overlap_y;

        //transform()->velocity.y = 0.0F;
        transform()->moved_velocity.y = 0.0F;

        //if (transform()->velocity.x < 0.0F) {
        transform()->velocity.y = transform()->velocity.x / -2.0F;
        //}
        m_is_on_ground = true;
        m_is_on_slope = true;
        /*if (transform()->velocity.x > transform()->max_velocity.x * 0.9F) {
            transform()->velocity.x = transform()->max_velocity.x * 0.9F;
        }*/
    } else if (other_type == entity::Type::slope_R_2x1_0 || other_type == entity::Type::slope_R_2x1_1) {
        //if (transform()->velocity.y < 0.0F) return;
        transform()->position.y -= overlap_y;

        transform()->velocity.y = 0.0F;
        transform()->moved_velocity.y = 0.0F;
        if (transform()->velocity.x > 0.0F) {
            transform()->velocity.y = transform()->velocity.x / 2.0F;
        }
        m_is_on_ground = true;
        m_is_on_slope = true;
        /*if (transform()->velocity.x < -transform()->max_velocity.x * 0.9F) {
            transform()->velocity.x = -transform()->max_velocity.x * 0.9F;
        }*/
    } else if (other_type == entity::Type::player) {
        //Console::log("entity::Bug::collide_y player overlap_y: ", overlap_y, "\n");
        if (m_state == entity::State::swim) {
            transform()->velocity.y += other_velocity.y;
            return;
        }
        if (our_rect.y > other_rect.h - 2.0F/* - other_velocity.y*/) {            
            if (m_state == entity::State::upended || m_state == entity::State::bounce) {
                transform()->position.y -= overlap_y;
                m_state = entity::State::upended;
                m_next_state = entity::State::bounce;
            } else {
                //other.owner->is_on_ground(true);
                //other.owner->transform()->velocity.y = 0.0F;
                //other.owner->transform()->moved_velocity = our_velocity;
            }
        }
        else if (our_rect.h <= other_rect.y + 4.0F) {

            /*Vec2F vel = transform()->velocity + transform()->moved_velocity;
            transform()->velocity.x = (other.owner->transform()->velocity.x + other.owner->transform()->moved_velocity.x) / 1.0F;
            transform()->velocity.y = -6.0F;

            cF32 v = other.owner->transform()->velocity.y;

            Console::log("entity::Bug::collide_y vel y: ", v, "\n");*/
            //transform()->position.y -= 8.0F;
            //transform()->position.y -= overlap_y;
            //other.owner->transform()->velocity = vel / 2.0F;

            //other.owner->hurt();

            cVec2F our_center = Vec2F{ our_rect.x + (our_rect.w - our_rect.x) / 2.0F,     our_rect.y + (our_rect.h - our_rect.y) / 2.0F };
            cVec2F other_center = Vec2F{ other_rect.x + (other_rect.w - other_rect.x) / 2.0F, other_rect.y + (other_rect.h - other_rect.y) / 2.0F };
            
            cVec2F hit_pos = Vec2F{ our_center.x + other_center.x, our_center.y + other_center.y } / 2.0F - Vec2F{ 8.0F, 8.0F };

            spawn(particle::Type::hit, hit_pos, {});
            
            if (m_melee_sound_id != -1 && sound::Manager::get(m_melee_sound_id)) {
                sound::Manager::get(m_melee_sound_id)->position({ hit_pos.x / 160.0F, hit_pos.y / 90.0F });
                sound::Manager::get(m_melee_sound_id)->play();
            }
        }
    } else if (other_type == entity::Type::water_line) {
        if (other.owner->transform())
            m_water_line_y = other.owner->transform()->start_position.y;

        if (transform()->velocity.y > 0.0F && m_state != entity::State::swim) {
            m_next_state = entity::State::swim;
        }
        if (m_state == entity::State::swim) {
            if (transform()->position.y > m_water_line_y) {
                transform()->velocity.y = other_velocity.y * 0.5F + transform()->velocity.y * 0.5F;
            }
        }
    }
};