// BattleUnit.h

#pragma once

enum class BattleUnitCategory {
  Land,
  Marine,
  Aerial,
};

enum class BattleUnitType {
  None,
  PatrolBoat,
  Interceptor,
  Cruiser,
  Dreadnought,
};

class BattleUnit {
  unsigned int destroyedSegments = 0;

protected:
  unsigned int segments;
  BattleUnitCategory category;

public:
  BattleUnit();
  virtual ~BattleUnit() = default;
  [[nodiscard]] virtual BattleUnitType GetType() const = 0;
  [[nodiscard]] BattleUnitCategory GetCategory() const;
  void DestroySegment();
  void FixSegment();
  [[nodiscard]] bool IsDestroyed() const;
};
