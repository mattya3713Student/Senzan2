#include "BossSpecialState.h"

#include "Game/04_Time/Time.h"
#include "00_MeshObject/00_Character/02_Boss/Boss.h"
#include "00_MeshObject/00_Character/02_Boss/BossIdolState/BossIdolState.h"
#include <DirectXMath.h>

using namespace DirectX;

BossSpecialState::BossSpecialState(Boss* owner)
	: BossAttackStateBase(owner)

	, m_List(enSpecial::None)

	, m_Velocity(0.0f, 0.0f, 0.0f)
	, m_SpecialPower(1.5f)
	, m_Gravity(0.089f)
	, m_SpecialFrag(false)
	, m_GroundedFrag(true)

	, m_Timer(0.0f)
	, m_TransitionTimer(5.0f)
	, m_AttackTimer(3.0f)

	, m_UpSpeed(0.15f)

	// 追尾突進用メンバ変数の初期化
	, m_TargetDirection(0.0f, 0.0f, 0.0f)
	, m_MaxTrackingAngle(30.0f)
	, m_AttackMoveSpeed(1.5f)
	//この数値をいじることでPlayerへの突進限界距離を設定している.
	//今は150までの距離までしかいけない
	//基本的に150.0の位置とり後ろには行かないとおもう.
	, m_AttackDistance(9999.0f)
	, m_DistanceTraveled(0.0f)
{
}

BossSpecialState::~BossSpecialState()
{
}

void BossSpecialState::Enter()
{
	m_Timer = 0.0f;
	m_Velocity = {};
	m_DistanceTraveled = 0.0f;
	m_List = enSpecial::None;
	m_GroundedFrag = true;
}

void BossSpecialState::Update()
{
	float deltaTime = Time::GetInstance().GetDeltaTime();
	const float floorY = 0.0f;

	switch (m_List)
	{
	case BossSpecialState::enSpecial::None:
		// 垂直初速をセットし、Chargeへ移行
		m_Velocity = {};
		m_Velocity.y = m_SpecialPower; // 垂直初速をセット (ジャンプ力)
		m_List = enSpecial::Charge;
		break;

	case BossSpecialState::enSpecial::Charge:
		ChargeTime();
		break;
	case BossSpecialState::enSpecial::Jump:
		JumpTime();
		break;
	case BossSpecialState::enSpecial::Attack:
		BossAttack();
		break;
	case BossSpecialState::enSpecial::CoolTime:
		//クールタイム.
		m_Timer += deltaTime;
		if (m_Timer > m_TransitionTimer)
		{
			m_List = enSpecial::Trans;
		}
		break;

	case BossSpecialState::enSpecial::Trans:
		//Idolへの遷移.
		if (GetAsyncKeyState(VK_RETURN) & 0x8000)
		{
			m_List = enSpecial::None;
		}
		m_pOwner->GetStateMachine()->ChangeState(std::make_shared<BossIdolState>(m_pOwner));
		break;

	default:
		break;
	}
}

void BossSpecialState::LateUpdate()
{
}

void BossSpecialState::Draw()
{
}

void BossSpecialState::Exit()
{
	m_GroundedFrag = true;
	m_SpecialFrag = false;
	m_AttackTimer = 0.0f;
}

void BossSpecialState::BossAttack()
{
	float deltaTime = Time::GetInstance().GetDeltaTime();
	const float floorY = 0.0f;
	const float PlayerYOffset = 1.0f; //PlayerのY座標を少し持ち上げるオフセット.
	//許容誤差.
	const float Tolerance = 2.0f;
	const float One = 1.0f;
	const float MinusOne = -1.0f;

	XMFLOAT3 CurrentPos = m_pOwner->GetPosition();
	DirectX::XMFLOAT3 TargetPosition = m_pOwner->GetTargetPos();

	//ターゲットのY座標にオフセットを追加
	TargetPosition.y += PlayerYOffset;

	//Player到達判定
	XMVECTOR BossPosVec = XMLoadFloat3(&CurrentPos);
	XMVECTOR PlayerPosVec = XMLoadFloat3(&TargetPosition);

	XMVECTOR DistanceVec = XMVectorSubtract(PlayerPosVec, BossPosVec);
	float Distance = XMVectorGetX(XMVector3Length(DistanceVec));

	// 目標地点に十分に近づいた場合.
	if (Distance <= Tolerance)
	{
		// Playerの位置に強制的に移動させ、突進終了
		// ただし、地面に着地させるため、目標位置のYオフセットを解除する
		CurrentPos = TargetPosition;
		CurrentPos.y = floorY; // 地面に着地させる
		m_pOwner->SetPosition(CurrentPos);

		m_Timer = 0.0f;
		m_List = enSpecial::CoolTime;
		return;
	}


	//その他の終了条件のチェック.

	//地面に着地したら強制終了
	if (CurrentPos.y <= floorY + 0.1f)
	{
		CurrentPos.y = floorY;
		m_pOwner->SetPosition(CurrentPos);

		m_Timer = 0.0f;
		m_List = enSpecial::CoolTime;
		return;
	}

	//突進距離が最大に達したら強制終了
	if (m_DistanceTraveled >= m_AttackDistance)
	{
		m_Timer = 0.0f;
		m_List = enSpecial::CoolTime;
		return;
	}

	//追尾方向の計算
	XMVECTOR TargetDir = XMLoadFloat3(&m_TargetDirection);
	XMVECTOR CurrentToPlayer = XMVectorSubtract(PlayerPosVec, BossPosVec);
	CurrentToPlayer = XMVector3Normalize(CurrentToPlayer);

	//限界角度チェックと finalMoveDir の決定
	XMVECTOR DotProduct = XMVector3Dot(TargetDir, CurrentToPlayer);
	float cosTheta = XMVectorGetX(DotProduct);
	float AngleRad = acosf(XMMin(One, XMMax(MinusOne, cosTheta)));

	XMVECTOR finalMoveDir;

	if (XMConvertToDegrees(AngleRad) > m_MaxTrackingAngle)
	{
		XMVECTOR rotationAxis = XMVector3Cross(TargetDir, CurrentToPlayer);

		if (XMVector3LengthSq(rotationAxis).m128_f32[0] > 0.0001f)
		{
			rotationAxis = XMVector3Normalize(rotationAxis);
			float maxAngleRad = XMConvertToRadians(m_MaxTrackingAngle);
			XMMATRIX rotationMatrix = XMMatrixRotationAxis(rotationAxis, maxAngleRad);
			finalMoveDir = XMVector3TransformNormal(TargetDir, rotationMatrix);
		}
		else
		{
			finalMoveDir = TargetDir;
		}
	}
	else
	{
		finalMoveDir = CurrentToPlayer;
	}

	finalMoveDir = XMVector3Normalize(finalMoveDir);

	//ボスの位置を更新
	XMVECTOR moveVector = XMVectorScale(finalMoveDir, m_AttackMoveSpeed * deltaTime);
	XMVECTOR newBossPosVec = XMVectorAdd(BossPosVec, moveVector);

	//Bossが地面を突き抜けないようにY座標を制限する
	XMFLOAT3 newBossPos;
	XMStoreFloat3(&newBossPos, newBossPosVec);

	if (newBossPos.y < floorY) {
		newBossPos.y = floorY;
	}

	//結果を保存
	m_pOwner->SetPosition(newBossPos);

	m_DistanceTraveled += m_AttackMoveSpeed * deltaTime;
}

