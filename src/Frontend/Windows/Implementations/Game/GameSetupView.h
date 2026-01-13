#pragma once

#include "Backend/Boards/ISegment.h"
#include "Backend/Computers/Computer.h"
#include "Backend/Games/Coordinates.h"
#include "Backend/Units/BattleUnitType.h"
#include "Frontend/Helpers/AnsiHelper.h"
#include "Frontend/Helpers/Grid.h"
#include "Frontend/Input/InputManager.h"
#include "Frontend/Windows/Api/Window.h"
#include <cstddef>
#include <string>
#include <unordered_map>

class GameSetupView : public Window {
public:
  GameSetupView();
  ~GameSetupView() override = default;

  void OnEnter() override;
  void OnExit() override;
  bool OnKeyPressed(ConsoleKeyDetails keyDetails) override;
  void OnResize(int width, int height) override;
  [[nodiscard]] bool IsCorrectSize(int width, int height) const override;

private:
  void ForceRender() override;

  static void ShowErrorMessage(const std::string& message);

  [[nodiscard]] bool AllUnitsPlaced() const;
  void ConfirmGridSetup();

  static void GenerateRandomSetup(ISegment* segmentBoard, ComputerType computerType);
  static void GenerateUnitPlacement(ISegment* segmentBoard, BattleUnitType unitType, size_t count);
  static void PlaceUnitAtRandom(ISegment* segmentBoard, BattleUnitType unitType);
  static bool CanPlaceUnitAt(
      ISegment* segmentBoard,
      BattleUnitType unitType,
      size_t startX,
      size_t startY,
      size_t orientation
  );
  static bool CheckAdjacentCells(ISegment* segmentBoard, size_t x, size_t y);

  void OnToggleCell(size_t x, size_t y, size_t posX, size_t posY);
  void RenderUnitsLeft(const std::unordered_map<BattleUnitType, size_t>& unitPool);
  void RenderCell(size_t x, size_t y, size_t posX, size_t posY, bool isCursor) const;
  static void RenderEmptyCell(size_t x, size_t y, size_t posX, size_t posY, bool isCursor);
  void RenderFilledCell(size_t x, size_t y, size_t posX, size_t posY, bool isCursor) const;

  [[nodiscard]] BattleUnitType GetUnitTypeOfCoordinate(const Coordinates& coord) const;
  [[nodiscard]] static AnsiColor GetColorForUnitType(BattleUnitType type);

  size_t currentPlayerIndex;
  Grid grid;
};
