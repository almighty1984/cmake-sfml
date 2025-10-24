//module;
//#include <sstream>
export module entity.particle.interact;
import console;
import entity;
import entity.particle;
import collider.aabb;

export namespace entity {
    class ParticleInteract : public Particle {
    public:
        ParticleInteract() {
            m_type = Type::particle_interact;
        }
        void collide_x(aabb::cInfo our, aabb::cInfo other) override {
            if (!other.owner || !m_parent || m_parent == other.owner) return;

            cRectF our_rect = { aabb::Manager::get(our.id)->points().at(0).x, aabb::Manager::get(our.id)->points().at(0).y,
                        aabb::Manager::get(our.id)->points().at(3).x, aabb::Manager::get(our.id)->points().at(3).y };

            entity::cType other_type = other.owner->type();
            aabb::cName   other_aabb_name = aabb::Manager::get(other.id)->name;
            cRectF        other_rect = { aabb::Manager::get(other.id)->points().at(0).x, aabb::Manager::get(other.id)->points().at(0).y,
                                         aabb::Manager::get(other.id)->points().at(3).x, aabb::Manager::get(other.id)->points().at(3).y };

            cF32 overlap_x = our_rect.x < other_rect.x ? our_rect.w - other_rect.x : -(other_rect.w - our_rect.x);

            if (other_type == entity::Type::clip_ledge) {
                if (!m_parent->is_on_ground() || our_rect.h < other_rect.h) return;
                m_parent->interact(other.owner);
            }
            else if (other_type == entity::Type::trigger) {
                if (!m_parent->is_on_ground()) return;
                //m_is_to_erase = true;
                //Console::log("ParticleInteract::collide_x: ", entity::to_string(other_type), "\n");
                m_parent->interact(other.owner);
            }
            else if (other_type == entity::Type::brick || other_type == entity::Type::bug) {
                //m_is_to_erase = true;
                m_parent->interact(other.owner);
            }
        }
        void collide_y(aabb::cInfo our, aabb::cInfo other) override {
            collide_x(our, other);
        }
    };
}