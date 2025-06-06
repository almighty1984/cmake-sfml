module;
#include <filesystem>
#include <string_view>
#include <vector>

export module tile;
import anim;
import console;
import sprite;
import transform;
import types;

export namespace tile {
    export class Object {
    protected:
        i32 m_transform_id    = -1,
            m_sprite_id       = -1,            
            m_current_anim_id = -1,
            m_idle_anim_id    = -1,
            m_dead_anim_id    = -1;

        std::vector<i32> m_aabb_ids;

        u16 m_time_left_dead = 0, m_time_to_be_dead = 50;

        Vec2f m_overlap;        
    public:
        bool is_on_ground = false,
             is_on_slope  = false;

        Vec2f moved_velocity;

        virtual void load_config(const std::filesystem::path& path);

        void set_anim(i32c anim_id) {
            if (m_current_anim_id == anim_id) return;
            m_current_anim_id = anim_id;
        }
        void reset_anim(i32c anim_id) {
            if (m_current_anim_id == anim_id) return;
            m_current_anim_id = anim_id;
            if (anim::Set::at(anim_id)) {
                anim::Set::at(anim_id)->loop = 0;
                anim::Set::at(anim_id)->source.x = 0;
            }
        }

        Object() {}
        virtual ~Object() {}


        virtual void collide_x(sprite::Infoc our, sprite::Infoc other) {
            if (our.type == sprite::Type::level_center) {
                if (other.type == sprite::Type::player) {
                    m_time_left_dead = m_time_to_be_dead;                    
                }
            }
        };
        virtual void collide_y(sprite::Infoc our, sprite::Infoc other) {
            if (our.type == sprite::Type::level_center) {
                collide_x(our, other);
            }            
        };

        i32c sprite_id()    const { return m_sprite_id;    }    void sprite_id(i32c id)    { m_sprite_id = id;    }
        i32c transform_id() const { return m_transform_id; }    void transform_id(i32c id) { m_transform_id = id; }


        sprite::Type sprite_type() { return sprite::Set::at(m_sprite_id) ? sprite::Set::at(m_sprite_id)->type : sprite::Type::null; }

        transform::Object* transform() { return transform::Set::is_valid(m_transform_id) ? transform::Set::at(m_transform_id) : nullptr; }
        sprite::Object* sprite() { return sprite::Set::is_valid(m_sprite_id) ? sprite::Set::at(m_sprite_id) : nullptr; }

        std::vector<i32>& aabb_ids() { return m_aabb_ids; }

        void set_layer(u8c layer) { sprite::Set::at(m_sprite_id)->layer = layer; }

        Rectic source_rect() const { return sprite::Set::at(m_sprite_id) ? sprite::Set::at(m_sprite_id)->source_rect : Recti{ 0, 0, 0, 0 }; }

        virtual void update() {
            if (m_time_left_dead > 0) {
                if (m_time_left_dead != -1) { // u16 max
                    --m_time_left_dead;
                }
                m_current_anim_id = m_dead_anim_id;
            } else {
                m_current_anim_id = m_idle_anim_id;
            }

            if (m_current_anim_id != -1 && anim::Set::at(m_current_anim_id)) {
                anim::Set::at(m_current_anim_id)->update();
                sprite()->source_rect.x = anim::Set::at(m_current_anim_id)->source.x;
                sprite()->source_rect.y = anim::Set::at(m_current_anim_id)->source.y;
            }
        }
    };
}