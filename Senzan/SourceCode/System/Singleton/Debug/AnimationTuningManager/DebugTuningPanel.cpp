#include "AnimationTuningManager.h"
#include "System/Singleton/ImGui/CImGuiManager.h"
#include "Game/01_GameObject/00_MeshObject/MeshObject.h"
#include "Game/01_GameObject/00_MeshObject/00_Character/Character.h"
#include "Game/01_GameObject/00_MeshObject/00_Character/01_Player/Player.h"
#include "Game/03_Collision/00_Core/ColliderBase.h"

#include <string>
#include <vector>
#include <algorithm>
#include <cstring>

// Helper mapping for collision flags
static const std::vector<std::pair<const char*, uint32_t>> g_CollisionFlagMap = {
    {"Player_Attack", static_cast<uint32_t>(eCollisionGroup::Player_Attack)},
    {"Player_Damage", static_cast<uint32_t>(eCollisionGroup::Player_Damage)},
    {"Player_Dodge", static_cast<uint32_t>(eCollisionGroup::Player_Dodge)},
    {"Player_JustDodge", static_cast<uint32_t>(eCollisionGroup::Player_JustDodge)},
    {"Player_Parry", static_cast<uint32_t>(eCollisionGroup::Player_Parry)},
    {"Enemy_Attack", static_cast<uint32_t>(eCollisionGroup::Enemy_Attack)},
    {"Enemy_PreAttack", static_cast<uint32_t>(eCollisionGroup::Enemy_PreAttack)},
    {"Enemy_Damage", static_cast<uint32_t>(eCollisionGroup::Enemy_Damage)},
    {"Press", static_cast<uint32_t>(eCollisionGroup::Press)}
};

static void RenderMaskEditor(const char* label, uint32_t& mask)
{
    ImGui::Text("%s", label);
    ImGui::Indent();
    for (const auto& p : g_CollisionFlagMap)
    {
        bool checked = (mask & p.second) != 0;
        if (ImGui::Checkbox(p.first, &checked))
        {
            if (checked) mask |= p.second;
            else mask &= ~p.second;
        }
    }
    ImGui::Unindent();
}

