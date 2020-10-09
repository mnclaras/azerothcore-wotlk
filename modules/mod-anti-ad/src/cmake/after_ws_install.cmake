if ( MSVC )
  add_custom_command(TARGET worldserver
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_ANTIAD_DIR}/conf/antiad.conf.dist ${CMAKE_BINARY_DIR}/bin/$(ConfigurationName)/
  )
elseif ( MINGW )
  add_custom_command(TARGET worldserver
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_ANTIAD_DIR}/conf/antiad.conf.dist ${CMAKE_BINARY_DIR}/bin/
  )
endif()

install(FILES "${CMAKE_ANTIAD_DIR}/conf/antiad.conf.dist" DESTINATION ${CONF_DIR})
