# data
# License: MIT

if(TARGET wmtk::data)
    return()
endif()

include(ExternalProject)
include(FetchContent)

set(WMT_DATA_ROOT "${PROJECT_SOURCE_DIR}/data/" CACHE PATH "Where should the toolkit download and look for test data?")

ExternalProject_Add(
    wmt_data
    PREFIX "${FETCHCONTENT_BASE_DIR}/wmtk-test-data"
    SOURCE_DIR ${WMT_DATA_ROOT}

    GIT_REPOSITORY https://github.com/wildmeshing/data.git
    GIT_TAG ae19b8c17726ceb23ae0f9e50ec2b3d4a1adba34
    GIT_SHALLOW FALSE

    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
    LOG_DOWNLOAD ON
)

# Create a dummy target for convenience
add_library(wmtk_data INTERFACE)
add_library(wmtk::data ALIAS wmtk_data)
target_compile_definitions(wmtk_data INTERFACE  WMT_DATA_DIR=\"${WMT_DATA_ROOT}\")
