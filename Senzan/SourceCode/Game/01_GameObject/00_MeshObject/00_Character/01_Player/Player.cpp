#include "Player.h"
#include "System/Utility/StateMachine/StateMachine.h"

#include "State/PlayerStateID.h"
#include "State/PlayerStateBase.h"

#include "State/Root/Root.h"

#include "State/Root/00_System/System.h"
#include "State/Root/00_System/00_Pause/Pause.h"
#include "State/Root/00_System/01_KnockBack/KnockBack.h"
#include "State/Root/00_System/02_Dead/Dead.h"
#include "State/Root/00_System/03_SpecialAttack/SpecialAttack.h"

#include "State/Root/01_Action/Action.h"
#include "State/Root/01_Action/00_Movement/Movement.h"
#include "State/Root/01_Action/00_Movement/00_Idle/Idle.h"
#include "State/Root/01_Action/00_Movement/01_Run/Run.h"

#include "State/Root/01_Action/01_Combat/Combat.h"
#include "State/Root/01_Action/01_Combat/00_AttackCombo_0/AttackCombo_0.h"
#include "State/Root/01_Action/01_Combat/01_AttackCombo_1/AttackCombo_1.h"
#include "State/Root/01_Action/01_Combat/02_AttackCombo_2/AttackCombo_2.h"

#include "State/Root/01_Action/02_Dodge/Dodge.h"
#include "State/Root/01_Action/02_Dodge/00_DodgeExecute/DodgeExecute.h"
#include "State/Root/01_Action/02_Dodge/01_JustDodge/JustDodge.h"

#include "Game/03_Collision/00_Core/01_Capsule/CapsuleCollider.h"
#include "Game/03_Collision/00_Core/Ex_CompositeCollider/CompositeCollider.h"

#include "Game/04_Time/Time.h"

#include "System/Singleton/CollisionDetector/CollisionDetector.h"
#include "System/Singleton/CameraManager/CameraManager.h"
#include "System/Singleton/ParryManager/ParryManager.h"
#include "Graphic/DirectX/DirectX11/DirectX11.h"
#include "Resource/Effect/EffectResource.h"
#include <random>

