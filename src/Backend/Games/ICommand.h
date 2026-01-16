// ICommand.h

#pragma once

class ICommand {
public:
  virtual ~ICommand() = default;
  virtual bool Execute() = 0;
  virtual void Undo() = 0;
  [[nodiscard]] virtual unsigned int ShotsHit() const = 0;
  [[nodiscard]] virtual unsigned int UnitsDestroyed() const = 0;
};
