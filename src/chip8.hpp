#pragma once

#ifndef CEE_CHIP8_HPP
#define CEE_CHIP8_HPP

#include <cstdint>
#include <vector>
#include <array>

namespace cee
{
    class Chip8
    {
    public:
        explicit Chip8();

        void reset();                                   // Reset emulation state to default settings
        void loadProgram(std::vector<uint8_t> program); // Load program into emulator's memory
        void updateCycle();                             // Emulates one cycle
        void updateKeys(std::array<uint8_t, 16> keys);  // Updates the key press inputs with new ones
        bool shouldDraw() const;                        // Should draw to screen
    private:
        uint16_t                  mIndex;        // Index Register
        uint16_t                  mCounter;      // Program Counter (PC)
        uint16_t                  mOpCode;       // Current OpCode
        uint16_t                  mStackPointer; // Current stack level
        uint8_t                   mDelayTimer;   // Counts down to 0
        uint8_t                   mSoundTimer;   // Counts down to 0, buzzes when 0
        std::array<uint16_t, 16>  mStack;        // 16 levels of stack
        std::array<uint8_t, 4096> mMemory;       // 4K available space
        std::array<uint8_t, 16>   mRegisters;    // General Purpose Registers
        std::array<uint8_t, 16>   mKeys;         // Current key state
        std::array<uint8_t, 2048> mGfx;          // 64 x 32 Pixel Resolution
    };
}

#endif // CEE_CHIP8_HPP