Player::Player()
	: Character         ()
	, m_RootState       ( std::make_unique<PlayerState::Root>(this) )
	, m_StateRefMap     ( )
	, m_NextStateID     ( PlayerState::eID::None )
	, m_IsStateChangeRequest    ( false )
	, m_MoveVec         ( { 0.f,0.f,0.f } )
	, m_Combo           ( 0 )
	, m_CurrentUltValue ( 0.0f )
	, m_MaxUltValue     ( 10000.0f )
	, m_IsKnockBack     ( false )
	, m_KnockBackVec    ( { 0.f,0.f,0.f } )
	, m_KnockBackPower  ( 0.f )
	, m_IsDead          ( false )
	, m_RunMoveSpeed    ( 50.f )
	, m_IsSuccessParry  ( false )
	, m_pAttackCollider ( nullptr )
	, m_IsJustDodgeTiming( false )
	, m_TargetPos        ( { 0.f,0.f,0.f } )
	, m_DebugForcedState ( PlayerState::eID::None )
	, m_DebugRepeatOnExit( false )
	, m_DebugWasInForcedState( false )
{
	// ステートの初期化.
	InitializeStateRefMap();

	// メッシュのアタッチ.
	auto mesh = ResourceManager::GetSkinMesh("player");
	_ASSERT_EXPR(mesh != nullptr, "メッシュの取得に失敗");
	AttachMesh(mesh);

	//デバック確認のため.
	DirectX::XMFLOAT3 pos = { 0.f, 0.f, -20.f };
	m_spTransform->SetPosition(pos);

	DirectX::XMFLOAT3 scale = { 3.f, 3.f, 3.f };
	m_spTransform->SetScale(scale);

	m_MaxHP = 10000.f;
	m_HP = m_MaxHP;

	// 被ダメの追加.
	std::unique_ptr<CapsuleCollider> damage_collider
        = std::make_unique<CapsuleCollider>(m_spTransform);

	m_pDamageCollider = damage_collider.get();

	damage_collider->SetColor(Color::eColor::Yellow);
	damage_collider->SetHeight(2.0f);
	damage_collider->SetRadius(0.5f);
	damage_collider->SetPositionOffset(0.f, 1.5f, 0.f);
	damage_collider->SetMyMask(eCollisionGroup::Player_Damage);
	damage_collider->SetTarGetTargetMask(eCollisionGroup::Enemy_Attack);

	m_upColliders->AddCollider(std::move(damage_collider));

	// パリィの追加.
	std::unique_ptr<CapsuleCollider> parry_collider = std::make_unique<CapsuleCollider>(m_spTransform);

	m_pParryCollider = parry_collider.get();

	parry_collider->SetActive(false);
	parry_collider->SetColor(Color::eColor::Green);
	parry_collider->SetHeight(2.0f);
	parry_collider->SetRadius(0.5f);
	parry_collider->SetPositionOffset(0.f, 1.5f, 0.f);
	parry_collider->SetMyMask(eCollisionGroup::Player_Parry_Fai | eCollisionGroup::Player_Parry_Suc);
	parry_collider->SetTarGetTargetMask(eCollisionGroup::Enemy_Attack);
	m_upColliders->AddCollider(std::move(parry_collider));

	// ジャスト回避の追加.
	std::unique_ptr<CapsuleCollider> justdodge_collider = std::make_unique<CapsuleCollider>(m_spTransform);

	justdodge_collider->SetColor(Color::eColor::Gray);
	justdodge_collider->SetHeight(45.0f);
	justdodge_collider->SetRadius(45.0f);
	justdodge_collider->SetPositionOffset(0.f, 1.5f, 0.f);
	justdodge_collider->SetMyMask(eCollisionGroup::Player_JustDodge);
	justdodge_collider->SetTarGetTargetMask(eCollisionGroup::Enemy_Attack);

	m_upColliders->AddCollider(std::move(justdodge_collider));

	// 押し戻しの追加.
	std::unique_ptr<CapsuleCollider> pressCollider = std::make_unique<CapsuleCollider>(m_spTransform);

	pressCollider->SetColor(Color::eColor::Cyan);
	pressCollider->SetHeight(3.0f);
	pressCollider->SetRadius(1.0f);
	pressCollider->SetPositionOffset(0.f, 1.5f, 0.f);
	pressCollider->SetMyMask(eCollisionGroup::Press);
	pressCollider->SetTarGetTargetMask(eCollisionGroup::BossPress);

	m_upColliders->AddCollider(std::move(pressCollider));

	// 攻撃の追加.
	std::unique_ptr<CapsuleCollider> attackCollider = std::make_unique<CapsuleCollider>(m_spTransform);

	m_pAttackCollider = attackCollider.get();

	attackCollider->SetActive(false);
	attackCollider->SetColor(Color::eColor::Red);
	attackCollider->SetAttackAmount(100.0f);
	attackCollider->SetHeight(3.0f);
	attackCollider->SetRadius(1.0f);
	attackCollider->SetPositionOffset(0.f, 1.5f, 2.f);
	attackCollider->SetMyMask(eCollisionGroup::Player_Attack);
	attackCollider->SetTarGetTargetMask(eCollisionGroup::Enemy_Damage);

	m_upColliders->AddCollider(std::move(attackCollider));

	CollisionDetector::GetInstance().RegisterCollider(*m_upColliders);

	// 各ステートの初期化.
	m_RootState.get()->Enter();
}

Player::~Player()
{
    if (m_upColliders)
    {
        CollisionDetector::GetInstance().UnregisterCollider(*m_upColliders);
    }
}

void Player::Update()
{
	Character::Update();

	// ステート遷移のチェック.
	if (m_NextStateID != PlayerState::eID::None)
	{
		m_RootState->ChangeState(m_NextStateID);
		m_NextStateID = PlayerState::eID::None;
	}
	if (!m_RootState) {
		return;
	}
	m_RootState->Update();

	m_IsJustDodgeTiming = false;

    EffekseerManager::GetInstance().GetManager()->Update();
}

void Player::LateUpdate()
{
	Character::LateUpdate();

	if (!m_RootState) {
		return;
	}

	// ステートマシーンの最終更新を実行.
	m_RootState->LateUpdate();

	// 押し戻し.
	HandleCollisionResponse();

    // 衝突イベント処理を実行
    HandleParry_SuccessDetection();
    HandleParry_FailDetection();
	HandleDamageDetection();
	HandleAttackDetection();
	HandleDodgeDetection();
}

void Player::Draw()
{
	// モデルの関係で前後反転.
	m_spTransform->SetRotationY(GetRotation().y + D3DXToRadian(180.0f));

	Character::Draw();

	m_spTransform->SetRotationY(GetRotation().y - D3DXToRadian(180.0f));
}

