#pragma once
#include "..//BossAttackStateBase.h"

/******************************************************************************
*	ため斬り攻撃用のChargeStateクラス.
**/

class Boss;
class BossChargeSlashState;

class BossChargeState final
	: public BossAttackStateBase
{
public:
	BossChargeState(Boss* owner);
	~BossChargeState() override;

	void Enter() override;
	void Update() override;
	void LateUpdate() override;
	void Draw() override;
	void Exit() override;

	//攻撃を実行させる関数.
    void DrawImGui() override;
    void LoadSettings() override;
    void SaveSettings() const override;
    std::filesystem::path GetSettingsFileName() const override { return std::filesystem::path("BossChargeState.json"); }
private:
	std::shared_ptr<BossChargeSlashState> m_ChargeSlash;

	// ためフラグ
	bool ChargeFlag = false;
};
