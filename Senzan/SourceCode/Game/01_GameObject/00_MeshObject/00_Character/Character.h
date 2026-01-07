#pragma once
#include "Game/01_GameObject/00_MeshObject/MeshObject.h"
#include <vector>
#include <unordered_map>
#include <cstdint>
#include <string>

#include "Game/03_Collision/00_Core/ColliderBase.h"
#include "Game/03_Collision/00_Core/ColliderSpec.h"

class CompositeCollider;
class ColliderBase;

/**********************************************************************************
* @author    : 淵脇 未来.
* @date      : 2026/01/03.
* @brief     : Character 基底クラス. コライダー管理と衝突応答の共通実装を提供する.
**********************************************************************************/

class Character
	: public MeshObject
{
public:
	// 攻撃タイプは派生クラスごとに定義できるように、基底はID型のみ公開する。
	using AttackTypeId = uint8_t;

	Character();
	virtual ~Character();

	virtual void Update() override;
	virtual void LateUpdate() override;
	virtual void Draw() override;

	inline float GetMaxHP() const noexcept { return m_MaxHP; }
	inline float GetHP() const noexcept { return m_HP; }

	/**********************************************************
	* @brief                : 指定したコライダーを遅延付きで有効化スケジュールする.
	* @param[in] type       : カテゴリ(派生側で意味づけする).
	* @param[in] index      : カテゴリ内のコライダーインデックス.
	* @param[in] delay      : 有効化までの遅延秒.
	* @param[in] duration   : 有効化継続時間（秒）.
	**********************************************************/
	void ScheduleCollider(AttackTypeId type, size_t index, float delay, float duration);

	/**********************************************************
	* @brief                : 指定したスケジュールをキャンセルする.
	* @param[in] type       : カテゴリ.
	* @param[in] index      : コライダーインデックス.
	**********************************************************/
	void CancelScheduledCollider(AttackTypeId type, size_t index);

	/**********************************************************
	* @brief                : 指定カテゴリの全コライダーを即時有効/無効化する.
	* @param[in] type       : カテゴリ.
	* @param[in] active     : true=有効化, false=無効化.
	**********************************************************/
	void SetCollidersActive(AttackTypeId type, bool active);

	/**********************************************************
	* @brief                : 指定カテゴリの単一コライダーを即時有効/無効化する.
	* @param[in] type       : カテゴリ.
	* @param[in] index      : コライダーインデックス.
	* @param[in] active     : true=有効化, false=無効化.
	**********************************************************/
	void SetColliderActive(AttackTypeId type, size_t index, bool active);

	// 外部ツールからコライダー定義を追加するための公開ラッパー
	void AddCollidersFromDefsPublic(const std::unordered_map<AttackTypeId, std::vector<struct ColliderSpec>>& defs)
	{
		// 保守性のため protected 実装へフォワード
		CreateCollidersFromDefs(defs);
	}

	// 指定カテゴリのコライダー数を取得
	size_t GetColliderCount(AttackTypeId type) const;

	// -----------------------------------------------------------------
	// 旧 API 互換ラッパー (既存コードの呼び出しを壊さないため)
	// -----------------------------------------------------------------
	// 既存コードは ScheduleAttackCollider 等を呼んでいるため、新 API へ転送する。
	inline void ScheduleAttackCollider(AttackTypeId type, size_t index, float delay, float duration) { ScheduleCollider(type, index, delay, duration); }
	inline void CancelScheduledAttackCollider(AttackTypeId type, size_t index) { CancelScheduledCollider(type, index); }
	inline void SetAttackCollidersActive(AttackTypeId type, bool active) { SetCollidersActive(type, active); }
	inline void SetAttackColliderActive(AttackTypeId type, size_t index, bool active) { SetColliderActive(type, index, active); }
	inline void AddAttackCollidersFromDefsPublic(const std::unordered_map<AttackTypeId, std::vector<struct ColliderSpec>>& defs) { CreateCollidersFromDefs(defs); }
	inline size_t GetAttackColliderCount(AttackTypeId type) const { return GetColliderCount(type); }
	// -----------------------------------------------------------------

	// ボーン名からワールド姿勢を取得するヘルパー
	bool GetBoneWorldPose(const std::string& name, DirectX::XMFLOAT3& outPos, DirectX::XMFLOAT3& outForward) const;

	// コライダーのワールド位置を直接反映するヘルパー
	void SetColliderTransform(AttackTypeId type, size_t index, const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& forward);

	// 内部コライダー取得ヘルパー.
	ColliderBase* GetColliderByIndex(AttackTypeId type, size_t index) const;

protected:

	// 衝突応答処理.
	void HandleCollisionResponse();

	// ダメージを適用する.
	void ApplyDamage(float damageAmount);

	// 衝突_被ダメージ.
	virtual void HandleDamageDetection() = 0;
	// 衝突_攻撃判定.
	virtual void HandleAttackDetection() = 0;
	// 衝突_回避.
	virtual void HandleDodgeDetection() = 0;

	// スケジュールされたコライダーの状態を更新する(毎フレーム呼び出し).
	void UpdateScheduledColliders();

	// スケジュール情報.
	struct ScheduledCollider
	{
        AttackTypeId Type;  // カテゴリID.
		size_t Index;     // コライダーインデックス.
		float Delay;      // 遅延秒.
		float Duration;   // 有効時間.
		float Elapsed;    // 経過時間.
		bool Active;      // 現在有効か.
	};

protected:
	// コライダー定義からコライダーを生成する.
	void CreateCollidersFromDefs(const std::unordered_map<AttackTypeId, std::vector<struct ColliderSpec>>& defs);

protected:
	std::unique_ptr<CompositeCollider>	m_upColliders; 	// 衝突コンテナ.

	float m_MaxHP;      // 最大HP.
	float m_HP;         // 現在HP.

	std::vector<ScheduledCollider> m_ScheduledColliders; // スケジュール一覧.
	// 派生クラスが利用できるカテゴリ分けされたコライダーポインタ.
	std::unordered_map<AttackTypeId, std::vector<ColliderBase*>> m_Colliders;   // 担当: 淵脇 未来.
};

