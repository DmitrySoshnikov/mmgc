/**
 * The MIT License (MIT)
 * Copyright (c) 2018-present Dmitry Soshnikov <dmitry.soshnikov@gmail.com>
 */

#pragma once

#include <iomanip>
#include <sstream>
#include <string>

/**
 * Converts a number to hex string.
 */
template <typename T>
std::string int_to_hex(T i, bool usePrefix = true) {
  std::stringstream stream;
  stream << (usePrefix ? "0x" : "") << std::setfill('0')
         << std::setw(sizeof(T) * 2) << std::uppercase << std::hex << i
         << std::dec;
  return stream.str();
}

/**
 * Inserts delimeter after each N characters.
 */
inline void insert_delimeter(std::string& str, uint32_t n,
                             std::string delimeter) {
  for (size_t i = n; i < str.size(); i += n + 1) {
    str.insert(i, delimeter);
  }
}

/**
 * Aligns the size by the word address.
 */
template<typename T>
inline uint32_t align(uint32_t n) {
  return (((n - 1) >> 2) << 2) + sizeof(T);
}