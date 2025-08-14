# CMakeList.txt : CMake project for MAP2FOMAP, Fallout map format converter into FOnline map format
#
cmake_minimum_required (VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("MAP2FOMAP")

include_directories("${PROJECT_SOURCE_DIR}/Formats" "${PROJECT_SOURCE_DIR}/Utils")

file(GLOB UTILS_SRC
     "${PROJECT_SOURCE_DIR}/Utils/*.cpp"
)

file(GLOB FORMATS_SRC
     "${PROJECT_SOURCE_DIR}/Formats/*.cpp"
)

set(1STPARTY_SRC
	"${PROJECT_SOURCE_DIR}/main.cpp"
)

add_executable (MAP2FOMAP ${UTILS_SRC} ${FORMATS_SRC} ${1STPARTY_SRC} )
set_target_properties(MAP2FOMAP PROPERTIES
                      RUNTIME_OUTPUT_DIRECTORY_DEBUG "${PROJECT_SOURCE_DIR}/BIN/DEBUG"
                      RUNTIME_OUTPUT_DIRECTORY_RELEASE "${PROJECT_SOURCE_DIR}/BIN/RELEASE"
					  RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${PROJECT_SOURCE_DIR}/BIN/RELWITHDBG"
					  RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL "${PROJECT_SOURCE_DIR}/BIN/MINSIZE")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET MAP2FOMAP PROPERTY CXX_STANDARD 20)
endif()
