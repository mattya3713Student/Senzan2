#pragma once
#include "../GameObject.h"
#include "System/Singleton/ResourceManager/ResourceManager.h"

// 前方宣言.
class MeshBase;

/****************************************************************************
*	メッシュオブジェクトクラス.
****************************************************************************/
class MeshObject
	: public GameObject
{
public:
	MeshObject();
	virtual ~MeshObject()override;

	virtual void Update() override;
	virtual void LateUpdate() override;
	virtual void Draw() override;
	void DrawDepth();

	/************************************************************************
	* @brief メッシュを接続.
	* @param pMesh：接続するメッシュ.
	************************************************************************/
	void AttachMesh(const std::shared_ptr<MeshBase>& pMesh);


	/************************************************************************
	* @brief メッシュを切り離す.
	************************************************************************/
	void DetachMesh();

public: // Getter、Setter.

	/************************************************************************
	* @brief 接続メッシュ名を取得.
	************************************************************************/
	const std::string GetResourceName() const;

	
	/************************************************************************
	* @brief 接続メッシュ名を取得.
	************************************************************************/
	const std::weak_ptr<MeshBase> GetAttachMesh() const {
		return m_pMesh;
	};


	/******************************************************************
	* @brief ライトを使用するか設定.
	******************************************************************/
	void SetIsLight(const bool& isLight);


	/******************************************************************
	* @brief 影を落とすか設定.
	******************************************************************/
	void SetIsShadow(const bool& isShadow);


	// アニメーション速度を設定.
	void SetAnimSpeed(double speed);

	// アニメーションループの設定.
	void SetIsLoop(bool isLoop);
	
	// アニメーションを変更.
	template<typename T>
	void ChangeAnim(T index_T)
	{
		int index = static_cast<int>(index_T);
		//もし、アニメーションナンバーが自分の設定した番号と一緒なら返す.
		if (m_AnimNo == index) { return; }

		m_AnimNo = index;
		if (std::shared_ptr<SkinMesh> skinMesh = std::dynamic_pointer_cast<SkinMesh>(m_pMesh.lock()))
		{
			skinMesh->ChangeAnimSet(index, m_pAnimCtrl);
		}
	}

	/************************************************************************
	* @brief アニメーション終了時間を取得.
	************************************************************************/
	const double GetAnimPeriod(const int& index) const;


private:
	/************************************************************************
	* ループするかを考慮したアニメーション再生.
	************************************************************************/
	void IsLoopAnimTimeSet();


	/************************************************************************
	* @brief 交差位置のポリゴンの頂点を見つける.
	************************************************************************/
	HRESULT FindVerticesOnPoly(
		LPD3DXMESH pMesh,
		DWORD dwPolyIndex,
		DirectX::XMFLOAT3* pVertices);

protected:
	std::weak_ptr<MeshBase>			m_pMesh;

	bool m_IsLight;	// ライトを使用するか.
	bool m_IsShadow;// 影を落とすか.

	// SkinMesh用.
	LPD3DXANIMATIONCONTROLLER	m_pAnimCtrl;//アニメーションコントローラ.		

	int							m_AnimNo;	//アニメーション番号.
	double						m_AnimSpeed; // アニメーション速度.
	double						m_AnimTimer;// アニメ―ション経過時間.
	bool						m_Isloop;	// アニメーションをループするか.
	DirectX::XMFLOAT3			m_BonePos;	// ボーン座標.
};