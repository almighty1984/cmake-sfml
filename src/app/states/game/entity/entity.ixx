module;
#include <filesystem>
#include <string>
#include <vector>
#include <set>
#include <list>
#include <map>

export module entity;
import anim;
import camera;
//import collider.aabb;
import console;
import health;
import sound;
import sprite;
import transform;
import types;
import window;

namespace entity {
    class Object;
}
export namespace aabb {
    class Object;

    struct Info {
        entity::Object* owner = nullptr;
        I32     id = -1;
        //RectF   rect = { 0.0F, 0.0F, 0.0F, 0.0F };
    };
    using cInfo = const Info;
}

export namespace entity {
    enum class Type {
        none = 0,
        arch_L_1x1, arch_R_1x1, arch_L_2x1_0, arch_L_2x1_1, arch_R_2x1_0, arch_R_2x1_1,
        brick, bug,
        clip, clip_duck, clip_D, clip_LD, clip_RD, clip_L, clip_R, clip_ledge, clip_ledge_L, clip_ledge_R, clip_U,
        conduit_U, conduit_D, conduit_L, conduit_R,
        conduit_LU, conduit_RU,
        conduit_LD, conduit_RD,
        conduit_UL, conduit_UR,
        conduit_DL, conduit_DR,
        frog,
        grass_0, grass_0_L_1x1, grass_0_R_1x1, grass_0_L_2x1_0, grass_0_L_2x1_1, grass_0_R_2x1_0, grass_0_R_2x1_1,
        grass_1, grass_1_L_1x1, grass_1_R_1x1, grass_1_L_2x1_0, grass_1_L_2x1_1, grass_1_R_2x1_0, grass_1_R_2x1_1,
        grass_2, grass_2_L_1x1, grass_2_R_1x1, grass_2_L_2x1_0, grass_2_L_2x1_1, grass_2_R_2x1_0, grass_2_R_2x1_1,
        level_center, level_L_0, level_R_0,
        logic_and_UD, logic_and_LR,
        logic_not_U, logic_not_D, logic_not_L, logic_not_R,
        logic_or_UD, logic_or_LR,
        particle_brick, particle_bubble, particle_down_thrust, particle_drop, particle_health, particle_interact, particle_melee, particle_sense,
        player,
        slope_L_1x1, slope_R_1x1, slope_L_2x1_0, slope_L_2x1_1, slope_R_2x1_0, slope_R_2x1_1, slope_U,
        spring_U,
        swing,
        track_U, track_D, track_L, track_R,
        track_UL, track_LD, track_DR, track_RU,
        track_UR, track_RD, track_DL, track_LU,

        track_LU_1x1, track_UL_1x1, track_LD_1x1, track_DL_1x1,
        track_RU_1x1, track_UR_1x1, track_RD_1x1, track_DR_1x1,

        track_LU_2x1_0, track_LU_2x1_1, track_LD_2x1_0, track_LD_2x1_1,
        track_RU_2x1_0, track_RU_2x1_1, track_RD_2x1_0, track_RD_2x1_1,

        track_UL_1x2_0, track_UL_1x2_1, track_UR_1x2_0, track_UR_1x2_1,
        track_DL_1x2_0, track_DL_1x2_1, track_DR_1x2_0, track_DR_1x2_1,

        train,
        trigger, track_trigger_UL, track_trigger_UR,
        water_line, water_line_L, water_line_R
    };
    using cType = const Type;

    bool is_arch(cType t) {
        return t == Type::arch_L_1x1 || t == Type::arch_L_2x1_0 || t == Type::arch_L_2x1_1 ||
            t == Type::arch_R_1x1 || t == Type::arch_R_2x1_0 || t == Type::arch_R_2x1_1;
    }
    bool is_slope(cType t) {
        return t == Type::slope_L_1x1 || t == Type::slope_L_2x1_0 || t == Type::slope_L_2x1_1 ||
            t == Type::slope_R_1x1 || t == Type::slope_R_2x1_0 || t == Type::slope_R_2x1_1 ||
            t == Type::slope_U;
    }
    bool is_clip(cType t) {
        return t == Type::clip || t == Type::clip_duck || t == Type::clip_U || t == Type::clip_D ||
            t == Type::clip_L || t == Type::clip_R || t == Type::clip_LD || t == Type::clip_RD ||
            t == Type::clip_ledge || t == Type::clip_ledge_L || t == Type::clip_ledge_R;
    }
    bool is_conduit(cType t) {
        return t == Type::conduit_U || t == Type::conduit_D || t == Type::conduit_L || t == Type::conduit_R ||
            t == Type::conduit_LU || t == Type::conduit_LD || t == Type::conduit_RU || t == Type::conduit_RD ||
            t == Type::conduit_UL || t == Type::conduit_UR || t == Type::conduit_DL || t == Type::conduit_DR;
    }

