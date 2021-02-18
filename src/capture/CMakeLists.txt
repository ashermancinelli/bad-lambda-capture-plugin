#[[#############################################################################

LambdaChecker target definitions and linked libraries. CMake targets are
exported to ${CMAKE_INSTALL_PREFIX}/lib/cmake/LambdaChecker.

]]##############################################################################

add_executable(${PROJECT_NAME}Standalone driver.cpp)
add_library(${PROJECT_NAME} SHARED plugin.cpp)

foreach(target ${PROJECT_NAME} ${PROJECT_NAME}Standalone)
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

install(
  TARGETS ${PROJECT_NAME} 
  DESTINATION lib
  EXPORT LambdaChecker
  )
install(
  TARGETS ${PROJECT_NAME}Standalone 
  DESTINATION bin
  EXPORT LambdaChecker
  )

install(EXPORT LambdaChecker
  FILE LambdaChecker.cmake
  NAMESPACE LambdaChecker::
  DESTINATION lib/cmake/LambdaChecker
  )
