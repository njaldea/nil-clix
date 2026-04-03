project(sandbox)

set(ENABLE_SANDBOX  OFF CACHE BOOL "[0 | OFF - 1 | ON]: build sandbox?")
if(NOT ENABLE_SANDBOX)
    return()
endif()

add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE clix)

if(ENABLE_C_API)
    add_executable(${PROJECT_NAME}-c main.c)
    target_link_libraries(${PROJECT_NAME}-c PRIVATE clix-c-api)
endif()
