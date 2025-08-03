module;
#include <concepts>
#include <string_view>

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


export constexpr u16 U16_MAX = -1;
//export constexpr u8c NUM_LAYERS = 2;

export constexpr u8 NUM_VISIBLE_LAYERS = 14;
export constexpr u8 NUM_LEVEL_LAYERS = 10;

export namespace aabb {
    enum class Name {
        null = 0,
        one, two, three, four, five, six, seven, eight, nine,
        up, down, left, right,
        body, body_swim,
        bone,
        hit_ground,
        interact,
        melee_L, melee_R,
        track
    };
    using Namec = const Name;
    const char* name_to_string(Namec n) {
        switch (n) {
        case Name::one:        return "one";        break;
        case Name::two:        return "two";        break;
        case Name::three:      return "three";      break;
        case Name::four:       return "four";       break;
        case Name::five:       return "five";       break;
        case Name::six:        return "six";        break;
        case Name::seven:      return "seven";      break;
        case Name::eight:      return "eight";      break;
        case Name::nine:       return "nine";       break;

        case Name::up:         return "up";         break;
        case Name::down:       return "down";       break;
        case Name::left:       return "left";       break;
        case Name::right:      return "right";      break;

        case Name::body:       return "body";       break;
        case Name::body_swim:  return "body_swim";  break;
        case Name::bone:       return "bone";       break;
        case Name::hit_ground: return "hit_ground"; break;
        case Name::interact:   return "interact";   break;
        case Name::melee_L:    return "melee_L";    break;
        case Name::melee_R:    return "melee_R";    break;
        case Name::track:      return "track";      break;
        default:               return "";           break;
        }
    }
    Namec string_to_name(const std::string_view& s) {
        if      (s == "one")        return Name::one;
        else if (s == "two")        return Name::two;
        else if (s == "three")      return Name::three;
        else if (s == "four")       return Name::four;
        else if (s == "five")       return Name::five;
        else if (s == "six")        return Name::six;
        else if (s == "seven")      return Name::seven;
        else if (s == "eight")      return Name::eight;
        else if (s == "nine")       return Name::nine;

        else if (s == "up")         return Name::up;
        else if (s == "down")       return Name::down;
        else if (s == "left")       return Name::left;
        else if (s == "right")      return Name::right;

        else if (s == "body")       return Name::body;
        else if (s == "body_swim")  return Name::body_swim;
        else if (s == "bone")       return Name::bone;
        else if (s == "hit_ground") return Name::hit_ground;
        else if (s == "interact")   return Name::interact;
        else if (s == "melee_L")    return Name::melee_L;
        else if (s == "melee_R")    return Name::melee_R;
        else if (s == "track")      return Name::track;
        return Name::null;
    }
}

export namespace start {
    enum class Type {
        null,
        center,
        L_0,
        R_0        
    };
    struct Info {
        start::Type type   = Type::center;
        u8          number = 0;
    };
    Type from_string(const char* s) {
        if      (s == "center") return Type::center;
        else if (s == "L_0")    return Type::L_0;
        else if (s == "R_0")    return Type::R_0;
        else                    return Type::null;
    }
    const char* to_string(const Type type) {
        switch (type) {
        case Type::center:  return "center";
        case Type::L_0:     return "L_0";
        case Type::R_0:     return "R_0";
        default:            return ""; }
    }
}
 

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
    bool operator ==(const Rect& other) const { return (x == other.x && y == other.y && w == other.w && h == other.h); }
    bool operator !=(const Rect& other) const { return !operator==(other); }

    Rect operator +(const Rect& other) const { return { x + other.x, y + other.y, w + other.w, h + other.h }; }
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