void BossSpecialState::ChargeTime()
{
	//deltaTime取得.
	float deltaTime = Time::GetInstance().GetDeltaTime();

	//ため時間.
	m_Timer += deltaTime;
	if (m_Timer > m_AttackTimer)
	{
		m_Timer = 0.0f;

		//斜めジャンプの初速を設定
		XMFLOAT3 CurrentPos = m_pOwner->GetPosition();
		XMFLOAT3 PlayerPos = m_pOwner->GetTargetPos();

		XMVECTOR BossPosVec = XMLoadFloat3(&CurrentPos);
		XMVECTOR PlayerPosVec = XMLoadFloat3(&PlayerPos);

		// 目標方向ベクトル (Y成分をゼロにして水平方向ベクトルを抽出)
		XMVECTOR HorizontalDir = XMVectorSubtract(PlayerPosVec, BossPosVec);
		HorizontalDir = XMVectorSetY(HorizontalDir, 0.0f);
		HorizontalDir = XMVector3Normalize(HorizontalDir);

		// 水平速度を設定 (垂直初速の半分程度を水平初速とする)
		float horizontalSpeed = m_SpecialPower * 0.5f;
		XMVECTOR initialVelocityXZ = XMVectorScale(HorizontalDir, horizontalSpeed);

		//m_VelocityのX, Z成分に水平初速をセット
		//Y成分は上書きしないように注意
		XMFLOAT3 currentVelocity = m_Velocity;
		XMStoreFloat3(&currentVelocity, initialVelocityXZ);

		m_Velocity.x = currentVelocity.x;
		m_Velocity.z = currentVelocity.z;
		m_Velocity.y = m_SpecialPower; //垂直初速をセット

		m_List = enSpecial::Jump;
	}
}

void BossSpecialState::JumpTime()
{
	//deltaTime取得.
	float deltaTime = Time::GetInstance().GetDeltaTime();

	//速度の更新 (Y成分のみに重力を適用)
	float gravityAcc = m_Gravity * deltaTime;
	m_Velocity.y -= gravityAcc;

	//位置の更新.
	XMVECTOR BossPos = XMLoadFloat3(&m_pOwner->GetPosition());
	XMVECTOR velocity = XMLoadFloat3(&m_Velocity);

	//水平成分 (X, Z) はそのまま残し、斜めに移動させる
	XMVECTOR moveVector = XMVectorScale(velocity, deltaTime);
	BossPos = XMVectorAdd(BossPos, moveVector);

	XMFLOAT3 newBossPos;
	XMStoreFloat3(&newBossPos, BossPos);
	m_pOwner->SetPosition(newBossPos);

	//遷移条件: 垂直速度がゼロ以下になったら (最高到達点到達)
	if (m_Velocity.y <= 0.0f)
	{
		//速度の慣性をリセット (空中静止状態へ)
		m_Velocity = {};

		//突進開始へ移行
		m_DistanceTraveled = 0.0f;

		//突進の目標方向を計算し、m_TargetDirectionにセット
		XMFLOAT3 PlayerPos = m_pOwner->GetTargetPos();
		XMVECTOR pPos = XMLoadFloat3(&PlayerPos);
		XMVECTOR bPos_now = XMLoadFloat3(&newBossPos);
		XMVECTOR targetVec = XMVectorSubtract(pPos, bPos_now);
		targetVec = XMVector3Normalize(targetVec);
		XMStoreFloat3(&m_TargetDirection, targetVec);

		m_List = enSpecial::Attack;
	}
}
