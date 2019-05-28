# Generated from xcb.pro.

# begin special case:
qt_find_package(X11_XCB)
qt_find_package(X11)
qt_find_package(XCB)
qt_find_package(XKB)
qt_find_package(PkgConfig)
qt_find_package(WrapFreetype)
qt_find_package(GLIB2)

pkg_check_modules(XKB_COMMON_X11 xkbcommon-x11>=0.4.1 IMPORTED_TARGET) # special case
# end special case:

#####################################################################
## XcbQpa Module:
#####################################################################

add_qt_module(XcbQpa
    NO_MODULE_HEADERS
    SOURCES
        gl_integrations/qxcbglintegration.cpp gl_integrations/qxcbglintegration.h
        gl_integrations/qxcbglintegrationfactory.cpp gl_integrations/qxcbglintegrationfactory.h
        gl_integrations/qxcbglintegrationplugin.h
        gl_integrations/qxcbnativeinterfacehandler.cpp gl_integrations/qxcbnativeinterfacehandler.h
        qxcbatom.cpp qxcbatom.h
        qxcbbackingstore.cpp qxcbbackingstore.h
        qxcbclipboard.cpp qxcbclipboard.h
        qxcbconnection.cpp qxcbconnection.h
        qxcbconnection_basic.cpp qxcbconnection_basic.h
        qxcbconnection_screens.cpp
        qxcbcursor.cpp qxcbcursor.h
        qxcbeventdispatcher.cpp qxcbeventdispatcher.h
        qxcbeventqueue.cpp qxcbeventqueue.h
        qxcbimage.cpp qxcbimage.h
        qxcbintegration.cpp qxcbintegration.h
        qxcbkeyboard.cpp qxcbkeyboard.h
        qxcbmime.cpp qxcbmime.h
        qxcbnativeinterface.cpp qxcbnativeinterface.h
        qxcbobject.h
        qxcbscreen.cpp qxcbscreen.h
        qxcbsystemtraytracker.cpp qxcbsystemtraytracker.h
        qxcbwindow.cpp qxcbwindow.h
        qxcbwmsupport.cpp qxcbwmsupport.h
        qxcbxsettings.cpp qxcbxsettings.h
    DEFINES
        QT_NO_FOREACH
        QT_BUILD_XCB_PLUGIN
    INCLUDE_DIRECTORIES
        gl_integrations
    LIBRARIES
        Qt::CorePrivate
        Qt::EdidSupportPrivate
        Qt::FontDatabaseSupportPrivate
        Qt::GuiPrivate
        Qt::ServiceSupportPrivate
        Qt::ThemeSupportPrivate
        Qt::XkbCommonSupportPrivate
        PkgConfig::XKB_COMMON_X11 # special case
    PUBLIC_LIBRARIES
        ${CMAKE_DL_LIBS}
        Qt::Core
        Qt::EdidSupport
        Qt::FontDatabaseSupport
        Qt::Gui
        Qt::ServiceSupport
        Qt::ThemeSupport
        Qt::XkbCommonSupport
        XCB::ICCCM
        XCB::IMAGE
        XCB::KEYSYMS
        XCB::RANDR
        XCB::RENDER
        XCB::RENDERUTIL
        XCB::SHAPE
        XCB::SHM
        XCB::SYNC
        XCB::XCB
        XCB::XFIXES
        XCB::XINERAMA
        XKB::XKB
)

#### Keys ignored in scope 3:.:.:xcb_qpa_lib.pro:<TRUE>:
# CONFIG = "no_module_headers" "internal_module"
# _LOADED = "qt_build_paths" "qt_module"

## Scopes:
#####################################################################

extend_target(XcbQpa CONDITION TARGET Qt::LinuxAccessibilitySupportPrivate
    LIBRARIES
        Qt::LinuxAccessibilitySupportPrivate
    PUBLIC_LIBRARIES
        Qt::LinuxAccessibilitySupport
)

