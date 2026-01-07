#include "MeshObject.h"
#include "Resource/Mesh/MeshBase.h"
#include "Resource/Mesh/01_Static/StaticMesh.h"
#include "Resource/Mesh/02_Skin/SkinMesh.h"

MeshObject::MeshObject()
	: GameObject()
	, m_pMesh()
	, m_IsLight(false)
	, m_IsShadow(true)
	, m_pAnimCtrl(nullptr)
	, m_AnimNo(0)
	, m_AnimSpeed(0.0)
	, m_AnimTimer(0.0)
	, m_Isloop (false)
	, m_BonePos()
	, m_AnimPaused(false)
	, m_SavedAnimSpeed(0.0)
{
}

//------------------------------------------------------------------------------------.

MeshObject::~MeshObject()
{
}

//------------------------------------------------------------------------------------.

void MeshObject::Update()
{
    
}

void MeshObject::LateUpdate()
{
}

//------------------------------------------------------------------------------------.

void MeshObject::Draw()
{
	if (!m_IsRenderActive) { return; }

	// メッシュ未接続の場合、描画しない.
	if (m_pMesh.expired()) { return; }

	// 描画直前で必要な情報をMesh本体に送る.
	if (auto pMesh = m_pMesh.lock())
	{
		pMesh->SetPosition(m_spTransform->Position);
		pMesh->SetRotation(m_spTransform->Rotation);
		pMesh->SetScale(m_spTransform->Scale);
		pMesh->SetIsLight(m_IsLight);
		pMesh->SetIsShadow(m_IsShadow);
	}

	// StaticMesh時の描画.
	if (std::shared_ptr<StaticMesh> staticMesh = std::dynamic_pointer_cast<StaticMesh>(m_pMesh.lock()))
	{
		staticMesh->Render();
	}
	// SkinMesh時の描画.
	else if (std::shared_ptr<SkinMesh> skinMesh = std::dynamic_pointer_cast<SkinMesh>(m_pMesh.lock()))
	{
		m_AnimTimer += m_AnimSpeed * GetDelta();
		IsLoopAnimTimeSet();
		m_pAnimCtrl->SetTrackPosition(0, m_AnimTimer);
        m_pAnimCtrl->AdvanceTime(0.0f, NULL);
		skinMesh->Render(m_pAnimCtrl);
	}
}

//------------------------------------------------------------------------------------.

void MeshObject::DrawDepth()
{
	if (!m_IsRenderActive) { return; }

	// メッシュ未接続の場合、深度描画しない.
	if (m_pMesh.expired()) { return; }

	// 描画直前で必要な情報をMesh本体に送る.
	if (auto pMesh = m_pMesh.lock())
	{
		pMesh->SetPosition(m_spTransform->Position);
		pMesh->SetRotation(m_spTransform->Rotation);
		pMesh->SetScale(m_spTransform->Scale);
	}

	// StaticMesh時の深度描画.
	if (std::shared_ptr<StaticMesh> staticMesh = std::dynamic_pointer_cast<StaticMesh>(m_pMesh.lock()))
	{
		staticMesh->RenderDepth();
	}
	// SkinMesh時の深度描画.
	else if (std::shared_ptr<SkinMesh> skinMesh = std::dynamic_pointer_cast<SkinMesh>(m_pMesh.lock()))
	{
		skinMesh->RenderDepth(m_pAnimCtrl);
	}

}

//------------------------------------------------------------------------------------.

void MeshObject::AttachMesh(const std::shared_ptr<MeshBase>& pMesh)
{
	if (!pMesh) { return; }

	m_pMesh = pMesh;

	// 接続メッシュがSkinならアニメーションコントローラーのクローンも作成する.
	if (std::shared_ptr<SkinMesh> skinMesh = std::dynamic_pointer_cast<SkinMesh>(pMesh))
	{
		//アニメーションコントローラを取得.
		LPD3DXANIMATIONCONTROLLER pAC = skinMesh->GetAnimationController();

		//アニメーションコントローラのクローンを作成.
		if (FAILED(
			pAC->CloneAnimationController(
			pAC->GetMaxNumAnimationOutputs(),
			pAC->GetMaxNumAnimationSets(),
			pAC->GetMaxNumTracks(),
			pAC->GetMaxNumEvents(),
			&m_pAnimCtrl)))		//(out)クローン作成先.
		{
			_ASSERT_EXPR(false, L"アニメーションコントローラのクローン作成失敗");
		}
	}
}

//------------------------------------------------------------------------------------.

void MeshObject::DetachMesh()
{
	m_pMesh.reset();
}

//------------------------------------------------------------------------------------.

const std::string MeshObject::GetResourceName() const
{
	if (auto pMesh = m_pMesh.lock())
	{
		return pMesh->GetResourceName();
	}
	return "None";
}

//------------------------------------------------------------------------------------.

void MeshObject::SetIsLight(const bool& isLight)
{
	m_IsLight = isLight;
}

void MeshObject::SetIsShadow(const bool& isShadow)
{
	m_IsShadow = isShadow;
}

// アニメーションの速度を設定.
void MeshObject::SetAnimSpeed(const double speed)
{
    if (m_AnimPaused)
    {
        // store when paused so resume restores correctly
        m_SavedAnimSpeed = speed;
    }
    else
    {
        m_AnimSpeed = speed;
    }
}

