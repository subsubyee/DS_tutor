cmake_minimum_required(VERSION 3.19)
project(tutor LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets)

qt_standard_project_setup()

qt_add_executable(tutor
    WIN32 MACOSX_BUNDLE
    main.cpp
    stack.cpp
    stack.h
    stack.ui
    queue.cpp
    queue.h
    mainwindow.cpp
    mainwindow.h
    deque.cpp
    deque.h
    linked_stack.cpp
    linked_stack.h
    circular_linked.h
    circular_linked.cpp
    node.h
    doubly_linked.cpp
    doubly_linked.h
    tree_bst.h
    tree_bst.cpp
    tree_bst_quiz.h
    tree_bst_quiz.cpp
    tree_avl.h
    tree_avl.cpp
    tree_avl_insert_quiz.h
    tree_avl_insert_quiz.cpp
    tree_avl_delete_quiz.h
    tree_avl_delete_quiz.cpp
    hash.h
    hash.cpp
    tree_sort.h
    tree_sort.cpp
    bubble_sort.h
    bubble_sort.cpp
    insertion_sort.cpp
    insertion_sort.h
    selection_sort.h
    selection_sort.cpp
    shell_sort.h
    shell_sort.cpp
    merge_sort.h
    merge_sort.cpp
    quick_sort.h
    quick_sort.cpp
    radix_sort.h
    radix_sort.cpp

)

target_link_libraries(tutor
    PRIVATE
        Qt::Core
        Qt::Widgets
)

install(TARGETS tutor
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET tutor
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
