#pragma once

#include "Backend/Games/Coordinates.h"
#include "Frontend/Helpers/Grid.h"
#include "Frontend/Input/InputManager.h"
#include "Frontend/Windows/Api/Window.h"
#include <cstddef>

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

  void HandleGameOver();
};