    bool is_grass_0(cType t) { return t == Type::grass_0 || t == Type::grass_0_L_1x1 || t == Type::grass_0_R_1x1 || t == Type::grass_0_L_2x1_0 || t == Type::grass_0_L_2x1_1 || t == Type::grass_0_R_2x1_0 || t == Type::grass_0_R_2x1_1; }
    bool is_grass_1(cType t) { return t == Type::grass_1 || t == Type::grass_1_L_1x1 || t == Type::grass_1_R_1x1 || t == Type::grass_1_L_2x1_0 || t == Type::grass_1_L_2x1_1 || t == Type::grass_1_R_2x1_0 || t == Type::grass_1_R_2x1_1; }
    bool is_grass_2(cType t) { return t == Type::grass_2 || t == Type::grass_2_L_1x1 || t == Type::grass_2_R_1x1 || t == Type::grass_2_L_2x1_0 || t == Type::grass_2_L_2x1_1 || t == Type::grass_2_R_2x1_0 || t == Type::grass_2_R_2x1_1; }
    bool is_grass(cType t) { return is_grass_0(t) || is_grass_1(t) || is_grass_2(t); }
    bool is_logic_and(cType t) { return t == Type::logic_and_UD || t == Type::logic_and_LR; }
    bool is_logic_not(cType t) { return t == Type::logic_not_U || t == Type::logic_not_D || t == Type::logic_not_L || t == Type::logic_not_R; }
    bool is_logic_or(cType t) { return t == Type::logic_or_UD || t == Type::logic_or_LR; }
    bool is_logic(cType t) { return is_logic_and(t) || is_logic_not(t) || is_logic_or(t); }
    bool is_particle(cType t) { return t == Type::particle_sense; }
    bool is_track(cType t) {
        return t == Type::track_U || t == Type::track_L || t == Type::track_D || t == Type::track_R ||
            t == Type::track_UR || t == Type::track_RD || t == Type::track_DL || t == Type::track_LU ||
            t == Type::track_LD || t == Type::track_DR || t == Type::track_RU || t == Type::track_UL ||
            t == Type::track_LU_1x1 || t == Type::track_UL_1x1 || t == Type::track_LD_1x1 || t == Type::track_DL_1x1 ||
            t == Type::track_RU_1x1 || t == Type::track_UR_1x1 || t == Type::track_RD_1x1 || t == Type::track_DR_1x1 ||
            t == Type::track_LU_2x1_0 || t == Type::track_LU_2x1_1 || t == Type::track_LD_2x1_0 || t == Type::track_LD_2x1_1 ||
            t == Type::track_RU_2x1_0 || t == Type::track_RU_2x1_1 || t == Type::track_RD_2x1_0 || t == Type::track_RD_2x1_1 ||
            t == Type::track_UL_1x2_0 || t == Type::track_UL_1x2_1 || t == Type::track_UR_1x2_0 || t == Type::track_UR_1x2_1 ||
            t == Type::track_DL_1x2_0 || t == Type::track_DL_1x2_1 || t == Type::track_DR_1x2_0 || t == Type::track_DR_1x2_1 ||
            t == Type::track_trigger_UL || t == Type::track_trigger_UR;
    }
    bool is_water_line(cType t) { return t == Type::water_line || t == Type::water_line_L || t == Type::water_line_R; }

