module;
#include <vector>
#include <sstream>

export module quad_node;
import collider;
import console;
import types;
import line;
import sprite;
import transform;
import tile;

export class QuadNode {
    static inline u16 m_max_objects = 3;
    static inline u16 m_window_w = 0, m_window_h = 0;
    static inline u8  m_layer       = 0;

    bool m_is_parent = false;
    
    QuadNode* m_child[4] = {nullptr};

    Rectf m_rect{ 0, 0, 0, 0 };
    std::vector<collider::Object*> m_objects;

    i32 m_line_up_id    = -1,
        m_line_down_id  = -1,
        m_line_left_id  = -1,
        m_line_right_id = -1;

    void split() {
        m_is_parent = true;
        f32c half_w = m_rect.w / 2.0f, half_h = m_rect.h / 2.0f;

        m_child[0] = new QuadNode(m_window_w, m_window_h, m_layer,  { m_rect.x,          m_rect.y,          half_w, half_h });
        m_child[0]->id = id;        

        m_child[1] = new QuadNode(m_window_w, m_window_h, m_layer,  { m_rect.x + half_w, m_rect.y,          half_w, half_h });
        m_child[1]->id = id + 1;        

        m_child[2] = new QuadNode(m_window_w, m_window_h, m_layer,  { m_rect.x,          m_rect.y + half_h, half_w, half_h});
        m_child[2]->id = id + 2;        
                
        m_child[3] = new QuadNode(m_window_w, m_window_h, m_layer,  { m_rect.x + half_w, m_rect.y + half_h, half_w, half_h });
        m_child[3]->id = id + 3;
        
        m_child[0]->depth = m_child[1]->depth = m_child[2]->depth = m_child[3]->depth = depth + 1;
        //m_child[0]->transform_id = m_child[1]->transform_id = m_child[2]->transform_id = m_child[3]->transform_id = transform_id;

        for (auto& object : m_objects) {
            m_child[0]->insert(object);
            m_child[1]->insert(object);
            m_child[2]->insert(object);
            m_child[3]->insert(object);
        }
        m_objects.clear();
    }
public:
    i32 id = 0;
    u16 depth = 0;
    static inline u16 max_depth = 3;
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
    std::vector<collider::Object*>& objects_at(Vec2fc position) {
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
        if (m_line_up_id != -1)    line::Set::erase(m_line_up_id);
        if (m_line_down_id != -1)  line::Set::erase(m_line_down_id);
        if (m_line_left_id != -1)  line::Set::erase(m_line_left_id);
        if (m_line_right_id != -1) line::Set::erase(m_line_right_id);
        m_line_up_id = m_line_down_id = m_line_left_id = m_line_right_id = -1;
    }    
    void init(u16c window_w, u16c window_h, u8c layer, Rectfc rect) {
        //if (m_line_up_id != -1) return;
        m_window_w = window_w, m_window_h = window_h;
        m_layer = layer;

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

        m_line_up_id    = line::Set::make( {m_rect.x, m_rect.y           }, {m_rect.x + m_rect.w, m_rect.y           } );
        m_line_down_id  = line::Set::make( {m_rect.x, m_rect.y + m_rect.h}, {m_rect.x + m_rect.w, m_rect.y + m_rect.h} );
        m_line_left_id  = line::Set::make( {m_rect.x, m_rect.y           }, {m_rect.x,            m_rect.y + m_rect.h} );
        m_line_right_id = line::Set::make( {m_rect.x + m_rect.w, m_rect.y}, {m_rect.x + m_rect.w, m_rect.y + m_rect.h} );

        //Console::log("x: ", m_rect.x, "\n");

        line::Set::at(m_line_up_id)->layer = m_layer;
        line::Set::at(m_line_down_id)->layer = m_layer;
        line::Set::at(m_line_left_id)->layer = m_layer;
        line::Set::at(m_line_right_id)->layer = m_layer;
        line::Set::at(m_line_up_id)->is_debug = true;
        line::Set::at(m_line_down_id)->is_debug = true;
        line::Set::at(m_line_left_id)->is_debug = true;
        line::Set::at(m_line_right_id)->is_debug = true;
        //line::Set::at(m_line_up_id)->transform_id = line::Set::at(m_line_down_id)->transform_id = line::Set::at(m_line_left_id)->transform_id = line::Set::at(m_line_right_id)->transform_id = transform_id;
        //Console::log("layer: ", (int)layer, "\n");
    }
    bool insert(collider::Object* object) {
        if (!object) return false;
        if (m_is_parent) {
            for (size_t i = 0; i < 4; ++i) {
                m_child[i]->insert(object);
            }
        } else {
            for (auto& point : object->points()) {
                if (point.x >= m_rect.x && point.x <= m_rect.x + m_rect.w &&
                    point.y >= m_rect.y && point.y <= m_rect.y + m_rect.h) {
                    //Console::log("point: ", point.x, " ", point.y, "\n");
                    m_objects.push_back(object);
                    break;
                }
            }
            if (m_objects.size() >= m_max_objects && depth < max_depth) {
                //Console::log("num objects: ", m_objects.size(), "\n");
                m_is_parent = true;
                split();
                m_objects.clear();
            }
        }
        return true;
    }

