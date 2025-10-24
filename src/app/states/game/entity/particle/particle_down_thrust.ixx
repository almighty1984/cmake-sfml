module;
#include <sstream>

export module entity.particle.down_thrust;
import camera;
import console;
import entity;
import entity.particle;
import collider.aabb;
import sound;

export namespace entity {
    class ParticleDownThrust : public Particle {
    public:
        ParticleDownThrust() {
            m_type = Type::particle_down_thrust;
            m_time_left_dead = 0;
        }
        void collide_x(aabb::cInfo our, aabb::cInfo other) override {
            if (is_dead() || !our.owner || !other.owner || our.owner == other.owner || !m_parent || m_parent == other.owner || other.owner->is_dead()) return;

            //if (!aabb::Manager::get(other.id)) return;

            cRectF our_rect = { aabb::Manager::get(our.id)->points().at(0).x, aabb::Manager::get(our.id)->points().at(0).y,
                                aabb::Manager::get(our.id)->points().at(3).x, aabb::Manager::get(our.id)->points().at(3).y };
            cRectF other_rect = { aabb::Manager::get(other.id)->points().at(0).x, aabb::Manager::get(other.id)->points().at(0).y,
                                  aabb::Manager::get(other.id)->points().at(3).x, aabb::Manager::get(other.id)->points().at(3).y };

            entity::cType other_type = other.owner->type();

            if (other_type == Type::particle_down_thrust) return;

            //Console::log("type: ", entity::to_string(other_type), "\n");

            if (entity::is_arch(other_type) || entity::is_clip(other_type) || entity::is_slope(other_type)) {
                //m_is_to_erase = true;
            } else if (other.owner->type() == entity::Type::particle_sense) {
                
            } else if (other.owner->type() == entity::Type::bug ||
                       other.owner->type() == entity::Type::frog) {
                
                cF32 our_w = our_rect.w - our_rect.x;
                cF32 our_h = our_rect.h - our_rect.y;

                cF32 other_w = other_rect.w - other_rect.x;
                cF32 other_h = other_rect.h - other_rect.y;

                cVec2F our_mid_point = transform()->position + Vec2F{ our_w / 2.0F, our_h / 2.0F };
                cVec2F other_mid_point = other.owner->transform()->position + Vec2F{ other_w / 2.0F, other_h / 2.0F };

                cVec2F hit_pos = Vec2F(our_mid_point + other_mid_point) / 2.0F - Vec2F{ our_w / 2.0F, our_h / 2.0F };

                other.owner->hurt(this);
                transform()->velocity = {};
                m_time_left_alive = 0;
                m_time_left_dead = m_time_to_be_dead;
                if (m_melee_sound_id != -1 && sound::Manager::get(m_melee_sound_id)) {
                    sound::Manager::get(m_melee_sound_id)->position({ hit_pos.x / 160.0F, hit_pos.y / 90.0F });
                    sound::Manager::get(m_melee_sound_id)->play();
                }
                spawn(particle::Type::hit, hit_pos, {});
            } else if (other_type == entity::Type::swing) {                
            }
        }
        void collide_y(aabb::cInfo our, aabb::cInfo other) override {
            collide_x(our, other);
        }
    };
}