    const std::map<std::string, Type> string_to_entity_type_map{
        { "arch_L_1x1"           , Type::arch_L_1x1           },
        { "arch_R_1x1"           , Type::arch_R_1x1           },
        { "arch_L_2x1_0"         , Type::arch_L_2x1_0         },
        { "arch_L_2x1_1"         , Type::arch_L_2x1_1         },
        { "arch_R_2x1_0"         , Type::arch_R_2x1_0         },
        { "arch_R_2x1_1"         , Type::arch_R_2x1_1         },
        { "brick"                , Type::brick                },
        { "bug"                  , Type::bug                  },
        { "clip"                 , Type::clip                 },
        { "clip_duck"            , Type::clip_duck            },
        { "clip_L"               , Type::clip_L               },
        { "clip_R"               , Type::clip_R               },
        { "clip_D"               , Type::clip_D               },
        { "clip_LD"              , Type::clip_LD              },
        { "clip_RD"              , Type::clip_RD              },
        { "clip_ledge"           , Type::clip_ledge           },
        { "clip_ledge_L"         , Type::clip_ledge_L         },
        { "clip_ledge_R"         , Type::clip_ledge_R         },
        { "clip_U"               , Type::clip_U               },
        { "conduit_U"            , Type::conduit_U            },
        { "conduit_D"            , Type::conduit_D            },
        { "conduit_L"            , Type::conduit_L            },
        { "conduit_R"            , Type::conduit_R            },
        { "conduit_LU"           , Type::conduit_LU           },
        { "conduit_RU"           , Type::conduit_RU           },
        { "conduit_LD"           , Type::conduit_LD           },
        { "conduit_RD"           , Type::conduit_RD           },
        { "conduit_UL"           , Type::conduit_UL           },
        { "conduit_UR"           , Type::conduit_UR           },
        { "conduit_DL"           , Type::conduit_DL           },
        { "conduit_DR"           , Type::conduit_DR           },
        { "frog"                 , Type::frog                 },
        { "grass_0"              , Type::grass_0              },
        { "grass_0_L_1x1"        , Type::grass_0_L_1x1        },
        { "grass_0_R_1x1"        , Type::grass_0_R_1x1        },
        { "grass_0_L_2x1_0"      , Type::grass_0_L_2x1_0      },
        { "grass_0_L_2x1_1"      , Type::grass_0_L_2x1_1      },
        { "grass_0_R_2x1_0"      , Type::grass_0_R_2x1_0      },
        { "grass_0_R_2x1_1"      , Type::grass_0_R_2x1_1      },
        { "grass_1"              , Type::grass_1              },
        { "grass_1_L_1x1"        , Type::grass_1_L_1x1        },
        { "grass_1_R_1x1"        , Type::grass_1_R_1x1        },
        { "grass_1_L_2x1_0"      , Type::grass_1_L_2x1_0      },
        { "grass_1_L_2x1_1"      , Type::grass_1_L_2x1_1      },
        { "grass_1_R_2x1_0"      , Type::grass_1_R_2x1_0      },
        { "grass_1_R_2x1_1"      , Type::grass_1_R_2x1_1      },
        { "grass_2"              , Type::grass_2              },
        { "grass_2_L_1x1"        , Type::grass_2_L_1x1        },
        { "grass_2_R_1x1"        , Type::grass_2_R_1x1        },
        { "grass_2_L_2x1_0"      , Type::grass_2_L_2x1_0      },
        { "grass_2_L_2x1_1"      , Type::grass_2_L_2x1_1      },
        { "grass_2_R_2x1_0"      , Type::grass_2_R_2x1_0      },
        { "grass_2_R_2x1_1"      , Type::grass_2_R_2x1_1      },
        { "level_center"         , Type::level_center         },
        { "level_L_0"            , Type::level_L_0            },
        { "level_R_0"            , Type::level_R_0            },
        { "logic_and_UD"         , Type::logic_and_UD         },
        { "logic_and_LR"         , Type::logic_and_LR         },
        { "logic_not_U"          , Type::logic_not_U          },
        { "logic_not_D"          , Type::logic_not_D          },
        { "logic_not_L"          , Type::logic_not_L          },
        { "logic_not_R"          , Type::logic_not_R          },
        { "logic_or_UD"          , Type::logic_or_UD          },
        { "logic_or_LR"          , Type::logic_or_LR          },

        { "particle_brick"       , Type::particle_brick       },
        { "particle_bubble"      , Type::particle_bubble      },
        { "particle_down_thrust" , Type::particle_down_thrust },
        { "particle_drop"        , Type::particle_drop        },
        { "particle_health"      , Type::particle_health      },
        { "particle_interact"    , Type::particle_interact    },
        { "particle_melee"       , Type::particle_melee       },
        { "particle_sense"       , Type::particle_sense       },

        { "player"               , Type::player               },
        { "slope_L_1x1"          , Type::slope_L_1x1          },
        { "slope_L_2x1_0"        , Type::slope_L_2x1_0        },
        { "slope_L_2x1_1"        , Type::slope_L_2x1_1        },
        { "slope_R_1x1"          , Type::slope_R_1x1          },
        { "slope_R_2x1_0"        , Type::slope_R_2x1_0        },
        { "slope_R_2x1_1"        , Type::slope_R_2x1_1        },
        { "spring_U"             , Type::spring_U             },
        { "slope_U"              , Type::slope_U              },
        { "swing"                , Type::swing                },
        { "train"                , Type::train                },

        { "track_U"              , Type::track_U              },
        { "track_D"              , Type::track_D              },
        { "track_L"              , Type::track_L              },
        { "track_R"              , Type::track_R              },

        { "track_UR"             , Type::track_UR             },
        { "track_RD"             , Type::track_RD             },
        { "track_DL"             , Type::track_DL             },
        { "track_LU"             , Type::track_LU             },
        { "track_LD"             , Type::track_LD             },
        { "track_DR"             , Type::track_DR             },
        { "track_RU"             , Type::track_RU             },
        { "track_UL"             , Type::track_UL             },

        { "track_LU_1x1"         , Type::track_LU_1x1         },
        { "track_UL_1x1"         , Type::track_UL_1x1         },
        { "track_LD_1x1"         , Type::track_LD_1x1         },
        { "track_DL_1x1"         , Type::track_DL_1x1         },
        { "track_RU_1x1"         , Type::track_RU_1x1         },
        { "track_UR_1x1"         , Type::track_UR_1x1         },
        { "track_RD_1x1"         , Type::track_RD_1x1         },
        { "track_DR_1x1"         , Type::track_DR_1x1         },

        { "track_LU_2x1_0"       , Type::track_LU_2x1_0       },
        { "track_LU_2x1_1"       , Type::track_LU_2x1_1       },
        { "track_LD_2x1_0"       , Type::track_LD_2x1_0       },
        { "track_LD_2x1_1"       , Type::track_LD_2x1_1       },

        { "track_RU_2x1_0"       , Type::track_RU_2x1_0       },
        { "track_RU_2x1_1"       , Type::track_RU_2x1_1       },
        { "track_RD_2x1_0"       , Type::track_RD_2x1_0       },
        { "track_RD_2x1_1"       , Type::track_RD_2x1_1       },

        { "track_UL_1x2_0"       , Type::track_UL_1x2_0       },
        { "track_UL_1x2_1"       , Type::track_UL_1x2_1       },
        { "track_UR_1x2_0"       , Type::track_UR_1x2_0       },
        { "track_UR_1x2_1"       , Type::track_UR_1x2_1       },

        { "track_DL_1x2_0"       , Type::track_DL_1x2_0       },
        { "track_DL_1x2_1"       , Type::track_DL_1x2_1       },
        { "track_DR_1x2_0"       , Type::track_DR_1x2_0       },
        { "track_DR_1x2_1"       , Type::track_DR_1x2_1       },

        { "trigger"              , Type::trigger              },
        { "track_trigger_UL"     , Type::track_trigger_UL     },
        { "track_trigger_UR"     , Type::track_trigger_UR     },

        { "water_line"           , Type::water_line           },
        { "water_line_L"         , Type::water_line_L         },
        { "water_line_R"         , Type::water_line_R         }
    };
    cType string_to_type(const std::string& s) {
        if (string_to_entity_type_map.find(s) != string_to_entity_type_map.end()) {
            return string_to_entity_type_map.at(s);
        }
        return Type::none;
    }
    const std::string to_string(cType type) {
        switch (type) {
        case Type::arch_L_1x1:           return "arch_L_1x1";
        case Type::arch_R_1x1:           return "arch_R_1x1";
        case Type::arch_L_2x1_0:         return "arch_L_2x1_0";
        case Type::arch_L_2x1_1:         return "arch_L_2x1_1";
        case Type::arch_R_2x1_0:         return "arch_R_2x1_0";
        case Type::arch_R_2x1_1:         return "arch_R_2x1_1";
        case Type::brick:                return "brick";
        case Type::bug:                  return "bug";
        case Type::clip:                 return "clip";
        case Type::clip_duck:            return "clip_duck";
        case Type::clip_L:               return "clip_L";
        case Type::clip_R:               return "clip_R";
        case Type::clip_D:               return "clip_D";
        case Type::clip_LD:              return "clip_LD";
        case Type::clip_RD:              return "clip_RD";
        case Type::clip_ledge:           return "clip_ledge";
        case Type::clip_ledge_L:         return "clip_ledge_L";
        case Type::clip_ledge_R:         return "clip_ledge_R";
        case Type::clip_U:               return "clip_U";
        case Type::conduit_U:            return "conduit_U";
        case Type::conduit_D:            return "conduit_D";
        case Type::conduit_L:            return "conduit_L";
        case Type::conduit_R:            return "conduit_R";
        case Type::conduit_LU:           return "conduit_LU";
        case Type::conduit_RU:           return "conduit_RU";
        case Type::conduit_LD:           return "conduit_LD";
        case Type::conduit_RD:           return "conduit_RD";
        case Type::conduit_UL:           return "conduit_UL";
        case Type::conduit_UR:           return "conduit_UR";
        case Type::conduit_DL:           return "conduit_DL";
        case Type::conduit_DR:           return "conduit_DR";
        case Type::frog:                 return "frog";
        case Type::grass_0:              return "grass_0";
        case Type::grass_0_L_1x1:        return "grass_0_L_1x1";
        case Type::grass_0_R_1x1:        return "grass_0_R_1x1";
        case Type::grass_0_L_2x1_0:      return "grass_0_L_2x1_0";
        case Type::grass_0_L_2x1_1:      return "grass_0_L_2x1_1";
        case Type::grass_0_R_2x1_0:      return "grass_0_R_2x1_0";
        case Type::grass_0_R_2x1_1:      return "grass_0_R_2x1_1";
        case Type::grass_1:              return "grass_1";
        case Type::grass_1_L_1x1:        return "grass_1_L_1x1";
        case Type::grass_1_R_1x1:        return "grass_1_R_1x1";
        case Type::grass_1_L_2x1_0:      return "grass_1_L_2x1_0";
        case Type::grass_1_L_2x1_1:      return "grass_1_L_2x1_1";
        case Type::grass_1_R_2x1_0:      return "grass_1_R_2x1_0";
        case Type::grass_1_R_2x1_1:      return "grass_1_R_2x1_1";
        case Type::grass_2:              return "grass_2";
        case Type::grass_2_L_1x1:        return "grass_2_L_1x1";
        case Type::grass_2_R_1x1:        return "grass_2_R_1x1";
        case Type::grass_2_L_2x1_0:      return "grass_2_L_2x1_0";
        case Type::grass_2_L_2x1_1:      return "grass_2_L_2x1_1";
        case Type::grass_2_R_2x1_0:      return "grass_2_R_2x1_0";
        case Type::grass_2_R_2x1_1:      return "grass_2_R_2x1_1";
        case Type::level_center:         return "level_center";
        case Type::level_L_0:            return "level_L_0";
        case Type::level_R_0:            return "level_R_0";
        case Type::logic_and_UD:         return "logic_and_UD";
        case Type::logic_and_LR:         return "logic_and_LR";
        case Type::logic_not_U:          return "logic_not_U";
        case Type::logic_not_D:          return "logic_not_D";
        case Type::logic_not_L:          return "logic_not_L";
        case Type::logic_not_R:          return "logic_not_R";
        case Type::logic_or_UD:          return "logic_or_UD";
        case Type::logic_or_LR:          return "logic_or_LR";

        case Type::particle_brick:       return "particle_brick";
        case Type::particle_bubble:      return "particle_bubble";
        case Type::particle_down_thrust: return "particle_down_thrust";
        case Type::particle_drop:        return "particle_drop";
        case Type::particle_health:      return "particle_health";
        case Type::particle_interact:    return "particle_interact";
        case Type::particle_melee:       return "particle_melee";
        case Type::particle_sense:       return "particle_sense";

        case Type::player:               return "player";
        case Type::slope_L_1x1:          return "slope_L_1x1";
        case Type::slope_R_1x1:          return "slope_R_1x1";
        case Type::slope_L_2x1_0:        return "slope_L_2x1_0";
        case Type::slope_L_2x1_1:        return "slope_L_2x1_1";
        case Type::slope_R_2x1_0:        return "slope_R_2x1_0";
        case Type::slope_R_2x1_1:        return "slope_R_2x1_1";
        case Type::slope_U:              return "slope_U";
        case Type::spring_U:             return "spring_U";
        case Type::swing:                return "swing";

        case Type::track_U:              return "track_U";
        case Type::track_D:              return "track_D";
        case Type::track_L:              return "track_L";
        case Type::track_R:              return "track_R";

        case Type::track_UR:             return "track_UR";
        case Type::track_RD:             return "track_RD";
        case Type::track_DL:             return "track_DL";
        case Type::track_LU:             return "track_LU";
        case Type::track_LD:             return "track_LD";
        case Type::track_DR:             return "track_DR";
        case Type::track_RU:             return "track_RU";
        case Type::track_UL:             return "track_UL";

        case Type::track_LU_1x1:         return "track_LU_1x1";
        case Type::track_UL_1x1:         return "track_UL_1x1";
        case Type::track_LD_1x1:         return "track_LD_1x1";
        case Type::track_DL_1x1:         return "track_DL_1x1";

        case Type::track_RU_1x1:         return "track_RU_1x1";
        case Type::track_UR_1x1:         return "track_UR_1x1";
        case Type::track_RD_1x1:         return "track_RD_1x1";
        case Type::track_DR_1x1:         return "track_DR_1x1";

        case Type::track_LU_2x1_0:       return "track_LU_2x1_0";
        case Type::track_LU_2x1_1:       return "track_LU_2x1_1";
        case Type::track_LD_2x1_0:       return "track_LD_2x1_0";
        case Type::track_LD_2x1_1:       return "track_LD_2x1_1";

        case Type::track_RU_2x1_0:       return "track_RU_2x1_0";
        case Type::track_RU_2x1_1:       return "track_RU_2x1_1";
        case Type::track_RD_2x1_0:       return "track_RD_2x1_0";
        case Type::track_RD_2x1_1:       return "track_RD_2x1_1";

        case Type::track_UL_1x2_0:       return "track_UL_1x2_0";
        case Type::track_UL_1x2_1:       return "track_UL_1x2_1";
        case Type::track_UR_1x2_0:       return "track_UR_1x2_0";
        case Type::track_UR_1x2_1:       return "track_UR_1x2_1";

        case Type::track_DL_1x2_0:       return "track_DL_1x2_0";
        case Type::track_DL_1x2_1:       return "track_DL_1x2_1";
        case Type::track_DR_1x2_0:       return "track_DR_1x2_0";
        case Type::track_DR_1x2_1:       return "track_DR_1x2_1";

        case Type::train:                return "train";
        case Type::trigger:              return "trigger";
        case Type::track_trigger_UL:     return "track_trigger_UL";
        case Type::track_trigger_UR:     return "track_trigger_UR";

        case Type::water_line:           return "water_line";
        case Type::water_line_L:         return "water_line_L";
        case Type::water_line_R:         return "water_line_R";
        default:                         return "";
        }
    }
    enum class State {
        none = 0,
        blocked, bounce, dead, hurt, idle, jump, melee, run, stunned, swim, swing, tossed, upended, walk
    };
    using cState = const State;
    const std::string to_string(cState s) {
        switch (s) {
        case State::blocked:    return "blocked";
        case State::bounce:     return "bounce";
        case State::hurt:       return "hurt";
        case State::idle:       return "idle";
        case State::jump:       return "jump";
        case State::melee:      return "melee";
        case State::run:        return "run";
        case State::stunned:    return "stunned";
        case State::swim:       return "swim";
        case State::swing:      return "swing";
        case State::tossed:     return "tossed";
        case State::upended:    return "upended";
        case State::walk:       return "walk";
        default:                return "";       }
    }
    
