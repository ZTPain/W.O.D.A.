#include "ReplayManager.h"

ReplayManager ReplayManager::instance;
ReplayManager& ReplayManager::GetInstance() { return instance; }

ReplayManager::ReplayManager() = default;
ReplayManager::~ReplayManager() = default;
