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
import window;
//import entity;

//struct EntityInfo {
//    std::shared_ptr<entity::Object> entity;
//    I32 collider_id = -1;
//
//    bool operator ==(const EntityInfo& other) const { return (entity == other.entity && collider_id == other.collider_id); }
//    bool operator <(const EntityInfo& other) const { return (entity < other.entity && collider_id < other.collider_id); }
//};

cF32 size_at_depth(cU8 d) {
    F32 size = 256.0F;
    for (U8 i = 0; i < d; ++i) {
        size /= 2.0F;
    }
    return size;
}

export class QuadNode {
    static inline U16 s_max_depth   = 5, s_max_objects = 8,
                      s_window_w    = 0, s_window_h    = 0;
    static inline U8  s_layer       = 0;
    
    bool m_is_parent = false;
    
    QuadNode* m_child[4] = {nullptr};

    RectF m_rect{ 0, 0, 0, 0 };
    std::vector<aabb::Object*> m_objects;

    I32 m_up_line_id    = -1,
        m_down_line_id  = -1,
        m_left_line_id  = -1,
        m_right_line_id = -1;

    void split() {
        m_is_parent = true;
        cF32 half_w = m_rect.w / 2.0F, half_h = m_rect.h / 2.0F;

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
                for (size_t i = 0; i < 4; ++i) {
                    m_child[i]->insert_point(object, point);
                }
            }
            // aabb too large? insert half sides and center
            if (object->width() >= size_at_depth(depth + 1) || object->height() >= size_at_depth(depth + 1)) {
                for (size_t i = 0; i < 4; ++i) {
                    m_child[i]->insert_point(object, object->point_0() + Vec2F{ object->width() / 2.0F,                    0.0F } );
                    m_child[i]->insert_point(object, object->point_0() + Vec2F{                   0.0F, object->height() / 2.0F } );
                    m_child[i]->insert_point(object, object->point_2() + Vec2F{ object->width() / 2.0F,                    0.0F } );
                    m_child[i]->insert_point(object, object->point_1() + Vec2F{                   0.0F, object->height() / 2.0F } );

                    m_child[i]->insert_point(object, object->point_0() + Vec2F{ object->width() / 2.0F, object->height() / 2.0F } );
                }
            }
        }
        m_objects.clear();
    }