    class Object {
    protected:
        Object* m_parent = nullptr;

        Type m_type = Type::none;
        U8 m_number = 0;

        State m_state      = State::none,
              m_next_state = State::none,
              m_prev_state = State::none;

        I32 m_transform_id = -1,
            m_sprite_id    = -1;

        I32 m_health_id = -1;

        I32 m_current_anim_id = -1,
            m_bounce_anim_id  = -1,
            m_dead_anim_id    = -1,
            m_hurt_anim_id    = -1,
            m_idle_anim_id    = -1,
            m_jump_anim_id    = -1,
            m_melee_anim_id   = -1,
            m_stunned_anim_id = -1,
            m_upended_anim_id = -1;

        I32 m_block_sound_id       = -1,
            m_bounce_sound_id      = -1,
            m_bump_head_sound_id   = -1,
            m_dead_sound_id        = -1,
            m_hurt_sound_id        = -1,
            m_melee_sound_id       = -1,
            m_water_enter_sound_id = -1,
            m_water_exit_sound_id  = -1;

        std::vector<I32> m_aabb_ids;

        std::list<Object*> m_sensed_objects;

        U16 m_time_left_alive          =  U16_MAX, m_time_to_be_alive    = U16_MAX,
            m_time_left_blocked        =  0,
            m_time_left_dead           =  0,       m_time_to_be_dead     = 0,
            m_time_left_hurt           =  0,       m_time_to_hurt        = 10,
            m_time_left_in_state       =  0,
            m_time_in_state            =  0,
            m_time_left_interacting    =  0,       m_time_to_interact    = 10,
            m_time_left_to_spawn_sense =  0,       m_time_to_spawn_sense = 10,
            m_time_left_to_react       =  0;

