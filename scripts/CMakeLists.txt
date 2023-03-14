option(UPDATE_DEPS "Run update_deps.py for user")
if (UPDATE_DEPS)
    if (CMAKE_GENERATOR_PLATFORM)
        set(_target_arch ${CMAKE_GENERATOR_PLATFORM})
    else()
        if (MSVC_IDE)
            message(WARNING "CMAKE_GENERATOR_PLATFORM not set. Using x64 as target architecture.")
        endif()
        set(_target_arch x64)
    endif()

    if (NOT CMAKE_BUILD_TYPE)
        message(WARNING "CMAKE_BUILD_TYPE not set. Using Debug for dependency build type")
        set(_build_type Debug)
    else()
        set(_build_type ${CMAKE_BUILD_TYPE})
    endif()

    message("********************************************************************************")
    message("* NOTE: Adding target profile_update_deps to run as needed for updating        *")
    message("*       dependencies.                                                          *")
    message("********************************************************************************")

    set(optional_args)
    if (NOT PROFILES_BUILD_TESTS)
        set(optional_args "--optional=tests")
    endif()

    if (UPDATE_DEPS_SKIP_EXISTING_INSTALL)
        set(optional_args ${optional_args} "--skip-existing-install")
    endif()

    if (DEFINED CMAKE_TOOLCHAIN_FILE)
        set(optional_args ${optional_args} "--cmake_var")
        set(optional_args ${optional_args} "CMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}")
    endif()

    # Add a target so that update_deps.py will run when necessary
    # NOTE: This is triggered off of the timestamps of known_good.json and helper.cmake
    add_custom_command(
        OUTPUT ${PROJECT_SOURCE_DIR}/external/helper.cmake
        COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_LIST_DIR}/update_deps.py
            --dir ${PROJECT_SOURCE_DIR}/external --arch ${_target_arch} --config ${_build_type} --generator "${CMAKE_GENERATOR}" ${optional_args}
        DEPENDS ${CMAKE_CURRENT_LIST_DIR}/known_good.json
    )

    # Check if update_deps.py needs to be run on first cmake run
    if (${CMAKE_CURRENT_LIST_DIR}/known_good.json IS_NEWER_THAN ${PROJECT_SOURCE_DIR}/external/helper.cmake)
        execute_process(
            COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_LIST_DIR}/update_deps.py
                --dir ${PROJECT_SOURCE_DIR}/external --arch ${_target_arch} --config ${_build_type} --generator "${CMAKE_GENERATOR}" ${optional_args}
            WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
            RESULT_VARIABLE _update_deps_result
        )
        if (NOT (${_update_deps_result} EQUAL 0))
            message(FATAL_ERROR "Could not run update_deps.py which is necessary to download dependencies.")
        endif()
    endif()
    include(${PROJECT_SOURCE_DIR}/external/helper.cmake)
else()
    message("********************************************************************************")
    message("* NOTE: Not adding target to run update_deps.py automatically.                 *")
    message("********************************************************************************")
    find_package(PythonInterp 3 QUIET)
endif()

if (JSONCPP_INSTALL_DIR)
    list(APPEND CMAKE_PREFIX_PATH ${JSONCPP_INSTALL_DIR})
endif()
if (VALIJSON_INSTALL_DIR)
    list(APPEND CMAKE_PREFIX_PATH ${VALIJSON_INSTALL_DIR})
endif()
if (GOOGLETEST_INSTALL_DIR)
    list(APPEND CMAKE_PREFIX_PATH ${GOOGLETEST_INSTALL_DIR})
endif()
if (VULKAN_HEADERS_INSTALL_DIR)
    list(APPEND CMAKE_PREFIX_PATH ${VULKAN_HEADERS_INSTALL_DIR})
endif()
if (VULKAN_VALIDATIONLAYERS_INSTALL_DIR)
    list(APPEND CMAKE_INCLUDE_PATH ${VULKAN_VALIDATIONLAYERS_INSTALL_DIR}/include/vulkan)
endif()

set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} PARENT_SCOPE)
set(CMAKE_INCLUDE_PATH ${CMAKE_INCLUDE_PATH} PARENT_SCOPE)
