#pragma once

#include "..//BossAttackStateBase.h"
#include "00_MeshObject/00_Character/02_Boss/BossIdolState/BossIdolState.h"

class BossSpinningState
    : public BossAttackStateBase
{
public:
    enum class enSpinning : byte
    {
        None,
        Anim,
        Attack,
        CoolDown,
        Trans
    };

public:
    BossSpinningState(Boss* owner);
    ~BossSpinningState() override;

    void Enter() override;
    void Update() override;
    void LateUpdate() override;
    void Draw() override;
    void Exit() override;

    // ImGui と設定の読み書き
    void DrawImGui() override;
    void LoadSettings() override;
    void SaveSettings() const override;
    std::filesystem::path GetSettingsFileName() const override { return std::filesystem::path("BossSpinningState.json"); }

private:
    enSpinning m_List;
    bool m_IsSpun;
    float m_Radius;
    float m_RotateSpeed;
    // 再生するアニメーションのインデックス(保存用)
    int m_AnimIdxChargeToIdol;
    int m_AnimIdxChargeAttack;
    int m_AnimIdxCharge;
};