        Vec2F m_start_offset      = { 0.0F, 0.0F };

        U8 m_num_jumps   = 0,
           m_start_layer = 0;        

        Vec2F m_direction = { 0.0F, 0.0F };

        bool m_is_first_update       = true,
             m_is_first_state_update = true;

        bool m_is_carrying    = false,
             m_is_ducking     = false,
             m_is_near_wall_L = false,
             m_is_near_wall_R = false,
             m_is_on_ground   = false,
             m_is_on_slope    = false,
             m_is_powered     = false;

        U16 m_input_limit = U16_MAX;

        F32 m_water_line_y = 0.0F;
        F32 m_rotation_speed = 1.0F;
    public:        
        I32 level_transform_id = -1;
        RectI start_source_rect = { 0, 0, 16, 16 };
        Object* parent() const { return m_parent; } void parent(Object* p) { m_parent = p; }

        virtual ~Object();

        virtual bool is_all_valid() {
            return m_sprite_id != -1 && sprite() && m_current_anim_id != -1 && anim::Manager::get(m_current_anim_id) && m_transform_id != -1 && transform::Manager::get(m_transform_id);
        }
        virtual bool load_config(const std::filesystem::path& path);
        virtual void collide_x(aabb::cInfo our, aabb::cInfo other) {}
        virtual void collide_y(aabb::cInfo our, aabb::cInfo other) {}

