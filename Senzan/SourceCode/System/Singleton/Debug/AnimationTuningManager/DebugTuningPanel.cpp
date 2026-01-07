#include "AnimationTuningManager.h"
#include "Game/01_GameObject/00_MeshObject/00_Character/ActionData.h"
#include "System/Singleton/ImGui/CImGuiManager.h"
#include "Game/01_GameObject/00_MeshObject/MeshObject.h"
#include "Game/03_Collision/00_Core/ColliderBase.h"
#include <filesystem>
#include <algorithm>
#include <unordered_map>

// 衝突フラグ名とビットの対応表
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

static void RenderMaskEditor(const char* label, uint32_t& mask, int columns = 3)
{
    // Ensure unique widget IDs per mask variable to avoid ImGui ID collisions when the same label is reused.
    ImGui::PushID(reinterpret_cast<const void*>(&mask));
    ImGui::Text("%s", label);
    ImGui::Indent();

    // If only one column requested, render as simple vertical list
    if (columns <= 1)
    {
        for (const auto& p : g_CollisionFlagMap)
        {
            bool checked = (mask & p.second) != 0;
            if (ImGui::Checkbox(p.first, &checked))
            {
                if (checked) mask |= p.second;
                else mask &= ~p.second;
            }
        }
    }
    else
    {
        ImGui::BeginGroup();
        ImGui::Columns(columns, nullptr, false);
        int idx = 0;
        for (const auto& p : g_CollisionFlagMap)
        {
            bool checked = (mask & p.second) != 0;
            if (ImGui::Checkbox(p.first, &checked))
            {
                if (checked) mask |= p.second;
                else mask &= ~p.second;
            }
            ImGui::NextColumn();
            ++idx;
        }
        // fill remaining columns to keep layout consistent
        for (int c = (int)g_CollisionFlagMap.size(); c < columns; ++c)
            ImGui::NextColumn();
        ImGui::Columns(1);
        ImGui::EndGroup();
    }

    ImGui::Unindent();
    ImGui::PopID();
}

// アクションタイムライン編集用の関数宣言
void RenderActionTimelineEditor();