extend_target(XcbQpa CONDITION QT_FEATURE_vulkan
    SOURCES
        qxcbvulkaninstance.cpp qxcbvulkaninstance.h
        qxcbvulkanwindow.cpp qxcbvulkanwindow.h
    LIBRARIES
        Qt::VulkanSupportPrivate
    PUBLIC_LIBRARIES
        Qt::VulkanSupport
)

extend_target(XcbQpa CONDITION QT_FEATURE_glib
    LIBRARIES
        GLIB2::GLIB2
)

extend_target(XcbQpa CONDITION QT_FEATURE_draganddrop
    SOURCES
        qxcbdrag.cpp qxcbdrag.h
)

extend_target(XcbQpa CONDITION QT_FEATURE_xcb_xlib
    PUBLIC_LIBRARIES
        X11::XCB
)

extend_target(XcbQpa CONDITION QT_FEATURE_xcb_xinput
    SOURCES
        qxcbconnection_xi2.cpp
    PUBLIC_LIBRARIES
        XCB::XINPUT
)

extend_target(XcbQpa CONDITION QT_FEATURE_xcb_sm
    SOURCES
        qxcbsessionmanager.cpp qxcbsessionmanager.h
    PUBLIC_LIBRARIES
        ${X11_SM_LIB} ${X11_ICE_LIB}
)

extend_target(XcbQpa CONDITION QT_FEATURE_xkb
    PUBLIC_LIBRARIES
        XCB::XKB
        XKB::XKB
)

extend_target(XcbQpa CONDITION CLANG AND NOT ICC
    COMPILE_OPTIONS
        "-ftemplate-depth=1024"
)

extend_target(XcbQpa CONDITION QT_FEATURE_xcb_native_painting
    SOURCES
        nativepainting/qbackingstore_x11.cpp nativepainting/qbackingstore_x11_p.h
        nativepainting/qcolormap_x11.cpp nativepainting/qcolormap_x11_p.h
        nativepainting/qpaintengine_x11.cpp nativepainting/qpaintengine_x11_p.h
        nativepainting/qpixmap_x11.cpp nativepainting/qpixmap_x11_p.h
        nativepainting/qt_x11_p.h
        nativepainting/qtessellator.cpp nativepainting/qtessellator_p.h
        nativepainting/qxcbnativepainting.cpp nativepainting/qxcbnativepainting.h
    INCLUDE_DIRECTORIES
        nativepainting
)

extend_target(XcbQpa CONDITION QT_FEATURE_xcb_native_painting AND QT_FEATURE_xrender
    PUBLIC_LIBRARIES
        PkgConfig::xrender
)

extend_target(XcbQpa CONDITION QT_FEATURE_fontconfig AND QT_FEATURE_xcb_native_painting
    LIBRARIES
        WrapFreetype::WrapFreetype
)
#####################################################################
## qxcb Plugin:
#####################################################################

add_qt_plugin(qxcb
    TYPE platforms
    SOURCES
        qxcbmain.cpp
    DEFINES
        QT_NO_FOREACH
    LIBRARIES
        Qt::CorePrivate
        Qt::GuiPrivate
        Qt::XcbQpaPrivate
    PUBLIC_LIBRARIES
        Qt::Core
        Qt::Gui
        Qt::XcbQpa
)

#### Keys ignored in scope 24:.:.:xcb-plugin.pro:<TRUE>:
# OTHER_FILES = "xcb.json" "README"
# PLUGIN_CLASS_NAME = "QXcbIntegrationPlugin"
# _LOADED = "qt_plugin"

## Scopes:
#####################################################################

#### Keys ignored in scope 25:.:.:xcb-plugin.pro:APPLE_OSX:
# CONFIG = "no_app_extension_api_only"

#### Keys ignored in scope 26:.:.:xcb-plugin.pro:NOT TARGET___equals____ss_QT_DEFAULT_QPA_PLUGIN:
# PLUGIN_EXTENDS = "-"
add_subdirectory(gl_integrations)

#### TODO: xcb-static sub folder # special case