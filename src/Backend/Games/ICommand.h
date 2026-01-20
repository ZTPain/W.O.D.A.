// ICommand.h

#pragma once

#include "Backend/Games/Coordinates.h"
#include <memory>
#include <vector>

class ICommand {
public:
  [[nodiscard]] virtual std::unique_ptr<ICommand> Clone() const = 0;
  virtual ~ICommand() = default;
  virtual bool Execute() = 0;
  virtual void Undo() = 0;
  [[nodiscard]] virtual unsigned int Shots() const = 0;
  [[nodiscard]] virtual unsigned int ShotsHit() const = 0;
  [[nodiscard]] virtual unsigned int UnitsDestroyed() const = 0;
  [[nodiscard]] virtual std::vector<Coordinates> GetCoordinates() const = 0;
};
