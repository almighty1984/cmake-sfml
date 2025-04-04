module;
#include "SFML/Graphics.hpp"

export module line;
import config;
import types;

export struct Line {
    i32 id = -1;
    sf::Vertex sf_vertices[4] = {};
    u8 size = 1, layer = 0;
    Color color{127};
    Color start_color{127};
    Color prev_color{127};
    Vec2f offset;
    Vec2f start, end;
    Vec2f delta;
    Vec2f velocity;

    f32 length = 0.0f,  max_length = 0.0f;
    f32 slope = 0.0f;
    
    Line(Vec2fc in_start, Vec2fc in_end) {
        set(in_start, in_end);
    }
    ~Line() {
        id = -1;
    }
    void set(Vec2f in_start, Vec2f in_end) {
        Vec2f prev_start = start;
        Vec2f prev_end   = end;
        start = in_start;
        end = in_end;

        f32c size_offset_x = (delta.x / length) * size;
        f32c size_offset_y = -(delta.y / length) * size;
        f32c extra_length_x = (delta.x / length) * size * 0.25f;
        f32c extra_length_y = (delta.y / length) * size * 0.25f;
        f32c dx = end.x - start.x;
        f32c dy = end.y - start.y;

        if (prev_start.x != start.x && prev_start.y != start.y && prev_end.x != end.x && prev_end.y != end.y) {
            //std::cout << "shit\n";
            length = std::sqrtf(dx * dx + dy * dy);
        }
        if (max_length > 0.0f && length > max_length) {
            f32c dlength = length - max_length;
            end.x -= (dlength * dx / length);
            end.y -= (dlength * dy / length);
        }
       
        Vec2f perpendicular = { -delta.y / length * size / 2.0f, delta.x / length * size / 2.0f };

        sf_vertices[0].position = { (start.x + perpendicular.x) * Config::scale(), (start.y + perpendicular.y) * Config::scale() };
        sf_vertices[1].position = { (end.x   + perpendicular.x) * Config::scale(), (end.y   + perpendicular.y) * Config::scale() };
        sf_vertices[2].position = { (end.x   - perpendicular.x) * Config::scale(), (end.y   - perpendicular.y) * Config::scale() };
        sf_vertices[3].position = { (start.x - perpendicular.x) * Config::scale(), (start.y - perpendicular.y) * Config::scale() };
    }
    void update() {
        Vec2f prev_delta = delta;        
        delta = end - start;
        
        if (delta != prev_delta) {
            length = std::sqrtf(delta.x * delta.x + delta.y * delta.y);
            slope = delta.y / delta.x;
            set(start, end);
        }

        if (color != prev_color) {
            prev_color = color;
            for (u8 i = 0; i < 4; ++i) {
                sf_vertices[i].color = { color.r, color.g, color.b };
            }
        }
    }
};