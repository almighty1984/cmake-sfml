export module entity.flag;
import anim;
import console;
import entity;

export namespace entity {
    class Flag : public Object {
    public:
        Flag() {
            //load_config("res/entity/level_center.cfg");
        }
        void collide_x(aabb::Infoc our, aabb::Infoc other) override {
            if (!sprite() || !our.owner || !other.owner) return;
            entity::Typec other_type = other.owner->type();
            if (other_type == entity::Type::player) {
                m_time_left_dead = m_time_to_be_dead;
            }
        };
        void collide_y(aabb::Infoc our, aabb::Infoc other) override {
            collide_x(our, other);
        };        
    };
}