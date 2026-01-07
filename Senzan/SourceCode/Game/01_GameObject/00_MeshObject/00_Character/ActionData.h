#pragma once

#include <string>
#include <vector>
#include <cstddef>

#include "System/Utility/FileManager/FileManager.h" // brings json alias

#include "Game/01_GameObject/00_MeshObject/00_Character/Character.h"
#include "Game/03_Collision/00_Core/ColliderSpec.h"

struct ColliderTimelineEvent
{
    Character::AttackTypeId type = 0;
    size_t index = 0;
    std::string boneName;
    float startTime = 0.0f;
    float duration = 0.0f;
    ColliderSpec spec;
};

struct ActionStateDefinition
{
    std::string stateName;
    std::string animationName;
    float totalDuration = 0.0f;
    std::vector<ColliderTimelineEvent> colliderEvents;
};

namespace nlohmann
{
    template<typename T>
    struct adl_serializer;
}

void to_json(nlohmann::json& j, const ColliderTimelineEvent& e);
void from_json(const nlohmann::json& j, ColliderTimelineEvent& e);
void to_json(nlohmann::json& j, const ActionStateDefinition& d);
void from_json(const nlohmann::json& j, ActionStateDefinition& d);
