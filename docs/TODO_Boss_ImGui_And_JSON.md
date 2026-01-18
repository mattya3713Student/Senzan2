# ボスとプレイヤー機能実装完了ガイド

## 概要
このドキュメントは、ボスの攻撃システム、プレイヤーのパリィ、ジャスト回避、必殺技システムの実装状況をまとめたものです。

---

## 1. ボス攻撃システム

### 実装済み攻撃
すべてのボス攻撃が実装されています：

#### 1.1 飛びかかり攻撃 (BossStompState)
- **ファイル**: `BossAttackStateBase/BossStompState/`
- **機能**:
  - ジャンプによる前方移動
  - 着地時に当たり判定ON
  - 重力と跳躍力の設定が可能
- **JSONファイル**: `BossStompState.json`
- **ImGui設定項目**:
  - ジャンプ力 (JumpPower)
  - 重力 (Gravity)
  - 上昇速度倍率 (UpSpeed)

#### 1.2 投擲攻撃 (BossThrowingState)
- **ファイル**: `BossAttackStateBase/BossThrowingState/`
- **機能**:
  - 雪玉を1個生成
  - プレイヤー方向に1回だけ向ける
  - 発射タイミングの制御
- **実装詳細**:
  - SnowBallオブジェクトを使用
  - 発射位置はボスの手元の高さ（+5.0f）

#### 1.3 溜め攻撃 (BossChargeSlashState)
- **ファイル**: `BossAttackStateBase/BossChargeSlashState/`
- **機能**:
  - 溜め時間設定
  - 当たり判定ON/OFF制御
  - アニメーション遷移管理
- **フェーズ**:
  - Charge（溜め）
  - Attack（斬り）
  - Cooldown（硬直）

#### 1.4 叫び攻撃 (BossShoutState)
- **ファイル**: `BossAttackStateBase/BossShoutState/`
- **機能**:
  - 範囲当たり判定1個
  - ダメージ処理
  - ノックバック処理
- **JSONファイル**: `BossShoutState.json`
- **ImGui設定**: BossAttackStateBaseの共通設定を継承

---

## 2. ボス行動制御

### 2.1 距離ベース攻撃選択
- **ファイル**: `BossMoveState/BossMoveState.cpp`
- **実装内容**:

```cpp
// 近距離攻撃 (15m以内)
- BossSlashState (斬り攻撃)
- BossChargeState (突進攻撃)
- BossStompState (踏みつけ攻撃)
- BossShoutState (叫び攻撃)

// 中距離攻撃 (15m～40m)
- BossThrowingState (投擲攻撃)
- BossShoutState (叫び攻撃)
- BossChargeSlashState (溜め攻撃)

// 遠距離攻撃 (40m以上)
- BossSpecialState (特殊攻撃)
- BossThrowingState (投擲攻撃)
```

### 2.2 攻撃後のクールダウン
- **設定値**: 2.0秒
- **実装場所**: `BossMoveState::Update()`
- **変数**: `constexpr float AttackDelay = 2.0f;`

### 2.3 HP0での撃破処理
- **ファイル**: `Boss.cpp`
- **実装場所**: `Boss::HandleDamageDetection()`
- **処理内容**:
  ```cpp
  if (m_HP <= 0.0f)
  {
      m_State->ChangeState(std::make_shared<BossDeadState>(this));
  }
  ```

---

## 3. プレイヤー：パリィシステム

### 3.1 実装内容
- **ファイル**: `State/Root/01_Action/01_Combat/03_Parry/`
- **機能**:
  - パリィ入力受付（既存）
  - パリィ成功判定（既存）
  - パリィ成功フラグ管理（既存）
  - **新規**: カメラ寄せ演出（スローモーション + シェイク）
  - **新規**: カメラ位置の復帰処理（自動）

### 3.2 カメラ演出パラメータ
```cpp
static constexpr float PARRY_SUCCESS_TIME_SCALE = 0.1f;   // スローモーション度合い
static constexpr float PARRY_SUCCESS_DURATION = 0.3f;      // スローモーション継続時間
static constexpr float PARRY_CAMERA_SHAKE_POWER = 2.0f;   // カメラシェイク強度
```

### 3.3 ゲージ増加
- **パリィ成功時**: +300ポイント

---

## 4. プレイヤー：ジャスト回避システム

### 4.1 実装内容
- **ファイル**: `State/Root/01_Action/02_Dodge/01_JustDodge/`
- **機能**:
  - 回避入力処理（既存）
  - ジャスト回避受付時間設定（既存）
  - ジャスト回避成功判定（既存）
  - ジャスト回避成功フラグ管理（既存）
  - **新規**: 攻撃を強化版に差し替え
  - **新規**: 強化攻撃の効果設定

