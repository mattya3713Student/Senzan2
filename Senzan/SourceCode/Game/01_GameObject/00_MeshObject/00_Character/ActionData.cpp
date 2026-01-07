#include "ActionData.h"

#include "System/Utility/FileManager/FileManager.h"

using nlohmann::json;

namespace
{
    constexpr const char* KEY_TYPE = "type";
    constexpr const char* KEY_INDEX = "index";
    constexpr const char* KEY_BONE = "boneName";
    constexpr const char* KEY_START = "startTime";
    constexpr const char* KEY_DURATION = "duration";
    constexpr const char* KEY_SPEC = "spec";

    constexpr const char* KEY_RADIUS = "radius";
    constexpr const char* KEY_HEIGHT = "height";
    constexpr const char* KEY_OFFSET = "offset";
    constexpr const char* KEY_ATTACK = "attackAmount";
    constexpr const char* KEY_MY_MASK = "myMask";
    constexpr const char* KEY_TARGET_MASK = "targetMask";
    constexpr const char* KEY_COLOR = "color";
    constexpr const char* KEY_ACTIVE = "active";
}

void to_json(json& j, const ColliderTimelineEvent& e)
{
    j = json{
        {KEY_TYPE, static_cast<uint32_t>(e.type)},
        {KEY_INDEX, e.index},
        {KEY_BONE, e.boneName},
        {KEY_START, e.startTime},
        {KEY_DURATION, e.duration},
        {KEY_SPEC, {
            {KEY_RADIUS, e.spec.Radius},
            {KEY_HEIGHT, e.spec.Height},
            {KEY_OFFSET, {e.spec.Offset.x, e.spec.Offset.y, e.spec.Offset.z}},
            {KEY_ATTACK, e.spec.AttackAmount},
            {KEY_MY_MASK, e.spec.MyMask},
            {KEY_TARGET_MASK, e.spec.TargetMask},
            {KEY_COLOR, {e.spec.DebugColor.x, e.spec.DebugColor.y, e.spec.DebugColor.z, e.spec.DebugColor.w}},
            {KEY_ACTIVE, e.spec.Active}
        }}
    };
}

void from_json(const json& j, ColliderTimelineEvent& e)
{
    e.type = static_cast<Character::AttackTypeId>(j.value(KEY_TYPE, 0u));
    e.index = j.value(KEY_INDEX, static_cast<size_t>(0));
    e.boneName = j.value(KEY_BONE, std::string());
    e.startTime = j.value(KEY_START, 0.0f);
    e.duration = j.value(KEY_DURATION, 0.0f);

    const json& specJson = j.value(KEY_SPEC, json::object());
    e.spec.Radius = specJson.value(KEY_RADIUS, 1.0f);
    e.spec.Height = specJson.value(KEY_HEIGHT, 1.0f);
    if (specJson.contains(KEY_OFFSET))
    {
        auto arr = specJson.at(KEY_OFFSET);
        if (arr.is_array() && arr.size() >= 3)
        {
            e.spec.Offset.x = arr.at(0).get<float>();
            e.spec.Offset.y = arr.at(1).get<float>();
            e.spec.Offset.z = arr.at(2).get<float>();
        }
    }
    e.spec.AttackAmount = specJson.value(KEY_ATTACK, 0.0f);
    e.spec.MyMask = specJson.value(KEY_MY_MASK, 0u);
    e.spec.TargetMask = specJson.value(KEY_TARGET_MASK, 0u);
    if (specJson.contains(KEY_COLOR))
    {
        auto arr = specJson.at(KEY_COLOR);
        if (arr.is_array() && arr.size() >= 4)
        {
            e.spec.DebugColor.x = arr.at(0).get<float>();
            e.spec.DebugColor.y = arr.at(1).get<float>();
            e.spec.DebugColor.z = arr.at(2).get<float>();
            e.spec.DebugColor.w = arr.at(3).get<float>();
        }
    }
    e.spec.Active = specJson.value(KEY_ACTIVE, false);
}

void to_json(json& j, const ActionStateDefinition& d)
{
    j = json{
        {"stateName", d.stateName},
        {"animationName", d.animationName},
        {"totalDuration", d.totalDuration},
        {"colliderEvents", d.colliderEvents}
    };
}

void from_json(const json& j, ActionStateDefinition& d)
{
    d.stateName = j.value("stateName", std::string());
    d.animationName = j.value("animationName", std::string());
    d.totalDuration = j.value("totalDuration", 0.0f);
    d.colliderEvents = j.value("colliderEvents", std::vector<ColliderTimelineEvent>{});
}