// ノック中か.
bool Player::IsKnockBack() const noexcept
{
	return m_IsKnockBack;
}

// 死亡中か.
bool Player::IsDead() const noexcept
{
	return m_IsDead;
}

bool Player::IsParry() const noexcept
{
	return m_IsSuccessParry;
}

// ポーズ中か.
bool Player::IsPaused() const noexcept
{
	return false;
}

// ステートの変更.
void Player::ChangeState(PlayerState::eID id)
{
    // 遷移ステートがより優先度の高いほうを設定.
    auto IsSystemState = [](PlayerState::eID s) {
        return s == PlayerState::eID::Pause ||
               s == PlayerState::eID::KnockBack ||
               s == PlayerState::eID::Dead ||
               s == PlayerState::eID::SpecialAttack;
    };

    // まだ予約がない場合はそのままセット
    if (m_NextStateID == PlayerState::eID::None)
    {
        m_NextStateID = id;
        return;
    }

    // 新しい遷移がSystem側なら優先して上書き
    if (IsSystemState(id))
    {
        m_NextStateID = id;
        return;
    }

    // 既に予約されている遷移がSystem側ならそれを優先（上書きしない）
    if (IsSystemState(m_NextStateID))
    {
        return;
    }

    // それ以外は新しい遷移で上書きする
    m_NextStateID = id;
}

std::reference_wrapper<PlayerStateBase> Player::GetStateReference(PlayerState::eID id)
{
	try
	{
		auto it = m_StateRefMap.find(id);

		if (it == m_StateRefMap.end())
		{
			throw std::out_of_range("PlayerStateID not mapped (ID: " + std::to_string(static_cast<int>(id)) + ")");
		}

		return it->second();
	}
	catch (const std::exception& e)
	{
		std::string message = "Player::GetStateReference failed (ID: " + std::to_string(static_cast<int>(id)) + "): " + e.what();
		//Debug::Warning(message.c_str());
		throw;
	}
}

// マッピングを初期化するヘルパー関数.
void Player::InitializeStateRefMap()
{
	// --- Systemステート ---.
	m_StateRefMap[PlayerState::eID::Pause]          = [this] { return m_RootState->GetPauseStateRef(); };
	m_StateRefMap[PlayerState::eID::KnockBack]      = [this] { return m_RootState->GetKnockBackStateRef(); };
	m_StateRefMap[PlayerState::eID::Dead]           = [this] { return m_RootState->GetDeadStateRef(); };
	m_StateRefMap[PlayerState::eID::SpecialAttack]  = [this] { return m_RootState->GetSpecialAttackStateRef(); };

	// --- Movementステート ---.
	m_StateRefMap[PlayerState::eID::Idle]           = [this] { return m_RootState->GetIdleStateRef(); };
	m_StateRefMap[PlayerState::eID::Run]            = [this] { return m_RootState->GetRunStateRef(); };

	// --- Dodgeステート ---.
	m_StateRefMap[PlayerState::eID::DodgeExecute]   = [this] { return m_RootState->GetDodgeExecuteStateRef(); };
	m_StateRefMap[PlayerState::eID::JustDodge]      = [this] { return m_RootState->GetJustDodgeStateRef(); };

	// --- Combatステート ---.
	m_StateRefMap[PlayerState::eID::AttackCombo_0]  = [this] { return m_RootState->GetCombo0StateRef(); };
	m_StateRefMap[PlayerState::eID::AttackCombo_1]  = [this] { return m_RootState->GetCombo1StateRef(); };
	m_StateRefMap[PlayerState::eID::AttackCombo_2]  = [this] { return m_RootState->GetCombo2StateRef(); };
	m_StateRefMap[PlayerState::eID::Parry]          = [this] { return m_RootState->GetParryStateRef(); };
}

