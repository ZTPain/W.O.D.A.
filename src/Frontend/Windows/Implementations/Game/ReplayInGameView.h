#pragma once

#include "Backend/Games/GameManager.h"
#include "Backend/Games/GameMode.h"
#include "Backend/Games/Player.h"
#include "Frontend/Input/InputManager.h"
#include "Frontend/Windows/Api/Window.h"
#include "Frontend/Windows/Implementations/Game/InGameView.h"
#include <cstddef>
#include <vector>

class ReplayInGameView : public InGameView {
public:
  ReplayInGameView() : InGameView(WindowType::GameReplay) {}
  ~ReplayInGameView() override = default;

  bool OnKeyPressed(ConsoleKeyDetails keyDetails) override;

protected:
  [[nodiscard]] const GameMode& GetGameMode() const override;
  [[nodiscard]] const std::vector<Player>& GetPlayers() const override;
  [[nodiscard]] const Player& GetPlayerAtIndex(size_t index) const override;
  [[nodiscard]] const Player& GetCurrentPlayer() const override;
  [[nodiscard]] GameState GetGameState() const override;

private:
  void HandleNextTurn() override;
  void OnToggleEnemyCell(size_t x, size_t y, size_t posX, size_t posY) override;
};
