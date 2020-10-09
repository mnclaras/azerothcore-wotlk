if ( MSVC )
  add_custom_command(TARGET worldserver
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_FACTIONICONSCHANNELS_DIR}/conf/factioniconschannels.conf.dist ${CMAKE_BINARY_DIR}/bin/$(ConfigurationName)/
  )
elseif ( MINGW )
  add_custom_command(TARGET worldserver
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_FACTIONICONSCHANNELS_DIR}/conf/factioniconschannels.conf.dist ${CMAKE_BINARY_DIR}/bin/
  )
endif()

install(FILES "${CMAKE_FACTIONICONSCHANNELS_DIR}/conf/factioniconschannels.conf.dist" DESTINATION ${CONF_DIR})
