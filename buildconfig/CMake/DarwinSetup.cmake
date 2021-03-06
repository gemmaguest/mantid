###########################################################################
# Set installation variables
###########################################################################
set ( BIN_DIR bin )
set ( ETC_DIR etc )
set ( LIB_DIR lib )
set ( SITE_PACKAGES ${LIB_DIR} )
set ( PLUGINS_DIR plugins )

set ( WORKBENCH_BIN_DIR ${BIN_DIR} )
set ( WORKBENCH_LIB_DIR ${LIB_DIR} )
set ( WORKBENCH_SITE_PACKAGES ${LIB_DIR} )
set ( WORKBENCH_PLUGINS_DIR ${PLUGINS_DIR} )

# Determine the version of macOS that we are running
# ##############################################################################

# Set the system name (and remove the space)
execute_process(
  COMMAND /usr/bin/sw_vers -productVersion
  OUTPUT_VARIABLE MACOS_VERSION
  RESULT_VARIABLE MACOS_VERSION_STATUS
)
# Strip off any /CR or /LF
string(STRIP ${MACOS_VERSION} MACOS_VERSION)

if(MACOS_VERSION VERSION_LESS 10.13)
  message(
    FATAL_ERROR
      "The minimum supported version of Mac OS X is 10.13 (High Sierra)."
  )
endif()

if(MACOS_VERSION VERSION_GREATER 10.13 OR MACOS_VERSION VERSION_EQUAL 10.13)
  set(MACOS_CODENAME "High Sierra")
elseif(MACOS_VERSION VERSION_GREATER 10.14 OR MACOS_VERSION VERSION_EQUAL 10.14)
  set(MACOS_CODENAME "Mojave")
else()
  message(FATAL_ERROR "Unknown version of macOS")
endif()

# Export variables globally
set(MACOS_VERSION
    ${MACOS_VERSION}
    CACHE INTERNAL ""
)
set(MACOS_CODENAME
    ${MACOS_CODENAME}
    CACHE INTERNAL ""
)

message(
  STATUS "Operating System: Mac OS X ${MACOS_VERSION} (${MACOS_CODENAME})"
)

# Enable the use of the -isystem flag to mark headers in Third_Party as system
# headers
set(CMAKE_INCLUDE_SYSTEM_FLAG_CXX "-isystem ")

# Set Qt5 dir according to homebrew location
set(Qt5_DIR /usr/local/opt/qt/lib/cmake/Qt5)