    void check_collision() {
        if (m_is_parent) {
            for (size_t i = 0; i < 4; ++i) {
                m_child[i]->check_collision();
            }
        } else {
            for (auto& a : m_objects) {
                if (!a) continue;
                
                for (auto& b : m_objects) {
                    if (!b || a == b || a->points().size() < 4 || b->points().size() < 4) continue;                    
                    //Console::log(a->point_0(c).x, " ", b->point_0().x, "\n");
                    if (is_colliding_aabb_y(*a, *b)) {
                        /*Rectfc a_rect = { a->transform()->position.x + a->points().at(0).x,
                                          a->transform()->position.y + a->points().at(0).y,
                                          a->transform()->position.x + a->points().at(3).x,
                                          a->transform()->position.y + a->points().at(3).y };
                        Rectfc b_rect = { b->transform()->position.x + b->points().at(0).x,
                                          b->transform()->position.y + b->points().at(0).y,
                                          b->transform()->position.x + b->points().at(3).x,
                                          b->transform()->position.y + b->points().at(3).y };*/
                        Rectfc a_rect = { a->points().at(0).x,
                                          a->points().at(0).y,
                                          a->points().at(3).x,
                                          a->points().at(3).y };
                        Rectfc b_rect = { b->points().at(0).x,
                                          b->points().at(0).y,
                                          b->points().at(3).x,
                                          b->points().at(3).y };

                        sprite::Infoc a_info = { .type = a->parent->sprite_type(),
                                                 .rect = a_rect,
                                                 .velocity = a->parent->transform()->velocity };
                        sprite::Infoc b_info = { .type = b->parent->sprite_type(),
                                                 .rect = b_rect,
                                                 .velocity = b->parent->transform()->velocity };
                        a->collide_y(a_info, b_info);
                        b->collide_y(b_info, a_info);
                    }
                    if (is_colliding_aabb_x(*a, *b)) {
                        /*Rectfc a_rect = { a->transform()->position.x + a->points().at(0).x,
                                          a->transform()->position.y + a->points().at(0).y,
                                          a->transform()->position.x + a->points().at(3).x,
                                          a->transform()->position.y + a->points().at(3).y };
                        Rectfc b_rect = { b->transform()->position.x + b->points().at(0).x,
                                          b->transform()->position.y + b->points().at(0).y,
                                          b->transform()->position.x + b->points().at(3).x,
                                          b->transform()->position.y + b->points().at(3).y };*/
                        Rectfc a_rect = { a->points().at(0).x,
                                          a->points().at(0).y,
                                          a->points().at(3).x,
                                          a->points().at(3).y };
                        Rectfc b_rect = { b->points().at(0).x,
                                          b->points().at(0).y,
                                          b->points().at(3).x,
                                          b->points().at(3).y };

                        sprite::Infoc a_info = { .type = a->parent->sprite_type(),
                                                 .rect = a_rect,
                                                 .velocity = a->parent->transform()->velocity };
                        sprite::Infoc b_info = { .type =  b->parent->sprite_type(),
                                                 .rect = b_rect,
                                                 .velocity = b->parent->transform()->velocity };
                        a->collide_x(a_info, b_info);
                        b->collide_x(b_info, a_info);
                    }                            
                }
            }
        }
    }

    /*bool is_colliding_aabb_x(collider::Object& a, collider::Object& b) {
        return a.points().size() >= 4 && b.points().size() >= 4 &&
               (a.transform()->position.x + a.points().at(1).x) >= (b.transform()->position.x + b.points().at(0).x) &&
               (a.transform()->position.y + a.points().at(0).x) <= (b.transform()->position.x + b.points().at(1).x) &&
               (a.transform()->position.y + a.points().at(2).y - 3) >= (b.transform()->position.y + b.points().at(0).y) &&
               (a.transform()->position.y + a.points().at(0).y + 3) <= (b.transform()->position.y + b.points().at(2).y);
    }
    bool is_colliding_aabb_y(collider::Object& a, collider::Object& b) {
        return a.points().size() >= 4 && b.points().size() >= 4 &&
               (a.transform()->position.y + a.points().at(2).y) >= (b.transform()->position.y + b.points().at(0).y) &&
               (a.transform()->position.y + a.points().at(0).y) <= (b.transform()->position.y + b.points().at(2).y) &&
               (a.transform()->position.x + a.points().at(1).x - 3) >= (b.transform()->position.x + b.points().at(0).x) &&
               (b.transform()->position.x + a.points().at(0).x + 3) <= (b.transform()->position.x + b.points().at(1).x);
    }*/
    bool is_colliding_aabb_x(collider::Object& a, collider::Object& b) {
        return a.points().size() >= 4 && b.points().size() >= 4 &&
            a.points().at(1).x >= b.points().at(0).x &&
            a.points().at(0).x <= b.points().at(1).x &&
            a.points().at(2).y - 3 >= b.points().at(0).y &&
            a.points().at(0).y + 3 <= b.points().at(2).y;
    }
    bool is_colliding_aabb_y(collider::Object& a, collider::Object& b) {
        return a.points().size() >= 4 && b.points().size() >= 4 &&
            a.points().at(2).y >= b.points().at(0).y &&
            a.points().at(0).y <= b.points().at(2).y &&
            a.points().at(1).x - 3 >= b.points().at(0).x &&
            a.points().at(0).x + 3 <= b.points().at(1).x;
    }
};