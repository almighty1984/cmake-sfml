module;
#include "SFML/Graphics.hpp"
#include <filesystem>

export module sprite;

import config;
import console;
import textures;
import transform;
import transforms;
import types;

export struct Sprite {
//private:
//    Vec2f m_transformed_position;
//public:
    i32    id           = -1,
           transform_id = -1;

    u8     layer       = 0,
           tile_set    = 0;
    bool   is_hidden   = false,
           is_leftward = false,
           is_upended  = false;
    Vec2f  offset,
           prev_position;
    Vec2f  start,
           level,
           origin,
           center;
    
    Color color, start_color;
    f32 rotation, start_rotation;
    std::filesystem::path texture_path;
    sf::Sprite sf_sprite;
    sf::Texture* sf_texture;
    Rect<i32> source_rect;
    Sprite() = delete;
    Sprite(std::filesystem::path path) : //m_transformed_position(),
                                         id(-1), transform_id(-1),
                                         layer(0), tile_set(0),
                                         is_hidden(false), is_leftward(false), is_upended(false),
                                         offset(), //prev_position(-1.0f, -1.0f),
                                         start(), level(), origin(), center(),
                                         color(), start_color(),
                                         rotation(0.0f), start_rotation(0.0f),
                                         texture_path(path),
                                         sf_sprite(*Textures::load(path)),
                                         sf_texture(Textures::load(path)),
                                         source_rect{0, 0, 16, 16} {
    }
    ~Sprite() {
        //Console::log("~Sprite layer: ", layer, " id: ", id, "\n");
        id = -1;
        transform_id = -1;
    }
    Sprite& operator=(const Sprite& other) {
        transform_id   = other.transform_id;
        layer          = other.layer;
        tile_set       = other.tile_set;
        is_hidden      = other.is_hidden;
        is_leftward    = other.is_leftward;
        is_upended     = other.is_upended;
        offset         = other.offset;
        start          = other.start;
        level          = other.level;
        origin         = other.origin;
        center         = other.center;
        color          = other.color;
        start_color    = other.start_color;
        rotation       = other.rotation;
        start_rotation = other.start_rotation;
        texture_path   = other.texture_path;     
        source_rect    = other.source_rect;
        return *this;
    }
    //void attach_transform(size_t id) { transform_id = id; }  void detach_transform() { transform_id = -1; }
    Transform* transform() { return Transforms::is_valid(transform_id) ? Transforms::at(transform_id) : nullptr; }
        
    void set_rotation(f32 r) {
        if (r > 360.0f) r -= 360.0f;
        rotation = r + start_rotation;
        if (rotation > 360.0f) rotation -= 360.0f;
        sf_sprite.setRotation(sf::degrees(rotation));
    }
    void rotate(f32c r) {
        rotation += r;
        if (rotation > 360.0f) rotation -= 360.0f;
        sf_sprite.setRotation(sf_sprite.getRotation() + sf::degrees(r));
    }
    bool texture(const std::filesystem::path path) {
        texture_path = path;
        sf_texture = Textures::load(path);
        if (!sf_texture) return false;
        sf_sprite.setTexture(*sf_texture, true);
        sf_sprite.setTextureRect(sf::IntRect({ source_rect.x, source_rect.y }, { source_rect.w, source_rect.h }));
        return true;
    }
    void texture_rect(Rect<i32> rect) {
        source_rect = rect;
        sf_sprite.setTextureRect(sf::IntRect({ rect.x, rect.y }, { rect.w, rect.h }));
    }
    void update() {
        if (!Transforms::is_valid(transform_id)) return;

        //position = Transforms::at(transform_id)->position;

        texture_rect(source_rect);
        sf_sprite.setOrigin({ origin.x, origin.y });
        sf_sprite.setScale({ (f32)Config::scale(), (f32)Config::scale()});

        //if (position != prev_position) {
            //m_transformed_position = position + transform()->position;
            //prev_position = position;
            //center = { position.x + source_rect.w / 2.0f, position.y + source_rect.h / 2.0f };
            //Console::log("Sprite::update position: ", prev_position.x, " ", transform()->position.x, "  ", prev_position.y, " ", transform()->position.y, "\n");        
            sf_sprite.setPosition({ (Transforms::at(transform_id)->position.x + offset.x) * Config::scale(),
                                    (Transforms::at(transform_id)->position.y + offset.y) * Config::scale() });
        //}
    }
   /* Vec2f position() {
        return Transforms::at(transform_id)->position + offset;
    }*/
};