// ImGuiによるアニメーション調整パネル
void RenderDebugTuningPanel(AnimationTuningManager& manager, MeshObject* player, MeshObject* boss)
{
    if (!manager.IsEnabled())
        return;

    // スタイルを少し調整して見た目を良くする
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10, 10));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 6.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8, 6));
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(6, 4));
    ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.09f, 0.45f, 0.75f, 0.12f));
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.16f, 0.5f, 0.85f, 0.6f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.22f, 0.6f, 0.95f, 0.9f));

    ImGui::Begin(IMGUI_JP("アニメーション調整"), nullptr, ImGuiWindowFlags_MenuBar);

    // メニューバー（ファイル、ツール）
    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu(IMGUI_JP("ファイル")))
        {
            if (ImGui::MenuItem(IMGUI_JP("保存"), "Ctrl+S"))
            {
                manager.Save("Data\\Json\\Animation\\AnimationTuning_Enhanced.json");
            }
            if (ImGui::MenuItem(IMGUI_JP("読み込み"), "Ctrl+O"))
            {
                manager.Load("Data\\Json\\Animation\\AnimationTuning_Enhanced.json");
            }
            ImGui::Separator();
            if (ImGui::MenuItem(IMGUI_JP("レガシー出力")))
            {
                manager.Save("Data\\Json\\Animation\\AnimationTuning_Legacy.txt");
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

    // エンティティ切替
    ImGui::Separator();
    ImGui::Text(IMGUI_JP("対象エンティティ"));

    static int selectedEntity = 0; // 0 = Player, 1 = Boss
    static std::string currentEntityType = "Player";

    const char* entityTypes[] = { IMGUI_JP("プレイヤー"), IMGUI_JP("ボス") };
    ImGui::Combo(IMGUI_JP("エンティティタイプ"), &selectedEntity, entityTypes, IM_ARRAYSIZE(entityTypes));
    // Always keep currentEntityType in sync with selectedEntity
    currentEntityType = (selectedEntity == 0) ? "Player" : "Boss";

    // エンティティ調整データ取得
    EntityTuning* entityTuning = manager.GetEntityTuning(currentEntityType);
    if (!entityTuning)
    {
        // 存在しなければ初期化
        manager.GetAnimationStateTuning(currentEntityType, "Default");
        entityTuning = manager.GetEntityTuning(currentEntityType);
        if (entityTuning)
        {
            // resourceName はエンティティタイプで判別できるため初期設定は行わない
        }
    }

    if (!entityTuning)
    {
        ImGui::Text(IMGUI_JP("エンティティ設定の作成に失敗しました"));
        ImGui::End();
        // スタイルから復帰
        ImGui::PopStyleColor(3);
        ImGui::PopStyleVar(4);
        return;
    }

    // === Entity default masks UI ===
    ImGui::Separator();
    ImGui::Text(IMGUI_JP("エンティティ デフォルトマスク (省略時に使用)"));
    // Display and edit default masks using RenderMaskEditor
    RenderMaskEditor(IMGUI_JP("自分のデフォルトマスク"), entityTuning->defaultMyMask);
    RenderMaskEditor(IMGUI_JP("対象のデフォルトマスク"), entityTuning->defaultTargetMask);
    ImGui::Separator();

    // Ensure default preset states exist if tuning is empty (create entries via manager)
    if (entityTuning->states.empty())
    {
        if (currentEntityType == "Player")
        {
            const std::vector<std::string> preset = {
                "Idle", "Run", "AttackCombo_0", "AttackCombo_1", "AttackCombo_2",
                "Parry", "DodgeExecute", "JustDodge", "KnockBack", "Dead", "SpecialAttack", "Pause"
            };
            for (const auto& s : preset) manager.GetAnimationStateTuning(currentEntityType, s);
        }
        else // Boss
        {
            const std::vector<std::string> preset = {
                "Idol", "IdolToRun", "Run", "RunToIdol", "LeftMove", "RightMove",
                "Dead", "Slash", "SlashToIdol", "Charge", "ChargeAttack", "ChargeToIdol",
                "LaserCharge", "Laser", "LaserEnd", "Shout", "Special_0", "Special_1", "SpecialToIdol",
                "Stomp", "Throwing", "Pause", "KnockBack", "SpecialAttack"
            };
            for (const auto& s : preset) manager.GetAnimationStateTuning(currentEntityType, s);
        }
        // refresh pointer
        entityTuning = manager.GetEntityTuning(currentEntityType);
    }

    // エンティティ情報表示は不要のため省略

    // 編集モード: ステート毎 or 汎用(グローバル)
    ImGui::Separator();
    ImGui::Text(IMGUI_JP("編集モード"));
    static int editMode = 0; // 0 = state-specific, 1 = global colliders
    const char* editModes[] = { IMGUI_JP("ステート固有"), IMGUI_JP("汎用(グローバル)コライダー") };
    ImGui::Combo(IMGUI_JP("モード"), &editMode, editModes, IM_ARRAYSIZE(editModes));

    bool isGlobalEdit = (editMode == 1);

    // If global edit mode, render entity-level colliders editor
    if (isGlobalEdit)
    {
        ImGui::Separator();
        ImGui::Text(IMGUI_JP("エンティティ共通コライダー (グローバル)"));

        if (ImGui::Button(IMGUI_JP("グローバルコライダー追加")))
        {
            EnhancedColliderSpec newCollider;
            newCollider.name = "Global_" + std::to_string(entityTuning->globalColliders.size());
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
            entityTuning->globalColliders.push_back(newCollider);
            // Apply immediately to existing entity instance
            MeshObject* targetEntity = nullptr;
            if (selectedEntity == 0 && player) targetEntity = player;
            if (selectedEntity == 1 && boss) targetEntity = boss;
            if (targetEntity) manager.ApplyToEntity(targetEntity);
        }

        for (size_t i = 0; i < entityTuning->globalColliders.size(); ++i)
        {
            ImGui::PushID(static_cast<int>(i));
            EnhancedColliderSpec& collider = entityTuning->globalColliders[i];
            if (ImGui::CollapsingHeader(collider.name.c_str()))
            {
                ImGui::Indent();
                char nameBufG[128];
                strncpy_s(nameBufG, collider.name.c_str(), sizeof(nameBufG) - 1);
                // Use EnterReturnsTrue so editing the name doesn't immediately reapply and steal focus per character
                if (ImGui::InputText(IMGUI_JP("名前"), nameBufG, sizeof(nameBufG), ImGuiInputTextFlags_EnterReturnsTrue))
                {
                    collider.name = std::string(nameBufG);
                }
                if (ImGui::DragFloat3(IMGUI_JP("オフセット"), collider.offset, 0.1f)) { }
                // Bone tracking options for global collider
                if (ImGui::Checkbox(IMGUI_JP("ボーン追従"), &collider.trackBone)) { }
                if (collider.trackBone)
                {
                    // Determine target mesh from selectedEntity and available pointers
                    MeshObject* targetEntity = nullptr;
                    if (selectedEntity == 0 && player) targetEntity = player;
                    if (selectedEntity == 1 && boss) targetEntity = boss;

                    std::vector<std::string> boneNames;
                    if (targetEntity)
                    {
                        auto mesh = targetEntity->GetAttachMesh().lock();
                        if (mesh)
                        {
                            std::shared_ptr<SkinMesh> skin = std::dynamic_pointer_cast<SkinMesh>(mesh);
                            if (skin)
                            {
                                skin->GetBoneNames(boneNames);
                            }
                        }
                    }

                    if (!boneNames.empty())
                    {
                        // build cstring list
                        std::vector<const char*> ptrs;
                        ptrs.reserve(boneNames.size());
                        for (auto &n : boneNames) ptrs.push_back(n.c_str());

                        // find current index
                        int cur = 0;
            for (size_t bi = 0; bi < boneNames.size(); ++bi) if (boneNames[bi] == collider.boneName) { cur = static_cast<int>(bi); break; }
                        if (ImGui::Combo(IMGUI_JP("ボーン選択"), &cur, ptrs.data(), static_cast<int>(ptrs.size())))
                        {
                            collider.boneName = boneNames[cur];
                        }
                    }
                    else
                    {
                        ImGui::Text(IMGUI_JP("ボーン情報が未ロードです"));
                        // allow manual entry
                        char boneBufG[128];
                        strncpy_s(boneBufG, collider.boneName.c_str(), sizeof(boneBufG) - 1);
                        if (ImGui::InputText(IMGUI_JP("ボーン名 (手入力)"), boneBufG, sizeof(boneBufG), ImGuiInputTextFlags_EnterReturnsTrue))
                        {
                            collider.boneName = std::string(boneBufG);
                        }
                    }
                }
                if (ImGui::DragFloat(IMGUI_JP("半径"), &collider.radius, 0.1f, 0.1f, 100.0f)) { }
                if (ImGui::DragFloat(IMGUI_JP("高さ"), &collider.height, 0.1f, 0.1f, 100.0f)) { }

                ImGui::Separator();
                ImGui::Text(IMGUI_JP("戦闘"));
                if (ImGui::DragFloat(IMGUI_JP("攻撃量"), &collider.attackAmount, 1.0f, 0.0f, 1000.0f)) { }

                ImGui::Separator();
                uint32_t beforeMyG = collider.myMask;
                RenderMaskEditor(IMGUI_JP("自分のマスク"), collider.myMask);
                uint32_t beforeTargetG = collider.targetMask;
                RenderMaskEditor(IMGUI_JP("対象のマスク"), collider.targetMask);

                ImGui::Separator();
                ImGui::Text(IMGUI_JP("表示"));
                if (ImGui::ColorEdit4(IMGUI_JP("デバッグ色"), collider.debugColor)) { }
                if (ImGui::Checkbox(IMGUI_JP("有効"), &collider.active)) { }

                // Spawn timing controls
                ImGui::Separator();
                ImGui::Text(IMGUI_JP("生成タイミング"));
                if (ImGui::DragFloat(IMGUI_JP("生成遅延 (秒)"), &collider.spawnDelay, 0.01f, 0.0f, 60.0f)) { }
                if (ImGui::DragFloat(IMGUI_JP("生成継続時間 (秒)"), &collider.spawnDuration, 0.01f, 0.0f, 60.0f)) { }

                ImGui::Separator();
                if (ImGui::Button(IMGUI_JP("グローバルコライダー削除")))
                {
                    entityTuning->globalColliders.erase(entityTuning->globalColliders.begin() + i);
                    ImGui::Unindent();
                    ImGui::PopID();
                    // apply removal
                    MeshObject* targetEntity = nullptr;
                    if (selectedEntity == 0 && player) targetEntity = player;
                    if (selectedEntity == 1 && boss) targetEntity = boss;
                    if (targetEntity) manager.ApplyToEntity(targetEntity);
                    break;
                }
                ImGui::Unindent();
            }
            ImGui::PopID();
        }

        // When editing global colliders, also show per-state options for how they use global colliders
        ImGui::Separator();
        ImGui::Text(IMGUI_JP("各ステートでのグローバルコライダー適用設定"));

        // list states and allow selecting global collider mode per state
        std::vector<std::string> stateNames;
        for (const auto& kv : entityTuning->states) stateNames.push_back(kv.first);
        if (!stateNames.empty())
        {
            static int selStateForGlobal = 0;
            if (selStateForGlobal >= (int)stateNames.size()) selStateForGlobal = 0;
            std::vector<const char*> namesPtr;
            for (auto& n : stateNames) namesPtr.push_back(n.c_str());
            ImGui::Combo(IMGUI_JP("ステートを選択"), &selStateForGlobal, namesPtr.data(), static_cast<int>(namesPtr.size()));
            AnimationStateTuning* st = manager.GetAnimationStateTuning(currentEntityType, stateNames[selStateForGlobal]);
            if (st)
            {
                bool beforeUse = st->useGlobalColliders;
                if (ImGui::Checkbox(IMGUI_JP("グローバルコライダーを使用"), &st->useGlobalColliders))
                {
                    MeshObject* targetEntity = nullptr;
                    if (selectedEntity == 0 && player) targetEntity = player;
                    if (selectedEntity == 1 && boss) targetEntity = boss;
                    if (targetEntity) manager.ApplyAnimationState(targetEntity, stateNames[selStateForGlobal]);
                }
                const char* modesG[] = { IMGUI_JP("常に適用"), IMGUI_JP("エンター時のみ"), IMGUI_JP("適用しない") };
                if (ImGui::Combo(IMGUI_JP("グローバル適用モード"), &st->globalColliderMode, modesG, IM_ARRAYSIZE(modesG)))
                {
                    MeshObject* targetEntity = nullptr;
                    if (selectedEntity == 0 && player) targetEntity = player;
                    if (selectedEntity == 1 && boss) targetEntity = boss;
                    if (targetEntity) manager.ApplyAnimationState(targetEntity, stateNames[selStateForGlobal]);
                }
            }
        }
    }
    else
    {
        // === State-specific editor (existing UI) ===
        // アニメーション状態管理
        ImGui::Separator();
        ImGui::Text(IMGUI_JP("アニメーション状態"));

        // 現在のステート編集
        if (!entityTuning->states.empty())
        {
            static int selectedState = 0;
            static std::string prevEntityType = "";
            if (prevEntityType != currentEntityType)
            {
                selectedState = 0;
                prevEntityType = currentEntityType;
            }

            std::vector<std::string> stateList;
            // Preferred ordering matching manager initialization
            const std::vector<std::string> preferredPlayer = {
                "Idle", "Run", "AttackCombo_0", "AttackCombo_1", "AttackCombo_2",
                "Parry", "DodgeExecute", "JustDodge", "KnockBack", "Dead", "SpecialAttack", "Pause"
            };
            const std::vector<std::string> preferredBoss = {
                "Idol", "IdolToRun", "Run", "RunToIdol", "LeftMove", "RightMove",
                "Dead", "Slash", "SlashToIdol", "Charge", "ChargeAttack", "ChargeToIdol",
                "LaserCharge", "Laser", "LaserEnd", "Shout", "Special_0", "Special_1", "SpecialToIdol",
                "Stomp", "Throwing", "Pause", "KnockBack", "SpecialAttack"
            };

            const std::vector<std::string>& preferred = (currentEntityType == "Player") ? preferredPlayer : preferredBoss;

            // Add preferred states first (if they exist in tuning)
            for (const auto& name : preferred)
            {
                if (entityTuning->states.find(name) != entityTuning->states.end())
                    stateList.push_back(name);
            }

            // Append any remaining states that weren't in preferred list
            for (const auto& statePair : entityTuning->states)
            {
                const auto& name = statePair.first;
                if (std::find(stateList.begin(), stateList.end(), name) == stateList.end())
                {
                    stateList.push_back(name);
                }
            }

            std::vector<const char*> stateListPtrs;
            for (const auto& name : stateList)
            {
                stateListPtrs.push_back(name.c_str());
            }

            // ステート選択コンボ
            if (ImGui::Combo(IMGUI_JP("編集する状態"), &selectedState, stateListPtrs.data(), static_cast<int>(stateListPtrs.size())))
            {
                // Notify manager which state is currently selected for this entity type so debug state can be applied per-frame
                manager.SetSelectedState(currentEntityType, stateList[selectedState]);
            }

             if (selectedState >= 0 && selectedState < stateList.size())
             {
                 std::string currentStateName = stateList[selectedState];
                 AnimationStateTuning* currentState = manager.GetAnimationStateTuning(currentEntityType, currentStateName);

                 if (currentState)
                 {
                    bool stateDirty = false; // track if UI changed

                    ImGui::Separator();
                    ImGui::Text("%s: %s", IMGUI_JP("状態"), currentStateName.c_str());

                    // アニメーション設定
                    if (ImGui::SliderFloat(IMGUI_JP("アニメ速度"), &currentState->animSpeed, 0.1f, 10.0f, "%.2f")) stateDirty = true;
                    if (ImGui::Checkbox(IMGUI_JP("ループ"), &currentState->loop)) stateDirty = true;
                    if (ImGui::SliderInt(IMGUI_JP("アニメ番号"), &currentState->animationIndex, 0, 20)) stateDirty = true;

                    // playback controls
                    ImGui::Separator();
                    ImGui::Text(IMGUI_JP("再生制御"));

                    static bool isPlayingPlayer = false;
                    static bool isPlayingBoss = false;
                    bool& isPlaying = (selectedEntity == 0) ? isPlayingPlayer : isPlayingBoss;

                    // determine debug target entity pointer
                    MeshObject* debugEntity = nullptr;
                    if (selectedEntity == 0 && player) debugEntity = player;
                    if (selectedEntity == 1 && boss) debugEntity = boss;

                    // Play/Pause
                    if (ImGui::Button(isPlaying ? IMGUI_JP("一時停止") : IMGUI_JP("再生")))
                    {
                        isPlaying = !isPlaying;
                        if (debugEntity)
                        {
                            if (isPlaying)
                                debugEntity->SetAnimSpeed(static_cast<double>(currentState->animSpeed));
                            else
                                debugEntity->SetAnimSpeed(0.0);
                        }
                    }

                    ImGui::SameLine();
                    // Step backward one frame
                    if (ImGui::Button(IMGUI_JP("一フレーム戻す")))
                    {
                        if (debugEntity)
                        {
                            double frameSec = 1.0 / 60.0; // assume 60 FPS frame step
                            double t = debugEntity->GetAnimTime();
                            t -= frameSec;
                            if (t < 0.0) t = 0.0;
                            debugEntity->SetAnimTime(t);
                        }
                    }

                    ImGui::SameLine();
                    // Step forward one frame
                    if (ImGui::Button(IMGUI_JP("一フレーム進める")))
                    {
                        if (debugEntity)
                        {
                            double frameSec = 1.0 / 60.0; // assume 60 FPS frame step
                            double t = debugEntity->GetAnimTime();
                            t += frameSec;
                            // clamp to animation period if available
                            if (currentState->animationIndex >= 0)
                            {
                                double period = debugEntity->GetAnimPeriod(currentState->animationIndex);
                                if (period > 0.0 && t > period)
                                {
                                    // if not looping, clamp to end
                                    t = period;
                                }
                            }
                            debugEntity->SetAnimTime(t);
                        }
                    }

                    ImGui::SameLine();
                    if (ImGui::Button(IMGUI_JP("リセット")))
                    {
                        if (debugEntity)
                        {
                            debugEntity->SetAnimTime(0.0);
                        }
                    }

                    // コライダー管理
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
                        stateDirty = true;
                    }

                    for (size_t i = 0; i < currentState->colliders.size(); ++i)
                    {
                        ImGui::PushID(static_cast<int>(i));

                        EnhancedColliderSpec& collider = currentState->colliders[i];

                        if (ImGui::CollapsingHeader(collider.name.c_str()))
                        {
                            ImGui::Indent();

                            // 基本プロパティ
                            char nameBuf[128];
                            strncpy_s(nameBuf, collider.name.c_str(), sizeof(nameBuf) - 1);
                            // Only apply name change when Enter is pressed to avoid focus loss on each character input
                            if (ImGui::InputText(IMGUI_JP("名前"), nameBuf, sizeof(nameBuf), ImGuiInputTextFlags_EnterReturnsTrue))
                            {
                                collider.name = std::string(nameBuf);
                                stateDirty = true;
                            }

                            if (ImGui::DragFloat3(IMGUI_JP("オフセット"), collider.offset, 0.1f)) stateDirty = true;
                            // Per-collider bone tracking
                            if (ImGui::Checkbox(IMGUI_JP("ボーン追従"), &collider.trackBone)) stateDirty = true;
                            if (collider.trackBone)
                            {
                                MeshObject* targetEntity = nullptr;
                                if (selectedEntity == 0 && player) targetEntity = player;
                                if (selectedEntity == 1 && boss) targetEntity = boss;

                                std::vector<std::string> boneNames;
                                if (targetEntity)
                                {
                                    auto mesh = targetEntity->GetAttachMesh().lock();
                                    if (mesh)
                                    {
                                        std::shared_ptr<SkinMesh> skin = std::dynamic_pointer_cast<SkinMesh>(mesh);
                                        if (skin)
                                        {
                                            skin->GetBoneNames(boneNames);
                                        }
                                    }
                                }

                                if (!boneNames.empty())
                                {
                                    std::vector<const char*> ptrs;
                                    ptrs.reserve(boneNames.size());
                                    for (auto &n : boneNames) ptrs.push_back(n.c_str());
                                    int cur = 0;
                                    for (size_t bi = 0; bi < boneNames.size(); ++bi) if (boneNames[bi] == collider.boneName) { cur = static_cast<int>(bi); break; }
                                    if (ImGui::Combo(IMGUI_JP("ボーン選択"), &cur, ptrs.data(), static_cast<int>(ptrs.size())))
                                    {
                                        collider.boneName = boneNames[cur];
                                        stateDirty = true;
                                    }
                                }
                                else
                                {
                                    ImGui::Text(IMGUI_JP("ボーン情報が未ロードです"));
                                    char boneBuf[128];
                                    strncpy_s(boneBuf, collider.boneName.c_str(), sizeof(boneBuf) - 1);
                                    if (ImGui::InputText(IMGUI_JP("ボーン名 (手入力)"), boneBuf, sizeof(boneBuf), ImGuiInputTextFlags_EnterReturnsTrue))
                                    {
                                        collider.boneName = std::string(boneBuf);
                                        stateDirty = true;
                                    }
                                }
                            }

                            if (ImGui::DragFloat(IMGUI_JP("半径"), &collider.radius, 0.1f, 0.1f, 100.0f)) stateDirty = true;
                            if (ImGui::DragFloat(IMGUI_JP("高さ"), &collider.height, 0.1f, 0.1f, 100.0f)) stateDirty = true;

                            // 戦闘プロパティ
                            ImGui::Separator();
                            ImGui::Text(IMGUI_JP("戦闘"));
                            if (ImGui::DragFloat(IMGUI_JP("攻撃量"), &collider.attackAmount, 1.0f, 0.0f, 1000.0f)) stateDirty = true;

                            // マスク編集
                            ImGui::Separator();
                            // RenderMaskEditor returns void; we detect mask changes by copying and comparing
                            {
                                uint32_t before = collider.myMask;
                                RenderMaskEditor(IMGUI_JP("自分のマスク"), collider.myMask);
                                if (before != collider.myMask) stateDirty = true;
                            }
                            {
                                uint32_t before = collider.targetMask;
                                RenderMaskEditor(IMGUI_JP("対象のマスク"), collider.targetMask);
                                if (before != collider.targetMask) stateDirty = true;
                            }

                            // 見た目
                            ImGui::Separator();
                            ImGui::Text(IMGUI_JP("表示"));
                            if (ImGui::ColorEdit4(IMGUI_JP("デバッグ色"), collider.debugColor)) stateDirty = true;
                            if (ImGui::Checkbox(IMGUI_JP("有効"), &collider.active)) stateDirty = true;

                            // 操作
                            ImGui::Separator();
                            if (ImGui::Button(IMGUI_JP("コライダー削除")))
                            {
                                currentState->colliders.erase(currentState->colliders.begin() + i);
                                stateDirty = true;
                                ImGui::Unindent();
                                ImGui::PopID();
                                break;
                            }

                            // Spawn timing controls per collider
                            ImGui::Separator();
                            ImGui::Text(IMGUI_JP("生成タイミング"));
                            if (ImGui::DragFloat(IMGUI_JP("生成遅延 (秒)"), &collider.spawnDelay, 0.01f, 0.0f, 60.0f)) stateDirty = true;
                            if (ImGui::DragFloat(IMGUI_JP("生成継続時間 (秒)"), &collider.spawnDuration, 0.01f, 0.0f, 60.0f)) stateDirty = true;

                            ImGui::Unindent();
                        }

                        ImGui::PopID();
                    }

                    // --- Per-state mask override UI ---
                    ImGui::Separator();
                    ImGui::Text(IMGUI_JP("ステート固有のマスク/オーバーライド (省略でエンティティデフォルトを使用)"));
                    if (ImGui::Checkbox(IMGUI_JP("エンティティデフォルトを使用"), &currentState->useEntityDefaultMasks)) stateDirty = true;
                    if (!currentState->useEntityDefaultMasks)
                    {
                        uint32_t beforeMy = currentState->overrideMyMask;
                        uint32_t beforeTarget = currentState->overrideTargetMask;
                        RenderMaskEditor(IMGUI_JP("上書き: 自分のマスク"), currentState->overrideMyMask);
                        RenderMaskEditor(IMGUI_JP("上書き: 対象のマスク"), currentState->overrideTargetMask);
                        if (beforeMy != currentState->overrideMyMask || beforeTarget != currentState->overrideTargetMask) stateDirty = true;
                    }
                    if (ImGui::Checkbox(IMGUI_JP("ダメージ無効化"), &currentState->disableDamage)) stateDirty = true;

                    // If anything changed, apply immediately to the target entity
                    MeshObject* targetEntity = nullptr;
                    if (selectedEntity == 0 && player) targetEntity = player;
                    if (selectedEntity == 1 && boss) targetEntity = boss;
                    if (stateDirty && targetEntity)
                    {
                        manager.ApplyAnimationState(targetEntity, currentStateName);
                    }
                }
            }
        }
    }

    // End ImGui window and restore styles
    ImGui::End();

    // スタイルを戻す
    ImGui::PopStyleColor(3);
    ImGui::PopStyleVar(4);
}
