#include "SerializationHelper.h"

#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <string>

void SerializationHelper::SerializeInt64(
    uint8_t* buffer, size_t& offset, size_t bufferSize, uint64_t value
) {
  if (offset + 8 > bufferSize)
    throw std::out_of_range("Buffer overflow in SerializeInt64");

  for (int i = 7; i >= 0; --i) {
    buffer[offset++] = static_cast<uint8_t>((value >> (i * 8)) & 0xFF);
  }
}

uint64_t SerializationHelper::DeserializeInt64(
    const uint8_t* buffer, size_t& offset, size_t bufferSize
) {
  if (offset + 8 > bufferSize)
    throw std::out_of_range("Buffer overflow in DeserializeInt64");

  uint64_t value = 0;
  for (int i = 7; i >= 0; --i) {
    value |= static_cast<uint64_t>(buffer[offset++]) << (i * 8);
  }
  return value;
}

void SerializationHelper::SerializeInt32(
    uint8_t* buffer, size_t& offset, size_t bufferSize, uint32_t value
) {
  if (offset + 4 > bufferSize)
    throw std::out_of_range("Buffer overflow in SerializeInt32");

  for (int i = 3; i >= 0; --i) {
    buffer[offset++] = static_cast<uint8_t>((value >> (i * 8)) & 0xFF);
  }
}

uint32_t SerializationHelper::DeserializeInt32(
    const uint8_t* buffer, size_t& offset, size_t bufferSize
) {
  if (offset + 4 > bufferSize)
    throw std::out_of_range("Buffer overflow in DeserializeInt32");

  uint32_t value = 0;
  for (int i = 3; i >= 0; --i) {
    value |= static_cast<uint32_t>(buffer[offset++]) << (i * 8);
  }
  return value;
}

void SerializationHelper::SerializeInt16(
    uint8_t* buffer, size_t& offset, size_t bufferSize, uint16_t value
) {
  if (offset + 2 > bufferSize)
    throw std::out_of_range("Buffer overflow in SerializeInt16");

  for (int i = 1; i >= 0; --i) {
    buffer[offset++] = static_cast<uint8_t>((value >> (i * 8)) & 0xFF);
  }
}

uint16_t SerializationHelper::DeserializeInt16(
    const uint8_t* buffer, size_t& offset, size_t bufferSize
) {
  if (offset + 2 > bufferSize)
    throw std::out_of_range("Buffer overflow in DeserializeInt16");

  uint16_t value = 0;
  for (int i = 1; i >= 0; --i) {
    value |= static_cast<uint16_t>(buffer[offset++]) << (i * 8);
  }
  return value;
}

void SerializationHelper::SerializeString(
    uint8_t* buffer, size_t& offset, size_t bufferSize, const std::string& value
) {
  const auto length = static_cast<uint16_t>(value.size());
  SerializeInt16(buffer, offset, bufferSize, length);

  if (offset + length > bufferSize)
    throw std::out_of_range("Buffer overflow in SerializeString");

  for (const char c : value) {
    buffer[offset++] = static_cast<uint8_t>(c);
  }
}

std::string SerializationHelper::DeserializeString(
    const uint8_t* buffer, size_t& offset, size_t bufferSize
) {
  const auto length = DeserializeInt16(buffer, offset, bufferSize);

  if (offset + length > bufferSize)
    throw std::out_of_range("Buffer overflow in DeserializeString");

  std::string value;
  value.reserve(length);
  for (size_t i = 0; i < length; ++i) {
    value += static_cast<char>(buffer[offset++]);
  }
  return value;
}
