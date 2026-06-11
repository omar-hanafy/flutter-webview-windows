# Shared NuGet tooling for the plugin build and the native test build.
#
# Provides:
#   NUGET                       - path to a nuget executable (found or downloaded)
#   ensure_nuget_package(...)   - configure-time package install
#
# The NuGet packages are installed at configure time so imported .targets
# files (referenced by target_link_libraries) already exist when the build
# system is generated. A build-time download is too late: MSBuild evaluates
# those imports while loading the project, before any custom build step runs.

set(NUGET_URL https://dist.nuget.org/win-x86-commandline/v7.6.0/nuget.exe)
set(NUGET_SHA256 751ee5e79481626a428c1241dc7f94bca2739b32588e669715bc5fb54d8fb8a2)

# Find or download NuGet
find_program(NUGET nuget)
if(NOT NUGET)
  message(NOTICE "Nuget is not installed.")
  set(NUGET ${CMAKE_BINARY_DIR}/nuget.exe)
  if (NOT EXISTS ${NUGET})
    message(NOTICE "Attempting to download nuget.")
    file(DOWNLOAD ${NUGET_URL} ${NUGET}
         EXPECTED_HASH SHA256=${NUGET_SHA256}
         TLS_VERIFY ON
         STATUS NUGET_DL_STATUS)
    list(GET NUGET_DL_STATUS 0 NUGET_DL_CODE)
    if (NOT NUGET_DL_CODE EQUAL 0)
      list(GET NUGET_DL_STATUS 1 NUGET_DL_MESSAGE)
      file(REMOVE ${NUGET})
      message(FATAL_ERROR "Failed to download nuget: ${NUGET_DL_MESSAGE}")
    endif()
  endif()

  file(SHA256 ${NUGET} NUGET_DL_HASH)
  if (NOT NUGET_DL_HASH STREQUAL NUGET_SHA256)
    message(FATAL_ERROR "Integrity check for ${NUGET} failed.")
  endif()
endif()

# Installs PACKAGE_ID at PACKAGE_VERSION into ${CMAKE_BINARY_DIR}/packages
# unless the given sentinel file (relative to the package root) already
# exists.
function(ensure_nuget_package PACKAGE_ID PACKAGE_VERSION SENTINEL_FILE)
  set(_sentinel_path "${CMAKE_BINARY_DIR}/packages/${PACKAGE_ID}/${SENTINEL_FILE}")
  if(NOT EXISTS "${_sentinel_path}")
    message(STATUS "Installing NuGet package ${PACKAGE_ID} ${PACKAGE_VERSION}")
    execute_process(
      COMMAND ${NUGET} install ${PACKAGE_ID} -Version ${PACKAGE_VERSION}
              -ExcludeVersion -OutputDirectory ${CMAKE_BINARY_DIR}/packages
              -NonInteractive
      RESULT_VARIABLE _nuget_result)
    if(NOT _nuget_result EQUAL 0)
      message(FATAL_ERROR
              "NuGet install failed for ${PACKAGE_ID} (exit code ${_nuget_result})")
    endif()
  endif()
endfunction()
