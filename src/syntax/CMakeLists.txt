#[[#############################################################################

SyntaxChecker target definitions and linked libraries. CMake targets are
exported to ${CMAKE_INSTALL_PREFIX}/lib/cmake/SyntaxChecker.

]]##############################################################################

set(shared_sources recorddecl_visitor.cpp field_visitor.cpp)
add_executable(SyntaxCheckerStandalone driver.cpp ${shared_sources})
# add_library(SyntaxChecker SHARED plugin.cpp)

foreach(target SyntaxCheckerStandalone)
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

# install(
#   TARGETS SyntaxChecker 
#   DESTINATION lib
#   EXPORT SyntaxChecker
#   )
install(
  TARGETS SyntaxCheckerStandalone 
  DESTINATION bin
  EXPORT SyntaxChecker
  )

install(EXPORT SyntaxChecker
  FILE LambdaChecker.cmake
  NAMESPACE SyntaxChecker::
  DESTINATION lib/cmake/SyntaxChecker
  )