        //virtual void hurt(cType type, cVec2F velocity) {}
        virtual void hurt(Object* culprit) {}
        virtual void interact(Object* object) {}
        virtual void update() {
            if (!is_all_valid()) return;
            //anim::Manager::get(m_current_anim_id)->update();
            sprite()->source_rect.x = anim::Manager::get(m_current_anim_id)->source.x;
            sprite()->source_rect.y = anim::Manager::get(m_current_anim_id)->source.y;

            if (m_time_left_dead > 0) {
                //Console::log("m_time_left_dead: ", (int)m_time_left_dead, "\n");
                if (m_time_left_dead != U16_MAX) {
                    --m_time_left_dead;
                }
                if (m_current_anim_id != m_dead_anim_id) {
                    m_current_anim_id = m_dead_anim_id;
                    if (anim::Manager::get(m_dead_anim_id)) {
                        anim::Manager::get(m_dead_anim_id)->source.x = anim::Manager::get(m_dead_anim_id)->first_frame;
                    }
                }
            } else {
                if (m_time_left_alive != U16_MAX) {
                    --m_time_left_alive;
                }
                if (m_current_anim_id != m_idle_anim_id) {
                    m_current_anim_id = m_idle_anim_id;
                    if (anim::Manager::get(m_idle_anim_id)) {
                        anim::Manager::get(m_idle_anim_id)->source.x = anim::Manager::get(m_idle_anim_id)->first_frame;
                    }
                }
            }
        }
        virtual void draw(std::unique_ptr<Window>& window) {
            window->draw(sprite::Manager::get(m_sprite_id));
        }
        void draw_aabb(std::unique_ptr<Window>& window);

