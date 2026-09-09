set(TEST_SOURCE_FILES
    TestCHR.cpp
    ../source/Configuration.cpp
    ../source/Emulation/APU.cpp
    ../source/Emulation/CHRData.cpp
    ../source/Emulation/Controller.cpp
    ../source/Emulation/MemoryAccess.cpp
    ../source/Emulation/PPU.cpp
    ../source/SMB/SMB.cpp
    ../source/SMB/SMBData.cpp
    ../source/SMB/SMBEngine.cpp
    ../source/Util/Video.cpp)

add_executable(test_chr ${TEST_SOURCE_FILES})
target_link_libraries(test_chr ${SDL2_LIBRARY} ${SDL2_LIBRARIES})
add_test(NAME test_chr COMMAND test_chr)
