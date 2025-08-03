module;
#include <vector>
#include <sstream>
//#include <set>

export module quad_node;
import collider.aabb;
import console;
import types;
import line;
import sprite;
import transform;
//import entity;

//struct EntityInfo {
//    std::shared_ptr<entity::Object> entity;
//    i32 collider_id = -1;
//
//    bool operator ==(const EntityInfo& other) const { return (entity == other.entity && collider_id == other.collider_id); }
//    bool operator <(const EntityInfo& other) const { return (entity < other.entity && collider_id < other.collider_id); }
//};

export class QuadNode {
    static inline u16 s_max_depth   = 3, s_max_objects = 4,
                      s_window_w    = 0, s_window_h    = 0;
    static inline u8  s_layer       = 0;
    
    bool m_is_parent = false;
    
    QuadNode* m_child[4] = {nullptr};

    Rectf m_rect{ 0, 0, 0, 0 };
    std::vector<aabb::Object*> m_objects;

    i32 m_up_line_id    = -1,
        m_down_line_id  = -1,
        m_left_line_id  = -1,
        m_right_line_id = -1;

    void split() {
        m_is_parent = true;
        f32c half_w = m_rect.w / 2.0f, half_h = m_rect.h / 2.0f;

        m_child[0] = new QuadNode(s_window_w, s_window_h, s_layer,  { m_rect.x,          m_rect.y,          half_w, half_h });
        m_child[0]->id = id;        

        m_child[1] = new QuadNode(s_window_w, s_window_h, s_layer,  { m_rect.x + half_w, m_rect.y,          half_w, half_h });
        m_child[1]->id = id + 1;        

        m_child[2] = new QuadNode(s_window_w, s_window_h, s_layer,  { m_rect.x,          m_rect.y + half_h, half_w, half_h});
        m_child[2]->id = id + 2;        
                
        m_child[3] = new QuadNode(s_window_w, s_window_h, s_layer,  { m_rect.x + half_w, m_rect.y + half_h, half_w, half_h });
        m_child[3]->id = id + 3;
        
        m_child[0]->depth = m_child[1]->depth = m_child[2]->depth = m_child[3]->depth = depth + 1;
        //m_child[0]->transform_id = m_child[1]->transform_id = m_child[2]->transform_id = m_child[3]->transform_id = transform_id;

        for (auto& object : m_objects) {
            if (!object) continue;
            for (auto& point : object->points()) {
                m_child[0]->insert_point(object, point);
                m_child[1]->insert_point(object, point);
                m_child[2]->insert_point(object, point);
                m_child[3]->insert_point(object, point);
            }
        }
        m_objects.clear();
    }
public:
    i32 id = 0;
    u16 depth = 0;
    //i32 transform_id = -1;

    QuadNode() {}
    QuadNode(i32 t_id) { }
    QuadNode(u16c window_w, u16c window_h, u8c layer, Rectfc rect) {
        init(window_w, window_h, layer, rect);
    }
    ~QuadNode() {
        clear();
    }
    f32c x() const { return m_rect.x; }
    f32c y() const { return m_rect.y; }
    f32c w() const { return m_rect.w; }
    f32c h() const { return m_rect.h; }