// アニメーションの速度を取得.
double MeshObject::GetAnimSpeed() const
{
    return m_AnimPaused ? m_SavedAnimSpeed : m_AnimSpeed;
}

// 現在のアニメーションインデックスを取得.
int MeshObject::GetAnimIndex() const
{
    return m_AnimNo;
}

// 新: 再生一時停止フラグを設定/取得
void MeshObject::SetAnimPaused(bool paused)
{
    if (paused == m_AnimPaused) return;
    m_AnimPaused = paused;
    if (m_AnimPaused)
    {
        // save current speed and stop progression
        m_SavedAnimSpeed = m_AnimSpeed;
        m_AnimSpeed = 0.0;
    }
    else
    {
        // restore saved speed
        m_AnimSpeed = m_SavedAnimSpeed;
    }
}

bool MeshObject::IsAnimPaused() const
{
    return m_AnimPaused;
}

// アニメーションのループ設定.
void MeshObject::SetIsLoop(const bool isLoop)
{
	m_Isloop = isLoop;
}

// アニメーションループの設定.
void MeshObject::SetAnimTime(double StartTime)
{
	m_AnimTimer = StartTime;
}

// アニメーションを停止（再生速度を0に）
void MeshObject::StopAnimation()
{
	m_AnimSpeed = 0.0;
}

// 現在のアニメーション再生時間を取得
double MeshObject::GetAnimTime() const
{
	return m_AnimTimer;
}

// アニメーション終了時間を取得.
double MeshObject::GetAnimPeriod(int index) const
{
	if (std::shared_ptr<SkinMesh> skinMesh = std::dynamic_pointer_cast<SkinMesh>(m_pMesh.lock()))
	{
		return skinMesh->GetAnimPeriod(index);
	}
	return 0.0;
}

void MeshObject::IsLoopAnimTimeSet()
{
    // アニメーションの総時間を取得.
    double EndTime = GetAnimPeriod(m_AnimNo);
    if (EndTime <= 0.0) {
        return;
    }

    // ループしない場合終端で固定して停止.
    if (!m_Isloop) {
        if (m_AnimTimer >= EndTime) {
            // 再生時間を終端に合わせる.
            m_AnimTimer = EndTime;
            m_AnimSpeed = 0.0;
        }
    }
    // ループする場合終端到達で先頭に戻す.
    else {
        if (m_AnimTimer >= EndTime) {
            while (m_AnimTimer >= EndTime) {
                m_AnimTimer -= EndTime;
            }
            // MEMO : 負になる可能性があるため補正.
            if (m_AnimTimer < 0.0) {
                m_AnimTimer = 0.0;
            }
        }
    }
}

//------------------------------------------------------------------------------------.

HRESULT MeshObject::FindVerticesOnPoly(LPD3DXMESH pMesh, DWORD dwPolyIndex, DirectX::XMFLOAT3* pVertices)
{
	//頂点ごとのバイト数を取得.
	DWORD dwStride = pMesh->GetNumBytesPerVertex();
	//頂点数を取得.
	DWORD dwVertexAmt = pMesh->GetNumVertices();
	//面数を取得.
	DWORD dwPoly = pMesh->GetNumFaces();

	WORD* pwPoly = nullptr;

	//インデックスバッファをロック(読み込みモード).
	pMesh->LockIndexBuffer(
		D3DLOCK_READONLY,
		reinterpret_cast<VOID**>(&pwPoly));

	BYTE* pbVertices = nullptr; 		//頂点(バイト型).
	FLOAT* pfVertices = nullptr; 	//頂点(float型).
	LPDIRECT3DVERTEXBUFFER9 VB = nullptr; 	//頂点バッファ.

	//頂点情報の取得.
	pMesh->GetVertexBuffer(&VB);

	//頂点バッファのロック.
	if (SUCCEEDED(
		VB->Lock(0, 0, reinterpret_cast<VOID**>(&pbVertices), 0)))
	{
		//ポリゴンの頂点1つ目を取得.
		pfVertices = reinterpret_cast<FLOAT*>(&pbVertices[dwStride * pwPoly[dwPolyIndex * 3]]);
		pVertices[0].x = pfVertices[0];
		pVertices[0].y = pfVertices[1];
		pVertices[0].z = pfVertices[2];

		//ポリゴンの頂点2つ目を取得.
		pfVertices = reinterpret_cast<FLOAT*>(&pbVertices[dwStride * pwPoly[dwPolyIndex * 3 + 1]]);
		pVertices[1].x = pfVertices[0];
		pVertices[1].y = pfVertices[1];
		pVertices[1].z = pfVertices[2];

		//ポリゴンの頂点3つ目を取得.
		pfVertices = reinterpret_cast<FLOAT*>(&pbVertices[dwStride * pwPoly[dwPolyIndex * 3 + 2]]);
		pVertices[2].x = pfVertices[0];
		pVertices[2].y = pfVertices[1];
		pVertices[2].z = pfVertices[2];

		pMesh->UnlockIndexBuffer(); //√ロック解除.
		VB->Unlock(); 	//ロック解除.
	}
	VB->Release(); 	//不要になったので解放.

	return S_OK;
}
