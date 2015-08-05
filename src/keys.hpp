#pragma once

#ifndef CEE_KEYS_HPP
#define CEE_KEYS_HPP

#include <cstdint>

namespace cee
{
  struct Keys
  {
    uint16_t keysPressed;
    uint16_t lastKeyPressed;
  };
}

#endif // CEE_KEYS_HPP