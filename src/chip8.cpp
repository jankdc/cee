#include "chip8.hpp"

#include <cassert>

#include <bitset>
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
        mMemory[i] = CHIP8_FONTSET[i];

    // Load operations
    #ifndef ADD_OP
    #define ADD_OP(n) mOps.emplace(n, std::bind(&Chip8::op##n, this));

    ADD_OP(0x0000)
    ADD_OP(0x000E)
    ADD_OP(0x00EE)
    ADD_OP(0x1000)
    ADD_OP(0x2000)
    ADD_OP(0x3000)
    ADD_OP(0x4000)
    ADD_OP(0x5000)
    ADD_OP(0x6000)
    ADD_OP(0x7000)
    ADD_OP(0x8000)
    ADD_OP(0x8001)
    ADD_OP(0x8002)
    ADD_OP(0x8003)
    ADD_OP(0x8004)
    ADD_OP(0x8005)
    ADD_OP(0x8006)
    ADD_OP(0x8007)
    ADD_OP(0x800E)
    ADD_OP(0x9000)
    ADD_OP(0xA000)
    ADD_OP(0xB000)
    ADD_OP(0xC000)
    ADD_OP(0xD000)
    ADD_OP(0xE0A1)
    ADD_OP(0xE09E)
    ADD_OP(0xF007)
    ADD_OP(0xF00A)
    ADD_OP(0xF015)
    ADD_OP(0xF018)
    ADD_OP(0xF01E)
    ADD_OP(0xF029)
    ADD_OP(0xF033)
    ADD_OP(0xF055)
    ADD_OP(0xF065)

    #undef ADD_OP
    #endif // ADD_OP
}

void cee::Chip8::loadProgram(std::vector<uint8_t> program)
{
    // Prevent Memory Overflow
    assert(program.size() < (mMemory.size() - PROG_OFFSET));
    for (size_t i = 0; i < program.size(); i++)
        mMemory[i + PROG_OFFSET] = program[i];
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
    mKeyStates.fill(0);    // Reset key states
    mGfx.fill(0);          // Reset display

    // Reset memory (i.e. clear program data)
    // We don't clear before PROG_OFFSET because there are data
    // there that we use and doesn't need resetting such as the chip8 fontset.
    std::fill(mMemory.begin() + PROG_OFFSET, mMemory.end(), 0);

    // Get a new random number as seed for pseudo-random generator.
    // Also acts as a restart procedure for the generator.
    std::random_device rd;
    mRandGen.seed(rd());

}

void cee::Chip8::updateCycle()
{
    // Fetch opcode
    mOpCode = mMemory[mCounter] << 8 | mMemory[mCounter + 1];

    // Decode opcode
    auto op = mOpCode & 0xF000;

    // In cases when there's variation of options for an instruction.
    switch (op)
    {
    case 0x0000: case 0xE000: case 0xF000:
        op = mOpCode & 0xF0FF;
        break;
    case 0x8000:
        op = mOpCode & 0xF00F;
        break;
    default:
        // Do nothing...
        break;
    }

    // Execute opcode
    if (mOps.count(op)) mOps[op]();
    else printf("Chip8 Error: Unknown OpCode 0x%x\n", op);

    // Update delay timer
    if (mDelayTimer > 0) mDelayTimer -= 1;
    else // Do nothing

    // Update sound timer
    if (mSoundTimer > 0) mSoundTimer -= 1;
    else printf("Beep!\n");
}

void cee::Chip8::updateKeys(std::array<uint8_t, 17> keyStates)
{
    mKeyStates = keyStates;
}

/*
  ___  ____   ____ ___  ____  _____ ____
 / _ \|  _ \ / ___/ _ \|  _ \| ____/ ___|
| | | | |_) | |  | | | | | | |  _| \___ \
| |_| |  __/| |__| |_| | |_| | |___ ___) |
 \___/|_|    \____\___/|____/|_____|____/

*/

// Calls RCA 1802 program at address NNN.
void cee::Chip8::op0x0000()
{
    // This is ignored since we don't have that microprocessor.
}

// Clears the screen.
void cee::Chip8::op0x000E()
{
    mGfx.fill(0);
    mCounter += 2;
}

// Returns from a subroutine.
void cee::Chip8::op0x00EE()
{
    mCounter = mStack[mStackPointer];
    mStackPointer -= 1;
    mCounter += 2;
}

// Jumps to address NNN.
void cee::Chip8::op0x1000()
{
    mCounter = mOpCode & 0x0FFF;
}

// Calls subroutine at NNN.
void cee::Chip8::op0x2000()
{
    mStack[mStackPointer] = mCounter;
    mStackPointer += 1;
    mCounter = mOpCode & 0x0FFF;
}

// Skips the next instruction if VX equals NN.
void cee::Chip8::op0x3000()
{
    uint8_t vx = mRegisters[(mOpCode & 0x0F00) >> 8];
    uint8_t nn = mOpCode & 0x00FF;
    mCounter += (vx == nn ? 4 : 2);
}

