module;
#include <concepts>

export module types;

export using u8   = unsigned char;
export using u8c  = unsigned char const;
export using u16  = unsigned short;
export using u16c = unsigned short const;
export using u32  = unsigned int;
export using u32c = unsigned int const;
export using u64  = unsigned long;
export using u64c = unsigned long const;

export using i8   = char;
export using i8c  = char const;
export using i16  = short;
export using i16c = short const;
export using i32  = int;
export using i32c = int const;
export using i64  = long;
export using i64c = long const;

export using f32  = float;
export using f32c = float const;
export using f64  = double;
export using f64c = double const;

//export constexpr u8c NUM_LAYERS = 2;

export struct Color {
    u8 r = 127, g = 127, b = 127;
    bool operator ==(const Color& other) {  return (r == other.r && g == other.g && b == other.b);  }
    bool operator !=(const Color& other) {  return (r != other.r || g != other.g || b != other.b);  }
    Color& operator =(const Color& other) {
        r = other.r;
        g = other.g;
        b = other.b;
        return *this;
    }
};
export template<typename T> requires std::integral<T> || std::floating_point<T>
struct Rect {
    T x = 0, y = 0, w = 0, h = 0;
    Rect() {}
    Rect(T in_x, T in_y, T in_w, T in_h) : x(in_x), y(in_y), w(in_w), h(in_h) {}
    Rect& operator =(const Rect& other) {
        x = other.x;
        y = other.y;
        w = other.w;
        h = other.h;
        return *this;
    }
    template<typename U>
    Rect& operator *=(const U scalar) {
        x *= scalar, y *= scalar, w *= scalar, h *= scalar;
        return *this;
    }    
    bool operator ==(const Rect& other) { return (x == other.x && y == other.y && w == other.w && h == other.h); }
    bool operator !=(const Rect& other) { return (x != other.x || y != other.y || w != other.w || h != other.h); }
};
export template<typename T> requires std::integral<T> || std::floating_point<T>
struct Vec2 {
    T x = 0, y = 0;
    Vec2() {}
    Vec2(T in_x, T in_y) : x(in_x), y(in_y) {}
    Vec2& operator =(const Vec2& other) {
        x = other.x;
        y = other.y;
        return *this;
    }
    Vec2& operator +=(const Vec2& other) {
        x += other.x;
        y += other.y;
        return *this;
    }
    Vec2 operator +(const Vec2& other) { return { x + other.x, y + other.y };   }
    Vec2 operator -(const Vec2& other) { return { x - other.x, y - other.y };   }
    Vec2 operator -() { return { -x, -y }; }
    Vec2 operator *(const Vec2& other) { return { x * other.x, y * other.y };   }
    template<typename T>
    Vec2 operator *(const T scalar)     { return { x * scalar, y * scalar };     }
    template<typename T>
    Vec2 operator /(const T scalar)     { return { x / scalar, y / scalar };     }

    bool operator ==(const Vec2& other) { return (x == other.x && y == other.y); }
    //template<typename U>
    //bool operator ==(const U value) { return x == value && y == value; }
        
    bool operator !=(const Vec2& other) { return (x != other.x || y != other.y); }
    //template<typename U>
    //bool operator !=(const U value)      { return x == value && y == value;    }
};
export typedef Vec2<i32> Vec2i;
export typedef Vec2<i32> const Vec2ic;
export typedef Vec2<f32> Vec2f;
export typedef Vec2<f32> const Vec2fc;

export typedef Rect<i32> Recti;
export typedef Rect<i32> const Rectic;
export typedef Rect<f32> Rectf;
export typedef Rect<f32> const Rectfc;

export struct SpriteData {
    u8 tile_set = 0;
    u8 layer = 0;
    u16 source_y = 0;
    u16 source_x = 0;
    u16 offset_y = 0;
    u16 offset_x = 0;
};