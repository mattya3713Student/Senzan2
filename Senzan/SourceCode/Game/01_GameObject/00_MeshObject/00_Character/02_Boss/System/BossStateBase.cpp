#include "BossStateBase.h"

#include "..//Boss.h"

#include "Game/04_Time/Time.h"    

#include "System/Singleton/Debug/Log/DebugLog.h"    


BossStateBase::BossStateBase(Boss* pOwner)
	: StateBase<Boss> (pOwner)
{
}

Boss* BossStateBase::GetBoss() const
{
	return m_pOwner;
}

//プレイヤーの方に常に向くようにする.
void BossStateBase::RotateToPlayer(float RotationSpeed)
{
	//ボスの位置を取得.
	DirectX::XMFLOAT3 BossPos_XF = m_pOwner->GetPosition();
	//プレイヤーの位置の取得.
	DirectX::XMFLOAT3 PlayerPos_XF = m_pOwner->GetTargetPos();

	//プレイヤーへの方向ベクトルを計算.
	float dx = PlayerPos_XF.x - BossPos_XF.x;
	float dz = PlayerPos_XF.z - BossPos_XF.z;

	//距離が極端に近い場合は、角度が不安定になるので処理しない.
	if (std::abs(dx) < 0.001f && std::abs(dz) < 0.001f)
	{
		return;
	}

	//目標の角度を入手.
	float Target_Angle_Rad = std::atan2f(dx, dz);

	//ラジアンを度数に変換する.
	float Target_Angle_Deg = Target_Angle_Rad * (180.0f / DirectX::XM_PI);

	//指定した速度でゆっくり回転させる.
	//ToDo : 速度を変更するときは、第二引数を変更する.
	RotetoToTarget(Target_Angle_Deg, 200.0f);
}

//目的の角度に向かって指定した速度で回転させる.
void BossStateBase::RotetoToTarget(
	float TargetRote, 
	float RotetionSpeed)
{
	//現在の角度を入手する.
	DirectX::XMFLOAT3 Current_Rotation = m_pOwner->GetTransform()->GetRotationDegrees();
	float CurrentRote = Current_Rotation.y;
	float deltaTime = m_pOwner->GetDelta();

	//角度を0~360度の範囲に正規化する.
	TargetRote = MyMath::NormalizeAngleDegrees(TargetRote);
	CurrentRote = MyMath::NormalizeAngleDegrees(CurrentRote);

	//最短距離で回るための差分の計算.
	float Angle_Diff = TargetRote - CurrentRote;
	if (Angle_Diff > 180.0f)
	{
		Angle_Diff -= 360.0f;
	}
	if (Angle_Diff < -180.0f)
	{
		Angle_Diff += 360.0f;
	}

	//このフレームで回転できる最大量.
	float Max_Rotate_Amount = RotetionSpeed * deltaTime;

	//回転の適応
	if (std::abs(Angle_Diff) <= Max_Rotate_Amount)
	{
		//差が小さいなら目標角度に吸着(ガタつき防止)
		Current_Rotation.y = TargetRote;
	}
	else
	{
		//目標の方向に向かって最大量だけ回す.
		Current_Rotation.y += (Angle_Diff > 0)
			? Max_Rotate_Amount : -Max_Rotate_Amount;
	}

	//最後にセット.
	Current_Rotation.y = MyMath::NormalizeAngleDegrees(Current_Rotation.y);
	m_pOwner->GetTransform()->SetRotationDegrees(Current_Rotation);
}
