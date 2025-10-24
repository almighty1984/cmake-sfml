module;
#include <concepts>
#include <string>
#include <map>

export module types;

export using   U8 =       unsigned char;
export using  cU8 = const unsigned char;
export using  U16 =       unsigned short;
export using cU16 = const unsigned short;
export using  U32 =       unsigned int;
export using cU32 = const unsigned int;
export using  U64 =       unsigned long;
export using cU64 = const unsigned long;

export using   I8 =       char;
export using  cI8 = const char;
export using  I16 =       short;
export using cI16 = const short;
export using  I32 =       int;
export using cI32 = const int;
export using  I64 =       long;
export using cI64 = const long;

export using  F32 =       float;
export using cF32 = const float;
export using  F64 =       double;
export using cF64 = const double;


export constexpr U16 U16_MAX = -1;
//export constexpr cU8 NUM_LAYERS = 2;

export constexpr U8 NUM_VISIBLE_LAYERS = 15;
export constexpr U8 NUM_LEVEL_LAYERS = 10;

export namespace aabb {
    enum class Name {
        none = 0,
        one, two, three, four, five, six, seven, eight, nine,
        up, down, left, right,
        body, body_swim,
        bone,
        hit_ground,        
        track
    };
    using cName = const Name;
    const std::string to_string(cName n) {
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
        case Name::track:      return "track";      break;
        default:               return "";           break;
        }
    }

    const std::map<std::string, Name> string_to_aabb_name_map{
        { "one"        , Name::one        },
        { "two"        , Name::two        },
        { "three"      , Name::three      },
        { "four"       , Name::four       },
        { "five"       , Name::five       },
        { "six"        , Name::six        },
        { "seven"      , Name::seven      },
        { "eight"      , Name::eight      },
        { "nine"       , Name::nine       },        
        { "up"         , Name::up         },
        { "down"       , Name::down       },
        { "left"       , Name::left       },
        { "right"      , Name::right      },
        { "body"       , Name::body       },
        { "body_swim"  , Name::body_swim  },
        { "bone"       , Name::bone       },
        { "hit_ground" , Name::hit_ground },
        { "track"      , Name::track      }        
    };
    cName string_to_name(const std::string& s) {
        return string_to_aabb_name_map.at(s);
    }
}

export namespace state {
    enum class Type {
        none, edit, game, overlay
    };
    using cType = const Type;
    Type type_from(const std::string_view& s) {
        if (s == "edit")    return Type::edit;
        else if (s == "game")    return Type::game;
        else if (s == "overlay") return Type::overlay;
        return Type::none;
    }
    const std::string to_string(cType t) {
        switch (t) {
        case Type::edit:    return "edit";    break;
        case Type::game:    return "game";    break;
        case Type::overlay: return "overlay"; break;
        default:            return "";        break;
        }
    }
}
export namespace start {
    enum class Type {
        none = 0,
        center,
        L_0,
        R_0
    };
    using cType = const Type;
    struct Info {
        start::Type type   = Type::center;
        U8          number = 0;
    };
    using cInfo = const Info;
    Type from_string(const char* s) {
        if      (s == "center") return Type::center;
        else if (s == "L_0")    return Type::L_0;
        else if (s == "R_0")    return Type::R_0;
        else                    return Type::none;
    }
    const std::string to_string(cType type) {
        switch (type) {
        case Type::center:  return "center";
        case Type::L_0:     return "L_0";
        case Type::R_0:     return "R_0";
        default:            return ""; }
    }
}

export namespace particle {
    enum class Type {
        none = 0,
        brick, bubble,
        down_thrust, drop, drop_blood, dust,
        health, hit,
        interact,
        melee,
        sense,
        tile_line
    };
    using cType = const Type;
    const std::string to_string(cType t) {
        switch (t) {
        case Type::brick:       return "brick";
        case Type::bubble:      return "bubble";
        case Type::down_thrust: return "down_thrust";
        case Type::drop:        return "drop";
        case Type::drop_blood:  return "drop_blood";
        case Type::dust:        return "dust";
        case Type::health:      return "health";
        case Type::hit:         return "hit";
        case Type::interact:    return "interact";
        case Type::melee:       return "melee";
        case Type::sense:       return "sense";
        case Type::tile_line:   return "tile_line";
        default:                return "";
        }
    }
}

export struct Color {
    U8 r = 127, g = 127, b = 127;
    bool operator ==(const Color& other) { return (r == other.r && g == other.g && b == other.b); }
    bool operator !=(const Color& other) { return !operator==(other); }
    Color& operator =(const Color& other) {
        r = other.r;
        g = other.g;
        b = other.b;
        return *this;
    }
};
using cColor = const Color;

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

export using  Vec2I =       Vec2<I32>;
export using cVec2I = const Vec2<I32>;
export using  Vec2U =       Vec2<U32>;
export using cVec2U = const Vec2<U32>;
export using  Vec2F =       Vec2<F32>;
export using cVec2F = const Vec2<F32>;

export using  Vec3I =       Vec3<I32>;
export using cVec3I = const Vec3<I32>;
export using  Vec3U =       Vec3<U32>;
export using cVec3U = const Vec3<U32>;
export using  Vec3F =       Vec3<F32>;
export using cVec3F = const Vec3<F32>;

export using  RectI =       Rect<I32>;
export using cRectI = const Rect<I32>;
export using  RectU =       Rect<U32>;
export using cRectU = const Rect<U32>;
export using  RectF =       Rect<F32>;
export using cRectF = const Rect<F32>;