cmake_minimum_required(VERSION 3.16)

project(AKIP-Gratten-Generator-Manager LANGUAGES CXX)

# Устанавливаем политику для AUTOMOC
if(POLICY CMP0071)
    cmake_policy(SET CMP0071 NEW)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Находим Qt - только Core!
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core  Widgets Network)

# Создаем исполняемый файл
set(SOURCES
    main.cpp
    usbinterface.cpp
    usbinterface.h
)

add_executable(AKIP-Gratten-Generator-Manager ${SOURCES}
    mainwindow.h mainwindow.cpp mainwindow.ui
    akipfacade.h akipfacade.cpp
    iakipcontroller.h
    laninterface.h laninterface.cpp
    grattenga1483controller.h grattenga1483controller.cpp
    grattencontrolwidget.h grattencontrolwidget.cpp grattencontrolwidget.ui
    abstractscpicontroller.h abstractscpicontroller.cpp
    gratten_limits.h
    settingsmanager.h settingsmanager.cpp
    settingsdialog.h settingsdialog.cpp settingsdialog.ui
    thememanager.h
    resources.qrc)

# Включаем автоматический запуск MOC
set_target_properties(AKIP-Gratten-Generator-Manager PROPERTIES
    AUTOMOC ON
    AUTORCC ON
    AUTOUIC ON
)

# Подключаем библиотеки Qt
target_link_libraries(AKIP-Gratten-Generator-Manager PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Network
)

# --- КОНФИГУРАЦИЯ ДЛЯ CH375DLL ---
set(CH375_SDK_DIR "${CMAKE_CURRENT_SOURCE_DIR}/ch375_sdk")

# 1. Путь к заголовочным файлам
target_include_directories(AKIP-Gratten-Generator-Manager PRIVATE
    ${CH375_SDK_DIR}
)

# 2. Используем 64-битную версию (у вас MinGW 64-bit)
message(STATUS "Using 64-bit CH375DLL library")
set(CH375_LIB_FILE "${CH375_SDK_DIR}/CH375DLL64.lib")
set(CH375_DLL_FILE "${CH375_SDK_DIR}/CH375DLL64.dll")

# 3. Подключаем библиотеку .lib
target_link_libraries(AKIP-Gratten-Generator-Manager PRIVATE
    ${CH375_LIB_FILE}
)

# 4. Копируем DLL в папку сборки
add_custom_command(TARGET AKIP-Gratten-Generator-Manager POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CH375_DLL_FILE}"
        $<TARGET_FILE_DIR:AKIP-Gratten-Generator-Manager>
    COMMENT "Copying CH375DLL64.dll to output directory"
)

# 5. Для MinGW добавляем библиотеки Windows
target_link_libraries(AKIP-Gratten-Generator-Manager PRIVATE
    -luser32
    -lgdi32
    -ladvapi32
    -lshell32
    -lole32
    -loleaut32
    -luuid
    -lws2_32
)

# Опционально: установка
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install" CACHE PATH "..." FORCE)
endif()

install(TARGETS AKIP-Gratten-Generator-Manager
    RUNTIME DESTINATION bin
)