        aabb::Object* aabb_object(aabb::cName name);

        bool add_input_object(Object* object) {
            if (input_objects.size() >= m_input_limit) {
                return false;
            }
            if (std::find(input_objects.begin(), input_objects.end(), object) != input_objects.end()) {
                return false;
            }
            if (object->is_powered()) {
                m_is_powered = true;
            }
            input_objects.emplace_back(object);
            return true;
        }
        std::vector<Object*> input_objects;

        cU16 num_inputs() const { return input_objects.size(); }

        bool add_sensed_object(Object* object) {            
            if (std::find(m_sensed_objects.begin(), m_sensed_objects.end(), object) != m_sensed_objects.end()) {
                return false;
            }
            m_sensed_objects.emplace_back(object);
            return true;
        }

        bool is_blocked()     const { return m_time_left_blocked > 0; }
        bool is_carrying()    const { return m_is_carrying; }               void is_carrying(bool is) { m_is_carrying = is; }
        bool is_dead()        const { return m_time_left_dead > 0; }
        bool is_ducking()     const { return m_is_ducking; }
        bool is_hurting()     const { return m_time_left_hurt > 0; }
        bool is_near_wall_L() const { return m_is_near_wall_L; }
        bool is_near_wall_R() const { return m_is_near_wall_R; }
        bool is_on_ground()   const { return m_is_on_ground; }              void is_on_ground(bool is) { m_is_on_ground = is; }
        bool is_interacting() const { return m_time_left_interacting > 0; }
        bool is_powered()     const { return m_is_powered; }                void is_powered(bool is) { m_is_powered = is; }

        cType  type()   const { return m_type;   } void type(cType t)   { m_type = t;   }
        cU8    number() const { return m_number; } void number(cU8 n)   { m_number = n; }
        cState state()  const { return m_state;  } void state(cState s) { m_state = s;  }