// Skips the next instruction if VX doesn't equal NN.
void cee::Chip8::op0x4000()
{
    uint8_t vx = mRegisters[(mOpCode & 0x0F00) >> 8];
    uint8_t nn = mOpCode & 0x00FF;
    mCounter += (vx == nn ? 2 : 4);
}

// Skips the next instruction if VX equals VY.
void cee::Chip8::op0x5000()
{
    uint8_t vx = mRegisters[(mOpCode & 0x0F00) >> 8];
    uint8_t vy = mRegisters[(mOpCode & 0x00F0) >> 4];
    mCounter += (vx == vy ? 4 : 2);
}

// Sets VX to NN.
void cee::Chip8::op0x6000()
{
    mRegisters[(mOpCode & 0x0F00) >> 8] = mOpCode & 0x00FF;
    mCounter += 2;
}

// Adds NN to VX.
void cee::Chip8::op0x7000()
{
    mRegisters[(mOpCode & 0x0F00) >> 8] += mOpCode & 0x00FF;
    mCounter += 2;
}

// Sets VX to the value of VY.
void cee::Chip8::op0x8000()
{
    mRegisters[(mOpCode & 0x0F00) >> 8] = mRegisters[(mOpCode & 0x00F0) >> 4];
    mCounter += 2;
}

// Sets VX to VX or VY.
void cee::Chip8::op0x8001()
{
    mRegisters[(mOpCode & 0x0F00) >> 8] |= mRegisters[(mOpCode & 0x00F0) >> 4];
    mCounter += 2;
}

// Sets VX to VX and VY.
void cee::Chip8::op0x8002()
{
    mRegisters[(mOpCode & 0x0F00) >> 8] &= mRegisters[(mOpCode & 0x00F0) >> 4];
    mCounter += 2;
}

// Sets VX to VX xor VY.
void cee::Chip8::op0x8003()
{
    mRegisters[(mOpCode & 0x0F00) >> 8] ^= mRegisters[(mOpCode & 0x00F0) >> 4];
    mCounter += 2;
}

// Adds VY to VX. VF is set to 1 when carry, and to 0 when isn't.
void cee::Chip8::op0x8004()
{
    uint8_t vy = mRegisters[(mOpCode & 0x00F0) >> 4];
    uint8_t vx = mRegisters[(mOpCode & 0x0F00) >> 8];

    mRegisters[(mOpCode & 0x0F00) >> 8] = vx + vy;
    mRegisters[0xF] = (vx + vy > 0xFF) ? 1 : 0;
    mCounter += 2;
}

// VY is subtracted from VX. VF is set to 0 when borrow, and 1 when isn't.
void cee::Chip8::op0x8005()
{
    uint8_t vy = mRegisters[(mOpCode & 0x00F0) >> 4];
    uint8_t vx = mRegisters[(mOpCode & 0x0F00) >> 8];

    mRegisters[(mOpCode & 0x0F00) >> 8] = vx - vy;
    mRegisters[0xF] = (vy > vx) ? 0 : 1;
    mCounter += 2;
}

// Shifts VX right by 1. VF is set value of the least sig bit of VX before shift.
void cee::Chip8::op0x8006()
{
    mRegisters[0xF] = mRegisters[(mOpCode & 0x0F00) >> 8] & 1;
    mRegisters[(mOpCode & 0x0F00) >> 8] >>= 1;
    mCounter += 2;
}

// Sets VX to VY minus VX. VF is set to 0 when there's a borrow, and 1 when isn't.
void cee::Chip8::op0x8007()
{
    uint8_t vy = mRegisters[(mOpCode & 0x00F0) >> 4];
    uint8_t vx = mRegisters[(mOpCode & 0x0F00) >> 8];

    mRegisters[(mOpCode & 0x0F00) >> 8] = vy - vx;
    mRegisters[0xF] = (vx > vy) ? 0 : 1;
    mCounter += 2;
}

// Shifts VX left by 1. VF is set value of the most sig bit of VX before shift.
void cee::Chip8::op0x800E()
{
    mRegisters[0xF] = mRegisters[(mOpCode & 0x0F00) >> 8] & 128;
    mRegisters[(mOpCode & 0x0F00) >> 8] <<= 1;
    mCounter += 2;
}

// Skips the next instruction if VX doesn't equal VY.
void cee::Chip8::op0x9000()
{
    uint8_t vy = mRegisters[(mOpCode & 0x00F0) >> 4];
    uint8_t vx = mRegisters[(mOpCode & 0x0F00) >> 8];
    mCounter += (vx != vy ? 4 : 2);
}

// Sets I to the address NNN.
void cee::Chip8::op0xA000()
{
    mIndex = mOpCode & 0x0FFF;
    mCounter += 2;
}

