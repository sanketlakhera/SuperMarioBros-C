#include <iostream>
#include <cassert>
#include <cstdint>
#include <cstring>

#include "../source/Emulation/CHRData.hpp"
#include "../source/Emulation/PPU.hpp"
#include "../source/SMB/SMBEngine.hpp"
#include "../source/Util/Video.hpp"
#include "../source/Constants.hpp"

// Simple test runner
static int testsRun = 0;
static int testsPassed = 0;

#define TEST_ASSERT(cond, msg) \
    do { \
        testsRun++; \
        if (!(cond)) { \
            std::cerr << "FAIL: " << msg << " (" << __FILE__ << ":" << __LINE__ << ")\n"; \
            return false; \
        } else { \
            testsPassed++; \
        } \
    } while(0)

// Test 1: CHR data dimensions and non-emptiness
bool testCHRDataDimensions()
{
    std::cout << "[RUN] testCHRDataDimensions...\n";
    TEST_ASSERT(sizeof(CHRData::CHR_DATA) == 8192, "CHR_DATA must be exactly 8192 bytes");

    // Ensure it is not all zeros
    bool hasNonZero = false;
    for (size_t i = 0; i < 8192; i++)
    {
        if (CHRData::CHR_DATA[i] != 0)
        {
            hasNonZero = true;
            break;
        }
    }
    TEST_ASSERT(hasNonZero, "CHR_DATA must contain non-zero tile data");
    std::cout << "[PASS] testCHRDataDimensions\n";
    return true;
}

// Test 2: Verify known tile patterns in CHRData
bool testCHRDataKnownPatterns()
{
    std::cout << "[RUN] testCHRDataKnownPatterns...\n";

    // Tile 0 first 16 bytes match the authentic extracted ROM bytes
    const uint8_t expectedTile0[16] = {
        0x03, 0x0f, 0x1f, 0x1f, 0x1c, 0x24, 0x26, 0x66,
        0x00, 0x00, 0x00, 0x00, 0x1f, 0x3f, 0x3f, 0x7f
    };
    int cmp = memcmp(CHRData::CHR_DATA, expectedTile0, 16);
    TEST_ASSERT(cmp == 0, "Tile 0 must match authentic SMB CHR data");

    // Tile 1 (offset 16)
    const uint8_t expectedTile1[16] = {
        0xe0, 0xc0, 0x80, 0xfc, 0x80, 0xc0, 0x00, 0x20,
        0x00, 0x20, 0x60, 0x00, 0xf0, 0xfc, 0xfe, 0xfe
    };
    cmp = memcmp(CHRData::CHR_DATA + 16, expectedTile1, 16);
    TEST_ASSERT(cmp == 0, "Tile 1 must match authentic SMB CHR data");

    std::cout << "[PASS] testCHRDataKnownPatterns\n";
    return true;
}

// Test 3: Standalone SMBEngine lifecycle without ROM
bool testEngineStandalone()
{
    std::cout << "[RUN] testEngineStandalone (running 60 frames without ROM)...\n";
    SMBEngine engine;

    // Reset should succeed without crash
    engine.reset();

    // Step 60 frames (1 second of game time)
    for (int frame = 0; frame < 60; frame++)
    {
        engine.update();
    }

    // Allocate render buffer
    static uint32_t renderBuffer[RENDER_WIDTH * RENDER_HEIGHT];
    memset(renderBuffer, 0, sizeof(renderBuffer));

    engine.render(renderBuffer);

    // Verify that pixels were actually rendered to the buffer
    int nonZeroPixels = 0;
    for (int i = 0; i < RENDER_WIDTH * RENDER_HEIGHT; i++)
    {
        if (renderBuffer[i] != 0)
        {
            nonZeroPixels++;
        }
    }

    TEST_ASSERT(nonZeroPixels > 0, "Engine must produce non-zero pixels upon render without ROM");
    std::cout << "[PASS] testEngineStandalone (rendered " << nonZeroPixels << " active pixels)\n";
    return true;
}

// Test 4: Video utility tile drawing with embedded CHR
bool testVideoDrawCHRTile()
{
    std::cout << "[RUN] testVideoDrawCHRTile...\n";
    static uint32_t buffer[256 * 240];
    memset(buffer, 0, sizeof(buffer));

    // Draw text using characters from CHR
    drawText(buffer, 10, 10, "MARIO", 0x123456);

    int coloredPixels = 0;
    for (int i = 0; i < 256 * 240; i++)
    {
        if (buffer[i] != 0)
        {
            coloredPixels++;
        }
    }

    TEST_ASSERT(coloredPixels > 0, "drawText must render non-zero pixels using embedded CHR data");
    std::cout << "[PASS] testVideoDrawCHRTile (rendered " << coloredPixels << " text pixels)\n";
    return true;
}

int main()
{
    std::cout << "========================================\n";
    std::cout << " Running SuperMarioBros-C Phase 1 Tests \n";
    std::cout << "========================================\n";

    bool allPassed = true;
    allPassed = testCHRDataDimensions() && allPassed;
    allPassed = testCHRDataKnownPatterns() && allPassed;
    allPassed = testEngineStandalone() && allPassed;
    allPassed = testVideoDrawCHRTile() && allPassed;

    std::cout << "========================================\n";
    std::cout << " Tests run: " << testsRun << ", Passed: " << testsPassed << "\n";
    if (allPassed)
    {
        std::cout << " ALL TESTS PASSED!\n";
        return 0;
    }
    else
    {
        std::cout << " SOME TESTS FAILED!\n";
        return 1;
    }
}
