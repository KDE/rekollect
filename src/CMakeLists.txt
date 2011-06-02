set(rekollect_SRCS
   notefactory.cpp
   noteparser.cpp
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
   note/document.cpp
   note/paragraph.cpp
   note/fragment.cpp
   metadata/metadata.cpp

)

if (${CMAKE_BUILD_TYPE} MATCHES "Debug")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D DEBUG")
endif()

kde4_add_ui_files(rekollect_SRCS settings_general.ui settings_display.ui edit_link.ui)

kde4_add_kcfg_files(rekollect_SRCS settings.kcfgc )

kde4_add_executable(rekollect ${rekollect_SRCS})

target_link_libraries(rekollect rekollectionative
                                rekollectiotextile
                                rekollectiohtml
                                ${KDE4_KDEUI_LIBS}
                                ${KDE4_KIO_LIBS}
                                ${KDE4_KUTILS_LIBS}
                                ${QT_QTXMLPATTERNS_LIBRARY})

install(TARGETS rekollect ${INSTALL_TARGETS_DEFAULT_ARGS} )


########### install files ###############

install( FILES rekollect.desktop  DESTINATION ${XDG_APPS_INSTALL_DIR} )
install( FILES rekollect.kcfg  DESTINATION  ${KCFG_INSTALL_DIR} )
install( FILES rekollectui.rc  DESTINATION  ${DATA_INSTALL_DIR}/rekollect )

########### install plasmoid ############

install ( DIRECTORY applet/ DESTINATION ${DATA_INSTALL_DIR}/plasma/plasmoids/rekollect-notes )
install ( FILES applet/metadata.desktop DESTINATION ${SERVICES_INSTALL_DIR} RENAME plasma-applet-rekollect-notes.desktop )

add_subdirectory(metadata)
add_subdirectory(io)
add_subdirectory(note)
add_subdirectory(dataengine)
