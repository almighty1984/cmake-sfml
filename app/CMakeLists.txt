add_library(app)
target_compile_features(app PRIVATE cxx_std_23)
target_sources(app PRIVATE
               "src/states/edit/edit.cxx"
               "src/states/edit/edit_grid.cxx"
               "src/states/edit/edit_level.cxx"
               "src/states/edit/edit_set.cxx"
               "src/states/edit/edit_update.cxx"
               PUBLIC FILE_SET CXX_MODULES
               FILES
                   "src/app.ixx"
                   "src/player.ixx"
                   "src/states/edit.ixx"
                   "src/states/game_init.ixx")
target_link_libraries(app PRIVATE core)