// 衝突_被ダメージ.
void Player::HandleDamageDetection()
{
	if (!m_upColliders) return;

	const auto& internal_colliders = m_upColliders->GetInternalColliders();

	for (const auto& collider_ptr : internal_colliders)
	{
		const ColliderBase* current_collider = collider_ptr.get();

		if ((current_collider->GetMyMask() & eCollisionGroup::Player_Damage) == eCollisionGroup::None) {
			continue;
		}

		for (const CollisionInfo& info : current_collider->GetCollisionEvents())
		{
			if (!info.IsHit) continue;
			const ColliderBase* otherCollider = info.ColliderB;
			if (!otherCollider) { continue; }

			eCollisionGroup other_group = otherCollider->GetMyMask();

			if ((other_group & eCollisionGroup::Enemy_Attack) != eCollisionGroup::None)
			{
				SoundManager::GetInstance().Play("Damage");
				SoundManager::GetInstance().SetVolume("Damage",7000);

				// 既にスタン中や無敵時間であれば処理を中断
				if (IsKnockBack() || IsDead()) { continue; }

				m_Combo = 0;

				// ダメージを適用 
				ApplyDamage(info.AttackAmount);

				m_KnockBackVec = info.Normal;
				m_KnockBackPower = 100.f;

				// 状態をノックバックに遷移させる
				ChangeState(PlayerState::eID::KnockBack);

				CameraManager::GetInstance().ShakeCamera(0.5f, 4.5f); // カメラを少し揺らす.

				// 1フレームに1回.
				return;
			}
		}
	}
}

void Player::HandleAttackDetection()
{
	if (!m_upColliders) return;

	const auto& internal_colliders = m_upColliders->GetInternalColliders();

	for (const auto& collider_ptr : internal_colliders)
	{
		const ColliderBase* current_collider = collider_ptr.get();

		if ((current_collider->GetMyMask() & eCollisionGroup::Player_Attack) == eCollisionGroup::None) {
			continue;
		}

		for (const CollisionInfo& info : current_collider->GetCollisionEvents())
		{
			if (!info.IsHit) continue;
			const ColliderBase* otherCollider = info.ColliderB;
			if (!otherCollider) { continue; }

			eCollisionGroup other_group = otherCollider->GetMyMask();

            if ((other_group & eCollisionGroup::Enemy_Damage) != eCollisionGroup::None)
            {
                for (int i = 0; i < 3; ++i)
                { // 小さなランダムオフセットを追加してエフェクト位置に揺らぎを持たせる
                    static thread_local std::mt19937 s_rng((std::random_device())());
                    std::uniform_real_distribution<float> dist(-1.2f, 1.2f);
                    std::uniform_real_distribution<float> rotDist(0.0f, DirectX::XM_2PI);

                    DirectX::XMFLOAT3 jitterPos{
                        info.ContactPoint.x + dist(s_rng),
                        info.ContactPoint.y + 1.5f + dist(s_rng),
                        info.ContactPoint.z + dist(s_rng)
                    };

                    // ランダム回転（ラジアン）を作成してエフェクトに渡す
                    DirectX::XMFLOAT3 eulerRot{ rotDist(s_rng), rotDist(s_rng), rotDist(s_rng) };
                    PlayEffectAtWorldPos("Hit2", jitterPos, eulerRot);
                }
				SoundManager::GetInstance().Play("Hit1");
				SoundManager::GetInstance().SetVolume("Hit1", 9000);

				++m_Combo;
				m_CurrentUltValue = std::clamp(m_CurrentUltValue + (static_cast<float>(m_Combo) * 5.f), 0.0f, m_MaxUltValue);
				SetAttackColliderActive(false);

				// 一フレーム1回.
				return;
			}
		}
	}
}

void Player::HandleDodgeDetection()
{
	if (!m_upColliders) return;

	const auto& internal_colliders = m_upColliders->GetInternalColliders();

	for (const auto& collider_ptr : internal_colliders)
	{
		const ColliderBase* current_collider = collider_ptr.get();

		if ((current_collider->GetMyMask() & eCollisionGroup::Player_JustDodge) == eCollisionGroup::None) {
			continue;
		}

		for (const CollisionInfo& info : current_collider->GetCollisionEvents())
		{
			if (!info.IsHit) continue;
			const ColliderBase* otherCollider = info.ColliderB;
			if (!otherCollider) { continue; }

			// ジャスト回避成功
			m_IsJustDodgeTiming = true;
		}
	}
}

