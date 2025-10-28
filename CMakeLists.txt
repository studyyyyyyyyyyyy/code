#/****************************************************************************
# Copyright (c) 2013-2014 cocos2d-x.org
# Copyright (c) 2015-2017 Chukong Technologies Inc.
#
# http://www.cocos2d-x.org
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:

# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.

# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
# ****************************************************************************/

cmake_minimum_required(VERSION 3.5)

# 添加UTF-8编码支持
if(MSVC)
    add_compile_options(/utf-8)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
    # 禁用PDB文件缺失警告
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /ignore:4099")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /ignore:4099")
    set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} /ignore:4099")
endif()

set(APP_NAME TemplateCpp)

project(${APP_NAME})

# 设置x64架构目标 - 已注释，使用 Win32 (x86) 构建
# cocos2d-x 官方只提供 Win32 预编译库
# if(WINDOWS AND MSVC)
#     set(CMAKE_GENERATOR_PLATFORM "x64" CACHE STRING "Platform" FORCE)
#     set(CMAKE_VS_PLATFORM_NAME "x64")
#     # 强制使用x64预编译库
#     set(PLATFORM_SUFFIX "x64")
#     # 设置架构目录变量
#     set(ARCH_DIR "x64")
#     # 设置平台文件夹
#     set(PLATFORM_FOLDER "win10")
#     # 设置链接器目标架构
#     set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /machine:x64")
#     set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /machine:x64")
#     set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} /machine:x64")
#     
#     # 为预编译库添加x64路径支持
#     set(CMAKE_PREFIX_PATH "${CMAKE_PREFIX_PATH};${COCOS2DX_ROOT_PATH}/external/win10-specific")
#     
#     # 强制使用x64库，禁用win32回退
#     set(CMAKE_FIND_LIBRARY_SUFFIXES ".lib")
#     set(CMAKE_FIND_LIBRARY_PREFIXES "")
# endif()

set(COCOS2DX_ROOT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cocos2d)
set(CMAKE_MODULE_PATH ${COCOS2DX_ROOT_PATH}/cmake/Modules/)

include(CocosBuildSet)
if(NOT USE_COCOS_PREBUILT)
    add_subdirectory(${COCOS2DX_ROOT_PATH}/cocos ${ENGINE_BINARY_PATH}/cocos/core)
endif()

# record sources, headers, resources...
set(GAME_SOURCE)
set(GAME_HEADER)

set(GAME_RES_FOLDER
    "${CMAKE_CURRENT_SOURCE_DIR}/Resources"
    )
if(APPLE OR WINDOWS)
    cocos_mark_multi_resources(common_res_files RES_TO "Resources" FOLDERS ${GAME_RES_FOLDER})
endif()

include_directories(
        Classes
        ${COCOS2DX_ROOT_PATH}/cocos/audio/include/
)
# add cross-platforms source files and header files 
list(APPEND GAME_SOURCE
     Classes/AppDelegate.cpp
     Classes/HelloWorldScene.cpp
     # Configs
     Classes/configs/models/LevelConfig.cpp
     Classes/configs/models/CardResConfig.cpp
     Classes/configs/loaders/LevelConfigLoader.cpp
     # Models
     Classes/models/CardModel.cpp
     Classes/models/GameModel.cpp
     Classes/models/UndoModel.cpp
     # Views
     Classes/views/CardView.cpp
     Classes/views/GameView.cpp
     # Controllers
     Classes/controllers/GameController.cpp
     Classes/controllers/CardController.cpp
     # Managers
     Classes/managers/UndoManager.cpp
     Classes/managers/ResourceManager.cpp
     Classes/managers/GameModelGenerator.cpp
     # Services
     Classes/services/GameRulesService.cpp
     # Utils
     Classes/utils/CardGameUtils.cpp
     )
