idf_component_register(SRCS "main.cpp"
                            "types.cpp"
                            "lvgl/lvgl.cpp"
                            "espNow/espNow.cpp"
                            "UI/UI.cpp"
                            INCLUDE_DIRS "." "lvgl" "UI" "espNow")
