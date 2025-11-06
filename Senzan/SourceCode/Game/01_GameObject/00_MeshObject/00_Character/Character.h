#pragma once
#include "Game/01_GameObject/00_MeshObject/MeshObject.h"
#include "Game/03_Collision/ColliderBase.h"


/**************************************************
*   キャラクタークラス
**/
class Character
	: public MeshObject
{
public:
	Character();
	virtual ~Character();

	virtual void Update() override;
	virtual void LateUpdate() override;
	virtual void Draw() override;

protected:
	// 衝突応答処理.
	void HandleCollisionResponse();


protected:
	std::shared_ptr<ColliderBase>	m_pPressCollider;	// 衝突.
};