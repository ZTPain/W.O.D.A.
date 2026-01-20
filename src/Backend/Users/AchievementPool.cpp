#include "AchievementPool.h"
#include "Backend/Main/SerializationHelper.h"
#include "Backend/Users/UserManager.h"
#include "Backend/Users/UserProfile.h"
#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>

Achievement::Achievement(
    std::string_view name, std::string_view description, UnlockableContent content, bool unlocked
)
    : name(name), description(description), content(content), unlocked(unlocked) {}

AchievementPool::AchievementPool(const AchievementPool& other, PlayerId playerId)
    : nameToAchievementMap(other.nameToAchievementMap), playerId(playerId) {}

AchievementPool::AchievementPool() : AchievementPool(0) {}

AchievementPool::AchievementPool(PlayerId playerId) : playerId(playerId) {
  InitMapElement(
      "The Fastest Hand in the West", "Win a game in under 5 minutes.", UnlockableContent::SalvoMode
  );

  InitMapElement("Do You Feel Lucky?", "Win a PvP game.", UnlockableContent::HardComputer);

  InitMapElement("Texas Sharpshooter", "Land 50 shots.", UnlockableContent::CrosshairShipIcon);

  InitMapElement(
      "Per Aspera ad Astra", "Score over 100 points.", UnlockableContent::BlueBorderColor
  );

  InitMapElement("Smooth Sailing", "Win without getting hit.", UnlockableContent::ExtendedMode);

  InitMapElement("Pacifish", "Lose without landing a shot.", UnlockableContent::FlowerShipIcon);

  InitMapElement(
      "Get the Cool Shoeshine", "Customize unit icon.", UnlockableContent::RedBorderColor
  );

  InitMapElement(
      "I Want It Painted Black",
      "Change the border color to black.",
      UnlockableContent::StoneShipIcon
  );

  InitMapElement(
      "Oh Man, Look at Those Cavemen Go", "Play 3 games.", UnlockableContent::StarShipIcon
  );

  InitMapElement(
      "For the Voyage Is Long and the Winds Don't Blow",
      "Lose a ship.",
      UnlockableContent::MediumComputer
  );
}

void AchievementPool::InitMapElement(
    std::string_view name, std::string_view description, UnlockableContent content
) {
  nameToAchievementMap.emplace(name, Achievement{name, description, content, false});
}

const std::unordered_map<std::string, Achievement>& AchievementPool::NameToAchievementMap() const {
  return nameToAchievementMap;
}

void AchievementPool::Unlock(const std::string& name) {
  auto& achiv = nameToAchievementMap.at(name);

  auto& user = UserManager::GetInstance().GetUserById(playerId);

  user.unlockedContent |= static_cast<uint32_t>(achiv.content);

  nameToAchievementMap.at(name).unlocked = true;
}

std::unique_ptr<AchievementPool> AchievementPool::Clone(PlayerId playerId) {
  return std::make_unique<AchievementPool>(*this, playerId);
}

void AchievementPool::Serialize(uint8_t* buffer, size_t& offset, size_t bufferSize) const {
  auto unlockedCount = static_cast<uint16_t>(0);
  for (const auto& [name, achievement] : nameToAchievementMap) {
    if (achievement.unlocked) {
      ++unlockedCount;
    }
  }

  SerializationHelper::SerializeInt16(buffer, offset, bufferSize, unlockedCount);

  for (const auto& [name, achievement] : nameToAchievementMap) {
    if (achievement.unlocked) {
      SerializationHelper::SerializeString(buffer, offset, bufferSize, name);
    }
  }
}

std::unique_ptr<AchievementPool> AchievementPool::Deserialize(
    const uint8_t* buffer, size_t& offset, size_t bufferSize
) {
  auto achievementPool = std::make_unique<AchievementPool>();

  const uint16_t achievementCount =
      SerializationHelper::DeserializeInt16(buffer, offset, bufferSize);

  for (uint16_t i = 0; i < achievementCount; ++i) {
    const std::string achievementName =
        SerializationHelper::DeserializeString(buffer, offset, bufferSize);
    achievementPool->Unlock(achievementName);
  }

  return achievementPool;
}

void AchievementPool::SerializeAchievement(
    const Achievement& achievement, uint8_t* buffer, size_t& offset, size_t bufferSize
) {
  SerializationHelper::SerializeString(buffer, offset, bufferSize, achievement.name);
  SerializationHelper::SerializeString(buffer, offset, bufferSize, achievement.description);
  SerializationHelper::SerializeInt32(
      buffer, offset, bufferSize, static_cast<uint32_t>(achievement.content)
  );
  SerializationHelper::SerializeInt32(
      buffer, offset, bufferSize, static_cast<uint32_t>(achievement.unlocked ? 1 : 0)
  );
}

void AchievementPool::DeserializeAchievement(
    Achievement& achievement, const uint8_t* buffer, size_t& offset, size_t bufferSize
) {
  achievement.name = SerializationHelper::DeserializeString(buffer, offset, bufferSize);
  achievement.description = SerializationHelper::DeserializeString(buffer, offset, bufferSize);
  achievement.content = static_cast<UnlockableContent>(
      SerializationHelper::DeserializeInt32(buffer, offset, bufferSize)
  );
  achievement.unlocked = SerializationHelper::DeserializeInt32(buffer, offset, bufferSize) != 0;
}
