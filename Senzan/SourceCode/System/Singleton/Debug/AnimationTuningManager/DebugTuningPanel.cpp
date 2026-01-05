#include "AnimationTuningManager.h"
#include "System/Singleton/ImGui/CImGuiManager.h"

#include <string>
#include <vector>

// 簡易 ImGui パネル。必要に応じて拡張してください。
void RenderDebugTuningPanel(AnimationTuningManager& manager)
{
    if (!manager.IsEnabled())
        return;

    ImGui::Begin("Debug Tuning");

    static char entityBuf[128] = "Boss";
    static char stateBuf[128] = "Throw";
    ImGui::InputText("Entity", entityBuf, sizeof(entityBuf));
    ImGui::InputText("State", stateBuf, sizeof(stateBuf));

    std::string entityName(entityBuf);
    std::string stateName(stateBuf);

    StateTuning* tuning = manager.GetStateTuning(entityName, stateName);
    if (tuning)
    {
        ImGui::Separator();
        ImGui::Text("Anim");
        ImGui::SliderFloat("AnimSpeed", &tuning->animSpeed, 0.1f, 10.0f, "%.2f");
        ImGui::Checkbox("Loop", &tuning->loop);

        ImGui::Separator();
        ImGui::Text("Colliders");
        if (tuning->colliders.empty())
        {
            if (ImGui::Button("Add Collider"))
            {
                AnimationColliderSpec c{};
                c.name = "Collider";
                c.offset[0] = c.offset[1] = c.offset[2] = 0.0f;
                c.radius = 1.0f;
                c.height = 1.0f;
                c.activeWindow[0] = 0.0f;
                c.activeWindow[1] = 1.0f;
                c.active = true;
                tuning->colliders.push_back(c);
            }
        }

        for (size_t i = 0; i < tuning->colliders.size(); ++i)
        {
            AnimationColliderSpec& c = tuning->colliders[i];
            ImGui::PushID((int)i);
            char buf[64];
            std::snprintf(buf, sizeof(buf), "%s_%zu", c.name.c_str(), i);
            // 名前編集の安全な扱い: ローカルバッファを用いる
            char nameBuf[64];
            std::snprintf(nameBuf, sizeof(nameBuf), "%s", c.name.c_str());
            if (ImGui::InputText("Name", nameBuf, sizeof(nameBuf)))
            {
                c.name = std::string(nameBuf);
            }
            ImGui::DragFloat3("Offset", c.offset, 0.1f);
            ImGui::DragFloat("Radius", &c.radius, 0.1f, 0.0f, 100.0f);
            ImGui::DragFloat("Height", &c.height, 0.1f, 0.0f, 100.0f);
            ImGui::DragFloat2("ActiveWindow", c.activeWindow, 0.01f, 0.0f, 999.0f);
            ImGui::Checkbox("Active", &c.active);
            if (ImGui::Button("Remove"))
            {
                tuning->colliders.erase(tuning->colliders.begin() + i);
                ImGui::PopID();
                break;
            }
            ImGui::PopID();
            ImGui::Separator();
        }

        if (ImGui::Button("Apply"))
        {
            // ApplyToEntity を呼ぶのは本来 Game 側で行う。
            // ここでは manager に対してフラグをセットしておき、
            // Game のループで ApplyToEntity を呼んで適用する方式を推奨します。
        }

        ImGui::SameLine();
        if (ImGui::Button("Save"))
        {
            manager.Save("DebugTuning.txt"); // デフォルトパス（JSON 風テキスト）
        }
        ImGui::SameLine();
        if (ImGui::Button("Load"))
        {
            manager.Load("DebugTuning.txt");
        }
    }

    ImGui::End();
}
