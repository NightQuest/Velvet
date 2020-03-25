cmake_minimum_required(VERSION 3.15)
project(Velvet)

set(CMAKE_CXX_STANDARD 20)
include_directories(src)

# Modified from https://stackoverflow.com/a/1380048
MACRO(ADD_MSVC_PRECOMPILED_HEADER PrecompiledHeader PrecompiledSource SourcesVar)
	IF(MSVC)
		GET_FILENAME_COMPONENT(PrecompiledBasename ${PrecompiledHeader} NAME_WE)
		GET_FILENAME_COMPONENT(PrecompiledFilename ${PrecompiledHeader} NAME)
		SET(PrecompiledBinary "${CMAKE_CURRENT_BINARY_DIR}/${PrecompiledBasename}.pch")
		SET(Sources ${${SourcesVar}})

		SET_SOURCE_FILES_PROPERTIES(${PrecompiledSource}
									PROPERTIES COMPILE_FLAGS "/Yc\"${PrecompiledFilename}\" /Fp\"${PrecompiledBinary}\""
												OBJECT_OUTPUTS "${PrecompiledBinary}")
		SET_SOURCE_FILES_PROPERTIES(${Sources}
									PROPERTIES COMPILE_FLAGS "/Yu\"${PrecompiledHeader}\" /FI\"${PrecompiledHeader}\" /Fp\"${PrecompiledBinary}\""
												OBJECT_DEPENDS "${PrecompiledBinary}")  
		# Add precompiled header to SourcesVar
		LIST(APPEND ${SourcesVar} ${PrecompiledSource})
	ENDIF(MSVC)
ENDMACRO(ADD_MSVC_PRECOMPILED_HEADER)

set(header_files
	src/DBCFile.h
	src/tcharness.h
	src/Velvet.h)

set(res_files
	src/resource.h
	src/res/Velvet.rc)

set(source_files
	src/DBCFile.cpp
	src/Velvet.cpp)

if(MSVC)
	add_definitions(-DUNICODE -D_UNICODE) # Make sure we're using the Unicode WinAPI
	ADD_MSVC_PRECOMPILED_HEADER("src/preCompiled.h" "src/preCompiled.cpp" source_files)
	source_group("Header Files" FILES ${header_files})
	source_group("Resource Files" FILES ${res_files})
	source_group("Source Files" FILES ${source_files})
endif()

set(all_source_files
	${header_files}
	${res_files}
	${source_files})

add_executable(Velvet WIN32 ${all_source_files})
if(MSVC)
	set_property(TARGET Velvet PROPERTY VS_DPI_AWARE "PerMonitor")
endif()

install(TARGETS Velvet RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX})
