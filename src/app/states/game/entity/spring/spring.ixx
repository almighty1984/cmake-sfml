export module entity.spring;
import anim;
import console;
import entity;
import collider.aabb;
import sound;
import types;

export namespace entity {
    class Spring : public Object {
    public:
        Spring() {
            //load_config("res/entity/spring_U.cfg");
        }
        void collide_x(aabb::cInfo our, aabb::cInfo other) override {
            if (!our.owner || !other.owner) return;

            entity::cType other_type = other.owner->type();

            if ((other_type == entity::Type::brick || other_type == entity::Type::bug) && !other.owner->parent() ||
                other_type == entity::Type::frog  ||
                other_type == entity::Type::player) {
                //if (other.owner->parent()) return;
                
                reset_anim(m_dead_anim_id);
                m_time_left_dead = m_time_to_be_dead;
                if (m_dead_sound_id != -1 && sound::Manager::get(m_dead_sound_id)) {
                    sound::Manager::get(m_dead_sound_id)->position({ position().x / 160.0F, position().y / 90.0F });
                    sound::Manager::get(m_dead_sound_id)->play();
                }
            }

        }
        void collide_y(aabb::cInfo our, aabb::cInfo other) override {
            collide_x(our, other);
        }
    };
}