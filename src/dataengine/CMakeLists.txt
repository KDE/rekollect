set(rekollectnotes_engine_SRCS
    rekollectnotesengine.cpp
    ../metadata/metadata.cpp
)

kde4_add_plugin(plasma_engine_rekollectnotes
                ${rekollectnotes_engine_SRCS}
)

target_link_libraries(plasma_engine_rekollectnotes
                        ${KDE4_KDECORE_LIBS}
                        ${KDE4_KIO_LIBS}
                        ${KDE4_PLASMA_LIBS}
)

install(TARGETS plasma_engine_rekollectnotes
        DESTINATION ${PLUGIN_INSTALL_DIR}
)

install(FILES plasma-engine-rekollectnotes.desktop
        DESTINATION ${SERVICES_INSTALL_DIR}
)
