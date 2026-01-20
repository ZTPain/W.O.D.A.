#include "UserProfile.h"
#include "Backend/Computers/Computer.h"
#include "Backend/Main/SerializationHelper.h"
#include "Backend/Users/AchievementPool.h"
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <utility>

UserProfile::UserProfile() : UserProfile(0, "", nullptr) {}

UserProfile::UserProfile(const UserProfile& other)
    : UserProfile(
          other.userId,
          other.name,
          other.statistics,
          other.achievements->Clone(other.userId),
          other.unlockedContent,
          other.settings,
          other.ai
      ) {}

UserProfile::UserProfile(
    PlayerId userId, const std::string& name, std::unique_ptr<AchievementPool> achievements
)
    : UserProfile(userId, name, {}, std::move(achievements), 0, UserSettings{}, nullptr) {}

UserProfile::UserProfile(
    PlayerId userId,
    const std::string& name,
    std::unique_ptr<AchievementPool> achievements,
    class Computer* ai
)
    : UserProfile(userId, name, {}, std::move(achievements), 0, UserSettings{}, ai) {}

UserProfile::UserProfile(
    PlayerId userId,
    std::string name,
    Statistics statistics,
    std::unique_ptr<AchievementPool> achievements,
    uint64_t unlockedContent,
    UserSettings settings,
    class Computer* ai
)
    : userId(userId), ai(ai), name(std::move(name)), statistics(statistics),
      achievements(std::move(achievements)), unlockedContent(unlockedContent), settings(settings) {}

UserProfile::~UserProfile() = default;

unsigned int UserProfile::UserId() const { return userId; }

Computer* UserProfile::AI() const { return ai; }

UserProfile& UserProfile::operator=(const UserProfile& other) {
  if (this != &other) {
    userId = other.userId;
    ai = other.ai;
    name = other.name;
    statistics = other.statistics;
    achievements = other.achievements->Clone(other.userId);
    unlockedContent = other.unlockedContent;
    settings = other.settings;
  }
  return *this;
}

void UserProfile::Serialize(uint8_t* buffer, size_t& offset, size_t bufferSize) const {
  // Serializee userId
  SerializationHelper::SerializeInt32(buffer, offset, bufferSize, userId);

  // Serialize name
  SerializationHelper::SerializeString(buffer, offset, bufferSize, name);

  // Serialize unlockedContent
  SerializationHelper::SerializeInt64(buffer, offset, bufferSize, unlockedContent);

  // Serialize settings
  SerializeSettings(buffer, offset, bufferSize);

  // Serialize statistics
  SerializeStatistics(buffer, offset, bufferSize);

  // Serialize achievements
  achievements->Serialize(buffer, offset, bufferSize);
}

UserProfile UserProfile::Deserialize(const uint8_t* buffer, size_t& offset, size_t bufferSize) {
  // Deserialize userId
  const unsigned int userId = SerializationHelper::DeserializeInt32(buffer, offset, bufferSize);

  // Deserialize name
  std::string name = SerializationHelper::DeserializeString(buffer, offset, bufferSize);

  // Deserialize unlockedContent
  const uint64_t unlockedContent =
      SerializationHelper::DeserializeInt64(buffer, offset, bufferSize);

  // Deserialize settings
  UserSettings settings;
  DeserializeSettings(buffer, offset, bufferSize, settings);

  // Deserialize statistics
  Statistics statistics;
  DeserializeStatistics(buffer, offset, bufferSize, statistics);

  // Deserialize achievements
  std::unique_ptr<AchievementPool> achievements =
      AchievementPool::Deserialize(buffer, offset, bufferSize);

  const UserProfile userProfile(
      userId,
      std::move(name),
      statistics,
      std::move(achievements),
      unlockedContent,
      settings,
      nullptr
  );

  return userProfile;
}

void UserProfile::SerializeStatistics(uint8_t* buffer, size_t& offset, size_t bufferSize) const {
  // Serialize:   std::chrono::seconds fastestWonGame = 0s;
  SerializationHelper::SerializeInt64(
      buffer, offset, bufferSize, statistics.fastestWonGame.count()
  );

  // Serialize:   std::chrono::seconds totalPlaytime = 0s;
  SerializationHelper::SerializeInt64(buffer, offset, bufferSize, statistics.totalPlaytime.count());

  // Serialize:   unsigned int gamesPlayed = 0;
  SerializationHelper::SerializeInt32(buffer, offset, bufferSize, statistics.gamesPlayed);

  // Serialize:   unsigned int gamesWon = 0;
  SerializationHelper::SerializeInt32(buffer, offset, bufferSize, statistics.gamesWon);

  // Serialize:   unsigned int gamesLost = 0;
  SerializationHelper::SerializeInt32(buffer, offset, bufferSize, statistics.gamesLost);

  // Serialize:   unsigned int highestScore = 0;
  SerializationHelper::SerializeInt32(buffer, offset, bufferSize, statistics.highestScore);

  // Serialize:   unsigned int totalShotsFired = 0;
  SerializationHelper::SerializeInt32(buffer, offset, bufferSize, statistics.totalShotsFired);

  // Serialize:   unsigned int totalShotsHit = 0;
  SerializationHelper::SerializeInt32(buffer, offset, bufferSize, statistics.totalShotsHit);

  // Serialize:   unsigned int totalUnitsDestroyed = 0;
  SerializationHelper::SerializeInt32(buffer, offset, bufferSize, statistics.totalUnitsDestroyed);
}

