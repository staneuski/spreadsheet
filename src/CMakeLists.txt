cmake_minimum_required(VERSION 3.13 FATAL_ERROR)
project(spreadsheet)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
  set(
    CMAKE_CXX_FLAGS_DEBUG
    "${CMAKE_CXX_FLAGS_DEBUG} /JMC"
  )
else()
  set(
    CMAKE_CXX_FLAGS
    "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -Werror -Wno-unused-parameter -Wno-implicit-fallthrough"
  )
endif()

# add_compile_options(-Wall -Wextra -Werror)

set(ANTLR_EXECUTABLE ${CMAKE_CURRENT_SOURCE_DIR}/antlr-4.9.2-complete.jar)
include(${CMAKE_CURRENT_SOURCE_DIR}/FindANTLR.cmake)

add_definitions(
  -DANTLR4CPP_STATIC
  -D_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS
)

set(WITH_STATIC_CRT OFF CACHE BOOL "Visual C++ static CRT for ANTLR" FORCE)
add_subdirectory(antlr4_runtime)

antlr_target(FormulaParser Formula.g4 LEXER PARSER LISTENER)

include_directories(
  ${ANTLR4_INCLUDE_DIRS}
  ${ANTLR_FormulaParser_OUTPUT_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/antlr4_runtime/runtime/src
)

file(GLOB sources *.cpp *.h)

add_executable(
  spreadsheet
  ${ANTLR_FormulaParser_CXX_OUTPUTS}
  ${sources}
)

target_link_libraries(spreadsheet antlr4_static)

install(
  TARGETS spreadsheet
  DESTINATION bin
  EXPORT spreadsheet
)

set_directory_properties(PROPERTIES VS_STARTUP_PROJECT spreadsheet)