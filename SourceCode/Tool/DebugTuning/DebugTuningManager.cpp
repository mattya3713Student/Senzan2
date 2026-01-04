#include "DebugTuningManager.h"
#include <fstream>
#include <sstream>
#include <iomanip>

// ImGui パネルは別ファイルで定義（リンク時に結合）
void RenderDebugTuningPanel(DebugTuningManager& manager);

DebugTuningManager& DebugTuningManager::GetInstance()
{
    static DebugTuningManager s_Instance;
    return s_Instance;
}

DebugTuningManager::DebugTuningManager()
    : m_Enabled(false)
{
}

DebugTuningManager::~DebugTuningManager()
{
}

void DebugTuningManager::Init()
{
#ifdef ENABLE_DEBUG_TUNING
    m_Enabled = true;
#else
    m_Enabled = false;
#endif
}

void DebugTuningManager::Shutdown()
{
    // 将来的なクリーンナップ
}

void DebugTuningManager::UpdateImGui()
{
    if (!m_Enabled)
    {
        return;
    }

    // 実際の ImGui 描画処理は別関数へ委譲
    RenderDebugTuningPanel(*this);
}

void DebugTuningManager::ApplyToEntity(MeshObject* /*entity*/)
{
    // 実運用では MeshObject の API を用いて animSpeed, コライダ等を毎フレーム適用する
    // ここはゲーム側の MeshObject インターフェイスに合わせて実装を追加してください。
}

StateTuning* DebugTuningManager::GetStateTuning(const std::string& entityName, const std::string& stateName)
{
    auto it = m_Tunings.find(entityName);
    if (it == m_Tunings.end())
    {
        // 要素を作成して返す
        m_Tunings[entityName] = std::unordered_map<std::string, StateTuning>();
        it = m_Tunings.find(entityName);
    }

    auto& states = it->second;
    auto stIt = states.find(stateName);
    if (stIt == states.end())
    {
        states[stateName] = StateTuning();
        stIt = states.find(stateName);
    }

    return &stIt->second;
}

bool DebugTuningManager::Save(const std::string& path)
{
    std::ofstream ofs(path, std::ios::out | std::ios::trunc);
    if (!ofs.is_open())
    {
        return false;
    }

    ofs << std::fixed << std::setprecision(4);
    ofs << "{\n";
    bool firstEntity = true;
    for (const auto& e : m_Tunings)
    {
        if (!firstEntity) ofs << ",\n";
        firstEntity = false;
        ofs << "  \"" << e.first << "\": {\n";
        bool firstState = true;
        for (const auto& s : e.second)
        {
            if (!firstState) ofs << ",\n";
            firstState = false;
            ofs << "    \"" << s.first << "\": {\n";
            ofs << "      \"animSpeed\": " << s.second.animSpeed << ",\n";
            ofs << "      \"loop\": " << (s.second.loop ? "true" : "false") << ",\n";
            ofs << "      \"colliders\": [\n";
            bool firstCollider = true;
            for (const auto& c : s.second.colliders)
            {
                if (!firstCollider) ofs << ",\n";
                firstCollider = false;
                ofs << "        {\n";
                ofs << "          \"name\": \"" << c.name << "\",\n";
                ofs << "          \"offset\": [" << c.offset[0] << "," << c.offset[1] << "," << c.offset[2] << "],\n";
                ofs << "          \"radius\": " << c.radius << ",\n";
                ofs << "          \"height\": " << c.height << ",\n";
                ofs << "          \"activeWindow\": [" << c.activeWindow[0] << "," << c.activeWindow[1] << "],\n";
                ofs << "          \"active\": " << (c.active ? "true" : "false") << "\n";
                ofs << "        }";
            }
            ofs << "\n      ]\n";
            ofs << "    }";
        }
        ofs << "\n  }";
    }
    ofs << "\n}\n";
    ofs.close();
    return true;
}

bool DebugTuningManager::Load(const std::string& path)
{
    // 簡易実装: 現状は Load を未実装（将来的に JSON パーサーを入れて実装する）
    // Saveで書き出したフォーマットに対応した安全なパーサーを追加してください。
    (void)path;
    return false;
}
