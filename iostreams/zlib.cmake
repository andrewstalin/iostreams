if(MSVC)
    list(APPEND ZLIB_FILES 
        ${CMAKE_SOURCE_DIR}/3rdparty/zlib/adler32.c
        ${CMAKE_SOURCE_DIR}/3rdparty/zlib/compress.c
        ${CMAKE_SOURCE_DIR}/3rdparty/zlib/crc32.c
        ${CMAKE_SOURCE_DIR}/3rdparty/zlib/deflate.c
        ${CMAKE_SOURCE_DIR}/3rdparty/zlib/gzclose.c
        ${CMAKE_SOURCE_DIR}/3rdparty/zlib/gzlib.c
        ${CMAKE_SOURCE_DIR}/3rdparty/zlib/gzread.c
        ${CMAKE_SOURCE_DIR}/3rdparty/zlib/gzwrite.c
        ${CMAKE_SOURCE_DIR}/3rdparty/zlib/infback.c
        ${CMAKE_SOURCE_DIR}/3rdparty/zlib/inffast.c
        ${CMAKE_SOURCE_DIR}/3rdparty/zlib/inflate.c
        ${CMAKE_SOURCE_DIR}/3rdparty/zlib/inftrees.c
        ${CMAKE_SOURCE_DIR}/3rdparty/zlib/contrib/minizip/ioapi.c
        ${CMAKE_SOURCE_DIR}/3rdparty/zlib/contrib/minizip/unzip.c
        ${CMAKE_SOURCE_DIR}/3rdparty/zlib/contrib/minizip/zip.c
        ${CMAKE_SOURCE_DIR}/3rdparty/zlib/trees.c
        ${CMAKE_SOURCE_DIR}/3rdparty/zlib/zutil.c
        ${CMAKE_SOURCE_DIR}/3rdparty/zlib/uncompr.c
    )

    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        execute_process(COMMAND bld_ml64.bat
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/3rdparty/zlib/contrib/masmx86
        )  

        list(APPEND ZLIB_FILES ${CMAKE_SOURCE_DIR}/3rdparty/zlib/contrib/masmx64/inffas8664.c)
    else()
        execute_process(COMMAND bld_ml32.bat
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/3rdparty/zlib/contrib/masmx86
        )  
    endif()

    add_library(zlib STATIC ${ZLIB_FILES})
    set_named_compiler_options(zlib)

    target_compile_definitions(zlib PRIVATE 
        _CRT_NONSTDC_NO_DEPRECATE
        _CRT_SECURE_NO_DEPRECATE
        _CRT_NONSTDC_NO_WARNINGS
    )

    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        target_compile_definitions(zlib PRIVATE 
            ASMV
            ASMINF
        )
    endif()

    list(APPEND ZLIB_INCLUDE_DIRS "${CMAKE_SOURCE_DIR}/3rdparty/zlib")
    set_property(TARGET zlib APPEND PROPERTY INCLUDE_DIRECTORIES ${ZLIB_INCLUDE_DIRS})
else()
    add_library(zlib STATIC IMPORTED)
    set_property(TARGET zlib APPEND PROPERTY IMPORTED_CONFIGURATIONS NOCONFIG)
    set_target_properties(zlib PROPERTIES IMPORTED_LOCATION_NOCONFIG "${CMAKE_SOURCE_DIR}/3rdparty/zlib/libz.a")

    add_custom_target(libzlib ALL
        COMMAND ./configure
        --static
        COMMAND ${CMAKE_MAKE_PROGRAM}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/3rdparty/zlib
    )
endif()
