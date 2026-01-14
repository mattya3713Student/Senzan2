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
		none,			//何もしない.
		SlashAttack,	//斬る攻撃.
		SlashIdol,		//斬る攻撃から待機.
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
	// ホーミング停止秒数（m_StateTimer がこの値を超えるまでだけ追尾）
	float m_HomingEndTime = 0.2f;
	
	//攻撃開始位置.
	DirectX::XMFLOAT3 m_StartPos;

	enList m_List;
};

