module;
#include <sstream>

export module entity.particle.melee;
import camera;
import console;
import entity;
import entity.particle;
import collider.aabb;
import sound;

export namespace entity {
    class ParticleMelee : public Particle {
    public:
        ParticleMelee() {
            m_type = Type::particle_melee;
            m_time_left_dead = 0;
        }
        void collide_x(aabb::cInfo our, aabb::cInfo other) override {
            if (other.owner == this || m_is_to_erase || is_dead() || !m_parent || !other.owner || other.owner == m_parent || other.owner->is_dead() ||
                m_parent == other.owner || m_parent->is_blocked()) return;

            //if (!aabb::Manager::get(other.id)) return;

            cRectF our_rect = { aabb::Manager::get(our.id)->points().at(0).x, aabb::Manager::get(our.id)->points().at(0).y,
                                aabb::Manager::get(our.id)->points().at(3).x, aabb::Manager::get(our.id)->points().at(3).y };
            cRectF other_rect = { aabb::Manager::get(other.id)->points().at(0).x, aabb::Manager::get(other.id)->points().at(0).y,
                                  aabb::Manager::get(other.id)->points().at(3).x, aabb::Manager::get(other.id)->points().at(3).y };

            entity::cType other_type = other.owner->type();


            //Console::log("time_left_alive: ", m_time_left_alive, "\n");

            if (entity::is_arch(other_type) || entity::is_clip(other_type) || entity::is_slope(other_type)) {
                //m_is_to_erase = true;
            } else if (other.owner->type() == entity::Type::brick) {
                m_is_to_erase = true;
                other.owner->hurt(this);
            } else if (other.owner->type() == entity::Type::bug  ||
                       other.owner->type() == entity::Type::frog ||
                       other.owner->type() == entity::Type::player) {
                Console::log("ParticleMelee::collide_x type: ", entity::to_string(other_type), "\n");
                cF32 our_w = our_rect.w - our_rect.x;
                cF32 our_h = our_rect.h - our_rect.y;

                cF32 other_w = other_rect.w - other_rect.x;
                cF32 other_h = other_rect.h - other_rect.y;

                cVec2F our_mid_point = transform()->position + Vec2F{ our_w / 2.0F, our_h / 2.0F };
                cVec2F other_mid_point = other.owner->transform()->position + Vec2F{ other_w / 2.0F, other_h / 2.0F };

                cVec2F hit_pos = Vec2F(our_mid_point + other_mid_point) / 2.0F - Vec2F{ our_w / 2.0F, our_h / 2.0F };

                other.owner->hurt(this);
                //transform()->velocity = {};
                m_time_left_alive = 0;
                m_time_left_dead = m_time_to_be_dead;
                if (m_melee_sound_id != -1 && sound::Manager::get(m_melee_sound_id)) {
                    sound::Manager::get(m_melee_sound_id)->position({ hit_pos.x / 160.0F, hit_pos.y / 90.0F });
                    sound::Manager::get(m_melee_sound_id)->play();
                }
                spawn(particle::Type::hit, hit_pos, {});
            } 
            else if (other_type == entity::Type::particle_melee) {
                if (other.owner->parent()->type() == entity::Type::frog) {
                    other.owner->parent()->next_state(entity::State::blocked);
                    m_is_to_erase = true;
                    other.owner->time_left_alive(0);
                    other.owner->time_left_dead(0);
                }
            }
            else if (other_type == entity::Type::swing) {
                if (m_parent->type() == entity::Type::player) {
                    //if (m_parent->is_on_ground()) return;
                    m_parent->next_state(entity::State::swing);

                    //cVec2F rect_size = { other_rect.w - other_rect.x, other_rect.h - other_rect.y };
                    //m_parent->swing_level_offset = Vec2F{ other_rect.x, other_rect.y } + Camera::position - rect_size / 2;

                    m_parent->parent(other.owner);

                    //cVec2F pos_diff = m_parent->transform()->position - Vec2F{ other_rect.x + rect_size.x, other_rect.y + rect_size.y };

                    /*F32 time_left_in_state = std::abs(pos_diff.y / pos_diff.x) * 2.0F;

                    if (time_left_in_state < 0.0F) time_left_in_state = 0.0F;
                    if (time_left_in_state > 2.0F) time_left_in_state = 2.0F;

                    m_parent->time_left_in_state((int)time_left_in_state);*/
                    //m_parent->rotation_speed(-m_parent->transform()->velocity.x * m_parent->transform()->velocity.y / 2.0F);
                    transform()->velocity = {};

                    m_time_left_alive = 0;
                    m_time_left_dead = m_time_to_be_dead;

                    if (m_bounce_sound_id != -1 && sound::Manager::get(m_bounce_sound_id)) {
                        sound::Manager::get(m_bounce_sound_id)->position({ position().x / 160.0F, position().y / 90.0F });
                        sound::Manager::get(m_bounce_sound_id)->play();
                    }
                }
            }
        }
        void collide_y(aabb::cInfo our, aabb::cInfo other) override {
            collide_x(our, other);
        }
    };
}