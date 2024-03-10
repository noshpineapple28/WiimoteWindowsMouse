include_directories(../lib)
set(SOURCES
main.h
main.c)
add_executable(wiimote_windows_mouse ${SOURCES})
target_link_libraries(wiimote_windows_mouse wiiuse)

if(INSTALL_MANAGER)
    install(TARGETS wiimote_windows_mouse
        RUNTIME DESTINATION bin COMPONENT manager)
endif()