void Player::HandleParry_SuccessDetection()
{
	if (!m_upColliders) return;

	const auto& internal_colliders = m_upColliders->GetInternalColliders();

	for (const auto& collider_ptr : internal_colliders)
	{
		const ColliderBase* current_collider = collider_ptr.get();

		if ((current_collider->GetMyMask() & eCollisionGroup::Player_Parry_Suc) == eCollisionGroup::None) {
			continue;
		}

		for (const CollisionInfo& info : current_collider->GetCollisionEvents())
		{
			if (!info.IsHit) continue;
			const ColliderBase* otherCollider = info.ColliderB;
			if (!otherCollider) { continue; }

			eCollisionGroup other_group = otherCollider->GetMyMask();
            eCollisionGroup other_target_group = otherCollider->GetTargetMask();

			if ((other_group & eCollisionGroup::Enemy_Attack) != eCollisionGroup::None
                && (other_target_group & eCollisionGroup::Player_Parry_Suc) != eCollisionGroup::None)
			{
				SoundManager::GetInstance().Play("Parry");
				SoundManager::GetInstance().SetVolume("Parry",9000);
				m_IsSuccessParry = true;
				
				// パリィ成功時のゲージ増加
				m_CurrentUltValue += 500.0f;

				// パリィ成功時のカメラ演出（シェイク）
				CameraManager::GetInstance().ShakeCamera(0.15f, 0.3f);

                ParryManager::GetInstance().OnParrySuccess(true);

            // パリィエフェクトを衝突点に出す（小さなランダムオフセットを追加）
                for(int i = 0 ; i < 3; ++i)
            {
                static thread_local std::mt19937 s_rng((std::random_device())());
                std::uniform_real_distribution<float> dist(-1.15f, 1.15f);
                std::uniform_real_distribution<float> rotDist(0.0f, DirectX::XM_2PI);
                DirectX::XMFLOAT3 jitterPos{
                    info.ContactPoint.x + dist(s_rng),
                    info.ContactPoint.y + 3.5f+ dist(s_rng),
                    info.ContactPoint.z + dist(s_rng)
                };
                DirectX::XMFLOAT3 eulerRot{ rotDist(s_rng), rotDist(s_rng), rotDist(s_rng) };
                PlayEffectAtWorldPos("Spark", jitterPos, eulerRot, 3.f);
            }

                DirectX::XMFLOAT3 pos = info.ContactPoint;
                pos.y += 8.5f;
                PlayEffectAtWorldPos("Parry_Attack", info.ContactPoint);
				// 一フレーム1回.
				return;
			}
		}
	}
}


void Player::HandleParry_FailDetection()
{
	if (!m_upColliders) return;

	const auto& internal_colliders = m_upColliders->GetInternalColliders();

	for (const auto& collider_ptr : internal_colliders)
	{
		const ColliderBase* current_collider = collider_ptr.get();

		if ((current_collider->GetMyMask() & eCollisionGroup::Player_Parry_Fai) == eCollisionGroup::None) {
			continue;
		}

		for (const CollisionInfo& info : current_collider->GetCollisionEvents())
		{
			if (!info.IsHit) continue;
			const ColliderBase* otherCollider = info.ColliderB;
			if (!otherCollider) { continue; }

			eCollisionGroup other_group = otherCollider->GetMyMask();
			eCollisionGroup other_target_group = otherCollider->GetTargetMask();

			if ((other_group & eCollisionGroup::Enemy_Attack) != eCollisionGroup::None
             && (other_target_group & eCollisionGroup::Player_Parry_Fai) != eCollisionGroup::None)
			{
				SoundManager::GetInstance().Play("Parry");
				SoundManager::GetInstance().SetVolume("Parry",7000);
				m_IsSuccessParry = true;
				
				// パリィ成功時のゲージ増加
				m_CurrentUltValue += 500.0f;

				// パリィ成功時のカメラ演出（シェイク）
				CameraManager::GetInstance().ShakeCamera(0.15f, 0.3f);
			    // Boss に通知（アニメ再生のみ）
			    ParryManager::GetInstance().OnParrySuccess(false);

                // パリィエフェクトを衝突点に出す（ランダム回転）
                {
                    static thread_local std::mt19937 s_rng((std::random_device())());
                    std::uniform_real_distribution<float> rotDist(0.0f, DirectX::XM_2PI);
                    DirectX::XMFLOAT3 eulerRot{ rotDist(s_rng), rotDist(s_rng), rotDist(s_rng) };
                    PlayEffectAtWorldPos("Spark", info.ContactPoint, eulerRot);
                }

                DirectX::XMFLOAT3 pos = info.ContactPoint;
                pos.y += 8.5f;
                PlayEffectAtWorldPos("Parry_Attack", info.ContactPoint);

			// 一フレーム1回.
			return;
			}
		}
	}
}