    size_t depth_at(Vec2fc position) {
        if (m_is_parent) {
            for (size_t i = 0; i < 4; ++i) {
                if (m_child[i] &&
                    position.x >= m_child[i]->x() && position.x <= m_child[i]->x() + m_child[i]->w() &&
                    position.y >= m_child[i]->y() && position.y <= m_child[i]->y() + m_child[i]->h()) {
                    return m_child[i]->depth_at(position);
                }
            }
        }
        return depth;
    }
    size_t id_at(Vec2fc position) {
        if (m_is_parent) {
            for (size_t i = 0; i < 4; ++i) {
                if (m_child[i] &&
                    position.x >= m_child[i]->x() && position.x <= m_child[i]->x() + m_child[i]->w() &&
                    position.y >= m_child[i]->y() && position.y <= m_child[i]->y() + m_child[i]->h()) {
                    return m_child[i]->id_at(position);
                }
            }
        }
        return id;
    }
    std::vector<aabb::Object*>& objects_at(Vec2fc position) {
        for (size_t i = 0; i < 4; ++i) {
            if (m_child[i] &&
                position.x >= m_child[i]->x() && position.x <= m_child[i]->x() + m_child[i]->w() &&
                position.y >= m_child[i]->y() && position.y <= m_child[i]->y() + m_child[i]->h()) {
                return m_child[i]->objects_at(position);
            }
        }
        return m_objects;
    }
    void clear() {
        m_objects.clear();
        id = 0;
        depth = 0;
        m_is_parent = false;
        for (int i = 0; i < 4; ++i) {
            if (m_child[i]) {
                delete m_child[i];
                m_child[i] = nullptr;
            }
        }
        if (m_up_line_id != -1)    line::Set::erase(m_up_line_id);
        if (m_down_line_id != -1)  line::Set::erase(m_down_line_id);
        if (m_left_line_id != -1)  line::Set::erase(m_left_line_id);
        if (m_right_line_id != -1) line::Set::erase(m_right_line_id);
        m_up_line_id = m_down_line_id = m_left_line_id = m_right_line_id = -1;
    }    
    void init(u16c window_w, u16c window_h, u8c layer, Rectfc rect) {
        //if (m_up_line_id != -1) return;
        s_window_w = window_w, s_window_h = window_h;
        s_layer = layer;

        m_is_parent = false;
        m_rect = rect;

        /*if (transform::Set::at(transform_id)) {
            m_rect.x = rect.x + transform::Set::at(transform_id)->position.x;
            m_rect.y = rect.y + transform::Set::at(transform_id)->position.y;
        }*/

        if (m_rect.x < -m_rect.w || m_rect.x > window_w ||
            m_rect.y < -m_rect.h || m_rect.y > window_h) {
            return;
        }

        m_up_line_id    = line::Set::make( {m_rect.x, m_rect.y           }, {m_rect.x + m_rect.w, m_rect.y           } );
        m_down_line_id  = line::Set::make( {m_rect.x, m_rect.y + m_rect.h}, {m_rect.x + m_rect.w, m_rect.y + m_rect.h} );
        m_left_line_id  = line::Set::make( {m_rect.x, m_rect.y           }, {m_rect.x,            m_rect.y + m_rect.h} );
        m_right_line_id = line::Set::make( {m_rect.x + m_rect.w, m_rect.y}, {m_rect.x + m_rect.w, m_rect.y + m_rect.h} );

        //Console::log("x: ", m_rect.x, "\n");

        line::Set::at(m_up_line_id)->layer = s_layer;
        line::Set::at(m_down_line_id)->layer = s_layer;
        line::Set::at(m_left_line_id)->layer = s_layer;
        line::Set::at(m_right_line_id)->layer = s_layer;
        line::Set::at(m_up_line_id)->is_debug = true;
        line::Set::at(m_down_line_id)->is_debug = true;
        line::Set::at(m_left_line_id)->is_debug = true;
        line::Set::at(m_right_line_id)->is_debug = true;
        //line::Set::at(m_up_line_id)->transform_id = line::Set::at(m_down_line_id)->transform_id = line::Set::at(m_left_line_id)->transform_id = line::Set::at(m_right_line_id)->transform_id = transform_id;
        //Console::log("layer: ", (int)layer, "\n");
    }    

    bool insert_point(aabb::Object* parent, Vec2fc point) {
        if (parent->is_inactive()) return false;

        if (m_is_parent) {
            for (size_t i = 0; i < 4; ++i) {
                m_child[i]->insert_point(parent, point);
            }
        } else {
            if (point.x >= m_rect.x && point.x <= m_rect.x + m_rect.w &&
                point.y >= m_rect.y && point.y <= m_rect.y + m_rect.h) {
                if (std::find(m_objects.begin(), m_objects.end(), parent) == m_objects.end()) {
                    m_objects.push_back(parent);
                    //m_objects.insert(parent);
                    return true;
                }
            }
            if (m_objects.size() >= s_max_objects && depth < s_max_depth) {                
                m_is_parent = true;
                split();
                m_objects.clear();
            }
        }
        return false;
    }

    void check_collision() {
        if (m_is_parent) {
            for (size_t i = 0; i < 4; ++i) {
                m_child[i]->check_collision();
            }
        } else {
            for (auto& a : m_objects) {
                if (!a || !a->owner || a->is_inactive()) continue;
                if (a->time_left_colliding > 0) {
                    //Console::log("time left colliding: ", a->time_left_colliding, "\n");
                    --a->time_left_colliding;
                    continue;
                }
                
                for (auto& b : m_objects) {                    
                    if (!b || a == b || !b->owner || a->owner == b->owner || b->is_inactive() || a->points().size() < 4 || b->points().size() < 4) continue;

                    if (b->time_left_colliding > 0) {
                        continue;
                    }
                    //Console::log(a->point_0(c).x, " ", b->point_0().x, "\n");                    
                    if (is_colliding_aabb_y(*a, *b)) {
                        aabb::Infoc a_info = { a->owner, a->id };
                        aabb::Infoc b_info = { b->owner, b->id };
                        
                        a->collide_y(a_info, b_info);
                        b->collide_y(b_info, a_info);
                    }
                    if (is_colliding_aabb_x(*a, *b)) {
                        aabb::Infoc a_info = { a->owner, a->id };
                        aabb::Infoc b_info = { b->owner, b->id };
                        a->collide_x(a_info, b_info);
                        b->collide_x(b_info, a_info);
                    }
                }
            }
        }
    }
    bool is_colliding_aabb_x(aabb::Object& a, aabb::Object& b) {
        return a.points().size() >= 4 && b.points().size() >= 4 &&
               a.points().at(1).x > b.points().at(0).x     &&
               a.points().at(0).x < b.points().at(1).x     &&
               a.points().at(2).y - 3 > b.points().at(0).y &&
               a.points().at(0).y + 3 < b.points().at(2).y;
    }
    bool is_colliding_aabb_y(aabb::Object& a, aabb::Object& b) {
        return a.points().size() >= 4 && b.points().size() >= 4 &&
            a.points().at(2).y > b.points().at(0).y     &&
            a.points().at(0).y < b.points().at(2).y     &&
            a.points().at(1).x - 3 > b.points().at(0).x &&
            a.points().at(0).x + 3 < b.points().at(1).x;
    }
};