void UserProfile::DeserializeStatistics(
    const uint8_t* buffer, size_t& offset, size_t bufferSize, Statistics& statistics
) {
  // Deserialize:   std::chrono::seconds fastestWonGame = 0s;
  statistics.fastestWonGame =
      std::chrono::seconds(SerializationHelper::DeserializeInt64(buffer, offset, bufferSize));

  // Deserialize:   std::chrono::seconds totalPlaytime = 0s;
  statistics.totalPlaytime =
      std::chrono::seconds(SerializationHelper::DeserializeInt64(buffer, offset, bufferSize));

  // Deserialize:   unsigned int gamesPlayed = 0;
  statistics.gamesPlayed = SerializationHelper::DeserializeInt32(buffer, offset, bufferSize);

  // Deserialize:   unsigned int gamesWon = 0;
  statistics.gamesWon = SerializationHelper::DeserializeInt32(buffer, offset, bufferSize);

  // Deserialize:   unsigned int gamesLost = 0;
  statistics.gamesLost = SerializationHelper::DeserializeInt32(buffer, offset, bufferSize);

  // Deserialize:   unsigned int highestScore = 0;
  statistics.highestScore = SerializationHelper::DeserializeInt32(buffer, offset, bufferSize);

  // Deserialize:   unsigned int totalShotsFired = 0;
  statistics.totalShotsFired = SerializationHelper::DeserializeInt32(buffer, offset, bufferSize);

  // Deserialize:   unsigned int totalShotsHit = 0;
  statistics.totalShotsHit = SerializationHelper::DeserializeInt32(buffer, offset, bufferSize);

  // Deserialize:   unsigned int totalUnitsDestroyed = 0;
  statistics.totalUnitsDestroyed =
      SerializationHelper::DeserializeInt32(buffer, offset, bufferSize);
}

void UserProfile::SerializeSettings(uint8_t* buffer, size_t& offset, size_t bufferSize) const {
  // Serialize:   Color unitColor = Color::Default;
  SerializationHelper::SerializeInt32(
      buffer, offset, bufferSize, static_cast<uint32_t>(settings.unitColor)
  );

  // Serialize:   Color boardColor = Color::Default;
  SerializationHelper::SerializeInt32(
      buffer, offset, bufferSize, static_cast<uint32_t>(settings.boardColor)
  );

  // Serialize:   Color borderColor = Color::Default;
  SerializationHelper::SerializeInt32(
      buffer, offset, bufferSize, static_cast<uint32_t>(settings.borderColor)
  );

  // Serialize:   BorderPattern borderPattern = BorderPattern::Default;
  SerializationHelper::SerializeInt32(
      buffer, offset, bufferSize, static_cast<uint32_t>(settings.borderPattern)
  );

  // Serialize:   UnitPattern unitPattern = UnitPattern::Default;
  SerializationHelper::SerializeInt32(
      buffer, offset, bufferSize, static_cast<uint32_t>(settings.unitPattern)
  );

  // Serialize:   bool autoMarkEmptyFields = false;
  SerializationHelper::SerializeInt32(
      buffer, offset, bufferSize, static_cast<uint32_t>(settings.autoMarkEmptyFields ? 1 : 0)
  );
}

void UserProfile::DeserializeSettings(
    const uint8_t* buffer, size_t& offset, size_t bufferSize, UserSettings& settings
) {
  // Deserialize:   Color unitColor = Color::Default;
  settings.unitColor =
      static_cast<Color>(SerializationHelper::DeserializeInt32(buffer, offset, bufferSize));

  // Deserialize:   Color boardColor = Color::Default;
  settings.boardColor =
      static_cast<Color>(SerializationHelper::DeserializeInt32(buffer, offset, bufferSize));

  // Deserialize:   Color borderColor = Color::Default;
  settings.borderColor =
      static_cast<Color>(SerializationHelper::DeserializeInt32(buffer, offset, bufferSize));

  // Deserialize:   BorderPattern borderPattern = BorderPattern::Default;
  settings.borderPattern =
      static_cast<BorderPattern>(SerializationHelper::DeserializeInt32(buffer, offset, bufferSize));

  // Deserialize:   UnitPattern unitPattern = UnitPattern::Default;
  settings.unitPattern =
      static_cast<UnitPattern>(SerializationHelper::DeserializeInt32(buffer, offset, bufferSize));

  // Deserialize:   bool autoMarkEmptyFields = false;
  settings.autoMarkEmptyFields =
      SerializationHelper::DeserializeInt32(buffer, offset, bufferSize) != 0;
}