        cVec2F position() const { return transform::Manager::get(m_transform_id) ? transform::Manager::get(m_transform_id)->position : Vec2F{ 0.0F,0.0F }; }  void position(cVec2F p) { if (transform::Manager::get(m_transform_id)) transform::Manager::get(m_transform_id)->position = p; }
        cF32   x()        const { return transform::Manager::get(m_transform_id) ? transform::Manager::get(m_transform_id)->position.x : 0.0F; }              void x(cF32 in_x) { if (transform::Manager::get(m_transform_id)) transform::Manager::get(m_transform_id)->position.x = in_x; }
        cF32   y()        const { return transform::Manager::get(m_transform_id) ? transform::Manager::get(m_transform_id)->position.y : 0.0F; }              void y(cF32 in_y) { if (transform::Manager::get(m_transform_id)) transform::Manager::get(m_transform_id)->position.y = in_y; }
        cVec2F velocity() const { return transform::Manager::get(m_transform_id) ? transform::Manager::get(m_transform_id)->velocity : Vec2F{ 0.0F, 0.0F }; } void velocity(cVec2F v) { if (transform::Manager::get(m_transform_id)) transform::Manager::get(m_transform_id)->velocity = v; }

        cVec2F position_on_level() const { return Camera::position + position(); }

        cF32 rotation_speed() const { return m_rotation_speed; }

        cVec2F direction() const { return m_direction; }    void direction(cVec2F d) { m_direction = d; }

        cVec2F start_offset() const { return m_start_offset; } void start_offset(cVec2F offset) { m_start_offset = offset; }
        cU8    start_layer()  const { return m_start_layer;  } void start_layer(cU8 layer)      { m_start_layer = layer;   }

        cU16 time_left_dead()   const { return m_time_left_dead;   } void time_left_dead(cU16 t) { m_time_left_dead = t;   }
        cU16 time_left_alive()  const { return m_time_left_alive;  } void time_left_alive(cU16 t) { m_time_left_alive = t; }
        cU16 time_to_be_alive() const { return m_time_to_be_alive; } void time_to_be_alive(cU16 t) { m_time_to_be_alive = t; }

        cU16 time_left_hurt() const { return m_time_left_hurt; }    void time_left_hurt(cU16 t) { m_time_left_hurt = t; }
        cU16 time_to_hurt() const { return m_time_to_hurt; }

        cU16 time_left_interacting() const { return m_time_left_interacting; } void time_left_interacting(cU16 t) { m_time_left_interacting = t; }
        cU16 time_left_in_State()    const { return m_time_left_in_state;    } void time_left_in_state(cU16 t)    { m_time_left_in_state = t;    }

        anim::Object* anim() { return anim::Manager::get(m_current_anim_id); }

        void next_state(cState state) { m_next_state = state; }

        void set_anim(cI32 anim_id) {
            if (m_current_anim_id == anim_id || anim_id == -1) return;
            m_current_anim_id = anim_id;
        }
        void reset_anim(cI32 anim_id) {
            if (anim_id == -1) return;
            //if (m_current_anim_id == anim_id) return;
            m_current_anim_id = anim_id;
            if (anim::Manager::get(anim_id)) {
                anim::Manager::get(anim_id)->loop = 0;
                anim::Manager::get(anim_id)->source.x = 0;
            }
        }

        //cI32 sprite_id()    const { return m_sprite_id;    }    void sprite_id(cI32 id)    { m_sprite_id = id;    }
        cI32 transform_id() const { return m_transform_id; }    void transform_id(cI32 id) { m_transform_id = id; }
        //cI32 current_anim_id() const { return m_current_anim_id; }

        transform::Object* transform() { return transform::Manager::is_valid(m_transform_id) ? transform::Manager::get(m_transform_id) : nullptr; }
        sprite::Object* sprite() { return sprite::Manager::is_valid(m_sprite_id) ? sprite::Manager::get(m_sprite_id) : nullptr; }
        health::Object* health() { return health::Manager::is_valid(m_health_id) ? health::Manager::get(m_health_id) : nullptr; }

        std::vector<I32>& aabb_ids() { return m_aabb_ids; }

        void set_layer(cU8 layer) { sprite::Manager::get(m_sprite_id)->layer = layer; }

        cRectI source_rect() const { return sprite::Manager::get(m_sprite_id) ? sprite::Manager::get(m_sprite_id)->source_rect : RectI{ 0, 0, 0, 0 }; }

        cU8 num_jumps() const { return m_num_jumps; }

        void spawn(particle::cType type, cVec2F position, cVec2F velocity);        
        void spawn_fan(cF32 start_angle, cF32 end_angle, cU8 num, particle::cType type, cVec2F position, cVec2F velocity, cF32 speed);
    };
}