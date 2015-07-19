#include "chip8.hpp"

cee::Chip8::Chip8()
{
	this->reset();
}

void cee::Chip8::reset()
{
	// TODO...
}

void cee::Chip8::updateCycle()
{
	// TODO...
}

void cee::Chip8::updateKeys(std::array<uint8_t, 16> keys)
{
	mKeys = keys;
}

bool cee::Chip8::shouldDraw() const
{
	return true;
}

