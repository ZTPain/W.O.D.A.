#include "SaveManager.h"

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <ios>
#include <iterator>
#include <vector>

void SaveManager::SaveGame() {
  std::ofstream file("save.dat", std::ios::binary | std::ios::trunc);

  if (!file)
    return;

  // Prepare save state
  const SaveState saveState = CreateSaveState();

  // Allocate buffer
  std::vector<uint8_t> buffer(saveState.header.dataSize);

  // Serialize save state into buffer
  size_t offset = 0;
  SaveData(buffer.data(), offset, saveState.header.dataSize, saveState);
  assert(offset <= buffer.size());

  // Write buffer to file
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
  file.write(reinterpret_cast<const char*>(buffer.data()), static_cast<int64_t>(offset));
  file.close();
}

void SaveManager::LoadGame() {
  if (!std::filesystem::exists("save.dat"))
    return;

  std::ifstream file("save.dat", std::ios::binary);

  if (!file)
    return;

  std::vector<char> bytes((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

  file.close();

  const auto bufferSize = bytes.size();
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
  const auto* buffer = reinterpret_cast<const uint8_t*>(bytes.data());
  size_t const offset = 0;

  LoadData(buffer, offset, bufferSize);
}
