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
export struct IntRect {
    i32 x = 0, y = 0, w = 0, h = 0;
    IntRect() {}
    IntRect(i32c in_x, i32c in_y, i32c in_w, i32c in_h) : x(in_x), y(in_y), w(in_w), h(in_h) {}
    IntRect& operator =(const IntRect& other) {
        x = other.x;
        y = other.y;
        w = other.w;
        h = other.h;
        return *this;
    }
    template<typename T>
    IntRect& operator *=(const T scalar) {
        x *= scalar, y *= scalar, w *= scalar, h *= scalar;
        return *this;
    }    
    bool operator ==(const IntRect& other) { return (x == other.x && y == other.y && w == other.w && h == other.h); }
    bool operator !=(const IntRect& other) { return (x != other.x || y != other.y || w != other.w || h != other.h); }
};
export struct Vec2i {
    i32 x = 0, y = 0;
    Vec2i() {}
    Vec2i(i32c in_x, i32c in_y) : x(in_x), y(in_y) {}
    Vec2i& operator =(const Vec2i& other) {
        x = other.x;
        y = other.y;
        return *this;
    }
    Vec2i& operator +=(const Vec2i& other) {
        x += other.x;
        y += other.y;
        return *this;
    }
    Vec2i operator +(const Vec2i& other) { return { x + other.x, y + other.y };   }
    Vec2i operator -(const Vec2i& other) { return { x - other.x, y - other.y };   }
    Vec2i operator *(const Vec2i& other) { return { x * other.x, y * other.y };   }
    template<typename T>
    Vec2i operator *(const T scalar)     { return { x * scalar, y * scalar };     }
    bool operator ==(const Vec2i& other) { return (x == other.x && y == other.y); }
    bool operator !=(const Vec2i& other) { return (x != other.x || y != other.y); }
    template<typename T>
    bool operator !=(const T value)      { return (x != value && y != value);     }
};
export struct Vec2f {
    f32 x = 0.0f, y = 0.0f;
    Vec2f() {}
    Vec2f(f32c in_x, f32c in_y) : x(in_x), y(in_y) {}
    Vec2f& operator =(const Vec2f& other) {
        x = other.x;
        y = other.y;
        return *this;
    }
    Vec2f& operator +=(const Vec2f& other) {
        x += other.x;
        y += other.y;
        return *this;
    }
    Vec2f operator +(const Vec2f& other) {  return {x + other.x, y + other.y};     }
    Vec2f operator -(const Vec2f& other) {  return { x - other.x, y - other.y };   }
    Vec2f operator *(const Vec2f& other) {  return { x * other.x, y * other.y };   }
    template<typename T>
    Vec2f operator *(const T scalar)     {  return { x * scalar, y * scalar };     }
    template<typename T>
    Vec2f operator /(const T scalar)     { return { x / scalar, y / scalar }; }
    bool operator ==(const Vec2f& other) {  return (x == other.x && y == other.y); }    
    bool operator !=(const Vec2f& other) {  return (x != other.x || y != other.y); }
    template<typename T>
    bool operator ==(const T val)        {  return (x == val && y == val);         }
    template<typename T>
    bool operator !=(const T val)        {  return (x != val && y != val);         }
        
    //Vec2f operator %(const int val) { return { (int)x % val, (int)y % val }; }
 };

export typedef Vec2i const Vec2ic;
export typedef Vec2f const Vec2fc;