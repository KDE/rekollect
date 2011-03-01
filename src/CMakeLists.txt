set(rekollect_SRCS
   notefactory.cpp
   rekollectapplication.cpp
   datetimeitem.cpp
   negativefilterproxymodel.cpp
   linkeditdialog.cpp
   systemtrayicon.cpp
   notebrowserwindow.cpp
   notecollection.cpp
   note.cpp
   noteeditor.cpp
   notewindow.cpp
   windowcollection.cpp
   main.cpp
)

if (${CMAKE_BUILD_TYPE} MATCHES "Debug")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D DEBUG")
endif()

kde4_add_ui_files(rekollect_SRCS settings_general.ui settings_display.ui edit_link.ui)

kde4_add_kcfg_files(rekollect_SRCS settings.kcfgc )

kde4_add_executable(rekollect ${rekollect_SRCS})

target_link_libraries(rekollect rekollectionative
                                rekollectiotextile
                                ${KDE4_KDEUI_LIBS}
                                ${KDE4_KIO_LIBS}
                                ${KDE4_KUTILS_LIBS}
                                ${QT_QTXMLPATTERNS_LIBRARY})

install(TARGETS rekollect ${INSTALL_TARGETS_DEFAULT_ARGS} )


########### install files ###############

install( FILES rekollect.desktop  DESTINATION ${XDG_APPS_INSTALL_DIR} )
install( FILES rekollect.kcfg  DESTINATION  ${KCFG_INSTALL_DIR} )
install( FILES rekollectui.rc  DESTINATION  ${DATA_INSTALL_DIR}/rekollect )

add_subdirectory(io)