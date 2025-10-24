export module entity.particle.health;
import console;
import entity.particle;
import collider.aabb;
import sound;

export namespace entity {
    class ParticleHealth : public Particle {
    public:
        ParticleHealth() {
            m_type = Type::particle_health;
            m_state = entity::State::idle;
        }        
        void collide_x(aabb::cInfo our, aabb::cInfo other) override {
            if (is_dead() || m_is_to_erase || !other.owner || other.owner->is_dead() || !other.owner->health()/* || other.owner == m_parent*/) return;

            entity::cType other_type = other.owner->type();

            if (other_type == entity::Type::player) {
                if (other.owner->health()->amount == other.owner->health()->max) return;
                
                m_time_left_alive = 1;
                m_time_left_dead = m_time_to_be_dead;
                
                other.owner->health()->amount += 16.0F;
                
                if (other.owner->health()->amount > other.owner->health()->max) {
                    other.owner->health()->amount = other.owner->health()->max;
                }

                if (m_dead_sound_id != -1 && sound::Manager::get(m_dead_sound_id)) {
                    sound::Manager::get(m_dead_sound_id)->position({ transform()->position.x / 160.0F, transform()->position.y / 90.0F });
                    sound::Manager::get(m_dead_sound_id)->play();
                }
            }
        }
        void collide_y(aabb::cInfo our, aabb::cInfo other) override {
            collide_x(our, other);
        }
    };
}