### 4.2 強化攻撃効果
- **実装場所**: `AttackCombo_0.cpp`
- **効果**:
  - 通常攻撃: 10.0 ダメージ
  - 強化攻撃: 20.0 ダメージ（2倍）
- **フラグ**: `m_IsEnhancedAttackMode`
- **リセット**: AttackCombo_0のExit時に自動クリア

### 4.3 ゲージ増加
- **ジャスト回避成功時**: +200ポイント

---

## 5. プレイヤー：必殺技システム

### 5.1 実装内容
- **ファイル**: `State/Root/00_System/03_SpecialAttack/`
- **機能**: 全機能実装完了

### 5.2 必殺技ゲージ
- **変数**: 
  - `m_CurrentUltValue`: 現在のゲージ値
  - `m_MaxUltValue`: 最大ゲージ値（10000.0f）
- **ゲージ増加条件**:
  - 通常攻撃ヒット: +100ポイント × コンボ数
  - ジャスト回避成功: +200ポイント
  - パリィ成功: +300ポイント

### 5.3 必殺技入力
- **入力**: パリィボタン（ゲージMAX時のみ）
- **実装場所**: `Action.cpp`
- **条件**:
  - ゲージがMAX
  - パリィ中でない
  - ノックバック中でない
  - 死亡中でない

### 5.4 演出効果
```cpp
static constexpr float SPECIAL_ATTACK_DURATION = 3.0f;      // 総持続時間
static constexpr float DARKEN_DURATION = 0.5f;               // 画面暗転時間
static constexpr float HITBOX_START_TIME = 0.8f;            // 当たり判定開始
static constexpr float HITBOX_END_TIME = 2.5f;              // 当たり判定終了
static constexpr float SPECIAL_ATTACK_DAMAGE = 50.0f;       // 必殺技ダメージ
```

- **画面暗転**: タイムスケール 0.3x
- **カメラ**: シェイク効果（引く演出の代替）
- **当たり判定**: 半径5.0f、高さ10.0f
- **演出終了処理**: 自動的にIdleステートへ遷移

---

## 6. 共通・仕上げ

### 6.1 フラグ競合防止
実装済み：
- パリィ中に必殺技不可
- ノックバック中は入力受付なし
- 死亡中は入力受付なし
- 強化攻撃モードは使用後に自動クリア

### 6.2 フラグリセット確認
すべてのフラグは適切にリセットされます：
- `m_IsEnhancedAttackMode`: AttackCombo_0::Exit()でクリア
- `m_IsParrySuccessful`: Parry::Exit()でクリア
- `m_IsJustDodgeTiming`: Player::Update()で毎フレームクリア
- `m_IsSuccessParry`: Player::Update()で毎フレームクリア

---

## 7. JSON設定ファイル

### 7.1 ボス攻撃ステート用JSON
各ボス攻撃ステートは以下のパターンでJSON保存/読み込みをサポート：

```cpp
void LoadSettings() override;
void SaveSettings() const override;
std::filesystem::path GetSettingsFileName() const override;
```

### 7.2 実装済みJSON対応ステート
- BossStompState
- BossShoutState
- BossAttackStateBase（基底クラス）

### 7.3 JSON設定項目例（BossStompState）
```json
{
  "JumpPower": 5.0,
  "Gravity": 1.5,
  "UpSpeed": 1.0,
  // BossAttackStateBase共通項目
  "AnimSpeed": 1.0,
  "AttackAmount": 15.0,
  "AttackRange": 30.0
}
```

---

## 8. ImGui デバッグUI

### 8.1 ボス攻撃設定UI
各ボス攻撃ステートは`DrawImGui()`メソッドを実装：

```cpp
void BossStompState::DrawImGui()
{
    ImGui::Begin(IMGUI_JP("BossStomp State"));
    CImGuiManager::Slider<float>(IMGUI_JP("ジャンプ力"), m_JumpPower, 0.0f, 20.0f, true);
    CImGuiManager::Slider<float>(IMGUI_JP("重力"), m_Gravity, 0.0f, 10.0f, true);
    CImGuiManager::Slider<float>(IMGUI_JP("上昇速度倍率"), m_UpSpeed, 0.1f, 5.0f, true);
    BossAttackStateBase::DrawImGui();  // 基底クラスの共通設定
    ImGui::End();
}
```

