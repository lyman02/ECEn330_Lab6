idf_component_register(SRCS "main.c" "gameControl.c" "missile.c" "plane.c"
                       INCLUDE_DIRS "." "../../components"
                       )
# PRIV_REQUIRES "..."
# target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-format")
