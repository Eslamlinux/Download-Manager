cmake_minimum_required(VERSION 3.10)
project(AdvancedDownloadManager)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find wxWidgets
find_package(wxWidgets REQUIRED COMPONENTS core base net)
include(${wxWidgets_USE_FILE})

# Find CURL
find_package(CURL REQUIRED)
include_directories(${CURL_INCLUDE_DIRS})

# Include directories
include_directories(include)

# Source files
set(SOURCES
    src/App.cpp
    src/Common/EventIDs.cpp
    src/Common/CurlCallbacks.cpp
    src/Database/DatabaseManager.cpp
    src/Managers/DownloadManager.cpp
    src/Managers/YouTubeDownloader.cpp
    src/Models/AppSettings.cpp
    src/Models/DownloadItem.cpp
    src/UI/MainFrame.cpp
    src/UI/SettingsDialog.cpp
    src/UI/DownloadDialog.cpp
    src/UI/YouTubeDialog.cpp
    src/UI/SpeedLimitDialog.cpp
    src/Utils/FileUtils.cpp
)

# Add executable
add_executable(AdvancedDownloadManager ${SOURCES})

# Link libraries
target_link_libraries(AdvancedDownloadManager ${wxWidgets_LIBRARIES} ${CURL_LIBRARIES} sqlite3)
