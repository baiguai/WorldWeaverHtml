cmake_minimum_required(VERSION 3.10)
project(DataLoader)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find wxWidgets
find_package(wxWidgets REQUIRED COMPONENTS core base)
include(${wxWidgets_USE_FILE})

# Add executable
add_executable(dataloader dataloader.cpp)

# Link libraries
target_link_libraries(dataloader ${wxWidgets_LIBRARIES})

# Platform-specific settings
if(WIN32)
    target_link_libraries(dataloader -mwindows)
endif()
