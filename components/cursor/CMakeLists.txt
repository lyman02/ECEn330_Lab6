idf_component_register(SRCS "cursor.c"
                       INCLUDE_DIRS "."
                       PRIV_REQUIRES lcd joy)
# target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-format")
