
SET(NROOT ${CMAKE_CURRENT_LIST_DIR})

option(N_USE_PRECOMPILED_HEADERS "Use precompiled headers" OFF)

if(FIPS_WINDOWS)
	option(N_STATIC_BUILD "Use static runtime in windows builds" ON)
	if(N_STATIC_BUILD)
		add_definitions(-D__N_STATIC_BUILD)
	endif()
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /arch:AVX /fp:fast /GS-")
	SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /arch:AVX /fp:fast /GS-")
elseif(FIPS_LINUX)
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fpermissive -std=gnu++0x -msse4.2 -march=sandybridge -ffast-math -fPIC -fno-trapping-math -funsafe-math-optimizations -ffinite-math-only -mrecip=all")
	SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -msse4.2 -march=sandybridge -ffast-math -fPIC -fno-trapping-math -funsafe-math-optimizations -ffinite-math-only -mrecip=all")
endif()

if(FIPS_WINDOWS)
	option(N_MATH_DIRECTX "Use DirectXMath" ON)
endif()
if(N_MATH_DIRECTX)
	add_definitions(-D__USE_MATH_DIRECTX)
else()	
	add_definitions(-D__USE_VECMATH)
	OPTION(N_USE_AVX "Use AVX instructionset" OFF)	
endif()

set(N_QT4 OFF)
set(N_QT5 OFF)
set(DEFQT "N_QT4")
set(N_QT ${DEFQT} CACHE STRING "Qt Version")
set_property(CACHE N_QT PROPERTY STRINGS "N_QT4" "N_QT5")
set(${N_QT} ON)

if(N_QT5)
	add_definitions(-D__USE_QT5)
endif()

if(N_QT4)
	add_definitions(-D__USE_QT4)
endif()

if(N_USE_VULKAN)
	add_definitions(-DNEBULA_DEFAULT_FRAMESHADER_NAME="vkdebug")
	add_definitions(-D__VULKAN__)
endif()

option(N_BUILD_NVTT "use NVTT" ON)
