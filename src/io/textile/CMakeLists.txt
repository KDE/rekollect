add_library(rekollectiotextile
            textilewriter.cpp
)

target_link_libraries(rekollectiotextile
                        ${KDE4_KDEUI_LIBS}
                        ${KDE4_KIO_LIBS})

install(TARGETS rekollectiotextile ${INSTALL_TARGETS_DEFAULT_ARGS} )
