cmake_minimum_required(VERSION 3.13)

find_program(LEX_EXE
        flex
        )
if (LEX_EXE STREQUAL "LEX_EXE-NOTFOUND")
    message(FATAL_ERROR "dear user, plase install flex!")
endif (LEX_EXE STREQUAL "LEX_EXE-NOTFOUND")

find_program(YACC_EXE
        bison
        )
if (YACC_EXE STREQUAL "YACC_EXE-NOTFOUND")
    message(FATAL_ERROR "dear user, plase install bison!")
endif (YACC_EXE STREQUAL "YACC_EXE-NOTFOUND")

# reuseable cmake macro for yacc
MACRO(YACC_FILE _filename)
    GET_FILENAME_COMPONENT(_basename ${_filename} NAME_WE)
    ADD_CUSTOM_COMMAND(
            OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${_basename}.cpp
            ${CMAKE_CURRENT_BINARY_DIR}/${_basename}.h
            COMMAND ${YACC_EXE} -tvd
            --output=${_basename}.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/${_filename}
            DEPENDS ${_filename}
    )
ENDMACRO(YACC_FILE)

# reuseable cmake macro for lex
MACRO(LEX_FILE _filename)
    GET_FILENAME_COMPONENT(_basename ${_filename} NAME_WE)
    ADD_CUSTOM_COMMAND(
            OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${_basename}.cpp
            COMMAND ${LEX_EXE}
            -o${_basename}.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/${_filename}
            DEPENDS ${_filename})
ENDMACRO(LEX_FILE)

YACC_FILE(interp_bison.y)
LEX_FILE(interp_flex.l)

add_library(parser STATIC interp_bison.cpp interp_flex.cpp)

add_executable(interpreter main.cpp)

target_link_libraries(interpreter parser)

target_compile_options(parser PRIVATE -Wno-deprecated-register)