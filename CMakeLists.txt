cmake_minimum_required(VERSION 3.19)
project(projekt LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets)

qt_standard_project_setup()

qt_add_executable(projekt
    WIN32 MACOSX_BUNDLE
    main.cpp
    algovisualizer.cpp
    algovisualizer.h
    algovisualizer.ui
    sorter.h sorter.cpp
    avl_tree.h avl_tree.cpp
    rb_tree.h rb_tree.cpp
    graph.h graph.cpp
    maze.h maze.cpp
)

target_link_libraries(projekt
    PRIVATE
        Qt::Core
        Qt::Widgets
)

include(GNUInstallDirs)

install(TARGETS projekt
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET projekt
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
