add_executable(${PROJECT_NAME}-standalone driver.cpp)
add_library(${PROJECT_NAME} SHARED plugin.cpp)

foreach(target ${PROJECT_NAME} ${PROJECT_NAME}-standalone)
  target_link_libraries(${target}
    PRIVATE
    clangAST
    clangBasic
    clangFrontend
    clangSerialization
    clangTooling
    clangIndex
    clangRewrite
    clangTooling
    )
endforeach()

install(TARGETS ${PROJECT_NAME} DESTINATION lib)
install(TARGETS ${PROJECT_NAME}-standalone DESTINATION bin)
