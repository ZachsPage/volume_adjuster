set(COMPONENT_SRCS "src/MyBluetooth.cpp")
set(COMPONENT_ADD_INCLUDEDIRS "${CMAKE_CURRENT_LIST_DIR}/include")

set(COMPONENT_REQUIRES bt nvs_flash)

register_component()