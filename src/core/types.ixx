module;
#include <concepts>

export module types;

export using u8 = unsigned char;
export using u8c = unsigned char const;
export using u16 = unsigned short;
export using u16c = unsigned short const;
export using u32 = unsigned int;
export using u32c = unsigned int const;
export using u64 = unsigned long;
export using u64c = unsigned long const;

export using i8 = char;
export using i8c = char const;
export using i16 = short;
export using i16c = short const;
export using i32 = int;
export using i32c = int const;
export using i64 = long;
export using i64c = long const;

export using f32 = float;
export using f32c = float const;
export using f64 = double;
export using f64c = double const;

//export constexpr u8c NUM_LAYERS = 2;

export constexpr u8 NUM_VISIBLE_LAYERS = 14;

export struct Color {
    u8 r = 127, g = 127, b = 127;
    bool operator ==(const Color& other) { return (r == other.r && g == other.g && b == other.b); }
    bool operator !=(const Color& other) { return !operator==(other); }
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
    Rect& operator =(const Rect& other) { x = other.x, y = other.y, w = other.w, h = other.h; return *this; }
    template<typename T>
    Rect& operator *=(const T scalar) { x *= scalar, y *= scalar, w *= scalar, h *= scalar; return *this; }
    bool operator ==(const Rect& other) { return (x == other.x && y == other.y && w == other.w && h == other.h); }
    bool operator !=(const Rect& other) { return (x != other.x || y != other.y || w != other.w || h != other.h); }
};

export template<typename T> requires std::integral<T> || std::floating_point<T>
struct Vec2 {
    T x = 0, y = 0;
    Vec2() {}
    Vec2(T in_x, T in_y) : x(in_x), y(in_y) {}
    Vec2& operator =(const Vec2& other) { x = other.x, y = other.y;   return *this; }
    Vec2& operator =(const T value) { x = value, y = value;   return *this; }
    Vec2& operator +=(const Vec2& other) { x += other.x, y += other.y; return *this; }
    Vec2& operator -=(const Vec2& other) { x -= other.x, y -= other.y; return *this; }
    Vec2& operator *=(const Vec2& other) { x *= other.x, y *= other.y; return *this; }
    Vec2 operator +(const Vec2& other)  const { return { x + other.x, y + other.y }; }
    Vec2 operator -(const Vec2& other)  const { return { x - other.x, y - other.y }; }
    Vec2 operator -()                   const { return { -x, -y }; }
    Vec2 operator *(const Vec2& other)  const { return { x * other.x, y * other.y }; }
    template<typename T>
    Vec2 operator *(const T scalar)     const { return { x * scalar, y * scalar }; }
    template<typename T>
    Vec2 operator /(const T scalar)     const { return { x / scalar, y / scalar }; }
    bool operator ==(const Vec2& other) const { return x == other.x && y == other.y; }
    bool operator !=(const Vec2& other) const { return !operator==(other); }
    template<typename T>
    bool operator ==(const T value)     const { return x == value && y == value; }
    template<typename T>
    bool operator !=(const T value)     const { return x != value && y != value; }
    bool operator <(const Vec2& other)  const { return x < other.x && y < other.y; }
};

export template<typename T> requires std::integral<T> || std::floating_point<T>
struct Vec3 {
    T x = 0, y = 0, z = 0;
    Vec3() {}
    Vec3(T in_x, T in_y, T in_z) : x(in_x), y(in_y), z(in_z) {}
    Vec3& operator  =(const Vec3& other) { x = other.x, y = other.y, z = other.z;    return *this; }
    Vec3& operator  =(const T value) { x = value, y = value, z = value;          return *this; }
    Vec3& operator +=(const Vec3& other) { x += other.x, y += other.y, z += other.z; return *this; }
    Vec3& operator -=(const Vec3& other) { x -= other.x, y -= other.y, z -= other.z; return *this; }
    Vec3& operator *=(const Vec3& other) { x *= other.x, y *= other.y, z *= other.z; return *this; }
    Vec3  operator  +(const Vec3& other) const { return { x + other.x, y + other.y, z + other.z }; }
    Vec3  operator  -(const Vec3& other) const { return { x - other.x, y - other.y, z - other.z }; }
    Vec3  operator  -()                  const { return { -x, -y, -z }; }
    Vec3  operator  *(const Vec3& other) const { return { x * other.x, y * other.y, z * other.z }; }
    template<typename T>
    Vec3  operator  *(const T scalar)    const { return { x * scalar, y * scalar, z * scalar }; }
    template<typename T>
    Vec3  operator  /(const T scalar)    const { return { x / scalar, y / scalar, z / scalar }; }
    bool  operator ==(const Vec3& other) const { return x == other.x && y == other.y && z == other.z; }
    bool  operator !=(const Vec3& other) const { return !operator==(other); }
    template<typename T>
    bool  operator ==(const T value)     const { return x == value && y == value && z == value; }
    template<typename T>
    bool  operator !=(const T value)     const { return !operator==(value); }
    bool  operator  <(const Vec3& other) const { return x < other.x && y < other.y && z < other.z; }
};

export using Vec2i  = Vec2<i32>;
export using Vec2ic = Vec2<i32> const;
export using Vec2u  = Vec2<u32>;
export using Vec2uc = Vec2<u32> const;
export using Vec2f  = Vec2<f32>;
export using Vec2fc = Vec2<f32> const;

export using Vec3i  = Vec3<i32>;
export using Vec3ic = Vec3<i32> const;
export using Vec3u  = Vec3<u32>;
export using Vec3uc = Vec3<u32> const;
export using Vec3f  = Vec3<f32>;
export using Vec3fc = Vec3<f32> const;

export using Recti  = Rect<i32>;
export using Rectic = Rect<i32> const;
export using Rectu =  Rect<u32>;
export using Rectuc = Rect<u32> const;
export using Rectf  = Rect<f32>;
export using Rectfc = Rect<f32> const;