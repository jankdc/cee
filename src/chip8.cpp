#include "chip8.hpp"

#include <cassert>

#include <iostream>
#include <iomanip>

static constexpr std::array<uint8_t, 80> CHIP8_FONTSET =
{
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

cee::Chip8::Chip8()
{
    this->reset();
}

void cee::Chip8::loadProgram(std::vector<uint8_t> program)
{
    constexpr size_t OFFSET = 512;
    assert(program.size() < (mMemory.size() - OFFSET)); // Prevent Memory Overflow
    for (size_t i = 0; i < program.size(); i++)
    {
        mMemory[i + OFFSET] = program[i];
    }
}

void cee::Chip8::reset()
{
    mCounter      = 0x200; // Program counter starts at 0x200
    mOpCode       = 0;     // Reset current opcode
    mIndex        = 0;     // Reset index register
    mStackPointer = 0;     // Reset stack pointer
    mDelayTimer   = 0;     // Reset delay timer
    mSoundTimer   = 0;     // Reset sound timer
    mRegisters.fill(0);    // Reset registers
    mMemory.fill(0);       // Reset memory
    mStack.fill(0);        // Reset stack
    mKeys.fill(0);         // Reset key states
    mGfx.fill(0);          // Reset display

    // Load chip8 fontset
    for (size_t i = 0; i < CHIP8_FONTSET.size(); i++)
    {
        mMemory[i] = CHIP8_FONTSET[i];
    }
}

void cee::Chip8::updateCycle()
{
    // Fetch opcode
    mOpCode = mMemory[mCounter] << 8 | mMemory[mCounter + 1];

    // Decode opcode
    const auto op = mOpCode & 0xF000;

    // Execute opcode
    switch (op)
    {
        case 0xA000: // ANNN: Sets Index to the address NNN
            mIndex = mOpCode & 0x0FFF;
            mCounter += 2;
            break;
        default:
            std::cerr << "Chip8 Error: Unknown OpCode: 0x" << std::hex << op << "\n";
    }

    // Update timers
    if (mDelayTimer > 0)
    {
        mDelayTimer -= 1;
    }

    if (mSoundTimer > 0)
    {
        mSoundTimer -= 1;
    }
    else
    {
        std::cout << "Beep!\n";
    }
}

void cee::Chip8::updateKeys(std::array<uint8_t, 16> keys)
{
    mKeys = keys;
}

bool cee::Chip8::shouldDraw() const
{
    return true;
}
