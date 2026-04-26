project(clix)

find_package(Boost COMPONENTS program_options REQUIRED)

set(
    HEADERS
        publish/nil/clix.hpp
        publish/nil/clix/node.hpp
        publish/nil/clix/options.hpp
        publish/nil/clix/conf/Flag.hpp
        publish/nil/clix/conf/Number.hpp
        publish/nil/clix/conf/Param.hpp
        publish/nil/clix/conf/Params.hpp
        publish/nil/clix/prebuilt/Help.hpp
)

set(
    SOURCES
        src/structs.hpp
        src/node.cpp
        src/options.cpp
        src/prebuilt/Help.cpp
)

add_library(${PROJECT_NAME} SHARED ${SOURCES} ${HEADERS})

target_link_libraries(${PROJECT_NAME} PRIVATE $<BUILD_INTERFACE:Boost::program_options>)

add_test_subdirectory()

install(PROGRAMS nil-clix-fzf DESTINATION ${CMAKE_INSTALL_BINDIR})

nil_install_headers(${PROJECT_NAME} PUBLIC)
nil_install_targets(${PROJECT_NAME})

if(ENABLE_C_API)
    find_package(nil-xalt CONFIG REQUIRED)

    add_library(
        ${PROJECT_NAME}-c-api
        SHARED
        publish/nil/clix.h
        src/c/clix.cpp
    )

    target_link_libraries(${PROJECT_NAME}-c-api PRIVATE ${PROJECT_NAME})
    target_link_libraries(${PROJECT_NAME}-c-api PRIVATE $<BUILD_INTERFACE:nil::xalt>)

    nil_install_headers(${PROJECT_NAME}-c-api INTERFACE)
    nil_install_targets(${PROJECT_NAME}-c-api)
endif()