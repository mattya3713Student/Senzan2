#pragma once

#include "..//BossAttackStateBase.h"
#include "Game/01_GameObject/00_MeshObject/MeshObject.h"
#include "Game/03_Collision/00_Core/01_Capsule/CapsuleCollider.h"

/*************************************************************
*	ボスの攻撃: 斬る攻撃.
**/

class SkinMesh;
class BossIdolState;

class BossSlashState final
	: public BossAttackStateBase
{
public:
	enum class enList : byte
	{
		SlashAttack,	//斬る攻撃.
		SlashIdol,		//斬る攻撃から待機.

		none,
	};
public:
	BossSlashState(Boss* owner);
	~BossSlashState();

	void Enter() override;
	void Update() override;
	void LateUpdate() override;
	void Draw() override;
	void Exit() override;
private:
	void BossAttack() override;
private:

	//攻撃開始位置.
	DirectX::XMFLOAT3 m_StartPos;

	std::shared_ptr<BossIdolState>		m_pIdol;
	std::shared_ptr<SkinMesh>		m_Skin;			// スキンメッシュ設定用.
	std::shared_ptr<MeshObject>		m_Mesh;

	std::shared_ptr<Transform> m_pTransform;

	enList m_List;

	bool AnimChange;
};