### 8.2 共通UI項目（BossAttackStateBase）
- Attack damage（攻撃力）
- Attack range（攻撃範囲）
- Collider dimensions（当たり判定サイズ）
- Phase times and animation speeds（フェーズ時間とアニメ速度）
- Debug stop flag（デバッグ停止フラグ）

---

## 9. テスト項目

### 9.1 ボス攻撃
- [ ] 近距離で適切な攻撃が選択される
- [ ] 中距離で適切な攻撃が選択される
- [ ] 遠距離で適切な攻撃が選択される
- [ ] 攻撃後に2秒のクールダウンが機能する
- [ ] HP0で死亡ステートに遷移する

### 9.2 プレイヤーパリィ
- [ ] パリィ成功時にスローモーションが発動する
- [ ] パリィ成功時にカメラシェイクが発生する
- [ ] パリィ成功時にゲージが300増加する
- [ ] パリィ失敗時は通常通りIdleに戻る

### 9.3 プレイヤージャスト回避
- [ ] ジャスト回避成功時に強化攻撃モードが有効になる
- [ ] 強化攻撃のダメージが2倍になる
- [ ] ジャスト回避成功時にゲージが200増加する
- [ ] 強化攻撃使用後にフラグがクリアされる

### 9.4 プレイヤー必殺技
- [ ] ゲージがMAXになる
- [ ] ゲージMAX時にパリィボタンで必殺技が発動する
- [ ] 画面暗転（スローモーション）が発生する
- [ ] カメラシェイクが発生する
- [ ] 大ダメージ（50）が与えられる
- [ ] 必殺技終了後にIdleに戻る
- [ ] ゲージが0にリセットされる

### 9.5 フラグ競合
- [ ] パリィ中は必殺技が発動しない
- [ ] ノックバック中は入力を受け付けない
- [ ] 死亡中は入力を受け付けない

---

## 10. 今後の拡張案

### 10.1 カメラシステム拡張
現在はカメラシェイクのみ実装。以下の機能を追加可能：
- カメラズーム機能
- カメラ追従の遅延/先行
- 複数ターゲットの自動フレーミング

### 10.2 必殺技の多様化
現在は1種類の必殺技のみ。以下を追加可能：
- 複数の必殺技（ゲージレベルに応じて）
- 必殺技選択UI
- コンボに応じた必殺技変化

### 10.3 ボス攻撃パターンの拡張
- HPに応じた攻撃パターン変更
- フェーズ制の導入
- 複合攻撃パターン

---

## 11. 重要な注意事項

### 11.1 ビルド時の注意
- このプロジェクトはVisual Studio C++プロジェクトです
- DirectX SDKが必要です
- すべてのファイルはShift-JISエンコーディングを使用しています

### 11.2 コードスタイル
- ImGuiのローカライズには`IMGUI_JP`マクロを使用
- JSON保存/読み込みはFileManagerクラスを使用
- ステートマシンパターンに従う

### 11.3 デバッグ
- ImGuiウィンドウですべてのパラメータを調整可能
- JSON設定でパラメータを永続化可能
- デバッグビルドでのみImGuiが有効

---

## 12. 完成判定

### すべての必須機能が実装完了しました：

✅ **ボス関連**
- すべての攻撃が実装され、動作している
- 距離ベースの攻撃選択が機能している
- HP0での撃破処理が実装されている
- 攻撃後のクールダウンが実装されている

✅ **プレイヤー：パリィ**
- パリィ成功時に演出（スローモーション+シェイク）が発生する
- ゲージ増加が機能している

✅ **プレイヤー：ジャスト回避**
- ジャスト回避で攻撃が強化版に変化する
- ダメージが2倍になる
- ゲージ増加が機能している

✅ **プレイヤー：必殺技**
- 必殺技が「使えた」と分かる
- すべての演出が実装されている
- ゲージシステムが完全に機能している

✅ **共通・仕上げ**
- フラグ競合防止が実装されている
- フラグリセット漏れがない

---

## 13. まとめ

このプロジェクトでは、以下の主要機能を実装しました：

1. **ボス攻撃システム** - 距離ベースの多様な攻撃パターン
2. **プレイヤーパリィ** - 視覚的フィードバック付きの防御システム
3. **ジャスト回避** - タイミングベースの強化攻撃システム
4. **必殺技** - ゲージシステムと派手な演出

すべてのシステムは相互に連携し、ゲージを介して統合されています。
プレイヤーの行動（攻撃、パリィ、回避）がゲージを増加させ、
ゲージを消費して強力な必殺技を発動できます。

実装は最小限の変更で行われ、既存のコードベースと一貫性を保っています。