// ImGui パネルは別ファイルで定義（リンク時に結合）
void RenderDebugTuningPanel(AnimationTuningManager& manager, MeshObject* player, MeshObject* boss)
{
    if (!manager.IsEnabled())
        return;

    ImGui::Begin(IMGUI_JP("アニメーション調整"), nullptr, ImGuiWindowFlags_MenuBar);
    
    // Menu bar for global actions
    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu(IMGUI_JP("ファイル")))
        {
            if (ImGui::MenuItem(IMGUI_JP("保存"), "Ctrl+S"))
            {
                manager.Save("AnimationTuning_Enhanced.json");
            }
            if (ImGui::MenuItem(IMGUI_JP("読み込み"), "Ctrl+O"))
            {
                manager.Load("AnimationTuning_Enhanced.json");
            }
            ImGui::Separator();
            if (ImGui::MenuItem(IMGUI_JP("レガシー出力")))
            {
                manager.Save("AnimationTuning_Legacy.txt");
            }
            ImGui::EndMenu();
        }
        
        if (ImGui::BeginMenu(IMGUI_JP("ツール")))
        {
            static bool showTimeControls = true;
            ImGui::MenuItem(IMGUI_JP("時間操作"), nullptr, &showTimeControls);
            
            if (showTimeControls)
            {
                ImGui::Separator();
                ImGui::Text(IMGUI_JP("ワールド時間倍率"));
                
                static float timeScale = 1.0f;
                static float duration = 0.0f;
                
                timeScale = manager.GetCurrentWorldTimeScale();
                ImGui::SliderFloat(IMGUI_JP("倍率"), &timeScale, 0.1f, 5.0f, "%.2f");
                ImGui::SliderFloat(IMGUI_JP("継続時間(0=永久)"), &duration, 0.0f, 10.0f, "%.1fs");
                
                if (ImGui::Button(IMGUI_JP("適用")))
                {
                    manager.SetWorldTimeScale(timeScale, duration);
                }
                ImGui::SameLine();
                if (ImGui::Button(IMGUI_JP("リセット (1.0x)")))
                {
                    manager.SetWorldTimeScale(1.0f);
                }
            }
            
            ImGui::EndMenu();
        }
        
        ImGui::EndMenuBar();
    }

    // Entity Selection
    ImGui::Separator();
    ImGui::Text(IMGUI_JP("対象エンティティ"));
    
    static int selectedEntity = 0; // 0 = Player, 1 = Boss
    static std::string currentEntityType = "Player";
    
    const char* entityTypes[] = { IMGUI_JP("プレイヤー"), IMGUI_JP("ボス") };
    if (ImGui::Combo(IMGUI_JP("エンティティタイプ"), &selectedEntity, entityTypes, IM_ARRAYSIZE(entityTypes)))
    {
        currentEntityType = selectedEntity == 0 ? "Player" : "Boss";
    }
    
    // Get or create entity tuning
    EntityTuning* entityTuning = manager.GetEntityTuning(currentEntityType);
    if (!entityTuning)
    {
        // Initialize with default values via GetAnimationStateTuning
        manager.GetAnimationStateTuning(currentEntityType, "Default");
        entityTuning = manager.GetEntityTuning(currentEntityType);
        if (entityTuning)
        {
            entityTuning->resourceName = (currentEntityType == "Player") ? "player" : "boss";
        }
    }
    
    if (!entityTuning)
    {
        ImGui::Text(IMGUI_JP("エンティティ設定の作成に失敗しました"));
        ImGui::End();
        return;
    }

    ImGui::Separator();
    ImGui::Text("%s: %s", IMGUI_JP("エンティティ情報"), entityTuning->entityType.c_str());
    ImGui::Text("%s: %s", IMGUI_JP("リソース"), entityTuning->resourceName.c_str());
    
    // Debug Mode Controls
    ImGui::Separator();
    ImGui::Text(IMGUI_JP("デバッグ設定"));
    
    bool debugMode = entityTuning->enableDebugMode;
    if (ImGui::Checkbox(IMGUI_JP("デバッグ有効"), &debugMode))
    {
        manager.SetEntityDebugMode(currentEntityType, debugMode);
    }
    
    if (debugMode)
    {
        ImGui::Indent();
        
        // Current debug state selection
        std::vector<std::string> stateNames;
        for (const auto& statePair : entityTuning->states)
        {
            stateNames.push_back(statePair.first);
        }
        
        if (!stateNames.empty())
        {
            static int currentStateIndex = 0;
            std::vector<const char*> stateNamePtrs;
            for (const auto& name : stateNames)
            {
                stateNamePtrs.push_back(name.c_str());
            }
            
            if (ImGui::Combo(IMGUI_JP("デバッグ状態"), &currentStateIndex, stateNamePtrs.data(), static_cast<int>(stateNamePtrs.size())))
            {
                if (currentStateIndex >= 0 && currentStateIndex < stateNames.size())
                {
                    manager.SetEntityDebugState(currentEntityType, stateNames[currentStateIndex]);
                }
            }
        }
        
        ImGui::Unindent();
    }

    // Animation State Management
    ImGui::Separator();
    ImGui::Text(IMGUI_JP("アニメーション状態"));
    
    // State selection and creation
    static char newStateName[128] = "NewState";
    ImGui::InputText(IMGUI_JP("新しい状態名"), newStateName, sizeof(newStateName));
    ImGui::SameLine();
    if (ImGui::Button(IMGUI_JP("状態を作成")))
    {
        std::string stateName(newStateName);
        if (!stateName.empty())
        {
            AnimationStateTuning* newState = manager.GetAnimationStateTuning(currentEntityType, stateName);
            if (newState)
            {
                newState->animSpeed = 1.0f;
                newState->loop = false;
                newState->animationIndex = 0;
            }
        }
    }
    
    // Current state editing
    if (!entityTuning->states.empty())
    {
        static int selectedState = 0;
        std::vector<std::string> stateList;
        for (const auto& statePair : entityTuning->states)
        {
            stateList.push_back(statePair.first);
        }
        
        std::vector<const char*> stateListPtrs;
        for (const auto& name : stateList)
        {
            stateListPtrs.push_back(name.c_str());
        }
        
        ImGui::Combo(IMGUI_JP("編集する状態"), &selectedState, stateListPtrs.data(), static_cast<int>(stateListPtrs.size()));
        
        if (selectedState >= 0 && selectedState < stateList.size())
        {
            std::string currentStateName = stateList[selectedState];
            AnimationStateTuning* currentState = manager.GetAnimationStateTuning(currentEntityType, currentStateName);
            
            if (currentState)
            {
                ImGui::Separator();
                ImGui::Text("%s: %s", IMGUI_JP("状態"), currentStateName.c_str());
                
                // Animation Properties
                ImGui::SliderFloat(IMGUI_JP("アニメ速度"), &currentState->animSpeed, 0.1f, 10.0f, "%.2f");
                ImGui::Checkbox(IMGUI_JP("ループ"), &currentState->loop);
                ImGui::SliderInt(IMGUI_JP("アニメ番号"), &currentState->animationIndex, 0, 20);
                
                // Debug Animation Controls
                ImGui::Separator();
                ImGui::Text(IMGUI_JP("デバッグ再生"));
                ImGui::Checkbox(IMGUI_JP("デバッグループ"), &currentState->debugLoop);
                if (currentState->debugLoop)
                {
                    ImGui::SliderFloat(IMGUI_JP("ループ開始"), &currentState->debugLoopStartTime, 0.0f, 1.0f, "%.3f");
                    ImGui::SliderFloat(IMGUI_JP("ループ終了"), &currentState->debugLoopEndTime, 0.0f, 1.0f, "%.3f");
                }
                
                // Collider Management
                ImGui::Separator();
                ImGui::Text(IMGUI_JP("コライダー"));
                
                if (ImGui::Button(IMGUI_JP("コライダー追加")))
                {
                    EnhancedColliderSpec newCollider;
                    newCollider.name = "Collider_" + std::to_string(currentState->colliders.size());
                    
                    if (currentEntityType == "Player")
                    {
                        newCollider.myMask = static_cast<uint32_t>(eCollisionGroup::Player_Attack);
                        newCollider.targetMask = static_cast<uint32_t>(eCollisionGroup::Enemy_Damage);
                    }
                    else
                    {
                        newCollider.myMask = static_cast<uint32_t>(eCollisionGroup::Enemy_Attack);
                        newCollider.targetMask = static_cast<uint32_t>(eCollisionGroup::Player_Damage);
                    }
                    
                    currentState->colliders.push_back(newCollider);
                }
                
                for (size_t i = 0; i < currentState->colliders.size(); ++i)
                {
                    ImGui::PushID(static_cast<int>(i));
                    
                    EnhancedColliderSpec& collider = currentState->colliders[i];
                    
                    if (ImGui::CollapsingHeader(collider.name.c_str()))
                    {
                        ImGui::Indent();
                        
                        // Basic Properties
                        char nameBuf[128];
                        strncpy_s(nameBuf, collider.name.c_str(), sizeof(nameBuf) - 1);
                        if (ImGui::InputText(IMGUI_JP("名前"), nameBuf, sizeof(nameBuf)))
                        {
                            collider.name = std::string(nameBuf);
                        }
                        
                        ImGui::DragFloat3(IMGUI_JP("オフセット"), collider.offset, 0.1f);
                        ImGui::DragFloat(IMGUI_JP("半径"), &collider.radius, 0.1f, 0.1f, 100.0f);
                        ImGui::DragFloat(IMGUI_JP("高さ"), &collider.height, 0.1f, 0.1f, 100.0f);
                        
                        // Timing Properties
                        ImGui::Separator();
                        ImGui::Text(IMGUI_JP("タイミング"));
                        ImGui::DragFloat2(IMGUI_JP("アクティブ時間"), collider.activeWindow, 0.01f, 0.0f, 1.0f);
                        ImGui::DragFloat(IMGUI_JP("遅延(秒)"), &collider.delaySeconds, 0.01f, 0.0f, 10.0f);
                        ImGui::DragFloat(IMGUI_JP("継続(秒)"), &collider.durationSeconds, 0.01f, 0.01f, 10.0f);
                        
                        // Combat Properties
                        ImGui::Separator();
                        ImGui::Text(IMGUI_JP("戦闘"));
                        ImGui::DragFloat(IMGUI_JP("攻撃量"), &collider.attackAmount, 1.0f, 0.0f, 1000.0f);
                        
                        // Masks (checkbox UI)
                        ImGui::Separator();
                        RenderMaskEditor(IMGUI_JP("自分のマスク"), collider.myMask);
                        RenderMaskEditor(IMGUI_JP("対象のマスク"), collider.targetMask);

                        // Visual Properties
                        ImGui::Separator();
                        ImGui::Text(IMGUI_JP("表示"));
                        ImGui::ColorEdit4(IMGUI_JP("デバッグ色"), collider.debugColor);
                        ImGui::Checkbox(IMGUI_JP("有効"), &collider.active);
                        
                        // Actions
                        ImGui::Separator();
                        if (ImGui::Button(IMGUI_JP("コライダー削除")))
                        {
                            currentState->colliders.erase(currentState->colliders.begin() + i);
                            ImGui::Unindent();
                            ImGui::PopID();
                            break;
                        }
                        
                        ImGui::Unindent();
                    }
                    
                    ImGui::PopID();
                }
            }
        }
    }

    // Apply Controls
    ImGui::Separator();
    ImGui::Text(IMGUI_JP("適用"));
    
    MeshObject* targetEntity = nullptr;
    if (selectedEntity == 0 && player) targetEntity = player;
    if (selectedEntity == 1 && boss) targetEntity = boss;
    
    if (ImGui::Button(IMGUI_JP("エンティティへ適用")))
    {
        if (targetEntity)
        {
            manager.ApplyToEntity(targetEntity);
        }
    }
    ImGui::SameLine();
    if (ImGui::Button(IMGUI_JP("特定状態を適用") ) && !entityTuning->states.empty())
    {
        if (targetEntity && !entityTuning->currentDebugState.empty())
        {
            manager.ApplyAnimationState(targetEntity, entityTuning->currentDebugState);
        }
    }
    
    // Entity Status Display
    if (targetEntity)
    {
        ImGui::Separator();
        ImGui::Text(IMGUI_JP("エンティティ状況"));
        Character* character = dynamic_cast<Character*>(targetEntity);
        if (character)
        {
            ImGui::Text("%s: %.1f / %.1f", IMGUI_JP("HP"), character->GetHP(), character->GetMaxHP());
            
            size_t colliderCount = character->GetAttackColliderCount(255); // Using reserved type ID
            ImGui::Text("%s: %zu", IMGUI_JP("攻撃コライダー数"), colliderCount);
        }
        
        ImGui::Text("%s: %s", IMGUI_JP("リソース"), targetEntity->GetResourceName().c_str());
    }

    ImGui::End();
}
