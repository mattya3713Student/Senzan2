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


	/************************************************************************
	* @brief 衝突後の更新.
	* @attention ※必ず衝突判定後に実行すること.
	************************************************************************/
	void UpdateAfterCollision();


	/************************************************************************
	* @brief 衝突時の処理.
	* @param pObject：衝突したオブジェクト.
	************************************************************************/
	virtual void OnCollision(GameObject* pObject) {};


	/************************************************************************
	* @brief 衝突判定を切り離す.
	************************************************************************/
	void DetachCollider();

	/************************************************************************
	* @brief 衝突判定を取得.
	************************************************************************/
	const std::shared_ptr<ColliderBase>& GetCollider()const;

protected:
	std::shared_ptr<ColliderBase>	m_pCollider;	// 衝突.
};