public:
    I32 id = 0;
    U16 depth = 0;
    //I32 transform_id = -1;

    //QuadNode() = delete;
    //QuadNode(cI32 t_id) { }
    //QuadNode() { }
    QuadNode(cU16 window_w, cU16 window_h, cU8 layer, cRectF rect) {
        //Console::log("size at depth ", (int)s_max_depth, ": ", size_at_depth(s_max_depth), "\n");        
        init(window_w, window_h, layer, rect);
    }
    ~QuadNode() {
        clear();
    }
    void draw(std::unique_ptr<Window>& window) {
        if (m_is_parent) {
            for (size_t i = 0; i < 4; ++i) {
                m_child[i]->draw(window);
            }
        }
        window->draw(line::Manager::get(m_up_line_id));
        window->draw(line::Manager::get(m_down_line_id));
        window->draw(line::Manager::get(m_left_line_id));
        window->draw(line::Manager::get(m_right_line_id));
    }

    cF32 x() const { return m_rect.x; }
    cF32 y() const { return m_rect.y; }
    cF32 w() const { return m_rect.w; }
    cF32 h() const { return m_rect.h; }

    size_t depth_at(cVec2F position) {
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
    size_t id_at(cVec2F position) {
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
    std::vector<aabb::Object*>& objects_at(cVec2F position) {
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
        line::Manager::erase(m_up_line_id);
        line::Manager::erase(m_down_line_id);
        line::Manager::erase(m_left_line_id);
        line::Manager::erase(m_right_line_id);

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
        //m_up_line_id = m_down_line_id = m_left_line_id = m_right_line_id = -1;
    }    
    void init(cU16 window_w, cU16 window_h, cU8 layer, cRectF rect) {
        //if (m_up_line_id != -1) return;
        s_window_w = window_w, s_window_h = window_h;
        s_layer = layer;

        m_is_parent = false;
        m_rect = rect;

        /*if (transform::Manager::get(transform_id)) {
            m_rect.x = rect.x + transform::Manager::get(transform_id)->position.x;
            m_rect.y = rect.y + transform::Manager::get(transform_id)->position.y;
        }*/

        
        /*if (m_rect.x < -m_rect.w || m_rect.x > window_w ||  // FIXME: sometimes though in view??
            m_rect.y < -m_rect.h || m_rect.y > window_h) {
            return;
        }*/

        m_up_line_id    = line::Manager::make( {m_rect.x, m_rect.y           }, {m_rect.x + m_rect.w, m_rect.y           } );
        m_down_line_id  = line::Manager::make( {m_rect.x, m_rect.y + m_rect.h}, {m_rect.x + m_rect.w, m_rect.y + m_rect.h} );
        m_left_line_id  = line::Manager::make( {m_rect.x, m_rect.y           }, {m_rect.x,            m_rect.y + m_rect.h} );
        m_right_line_id = line::Manager::make( {m_rect.x + m_rect.w, m_rect.y}, {m_rect.x + m_rect.w, m_rect.y + m_rect.h} );

        //Console::log("x: ", m_rect.x, "\n");

        line::Manager::get(m_up_line_id)->layer = s_layer;
        line::Manager::get(m_down_line_id)->layer = s_layer;
        line::Manager::get(m_left_line_id)->layer = s_layer;
        line::Manager::get(m_right_line_id)->layer = s_layer;
        line::Manager::get(m_up_line_id)->is_debug = true;
        line::Manager::get(m_down_line_id)->is_debug = true;
        line::Manager::get(m_left_line_id)->is_debug = true;
        line::Manager::get(m_right_line_id)->is_debug = true;
        //line::Manager::get(m_up_line_id)->transform_id = line::Manager::get(m_down_line_id)->transform_id = line::Manager::get(m_left_line_id)->transform_id = line::Manager::get(m_right_line_id)->transform_id = transform_id;
        //Console::log("layer: ", (int)layer, "\n");
    }    

    bool insert_point(aabb::Object* object, cVec2F point) {
        if (!object->is_active()) return false;

        if (point.x < m_rect.x || point.x > m_rect.x + m_rect.w ||
            point.y < m_rect.y || point.y > m_rect.y + m_rect.h) {
            return false;
        }
                        

        if (m_is_parent) {
            for (size_t i = 0; i < 4; ++i) {
                m_child[i]->insert_point(object, point);
            }
            return false;
        } else {
            if (m_objects.size() >= s_max_objects && depth < s_max_depth) {
                m_is_parent = true;
                split();
                m_objects.clear();
            }
            else {
                /*if (point.x >= m_rect.x && point.x <= m_rect.x + m_rect.w &&
                    point.y >= m_rect.y && point.y <= m_rect.y + m_rect.h) {*/
                    if (std::find(m_objects.begin(), m_objects.end(), object) == m_objects.end()) {
                        m_objects.emplace_back(object);
                        return true;
                    }
                //}
            }
        }
        return false;
    }

    void check_collision() {
        if (m_is_parent) {
            for (size_t i = 0; i < 4; ++i) {
                m_child[i]->check_collision();                
            }
            return;
        }
        for (auto& a : m_objects) {
            if (!a || !a->owner || !a->is_active()) continue;
            if (a->time_left_colliding > 0) {
                //Console::log("time left colliding: ", a->time_left_colliding, "\n");
                --a->time_left_colliding;
                continue;
            }
                
            for (auto& b : m_objects) {                    
                if (!b || a == b || !b->owner || a->owner == b->owner || !b->is_active() || a->points().size() < 4 || b->points().size() < 4) continue;
                //a->owner->update();

                if (b->time_left_colliding > 0) {
                    continue;
                }
                //Console::log(a->point_0(c).x, " ", b->point_0().x, "\n");                    
                if (is_colliding_aabb_y(*a, *b)) {
                    aabb::cInfo a_info = { a->owner, a->id };
                    aabb::cInfo b_info = { b->owner, b->id };
                        
                    a->collide_y(a_info, b_info);
                    b->collide_y(b_info, a_info);
                }
                if (is_colliding_aabb_x(*a, *b)) {
                    aabb::cInfo a_info = { a->owner, a->id };
                    aabb::cInfo b_info = { b->owner, b->id };
                    a->collide_x(a_info, b_info);
                    b->collide_x(b_info, a_info);
                }
            }
        }
    }
    bool is_colliding_aabb_x(aabb::Object& a, aabb::Object& b) {
        if (a.points().size() < 4 || b.points().size() < 4) return false;

        F32 h_dec = (a.points().at(2).y - a.points().at(0).y) / 16.0F;
        //if (h_dec < 1.0F) h_dec = 1.0F;
        //if (h_dec > 4.0F) h_dec = 4.0F;
        //cF32 h_dec = 3.0F;

        return a.points().at(1).x > b.points().at(0).x         &&
               a.points().at(0).x < b.points().at(1).x         &&
               a.points().at(2).y - h_dec > b.points().at(0).y &&
               a.points().at(0).y + h_dec < b.points().at(2).y;
    }
    bool is_colliding_aabb_y(aabb::Object& a, aabb::Object& b) {
        if (a.points().size() < 4 || b.points().size() < 4) return false;

        F32 w_dec = (a.points().at(1).x - a.points().at(0).x) / 16.0F;
        //if (w_dec < 1.0F) w_dec = 1.0F;
        //if (w_dec > 4.0F) w_dec = 4.0F;

        return a.points().at(2).y > b.points().at(0).y         &&
               a.points().at(0).y < b.points().at(2).y         &&
               a.points().at(1).x - w_dec > b.points().at(0).x &&
               a.points().at(0).x + w_dec < b.points().at(1).x;
    }
};