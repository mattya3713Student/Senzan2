#pragma once

#include "..//BossAttackStateBase.h"
#include "Game\01_GameObject\00_MeshObject\MeshObject.h"
#include "Game\03_Collision\Capsule\CapsuleCollider.h"

/*************************************************************
*	ボスの攻撃: 斬る攻撃.
**/

class SkinMesh;

class BossSlashState final
	: public BossAttackStateBase
{
public:
	BossSlashState(Boss* owner);
	~BossSlashState();

	void Enter() override;
	void Update() override;
	void LateUpdate() override;
	void Draw() override;
	void Exit() override;
	void UpdateBoneCheck();
	size_t m_CurrentBoneIndex = 0;
	std::chrono::steady_clock::time_point m_LastUpdateTime;
	float m_Interval = 0.5f; // 0.5秒ごとに次のボーンへ
	std::vector<std::string> m_BoneNames =
	{
		"Bone002","Bone003","leg_l","Bone005","Bone006",
		"Bone002_mirrored_","Bone003_mirrored_","leg_r",
		"Bone007","Bone008","Bone009","Bone010","Bone011",
		"head","Bone017","Bone018","Bone019","rejectio",
		"Bone016","Bone014","Bone016_mirrored_","Bone013",
		"Bone021","Bone022","Bone023","hand_l",
		"Bone031","Bone032","Bone029","Bone030","Bone027","Bone028",
		"Bone025","Bone026","Bone033","Bone034","Bone035","Bone036",
		"blade_l","blade_l_head","Bone053",
		"Bone021_mirrored_","Bone022_mirrored_","Bone023_mirrored_",
		"hand_r","Bone031_mirrored_","Bone032_mirrored_",
		"Bone033_mirrored_","Bone034_mirrored_","Bone029_mirrored_",
		"Bone030_mirrored_","Bone035_mirrored_","Bone036_mirrored_",
		"blade_r","blade_r_head",
		"Bone027_mirrored_","Bone028_mirrored_","Bone025_mirrored_",
		"Bone026_mirrored_","Bone054"
	};
private:
	void BossAttack() override;
private:
	enum class Phase
	{
		Idol,
		Attack,
		CoolDown
	};

	const float m_IdolDuration;
	const float m_AttackDuration;
	const float m_CoolDownDuration;

	Phase m_CurrentPhase;
	float m_PhaseTime;


	//斬る攻撃のメンバ変数.
	//攻撃モーションの時間.
	const float m_SlashDuration;
	//今は使用しない.
	//当たり判定の最大距離.
	const float m_SlashRange;
	//扇形の攻撃角度.
	const float m_SlashAngle;

	//今は確認用で後ろに移動させている.
	//後退させる距離.
	const float m_BackWardDistance;
	//後退にかける時間.
	const float m_BackWardDuration;

	//攻撃開始位置.
	DirectX::XMFLOAT3 m_StartPos;
	//攻撃開始時に計算した前方ベクトル.
	DirectX::XMFLOAT3 m_ForWardVector;
	//判定を一度だけおこなう溜めのフラグ.
	bool m_HasHit;

	std::shared_ptr<SkinMesh>		m_Skin;			// スキンメッシュ設定用.
	std::shared_ptr<MeshObject>		m_Mesh;

	std::shared_ptr<Transform> m_pTransform;
};