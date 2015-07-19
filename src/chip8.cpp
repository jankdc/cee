#include "chip8.hpp"

#include <cassert>

#include <iostream>
#include <iomanip>
#include <algorithm>

static constexpr std::array<uint8_t, 80> CHIP8_FONTSET =
{{
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
}};

// This is the starting location on where the emulator should start
// reading any loaded program.
static constexpr size_t PROG_OFFSET = 512;

cee::Chip8::Chip8()
{
    this->reset();

    // Load chip8 fontset
    for (size_t i = 0; i < CHIP8_FONTSET.size(); i++)
    {
        mMemory[i] = CHIP8_FONTSET[i];
    }

    // Load operations
    #ifndef ADD_OP
    #define ADD_OP(n) mOps.emplace(n, std::bind(&Chip8::op##n, *this));

    ADD_OP(0xA000)
    ADD_OP(0x0000)

    #undef ADD_OP
    #endif
}

void cee::Chip8::loadProgram(std::vector<uint8_t> program)
{
    // Prevent Memory Overflow
    assert(program.size() < (mMemory.size() - PROG_OFFSET));
    for (size_t i = 0; i < program.size(); i++)
    {
        mMemory[i + PROG_OFFSET] = program[i];
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
    mStack.fill(0);        // Reset stack
    mKeys.fill(0);         // Reset key states
    mGfx.fill(0);          // Reset display

    // Reset memory (i.e. clear program data)
    // We don't clear before PROG_OFFSET because there are data
    // there that we use and doesn't need resetting such as the chip8 fontset.
    std::fill(mMemory.begin() + PROG_OFFSET, mMemory.end(), 0);
}

void cee::Chip8::updateCycle()
{
    // Fetch opcode
    mOpCode = mMemory[mCounter] << 8 | mMemory[mCounter + 1];

    // Decode opcode
    auto op = mOpCode & 0xF000;

    // In cases when the instruction prefix is at the end of the byte
    // e.g. 0x000E
    if (op == 0x0000)
    {
        op = mOpCode & 0x000F;
    }

    // Execute opcode
    if (mOps.count(op))
    {
        mOps[op]();
    }
    else
    {
        std::cerr << "Chip8 Error: Unknown OpCode 0x" << std::hex << op << "\n";
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

void cee::Chip8::op0xA000()
{
    // TODO
}

void cee::Chip8::op0x0000()
{
    // TODO
}
