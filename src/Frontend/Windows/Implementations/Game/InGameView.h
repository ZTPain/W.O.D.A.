#pragma once

#include "Backend/Games/Coordinates.h"
#include "Frontend/Helpers/Grid.h"
#include "Frontend/Input/InputManager.h"
#include "Frontend/Windows/Api/Window.h"
#include <cstddef>
#include <cstdint>
#include <tuple>
#include <vector>

class InGameView : public Window {
public:
  InGameView() : Window(WindowType::InGame) {}
  ~InGameView() override = default;

  void OnEnter() override;
  void OnExit() override;
  bool OnKeyPressed(ConsoleKeyDetails keyDetails) override;
  void OnResize(int width, int height) override;
  [[nodiscard]] bool IsCorrectSize(int width, int height) const override;

private:
  void ForceRender() override;

  size_t currentPlayerIndex = 0;
  Grid currentGrid;

  size_t enemyPlayerIndex = 1;
  Grid enemyGrid;

  std::vector<std::tuple<size_t, size_t, Coordinates, uint8_t>> actionHistory;

  bool inAnimation = false;
  bool invertGridPositions = false;
  void SetGridOffsets();

  void OnChangeTurn();

  void RenderCell(
      size_t x, size_t y, size_t posX, size_t posY, bool isCursor, size_t playerIndex
  ) const;

  void OnToggleEnemyCell(size_t x, size_t y, size_t posX, size_t posY);

  void RenderUnitsLeft() const;
  static void RenderUnitsLeftForPlayer(size_t playerIndex, size_t startX, size_t startY);

  void HandleAITurn();
  bool HandleFireAtCoordinate(const Coordinates& coord);
  void ShowPlayerFireAnimation(const Coordinates& coord);

  void RenderTurnQueue() const;
  void RenderLeaderboard() const;
  void RenderHistory() const;

  [[nodiscard]] static size_t CalculateSegmentsLeftForPlayer(
      size_t playerIndex, size_t* outMax = nullptr
  );

  void HandleGameOver();
};
