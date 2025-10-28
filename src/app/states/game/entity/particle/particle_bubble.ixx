module;
#include <cmath>

export module entity.particle.bubble;
import console;
import entity.particle;
import collider.aabb;

export namespace entity {
    class ParticleBubble : public Particle {
    public:
        ParticleBubble() {
            m_type = Type::particle_bubble;
            m_next_state = entity::State::idle;
        }
        void update_idle() override {
            //Console::log("entity::ParticleBubble::update_idle direction: ", m_direction.x, "\n");
            ++m_time_in_state;
            if (m_time_in_state < 10) return;

            if (m_direction.x == 0.0F) {
                transform()->velocity.x -= transform()->acceleration.x;
            } else {
                transform()->velocity.x += transform()->acceleration.x;
            }
            if (transform()->velocity.x < -0.2F) {
                m_direction.x = 1.0F;
            }
            else if (transform()->velocity.x > 0.2F) {
                m_direction.x = 0.0F;
            }
        }
        void collide_x(aabb::cInfo our, aabb::cInfo other) override {
            if (!other.owner || !other.owner->transform()) return;
            //collide_y(our, other);
            cRectF our_rect = { aabb::Manager::get(our.id)->points().at(0).x, aabb::Manager::get(our.id)->points().at(0).y,
                        aabb::Manager::get(our.id)->points().at(3).x, aabb::Manager::get(our.id)->points().at(3).y };

            entity::cType other_type = other.owner->type();
            aabb::cName   other_aabb_name = aabb::Manager::get(other.id)->name;
            cRectF        other_rect = { aabb::Manager::get(other.id)->points().at(0).x, aabb::Manager::get(other.id)->points().at(0).y,
                                         aabb::Manager::get(other.id)->points().at(3).x, aabb::Manager::get(other.id)->points().at(3).y };

            cF32 overlap_x = our_rect.x < other_rect.x ? our_rect.w - other_rect.x : -(other_rect.w - our_rect.x);

            if (entity::is_clip(other_type)) {
                m_time_in_state = 0;
                transform()->position.x -= overlap_x;
                transform()->velocity.x = 0.0F;
            } else if (other_type == entity::Type::particle_bubble) {
                transform()->position.x -= overlap_x;
                //cF32 vel_x = transform()->velocity.x;
                //transform()->velocity.x = (other.owner->velocity().x + vel_x) / 2.0F;
                //other.owner->transform()->velocity.x = (other.owner->transform()->velocity.x + vel_x) / 2.0F;
                if (std::abs(transform()->velocity.x) > std::abs(other.owner->transform()->velocity.x)) {
                    other.owner->transform()->velocity.x = transform()->velocity.x;
                } else {
                    transform()->velocity.x = other.owner->transform()->velocity.x;
                }
            }
        }
        void collide_y(aabb::cInfo our, aabb::cInfo other) override {
            if (!other.owner || !other.owner->transform()) return;

            cRectF our_rect = { aabb::Manager::get(our.id)->points().at(0).x, aabb::Manager::get(our.id)->points().at(0).y,
                        aabb::Manager::get(our.id)->points().at(3).x, aabb::Manager::get(our.id)->points().at(3).y };

            entity::cType other_type = other.owner->type();
            aabb::cName   other_aabb_name = aabb::Manager::get(other.id)->name;
            cRectF        other_rect = { aabb::Manager::get(other.id)->points().at(0).x, aabb::Manager::get(other.id)->points().at(0).y,
                                         aabb::Manager::get(other.id)->points().at(3).x, aabb::Manager::get(other.id)->points().at(3).y };

            cF32 overlap_y = our_rect.y < other_rect.y ? our_rect.h - other_rect.y : -(other_rect.h - our_rect.y);
            
            if (entity::is_clip(other.owner->type())) {
                transform()->position.y -= overlap_y;

                m_time_in_state = 0;

                //if (our_rect.x < other_rect.x + (other_rect.w - other_rect.x) / 2) {

                if (transform()->velocity.x == 0.0F) {
                    if (m_direction.x == 0.0F) {
                        transform()->velocity.x = -0.1F;
                        //transform()->position.x -= overlap_y;
                    } else {
                        transform()->velocity.x = 0.1F;
                        //transform()->position.x += overlap_y;
                    }
                }

                /*if (transform()->velocity.y < 0.0F) {
                    transform()->velocity.x = transform()->velocity.y;
                    transform()->velocity.y = 0.0F;
                }*/
            }
            else
            if (entity::is_water_line(other.owner->type())) {
                /*if (anim::Manager::get(m_dead_anim_id)) {
                    anim::Manager::get(m_dead_anim_id)->loop = 0;
                    anim::Manager::get(m_dead_anim_id)->source.x = 0;
                }*/
                if (!is_dead()) {
                    m_time_left_alive = 0;
                    m_time_left_dead = m_time_to_be_dead;
                    other.owner->transform()->velocity.y += our.owner->velocity().y / 4.0F;
                }
            }
        }
    };
}