# Python flags
set(PY_VER "${Python_VERSION_MAJOR}.${Python_VERSION_MINOR}")
execute_process(
  COMMAND python${PY_VER}-config --prefix
  OUTPUT_VARIABLE PYTHON_PREFIX
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

execute_process(
  COMMAND python${PY_VER}-config --abiflags
  OUTPUT_VARIABLE PY_ABI
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

# Tag used by dynamic loader to identify directory of loading library
set(DL_ORIGIN_TAG @loader_path)

# Generate a target to put a mantidpython wrapper in the appropriate directory
if(NOT TARGET mantidpython)
  if(MAKE_VATES)
    set(PARAVIEW_PYTHON_PATHS
        ":${ParaView_DIR}/lib:${ParaView_DIR}/lib/site-packages:${ParaView_DIR}/lib/site-packages/vtk"
    )
  else()
    set(PARAVIEW_PYTHON_PATHS "")
  endif()
  configure_file(
    ${CMAKE_MODULE_PATH}/Packaging/osx/mantidpython.in
    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/mantidpython @ONLY
  )

  add_custom_target(
    mantidpython ALL
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/mantidpython
            ${PROJECT_BINARY_DIR}/bin/${CMAKE_CFG_INTDIR}/mantidpython
    COMMENT "Generating mantidpython"
  )
  # Configure install script at the same time. Doing it later causes a warning
  # from ninja.
  if(MAKE_VATES)
    # Python packages go into bundle Python site-packages
    set(PARAVIEW_PYTHON_PATHS "")
    set(PV_PLUGIN_INSTALL_PATH "PV_PLUGIN_PATH=\"${INSTALLDIR}/PlugIns/paraview/qt4\"")
  else()
    set(PARAVIEW_PYTHON_PATHS "")
  endif()

  set(PYTHONHOME
      "\${INSTALLDIR}/Frameworks/Python.framework/Versions/${Python_VERSION_MAJOR}.${Python_VERSION_MINOR}"
  )
  configure_file(
    ${CMAKE_MODULE_PATH}/Packaging/osx/mantidpython.in
    ${CMAKE_BINARY_DIR}/mantidpython_osx_install @ONLY
  )
  unset(PYTHONHOME)
endif()

# directives similar to linux for conda framework-only build
set(BIN_DIR bin)
set(WORKBENCH_BIN_DIR bin)
set(ETC_DIR etc)
set(LIB_DIR lib)
set(SITE_PACKAGES lib)
set(PLUGINS_DIR plugins)

# ##############################################################################
# Mac-specific installation setup
# ##############################################################################
# use homebrew OpenSSL package
if(NOT OPENSSL_ROOT_DIR)
  set(OPENSSL_ROOT_DIR /usr/local/opt/openssl)
endif(NOT OPENSSL_ROOT_DIR)

if(NOT HDF5_ROOT)
    set(HDF5_ROOT /usr/local/opt/hdf5) # Only for homebrew! 
endif()

if(ENABLE_MANTIDPLOT OR ENABLE_WORKBENCH)
  set(CPACK_GENERATOR DragNDrop)
  set(CMAKE_INSTALL_PREFIX "")
  # Replace hdiutil command to retry on detach failure
  set(CPACK_COMMAND_HDIUTIL ${CMAKE_SOURCE_DIR}/installers/MacInstaller/hdiutilwrap)
  set(CMAKE_MACOSX_RPATH 1)
  set(CPACK_DMG_BACKGROUND_IMAGE
      ${CMAKE_SOURCE_DIR}/images/osx-bundle-background.png
  )
  string(REPLACE " " "" CPACK_SYSTEM_NAME ${MACOS_CODENAME})

  if(ENABLE_MANTIDPLOT AND NOT ENABLE_WORKBENCH)
    set(INBUNDLE MantidPlot.app/Contents/)
    set(CPACK_PACKAGE_EXECUTABLES MantidPlot)
    set(MANTIDPLOT_APP MantidPlot${CPACK_PACKAGE_SUFFIX_CAMELCASE}.app)
    set(INBUNDLE ${MANTIDPLOT_APP}/Contents/)
    # Copy the launcher script to the correct location
    configure_file(
      ${CMAKE_MODULE_PATH}/Packaging/osx/Mantid_osx_launcher.in
      ${CMAKE_BINARY_DIR}/bin/${INBUNDLE}MacOS/Mantid_osx_launcher
      @ONLY
    )

    # We know exactly where this has to be on Darwin, but separate whether we
    # have kit build or a regular build.
    if(ENABLE_CPACK AND MAKE_VATES)
      add_definitions(-DBUNDLE_PARAVIEW)
    else()
      set(PARAVIEW_APP_DIR "${ParaView_DIR}")
      set(PARAVIEW_APP_BIN_DIR "${PARAVIEW_APP_DIR}/bin")
      set(PARAVIEW_APP_LIB_DIR "${PARAVIEW_APP_DIR}/lib")
      set(PARAVIEW_APP_PLUGIN_DIR "${PARAVIEW_APP_DIR}/lib")
    endif()

    set(BIN_DIR ${INBUNDLE}/MacOS)
    set(LIB_DIR ${INBUNDLE}/MacOS)
    set(SITE_PACKAGES ${INBUNDLE}MacOS)
    # This is the root of the plugins directory
    set(PLUGINS_DIR ${INBUNDLE}PlugIns)
    # Separate directory of plugins to be discovered by the ParaView framework
    # These cannot be mixed with our other plugins. Further sub-directories
    # based on the Qt version will also be created by the installation targets
    set(PVPLUGINS_SUBDIR paraview)

    install(FILES ${CMAKE_SOURCE_DIR}/images/MantidPlot.icns
            DESTINATION ${INBUNDLE}Resources/
    )
    # Add launcher script for mantid python
    install(
      PROGRAMS ${CMAKE_BINARY_DIR}/mantidpython_osx_install
      DESTINATION ${INBUNDLE}MacOS/
      RENAME mantidpython
    )
  endif()

  if(ENABLE_WORKBENCH AND NOT ENABLE_MANTIDPLOT)
    set(WORKBENCH_BUNDLE MantidWorkbench.app/Contents/)
    set(WORKBENCH_APP MantidWorkbench${CPACK_PACKAGE_SUFFIX_CAMELCASE}.app)
    set(WORKBENCH_BUNDLE ${WORKBENCH_APP}/Contents/)
    set(WORKBENCH_BIN_DIR ${WORKBENCH_BUNDLE}MacOS)
    set(WORKBENCH_LIB_DIR ${WORKBENCH_BUNDLE}MacOS)
    set(WORKBENCH_SITE_PACKAGES ${WORKBENCH_BUNDLE}MacOS)
    set(WORKBENCH_PLUGINS_DIR ${WORKBENCH_BUNDLE}PlugIns)

    install(
      PROGRAMS ${CMAKE_BINARY_DIR}/mantidpython_osx_install
      DESTINATION ${WORKBENCH_BUNDLE}/MacOS/
      RENAME mantidpython
    )
    install(
      FILES ${CMAKE_SOURCE_DIR}/images/mantid_workbench${CPACK_PACKAGE_SUFFIX}.icns
      DESTINATION ${WORKBENCH_BUNDLE}Resources/
    )
  endif()
  set(BUNDLES ${INBUNDLE} ${WORKBENCH_BUNDLE})

  # Produce script to move icons in finder window to the correct locations
  configure_file(${CMAKE_SOURCE_DIR}/installers/MacInstaller/CMakeDMGSetup.scpt.in
                 ${CMAKE_BINARY_DIR}/DMGSetup.scpt @ONLY)
  set(CPACK_DMG_DS_STORE_SETUP_SCRIPT ${CMAKE_BINARY_DIR}/DMGSetup.scpt)
endif()
