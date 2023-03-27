cmake_minimum_required(VERSION 3.15)

if(NOT TARGET flags)
    add_library(flags INTERFACE)
endif()

###  Add optional RELEASE/DEBUG compile to flags
target_compile_options(flags INTERFACE $<$<AND:$<CONFIG:DEBUG>,$<CXX_COMPILER_ID:Clang>>: -fstandalone-debug>)
target_compile_options(flags INTERFACE $<$<AND:$<CONFIG:RELWITHDEBINFO>,$<CXX_COMPILER_ID:Clang>>: -fstandalone-debug>)
target_compile_options(flags INTERFACE
                       $<$<AND:$<COMPILE_LANGUAGE:CXX>,$<CXX_COMPILER_ID:MSVC>>:/W4>
                       $<$<AND:$<COMPILE_LANGUAGE:CXX>,$<NOT:$<CXX_COMPILER_ID:MSVC>>>:-Wall -Wextra -Wpedantic -Wconversion -Wunused>)

###  Enable c++17 support
target_compile_features(flags INTERFACE cxx_std_17)

# Settings for sanitizers
if(COMPILER_ENABLE_ASAN)
    target_compile_options(flags INTERFACE $<$<COMPILE_LANGUAGE:CXX>:-fsanitize=address>) #-fno-omit-frame-pointer
    target_link_libraries(flags INTERFACE -fsanitize=address)
endif()
if(COMPILER_ENABLE_USAN)
    target_compile_options(flags INTERFACE $<$<COMPILE_LANGUAGE:CXX>:-fsanitize=undefined,leak,pointer-compare,pointer-subtract,alignment,bounds -fsanitize-undefined-trap-on-error>) #  -fno-omit-frame-pointer
    target_link_libraries(flags INTERFACE -fsanitize=undefined,leak,pointer-compare,pointer-subtract,alignment,bounds -fsanitize-undefined-trap-on-error)
endif()


# Enable static linking
function(target_enable_static_libgcc tgt)
    if(BUILD_SHARED_LIBS)
        return()
    endif()
    message(STATUS "Enabling static linking on target [${tgt}]")
    target_link_options(${tgt} BEFORE PUBLIC
                        $<$<COMPILE_LANG_AND_ID:CXX,GNU>:-static-libstdc++ -static-libgcc>
                        $<$<COMPILE_LANG_AND_ID:CXX,Clang>:-static-libgcc>
                        )
endfunction()
