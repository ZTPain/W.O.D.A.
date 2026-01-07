#include "AchievementPool.h"
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>

Achievement::Achievement(
    std::string_view name, std::string_view description, UnlockableContent content, bool unlocked
)
    : name(name), description(description), content(content), unlocked(unlocked) {}

AchievementPool::AchievementPool() {
  InitMapElement(
      "The Fastest Hand in the West", "Win a game in under 5 minutes.", UnlockableContent::SalvoMode
  );

  InitMapElement("Do You Feel Lucky?", "Win a PvP game.", UnlockableContent::HardComputer);

  InitMapElement("Texas Sharpshooter", "Land 50 shots.", UnlockableContent::None);

  InitMapElement("Per Aspera ad Astra", "Score over 100 points.", UnlockableContent::None);

  InitMapElement("Smooth Sailing", "Win without getting hit.", UnlockableContent::ExtendedMode);

  InitMapElement("Pacifish", "Lose without landing a shot.", UnlockableContent::None);

  InitMapElement("Get the Cool Shoeshine", "Customize unit color.", UnlockableContent::None);

  InitMapElement(
      "I Want It Painted Black", "Change the border color to black.", UnlockableContent::None
  );

  InitMapElement(
      "Oh Man, Look at Those Cavemen Go", "Play 3 games.", UnlockableContent::MediumComputer
  );

  InitMapElement(
      "For the Voyage Is Long and the Winds Don't Blow", "Lose a ship.", UnlockableContent::None
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
  nameToAchievementMap.at(name).unlocked = true;
}

std::unique_ptr<AchievementPool> AchievementPool::Clone() {
  return std::make_unique<AchievementPool>(*this);
}
