project(clix)

find_package(Boost COMPONENTS program_options REQUIRED)

set(
    HEADERS
        publish/nil/clix.hpp
        publish/nil/clix/Node.hpp
        publish/nil/clix/Options.hpp
        publish/nil/clix/IOption.hpp
        publish/nil/clix/conf/Flag.hpp
        publish/nil/clix/conf/Number.hpp
        publish/nil/clix/conf/Param.hpp
        publish/nil/clix/conf/Params.hpp
        publish/nil/clix/prebuilt/Help.hpp
        src/Option.hpp
        src/OptionsImpl.hpp
)

set(
    SOURCES
        src/Node.cpp
        src/Option.cpp
        src/Options.cpp
        src/OptionsImpl.cpp
        src/prebuilt/Help.cpp
)

add_library(${PROJECT_NAME} ${SOURCES} ${HEADERS})

target_link_libraries(${PROJECT_NAME} PRIVATE Boost::program_options)

target_include_directories(
    ${PROJECT_NAME}
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/publish>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

add_test_subdirectory()
install_for_package(${PROJECT_NAME})
