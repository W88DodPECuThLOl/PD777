cmake_minimum_required(VERSION 3.23)

# プロジェクト名
set(PROJECT_NAME uPD777)

# プロジェクトを定義
project(${PROJECT_NAME} CXX C)

# DebugとReleaseだけにする
set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)

#=====================================================================================
# ヘッダとソースを再帰的に検索
#=====================================================================================

#-------------------------------------------------------------------------------------
# ソースファイル
#-------------------------------------------------------------------------------------
file(GLOB_RECURSE PRIVATE_SOURCE_FILES RELATIVE "${CMAKE_SOURCE_DIR}" "source/*.cpp")
file(GLOB_RECURSE PRIVATE_HEADER_FILES RELATIVE "${CMAKE_SOURCE_DIR}" "source/*.h")

# 実行ファイルを追加
add_executable(${PROJECT_NAME} ${PRIVATE_SOURCE_FILES} ${PRIVATE_HEADER_FILES})
# スタートアッププロジェクトを設定(VC++用)
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})

# C++の規格を設定
#   C++11ならcxx_std_11
#   C++14ならcxx_std_14
#   C++17ならcxx_std_17
#   C++20ならcxx_std_20
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)

#=====================================================================================
# VC++用にフィルターを設定
#=====================================================================================
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
source_group(TREE ${CMAKE_SOURCE_DIR}/source  FILES ${PRIVATE_SOURCE_FILES})
source_group(TREE ${CMAKE_SOURCE_DIR}/source  FILES ${PRIVATE_HEADER_FILES})

#=====================================================================================
# Doxygen
#-------------------------------------------------------------------------------------
# https://cmake.org/cmake/help/latest/module/FindDoxygen.html
find_package(Doxygen)
if(DOXYGEN_FOUND)
	set( DOXYGEN_OUTPUT_DIRECTORY doxygen )
	set( DOXYGEN_COLLABORATION_GRAPH YES )
	set( DOXYGEN_EXTRACT_ALL YES )
	set( DOXYGEN_CLASS_DIAGRAMS YES )
	set( DOXYGEN_HIDE_UNDOC_RELATIONS NO )
	set( DOXYGEN_HAVE_DOT YES )
	set( DOXYGEN_CLASS_GRAPH YES )
	set( DOXYGEN_CALL_GRAPH YES )
	set( DOXYGEN_CALLER_GRAPH YES )
	set( DOXYGEN_COLLABORATION_GRAPH YES )
	set( DOXYGEN_BUILTIN_STL_SUPPORT YES )
	set( DOXYGEN_EXTRACT_PRIVATE YES )
	set( DOXYGEN_EXTRACT_PACKAGE YES )
	set( DOXYGEN_EXTRACT_STATIC YES )
	set( DOXYGEN_EXTRACT_LOCALMETHODS YES )
	set( DOXYGEN_UML_LOOK YES )
	set( DOXYGEN_UML_LIMIT_NUM_FIELDS 50 )
	set( DOXYGEN_TEMPLATE_RELATIONS YES )
	set( DOXYGEN_DOT_GRAPH_MAX_NODES 100 )
	set( DOXYGEN_MAX_DOT_GRAPH_DEPTH 0 )
	set( DOXYGEN_DOT_TRANSPARENT YES )

	# https://cmake.org/cmake/help/latest/module/FindDoxygen.html#command:doxygen_add_docs
	doxygen_add_docs( Doxygen ${PRIVATE_HEADER_FILES} )
else (DOXYGEN_FOUND)
	message (WARNING "doxygen binary couldn't be found")
endif (DOXYGEN_FOUND)