list(APPEND GAME_HEADER
     Classes/AppDelegate.h
     Classes/HelloWorldScene.h
     # Configs
     Classes/configs/models/LevelConfig.h
     Classes/configs/models/CardResConfig.h
     Classes/configs/loaders/LevelConfigLoader.h
     # Models
     Classes/models/CardModel.h
     Classes/models/GameModel.h
     Classes/models/UndoModel.h
     # Views
     Classes/views/CardView.h
     Classes/views/GameView.h
     # Controllers
     Classes/controllers/GameController.h
     Classes/controllers/CardController.h
     # Managers
     Classes/managers/UndoManager.h
     Classes/managers/ResourceManager.h
     Classes/managers/GameModelGenerator.h
     # Services
     Classes/services/GameRulesService.h
     # Utils
     Classes/utils/CardGameUtils.h
     Classes/utils/GameConstants.h
     )

if(ANDROID)
    # change APP_NAME to the share library name for Android, it's value depend on AndroidManifest.xml
    set(APP_NAME MyGame)
    list(APPEND GAME_SOURCE
         proj.android/app/jni/hellocpp/main.cpp
         )
elseif(LINUX)
    list(APPEND GAME_SOURCE
         proj.linux/main.cpp
         )
elseif(WINDOWS)
    list(APPEND GAME_HEADER
         proj.win32/main.h
         proj.win32/resource.h
         )
    list(APPEND GAME_SOURCE
         proj.win32/main.cpp
         proj.win32/game.rc
         ${common_res_files}
         )
elseif(APPLE)
    if(IOS)
        list(APPEND GAME_HEADER
             proj.ios_mac/ios/AppController.h
             proj.ios_mac/ios/RootViewController.h
             )
        set(APP_UI_RES
            proj.ios_mac/ios/LaunchScreen.storyboard
            proj.ios_mac/ios/LaunchScreenBackground.png
            proj.ios_mac/ios/Images.xcassets
            )
        list(APPEND GAME_SOURCE
             proj.ios_mac/ios/main.m
             proj.ios_mac/ios/AppController.mm
             proj.ios_mac/ios/RootViewController.mm
             proj.ios_mac/ios/Prefix.pch
             ${APP_UI_RES}
             )
    elseif(MACOSX)
        set(APP_UI_RES
            proj.ios_mac/mac/Icon.icns
            proj.ios_mac/mac/Info.plist
            )
        list(APPEND GAME_SOURCE
             proj.ios_mac/mac/main.cpp
             proj.ios_mac/mac/Prefix.pch
             ${APP_UI_RES}
             )
    endif()
    list(APPEND GAME_SOURCE ${common_res_files})
endif()

# mark app complie info and libs info
set(APP_SRC
    ${GAME_SOURCE}
    ${GAME_HEADER}
    )

# 设置源文件编码属性
if(MSVC)
    set_source_files_properties(${GAME_SOURCE} PROPERTIES COMPILE_FLAGS "/utf-8")
    set_source_files_properties(${GAME_HEADER} PROPERTIES COMPILE_FLAGS "/utf-8")
endif()

cocos_build_app(${APP_NAME}
                APP_SRC "${APP_SRC}"
                DEPEND_COMMON_LIBS "cocos2d"
                DEPEND_ANDROID_LIBS "cocos2d_android"
                )

if(APPLE)
    set_target_properties(${APP_NAME} PROPERTIES RESOURCE "${APP_UI_RES}")
    if(MACOSX)
        set_target_properties(${APP_NAME} PROPERTIES
                              MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/proj.ios_mac/mac/Info.plist"
                              )
    elseif(IOS)
        cocos_pak_xcode(${APP_NAME} INFO_PLIST "iOSBundleInfo.plist.in")
        set_xcode_property(${APP_NAME} ASSETCATALOG_COMPILER_APPICON_NAME "AppIcon")
    endif()
elseif(WINDOWS)
    cocos_copy_target_dll(${APP_NAME} COPY_TO ${APP_BIN_DIR})
endif()

if(LINUX OR WINDOWS)
    cocos_copy_target_res(${APP_NAME} COPY_TO ${APP_RES_DIR} FOLDERS ${GAME_RES_FOLDER})
    # 复制configs文件夹
    cocos_copy_target_res(${APP_NAME} COPY_TO ${APP_RES_DIR} FOLDERS "${CMAKE_CURRENT_SOURCE_DIR}/configs")
endif()
