foreach(TEST
        ast_test
        ast_parse_test
        ast_generate_test
        attr_map_test
        instance_test
        attr_header_test
        attr_map_header_test
        class_header_test
        if_elif_else_header_test
        instance_header_test
        scope_header_test)
    add_executable(${TEST} ${TEST}.cpp)
    target_link_libraries(${TEST} ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY} pyffi)
    add_test(pyffi::object_models::${TEST} ${TEST})
endforeach()
