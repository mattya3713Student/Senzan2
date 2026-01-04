#include "DebugTuningManager.h"
#include "System/Singleton/Debug/AnimationTuningManager.h"

// Forwarding functions for compatibility
AnimationTuningManager& DebugTuningManager::GetInstance() { return AnimationTuningManager::GetInstance(); }

// Note: other methods can be forwarded by qualifying with AnimationTuningManager:: if needed.
