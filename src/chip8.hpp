#pragma once

#ifndef CEE_CHIP8_HPP
#define CEE_CHIP8_HPP

#include <cstdint>

#include <functional>
#include <vector>
#include <array>
#include <random>
#include <map>

#include "keys.hpp"

namespace cee
{
    class Chip8
    {
    public:
        explicit Chip8();

        void reset();                                    // Reset emulation state to default settings
        void loadProgram(std::vector<uint8_t> program);  // Load program into emulator's memory
        void updateKeys(cee::Keys keys);                 // Updates key states
        void updateCycle();                              // Emulates one cycle

        const uint8_t * getGfx() const;                  // Chip8 Graphics Representation.
        bool            isBeeping() const;               // Check if the emulator is beeping.
    private:
        using Op   = std::function<void()>;
        using Dist = std::uniform_int_distribution<uint8_t>;

        uint16_t                  mIndex;        // Index Register
        uint16_t                  mCounter;      // Program Counter (PC)
        uint16_t                  mOpCode;       // Current OpCode
        uint16_t                  mStackPointer; // Current stack level
        uint8_t                   mDelayTimer;   // Counts down to 0
        uint8_t                   mSoundTimer;   // Counts down to 0, buzzes when 0
        std::array<uint16_t, 16>  mStack;        // 16 levels of stack
        std::array<uint8_t, 4096> mMemory;       // 4K available space
        std::array<uint8_t, 16>   mRegisters;    // General Purpose Registers
        std::array<uint8_t, 2048> mGfx;          // 64 x 32 Pixel Resolution
        std::map<uint16_t, Op>    mOps;          // Mapping of operations (Ops)
        std::mt19937              mRandGen;      // Pseudo-Random Number Generator
        cee::Keys                 mKeys;         // Current key states
        Dist                      mDist;         // Random Distribution between 0 - 255

        // Operations based on opcode
        void op0x0000(); // Calls RCA 1802 program at address NNN.
        void op0x00E0(); // Clears the screen.
        void op0x00EE(); // Returns from a subroutine.
        void op0x1000(); // Jumps to address NNN.
        void op0x2000(); // Calls subroutine at NNN.
        void op0x3000(); // Skips the next instruction if VX equals NN.
        void op0x4000(); // Skips the next instruction if VX doesn't equal NN.
        void op0x5000(); // Skips the next instruction if VX equals VY.
        void op0x6000(); // Sets VX to NN.
        void op0x7000(); // Adds NN to VX.
        void op0x8000(); // Sets VX to the value of VY.
        void op0x8001(); // Sets VX to VX or VY.
        void op0x8002(); // Sets VX to VX and VY.
        void op0x8003(); // Sets VX to VX xor VY.
        void op0x8004(); // Adds VY to VX. VF is set to 1 when carry, and to 0 when isn't.
        void op0x8005(); // VY is subtracted from VX. VF is set to 0 when borrow, and 1 when isn't.
        void op0x8006(); // Shifts VX right by 1. VF is set value of the least sig bit of VX before shift.
        void op0x8007(); // Sets VX to VY minus VX. VF is set to 0 when there's a borrow, and 1 when isn't.
        void op0x800E(); // Shifts VX left by 1. VF is set value of the most sig bit of VX before shift.
        void op0x9000(); // Skips the next instruction if VX doesn't equal VY.
        void op0xA000(); // Sets I to the address NNN.
        void op0xB000(); // Jumps to the address NNN plus V0.
        void op0xC000(); // Sets VX to a random number, masked by NN.

        // Draws a sprite (which is a sequence of bytes)
        // at coordinate (VX, VY) that has a width of 8 pixels and
        // a height of N pixels.
        // Each row of 8 pixels is read as bit-coded starting from memory location I;
        // I value doesn’t change after the execution of this instruction.
        // As described above, VF is set to 1 if any screen pixels are flipped from set
        // to unset when the sprite is drawn, and to 0 if that doesn’t happen.
        void op0xD000();

        void op0xE09E(); // Skips the next instruction if the key stored in VX is pressed.
        void op0xE0A1(); // Skips the next instruction if the key stored in VX isn't pressed.
        void op0xF007(); // Sets VX to the value of the delay timer.
        void op0xF00A(); // A key press is awaited, and then stored in VX.
        void op0xF015(); // Sets the delay timer to VX.
        void op0xF018(); // Sets the sound timer to VX.
        void op0xF01E(); // Adds VX to I.

        // Sets I to the location of the sprite for the character in VX.
        // Characters 0-F (in hexadecimal) are represented by a 4x5 font.
        void op0xF029();

        // Stores the Binary-coded decimal representation of VX,
        // with the most significant of three digits at the address in I,
        // the middle digit at I plus 1, and the least significant digit at I plus 2.
        // (In other words, take the decimal representation of VX,
        // place the hundreds digit in memory at location in I, the tens digit at location I+1,
        // and the ones digit at location I+2.).
        void op0xF033();

        void op0xF055(); // Stores V0 to VX in memory starting at address I.
        void op0xF065(); // Fills V0 to VX with values from memory starting at address I.
    };
}

#endif // CEE_CHIP8_HPP
