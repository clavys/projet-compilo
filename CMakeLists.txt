cmake_minimum_required(VERSION 3.16)
project(PROJET-COMPILO)

set(CMAKE_CXX_STANDARD 17)

# Inclure le dossier include/
include_directories(${CMAKE_SOURCE_DIR}/include)

# Fichiers sources
# Make sure src/main.cpp exists, or update the path below if it is located elsewhere.
add_executable(main

    # Main
    src/main.cpp
    
    # ast
    src/ast/ast.cpp

    # Meta
    src/meta/meta_ast_builder.cpp
    src/meta/meta_lexer.cpp
    src/meta/meta_parser.cpp

    # compilo
    src/compilo/lexer.cpp
    src/compilo/parser.cpp
    src/compilo/pcodegen.cpp
    src/compilo/runtime.cpp

)

add_executable(test_meta_parser
    # Tests
    tests/test_meta_parser.cpp

    # ast
    src/ast/ast.cpp

    # Meta
    src/meta/meta_ast_builder.cpp
    src/meta/meta_lexer.cpp
    src/meta/meta_parser.cpp
    
)

add_executable(test_parser
    # Tests
    tests/test_parser.cpp

    # ast
    src/ast/ast.cpp

    # Meta
    src/meta/meta_ast_builder.cpp
    src/meta/meta_lexer.cpp
    src/meta/meta_parser.cpp

    # compilo
    src/compilo/lexer.cpp
    src/compilo/parser.cpp
    src/compilo/pcodegen.cpp
    
    
)


add_executable(test_runtime
    # Tests
    tests/test_runtime.cpp

    # ast
    src/ast/ast.cpp

    # Meta
    src/meta/meta_ast_builder.cpp
    src/meta/meta_lexer.cpp
    src/meta/meta_parser.cpp

    # compilo
    src/compilo/lexer.cpp
    src/compilo/parser.cpp
    src/compilo/pcodegen.cpp
    src/compilo/runtime.cpp
    
    
)