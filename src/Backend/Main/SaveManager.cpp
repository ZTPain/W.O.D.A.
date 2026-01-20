#include "SaveManager.h"
#include "Backend/Games/GameManager.h"
#include "Backend/Replays/ReplayManager.h"

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
  // Prepare save state
  const SaveState saveState = CreateSaveState();

  // Allocate buffer
  constexpr auto MAX_SAVE_SIZE = static_cast<const size_t>(10 * 1024 * 1024); // 10 MB
  std::vector<uint8_t> buffer(MAX_SAVE_SIZE);

  // Serialize save state into buffer
  size_t offset = 0;
  SaveData(buffer.data(), offset, MAX_SAVE_SIZE, saveState);
  assert(offset <= buffer.size());

  std::ofstream file("save.dat", std::ios::binary | std::ios::trunc);

  assert(file);

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

  GameManager::InitialzieNextGameId(ReplayManager::GetInstance().Replays().size() + 1);
}
