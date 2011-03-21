add_library(rekollectiohtml
            htmlwriter.cpp
)

target_link_libraries(rekollectiohtml
                        ${KDE4_KDEUI_LIBS}
                        ${KDE4_KIO_LIBS})

install(TARGETS rekollectiohtml ${INSTALL_TARGETS_DEFAULT_ARGS} )
