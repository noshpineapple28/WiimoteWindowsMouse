if(NOT MSVC)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra")
endif()

set(SOURCES
    classic.c
    dynamics.c
    events.c
    guitar_hero_3.c
    io.c
    ir.c
    nunchuk.c
    wiiuse.c
    wiiboard.c
    classic.h
    definitions.h
    definitions_os.h
    dynamics.h
    events.h
    guitar_hero_3.h
    motion_plus.h
    motion_plus.c
    io.h
    ir.h
    nunchuk.h
    os.h
    util.c
    wiiuse_internal.h
    wiiboard.h)
set(API
    wiiuse.h)

list(APPEND SOURCES os_win.c)
set(CMAKE_DEBUG_POSTFIX _debug)

if(MSVC)
    list(APPEND API wiiuse_msvcstdint.h)
endif()

add_definitions(-DWIIUSE_COMPILE_LIB)

add_library(wiiuse ${SOURCES} ${API})

if(WIN32)
    target_link_libraries(wiiuse ws2_32 setupapi ${WINHID_LIBRARIES})
endif()

set_property(TARGET
    wiiuse
    PROPERTY
    PUBLIC_HEADER
    ${API})

set_property(TARGET wiiuse APPEND PROPERTY
  COMPILE_DEFINITIONS $<$<CONFIG:Debug>:WITH_WIIUSE_DEBUG>
)

set_target_properties(wiiuse PROPERTIES
    SOVERSION ${PROJECT_VERSION_MAJOR}
    VERSION ${PROJECT_VERSION})

install(TARGETS wiiuse
    RUNTIME DESTINATION bin
    COMPONENT runtime
    ARCHIVE
    DESTINATION
    ${CMAKE_INSTALL_LIBDIR}
    COMPONENT
    development
    LIBRARY
    DESTINATION
    ${CMAKE_INSTALL_LIBDIR}
    COMPONENT
    runtime
    PUBLIC_HEADER
    DESTINATION
    include
    COMPONENT
    development)
