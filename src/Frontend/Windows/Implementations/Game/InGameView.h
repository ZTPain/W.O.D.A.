#pragma once

#include "Backend/Games/Coordinates.h"
#include "Backend/Games/GameManager.h"
#include "Backend/Games/GameMode.h"
#include "Backend/Games/Player.h"
#include "Frontend/Helpers/Grid.h"
#include "Frontend/Windows/Api/Window.h"
#include <cstddef>
#include <cstdint>
#include <map>
#include <tuple>
#include <utility>
#include <vector>

class InGameView : public Window {
public:
  InGameView(WindowType type) : Window(type) {}
  ~InGameView() override = default;

  void OnEnter() override;
  void OnExit() override;
  void OnResize(int width, int height) override;
  [[nodiscard]] bool IsCorrectSize(int width, int height) const override;

protected:
  [[nodiscard]] virtual const GameMode& GetGameMode() const = 0;
  [[nodiscard]] virtual const std::vector<Player>& GetPlayers() const = 0;
  [[nodiscard]] virtual const Player& GetPlayerAtIndex(size_t index) const = 0;
  [[nodiscard]] virtual const Player& GetCurrentPlayer() const = 0;
  [[nodiscard]] virtual GameState GetGameState() const = 0;

  void ForceRender() override;

  size_t currentPlayerIndex = 0;
  Grid currentGrid;

  size_t enemyPlayerIndex = 1;
  Grid enemyGrid;

  std::vector<Coordinates> salvoSelectionCoordinates;

  std::vector<std::tuple<size_t, size_t, Coordinates, uint8_t>> actionHistory;
  std::map<size_t, std::pair<size_t, Coordinates>> lastPositionsPerPlayer;

  bool fastForwardEnabled = false;
  bool ultraFastForwardEnabled = false;
  bool inAnimation = false;
  bool invertGridPositions = false;
  void SetGridOffsets();

  void OnChangeTurn();

  void RenderCell(
      size_t x, size_t y, size_t posX, size_t posY, bool isCursor, size_t playerIndex
  ) const;

  virtual void OnToggleEnemyCell(size_t x, size_t y, size_t posX, size_t posY) = 0;

  void RenderUnitsLeft() const;
  void RenderUnitsLeftForPlayer(size_t playerIndex, size_t startX, size_t startY) const;

  virtual void HandleNextTurn() = 0;
  void HandleAfterFireAtCoordinate();

  void ShowPlayerFireAnimation();
  void CheckForPeriod();
  void HandleEscape();
  static std::vector<std::vector<std::pair<Coordinates, bool>>> CalculateBulletPaths(
      const std::vector<Coordinates>& targetCoordinates,
      const Grid& enemyGrid,
      bool invertGridPositions,
      size_t& maxDistance,
      size_t bulletX
  );

  static void CalculateAnimationCoordinates(
      const Coordinates& coord,
      const Grid& enemyGrid,
      bool invertGridPositions,
      size_t& outTargetX,
      size_t& outTargetY
  );

  static void CalculateBulletPath(
      size_t startX,
      size_t startY,
      size_t targetX,
      std::vector<std::pair<Coordinates, bool>>& outPath,
      const Grid& enemyGrid
  );

  void RenderTurnQueue() const;
  void RenderLeaderboard() const;
  void RenderHistory() const;

  [[nodiscard]] size_t CalculateSegmentsLeftForPlayer(
      size_t playerIndex, size_t* outMax = nullptr
  ) const;

  static void HandleGameOver();
};
