#pragma once

#include <cstddef>
#include <cstdint>
#include <string>

class SerializationHelper {
public:
  static void SerializeInt64(uint8_t* buffer, size_t& offset, size_t bufferSize, uint64_t value);
  static uint64_t DeserializeInt64(const uint8_t* buffer, size_t& offset, size_t bufferSize);

  static void SerializeInt32(uint8_t* buffer, size_t& offset, size_t bufferSize, uint32_t value);
  static uint32_t DeserializeInt32(const uint8_t* buffer, size_t& offset, size_t bufferSize);

  static void SerializeInt16(uint8_t* buffer, size_t& offset, size_t bufferSize, uint16_t value);
  static uint16_t DeserializeInt16(const uint8_t* buffer, size_t& offset, size_t bufferSize);

  static void SerializeString(
      uint8_t* buffer, size_t& offset, size_t bufferSize, const std::string& value
  );
  static std::string DeserializeString(const uint8_t* buffer, size_t& offset, size_t bufferSize);
};
