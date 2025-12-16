// ICommand.h

#pragma once

class ICommand {
public:
  virtual ~ICommand() = default;
  virtual bool Execute() = 0;
  virtual void Undo() = 0;
};
