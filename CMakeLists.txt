
project(DX)

# Set project-wide variables
set(DX_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(DX_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/source)

# Set headers and sources for the framework library
set(DX_FW_HEADERS
  ${DX_INCLUDE_DIR}/BaseApplication.h
  ${DX_INCLUDE_DIR}/BaseMesh.h
  ${DX_INCLUDE_DIR}/BaseShaders.h
  ${DX_INCLUDE_DIR}/Camera.h
  ${DX_INCLUDE_DIR}/ColourShader.h
  ${DX_INCLUDE_DIR}/CubeMesh.h
  ${DX_INCLUDE_DIR}/D3D.h
  ${DX_INCLUDE_DIR}/Input.h
  #  ${DX_INCLUDE_DIR}/Lab1.h
  ${DX_INCLUDE_DIR}/Light.h
  ${DX_INCLUDE_DIR}/Model.h
  ${DX_INCLUDE_DIR}/OrthoMesh.h
  ${DX_INCLUDE_DIR}/PlaneMesh.h
  ${DX_INCLUDE_DIR}/PointMesh.h
  ${DX_INCLUDE_DIR}/QuadMesh.h
  ${DX_INCLUDE_DIR}/RenderTexture.h
  ${DX_INCLUDE_DIR}/SphereMesh.h
  ${DX_INCLUDE_DIR}/System.h
  ${DX_INCLUDE_DIR}/TessellationMesh.h
  ${DX_INCLUDE_DIR}/Texture.h
  ${DX_INCLUDE_DIR}/Timer.h
  ${DX_INCLUDE_DIR}/TokenStream.h
  ${DX_INCLUDE_DIR}/TriangleMesh.h
)
set(DX_FW_SOURCES
  ${DX_SOURCE_DIR}/BaseApplication.cpp
  ${DX_SOURCE_DIR}/BaseMesh.cpp
  ${DX_SOURCE_DIR}/BaseShaders.cpp
  ${DX_SOURCE_DIR}/Camera.cpp
  ${DX_SOURCE_DIR}/ColourShader.cpp
  ${DX_SOURCE_DIR}/CubeMesh.cpp
  ${DX_SOURCE_DIR}/D3D.cpp
  ${DX_SOURCE_DIR}/Input.cpp
  # ${DX_SOURCE_DIR}/Lab1.cpp
  ${DX_SOURCE_DIR}/Light.cpp
  ${DX_SOURCE_DIR}/Model.cpp
  ${DX_SOURCE_DIR}/OrthoMesh.cpp
  ${DX_SOURCE_DIR}/PlaneMesh.cpp
  ${DX_SOURCE_DIR}/PointMesh.cpp
  ${DX_SOURCE_DIR}/QuadMesh.cpp
  ${DX_SOURCE_DIR}/RenderTexture.cpp
  ${DX_SOURCE_DIR}/SphereMesh.cpp
  ${DX_SOURCE_DIR}/System.cpp
  ${DX_SOURCE_DIR}/TessellationMesh.cpp
  ${DX_SOURCE_DIR}/Texture.cpp
  ${DX_SOURCE_DIR}/Timer.cpp
  ${DX_SOURCE_DIR}/TokenStream.cpp
  ${DX_SOURCE_DIR}/TriangleMesh.cpp
  ${DX_SOURCE_DIR}/Main.cpp
)

# Set project-wide include dirs
include_directories(
  ${DX_INCLUDE_DIR}
)

# Build the framework as a static library
add_library(dx_fw ${DX_FW_HEADERS} ${DX_FW_SOURCES})

# Attempt to find the location of the DX11 libraries
find_package(D3D11 REQUIRED)

if(DirectX_D3D11_FOUND)
  # Add the DX11 headers and the library to the framework library 
  # (Report it)
  message(STATUS "Found DX!")
  include_directories(${DirectX_D3D11_INCLUDE_DIR})
  target_link_libraries(dx_fw ${DirectX_D3D11_INCLUDE_DIR})
endif()


# Set headers and sources for the framework app 
set(DX_APP_HEADERS
  ${DX_INCLUDE_DIR}/Lab1.h
)
set(DX_APP_SOURCES
  ${DX_SOURCE_DIR}/Lab1.cpp
)

# Build the framework app as an executable
add_executable(dx_app ${DX_APP_HEADERS} ${DX_APP_SOURCES})

# Link the framework library to the executable
target_link_libraries(dx_app
  dx_fw
)
