module;
#include <sstream>
export module entity.particle.sense;
import camera;
import console;
import entity;
import entity.particle;
import collider.aabb;

export namespace entity {
    class ParticleSense : public Particle {
    public:
        ParticleSense() {
            m_type = Type::particle_sense;
        }
        void collide_x(aabb::cInfo our, aabb::cInfo other) override {
            if (!m_parent || !other.owner || other.owner->is_dead() || m_parent == other.owner || m_parent->is_blocked() || !other.owner->transform()) return;
            if (other.owner->transform()->velocity.y >= 5.0F) {
                //Console::log("too fast!\n");
                return;
            }
            entity::cType other_type = other.owner->type();
            if (entity::is_arch(other_type)       ||
                entity::is_clip(other_type)       ||
                entity::is_slope(other_type)) {
                m_is_to_erase = true;
                //Console::log("ParticleSense::collide_x type: ", entity::to_string(other.owner->type()), "\n");
            } else if (other_type == entity::Type::brick  ||
                       other_type == entity::Type::bug    ||
                       other_type == entity::Type::frog   ||
                       other_type == entity::Type::player ||
                       other_type == entity::Type::water_line
                ) {
                
                //Console::log("vel: ", other.owner->transform()->velocity.x, " ", other.owner->transform()->velocity.y, "\n");
                if (m_parent) {
                    m_parent->add_sensed_object(other.owner);
                }
                m_is_to_erase = true;
            } else if (other_type == entity::Type::particle_down_thrust) {
                m_is_to_erase = true;
            } else if (other_type == entity::Type::particle_sense) {

            }
        }
        void collide_y(aabb::cInfo our, aabb::cInfo other) override {
            collide_x(our, other);
        }
    };
}