// Jumps to the address NNN plus V0.
void cee::Chip8::op0xB000()
{
    mCounter = (mOpCode & 0x0FFF) + mRegisters[0x0];
}

// Sets VX to a random number, masked by NN.
void cee::Chip8::op0xC000()
{
    mRegisters[(mOpCode & 0x0F00) >> 8] = mDist(mRandGen) & (mOpCode & 0x00FF);
    mCounter += 2;
}

// Draws a sprite (which is a sequence of bytes)
// at coordinate (VX, VY) that has a width of 8 pixels and
// a height of N pixels.
// Each row of 8 pixels is read as bit-coded starting from memory location I;
// I value doesn’t change after the execution of this instruction.
// As described above, VF is set to 1 if any screen pixels are flipped from set
// to unset when the sprite is drawn, and to 0 if that doesn’t happen.
void cee::Chip8::op0xD000()
{
    uint8_t nr = mOpCode & 0x000F; // Number of rows.
    uint8_t vy = mRegisters[(mOpCode & 0x00F0) >> 4];
    uint8_t vx = mRegisters[(mOpCode & 0x0F00) >> 8];

    // Start with VF being 0, presuming that no screen pixels were flipped.
    // The for-loop below will determine if that's not the case though.
    mRegisters[0xF] = 0;

    for (uint8_t y = 0; y < nr; y++)
    {
        std::bitset<8> pixels(mMemory[mIndex + y]);

        for (uint8_t x = 0; x < pixels.size(); ++x)
        {
            // Check if the pixel on the display is set to 1.
            // If it is set, we need to register the collision
            // by setting the VF register.
            if (pixels[x])
            {
                // Y represents the row so multiplying the row
                // by 64 (which is the max width of our pixel resolution)
                // gets us the current row.
                uint8_t location = (vx + x + ((vy + y) * 64));

                if (mGfx[location] == 1)
                {
                    mRegisters[0xF] = 1;
                }

                mGfx[location] ^= 1;
            }
        }
    }

    mCounter += 2;
}


// Skips the next instruction if the key stored in VX is pressed.
void cee::Chip8::op0xE0A1()
{
    uint8_t x = (mOpCode & 0x0F00) >> 8;
    mCounter += (mKeyStates[x] == 1) ? 4 : 2;
}

// Skips the next instruction if the key stored in VX isn't pressed.
void cee::Chip8::op0xE09E()
{
    uint8_t x = (mOpCode & 0x0F00) >> 8;
    mCounter += (mKeyStates[x] != 1) ? 4 : 2;
}

// Sets VX to the value of the delay timer.
void cee::Chip8::op0xF007()
{
    mRegisters[(mOpCode & 0x0F00) >> 8] = mDelayTimer;
    mCounter += 2;
}

// A key press is awaited, and then stored in VX.
void cee::Chip8::op0xF00A()
{
    auto isPressed = [](uint8_t i) { return i == 1; };

    // We put a -1 at the end so it doesn't take the lastKeyPressed to account
    // because that's not part of the set of keys.
    if (std::any_of(mKeyStates.cbegin(), mKeyStates.cend() - 1, isPressed))
    {
        mRegisters[(mOpCode & 0x0F00) >> 8] = mKeyStates[0x10];
        mCounter += 2;
    }

    // We don't increment the PC if no keys were pressed.
}

// Sets the delay timer to VX.
void cee::Chip8::op0xF015()
{
    mDelayTimer = mRegisters[(mOpCode & 0x0F00) >> 8];
    mCounter += 2;
}

// Sets the sound timer to VX.
void cee::Chip8::op0xF018()
{
    mSoundTimer = mRegisters[(mOpCode & 0x0F00) >> 8];
    mCounter += 2;
}

// Adds VX to I.
void cee::Chip8::op0xF01E()
{
    mIndex += mRegisters[(mOpCode & 0x0F00) >> 8];
    mCounter += 2;
}

// Sets I to the location of the sprite for the character in VX.
// Characters 0-F (in hexadecimal) are represented by a 4x5 font.
void cee::Chip8::op0xF029()
{
    // TODO
}

// Stores the Binary-coded decimal representation of VX,
// with the most significant of three digits at the address in I,
// the middle digit at I plus 1, and the least significant digit at I plus 2.
// (In other words, take the decimal representation of VX,
// place the hundreds digit in memory at location in I, the tens digit at location I+1,
// and the ones digit at location I+2.).
void cee::Chip8::op0xF033()
{
    // TODO
}

// Stores V0 to VX in memory starting at address I.
void cee::Chip8::op0xF055()
{
    for (size_t i = 0; i < mRegisters.size(); i++)
        mMemory[mIndex + i] = mRegisters[i];

    mCounter += 2;
}

// Fills V0 to VX with values from memory starting at address I.
void cee::Chip8::op0xF065()
{
    for (size_t i = 0; i < mRegisters.size(); i++)
        mRegisters[i] = mMemory[mIndex + i];

    mCounter += 2;
}
