cmake_minimum_required(VERSION 2.8)

include_directories(
    ${CMAKE_SOURCE_DIR}/src
)
list_directories(TESTS ${CMAKE_CURRENT_SOURCE_DIR})

foreach(TEST ${TESTS})
    set(TARGET_NAME test_eval_${TEST})

    set(INPUT_FILE   ${CMAKE_CURRENT_SOURCE_DIR}/${TEST}/main.x)
    set(STDERR_FILE  ${CMAKE_CURRENT_SOURCE_DIR}/${TEST}/stderr.txt)
    set(STDOUT_FILE  ${CMAKE_CURRENT_SOURCE_DIR}/${TEST}/stdout.txt)
    set(RETCODE_FILE ${CMAKE_CURRENT_SOURCE_DIR}/${TEST}/returncode.txt)

    add_executable(${TARGET_NAME} main.c ${INPUT_FILE})
	set_source_files_properties(
		${INPUT_FILE}
		${STDERR_FILE}
		${STDOUT_FILE}
		${RETCODE_FILE}
		PROPERTIES
			HEADER_FILE_ONLY True
	)

	foreach(DEFINE INPUT_FILE STDERR_FILE STDOUT_FILE RETCODE_FILE)
		if(EXISTS ${${DEFINE}})
			target_compile_definitions(${TARGET_NAME} PUBLIC -D${DEFINE}="${${DEFINE}}")
		endif()
	endforeach()

    target_link_libraries(${TARGET_NAME} compilerlib)
    set_property(TARGET ${TARGET_NAME} PROPERTY FOLDER "Test/Eval")
	
    add_test(
        NAME "eval_${TEST}"
        COMMAND $<TARGET_FILE:${TARGET_NAME}>
    )
endforeach()
