#pragma once

#include <string>
#include <unordered_map>
#include <vector>

class MeshObject; // forward

struct ColliderSpec
{
    std::string name;
    float offset[3];
    float radius;
    float height;
    float activeWindow[2];
    bool active;
};

struct StateTuning
{
    float animSpeed;
    bool loop;
    std::vector<ColliderSpec> colliders;
    StateTuning()
        : animSpeed(1.0f)
        , loop(false)
    {
    }
};

class DebugTuningManager
{
public:
    static DebugTuningManager& GetInstance();

    void Init();
    void Shutdown();

    // 呼び出し元（ゲームループ）の ImGui フレーム内で毎フレーム呼ぶ
    void UpdateImGui();

    // エンティティに対して調整値を適用する（呼び出しは StateMachine の前後で制御してください）
    void ApplyToEntity(MeshObject* entity);

    bool Save(const std::string& path);
    bool Load(const std::string& path);

    bool IsEnabled() const { return m_Enabled; }
    void SetEnabled(bool en) { m_Enabled = en; }

    // 設定アクセス（簡易）
    StateTuning* GetStateTuning(const std::string& entityName, const std::string& stateName);

private:
    DebugTuningManager();
    ~DebugTuningManager();

    // 非コピー
    DebugTuningManager(const DebugTuningManager&) = delete;
    DebugTuningManager& operator=(const DebugTuningManager&) = delete;

private:
    // EntityName -> (StateName -> tuning)
    std::unordered_map<std::string, std::unordered_map<std::string, StateTuning>> m_Tunings;
    bool m_Enabled;
};
