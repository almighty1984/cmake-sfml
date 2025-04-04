module;
#include <vector>
#include <sstream>

export module quad_node;

import console;
import types;
import lines;
import transforms;

export class QuadNode {
    i32 m_line_up    = -1,
        m_line_down  = -1,
        m_line_left  = -1,
        m_line_right = -1;
            
    static inline u16 m_max_points = 8;
    static inline u8 m_layer = 0;

    std::vector<Vec2f> m_points;

    QuadNode* m_child[4] = {nullptr};

    Rectf m_rect{ 0, 0, 0, 0 };

    bool m_is_parent = false;

    void split() {
        if (depth >= max_depth) {
            return;
        }
        m_is_parent = true;
        f32c half_w = m_rect.w / 2.0f, half_h = m_rect.h / 2.0f;

        m_child[0] = new QuadNode(m_layer, { m_rect.x,          m_rect.y,          half_w, half_h });
        m_child[0]->id = id;        

        m_child[1] = new QuadNode(m_layer, { m_rect.x + half_w, m_rect.y,          half_w, half_h });
        m_child[1]->id = id + 1;        

        m_child[2] = new QuadNode(m_layer, { m_rect.x,          m_rect.y + half_h, half_w, half_h});
        m_child[2]->id = id + 2;        
                
        m_child[3] = new QuadNode(m_layer, { m_rect.x + half_w, m_rect.y + half_h, half_w, half_h });
        m_child[3]->id = id + 3;
        
        m_child[0]->depth = m_child[1]->depth = m_child[2]->depth = m_child[3]->depth = depth + 1;
        m_child[0]->transform_id = m_child[1]->transform_id = m_child[2]->transform_id = m_child[3]->transform_id = transform_id;

        for (auto& point : m_points) {            
            m_child[0]->insert(point);            
            m_child[1]->insert(point);            
            m_child[2]->insert(point);            
            m_child[3]->insert(point);
        }
    }
public:
    i32 id = 0;
    u16 depth = 0;
    static inline u16 max_depth = 8;
    i32 transform_id = -1;

    QuadNode() {}
    QuadNode(u8c layer, Rectfc r) {        
        init(layer, r);        
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
    std::vector<Vec2f>& points_at(Vec2fc position) {
        for (size_t i = 0; i < 4; ++i) {
            if (m_child[i] &&
                position.x >= m_child[i]->x() && position.x <= m_child[i]->x() + m_child[i]->w() &&
                position.y >= m_child[i]->y() && position.y <= m_child[i]->y() + m_child[i]->h()) {
                return m_child[i]->points_at(position);
            }
        }
        return m_points;
    }
    void clear() {
        m_points.clear();
        id = 0;
        depth = 0;
        m_is_parent = false;
        for (int i = 0; i < 4; ++i) {
            if (m_child[i]) {
                delete m_child[i];
                m_child[i] = nullptr;
            }
        }
        Lines::erase(m_line_up);
        Lines::erase(m_line_down);
        Lines::erase(m_line_left);
        Lines::erase(m_line_right);
        m_line_up = m_line_down = m_line_left = m_line_right = -1;
    }    
    void init(u8c layer, Rectfc r) {
        //if (m_line_up != -1) return;
        m_layer = layer;

        m_is_parent = false;
        m_rect = r;

        if (Transforms::at(transform_id)) {
            m_rect.x = r.x + Transforms::at(transform_id)->position.x;
            m_rect.y = r.y + Transforms::at(transform_id)->position.y;
        }

        m_line_up    = Lines::make( {m_rect.x, m_rect.y           }, {m_rect.x + m_rect.w, m_rect.y           } );
        m_line_down  = Lines::make( {m_rect.x, m_rect.y + m_rect.h}, {m_rect.x + m_rect.w, m_rect.y + m_rect.h} );
        m_line_left  = Lines::make( {m_rect.x, m_rect.y           }, {m_rect.x,            m_rect.y + m_rect.h} );
        m_line_right = Lines::make( {m_rect.x + m_rect.w, m_rect.y}, {m_rect.x + m_rect.w, m_rect.y + m_rect.h} );

        Lines::at(m_line_up)->layer = Lines::at(m_line_down)->layer = Lines::at(m_line_left)->layer = Lines::at(m_line_right)->layer = m_layer;        
        //Console::log("layer: ", (int)layer, "\n");
    }
    void insert(Vec2f point) {
        if (!m_is_parent) {
            if (point.x >= m_rect.x && point.x <= m_rect.x + m_rect.w &&
                point.y >= m_rect.y && point.y <= m_rect.y + m_rect.h) {
                //Console::log("point: ", point.x, " ", point.y, "\n");
                m_points.push_back(point);
                
                if (m_points.size() >= m_max_points) {
                    m_is_parent = true;
                    split();
                    //Console::log("points: ", m_points.size(), "\n");
                    m_points.clear();
                }
            }
        }
    }    
};