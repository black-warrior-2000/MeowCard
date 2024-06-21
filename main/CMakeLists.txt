set(src_dirs .
            APP)

set(inc_dirs .
            APP)


#idf_component_register(SRCS "main.c"
#                   INCLUDE_DIRS ".")

idf_component_register(SRC_DIRS ${src_dirs} INCLUDE_DIRS ${inc_dirs})


# idf_component_register(
#     SRC_DIRS 
#         "." 
#         "APP" 
#     INCLUDE_DIRS 
#         "."
#         "APP" )