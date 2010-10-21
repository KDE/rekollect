add_library(rekollectionative
            notereader.cpp
            notewriter.cpp
)

target_link_libraries(rekollectionative
                        ${KDE4_KDEUI_LIBS}
                        ${KDE4_KIO_LIBS}
                        ${KDE4_KHTML_LIBS}
                        ${QT_QTXMLPATTERNS_LIBRARY})


install(TARGETS rekollectionative ${INSTALL_TARGETS_DEFAULT_ARGS} )