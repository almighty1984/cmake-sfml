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

        void collide_x(aabb::Infoc our, aabb::Infoc other) override {
            if (entity::is_arch(other.owner->type()) || entity::is_clip(other.owner->type()) || other.owner->type() == entity::Type::player) {
                if (m_parent) {
                    m_parent->add_sensed_object(other.owner);
                }
            }
        }
        void collide_y(aabb::Infoc our, aabb::Infoc other) override {

        }
    };
}