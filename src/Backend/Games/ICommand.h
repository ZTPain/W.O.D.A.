// ICommand.h

#pragma once

#include <memory>
class ICommand {
public:
  [[nodiscard]] virtual std::unique_ptr<ICommand> Clone() const = 0;
  virtual ~ICommand() = default;
  virtual bool Execute() = 0;
  virtual void Undo() = 0;
  [[nodiscard]] virtual unsigned int ShotsHit() const = 0;
  [[nodiscard]] virtual unsigned int UnitsDestroyed() const = 0;
};
