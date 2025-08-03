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
        void collide_x(aabb::Infoc our, aabb::Infoc other) override {
            if (!our.owner || !other.owner || m_time_left_dead > 0) return;

            entity::Typec other_type = other.owner->type();

            if (other_type != entity::Type::player) return;

            reset_anim(m_dead_anim_id);
            m_time_left_dead = m_time_to_be_dead;

            if (m_dead_sound_id != -1 && sound::Set::at(m_dead_sound_id)) {
                sound::Set::at(m_dead_sound_id)->position({ position().x / 160.0f, position().y / 90.0f });
                sound::Set::at(m_dead_sound_id)->play();
            }

        }
        void collide_y(aabb::Infoc our, aabb::Infoc other) override {
            collide_x(our, other);
        }
    };
}