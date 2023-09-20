#pragma once
#include "EnemyBullet.h"
#include "Input.h"
#include "Model.h"
#include "WorldTransform.h"
#include <list>

//時機クラスの前方宣言
class Player;

//GameSceneの前方宣言(苦肉の策)
class GameScene;

/// <summary>
/// 敵
/// </summary>
class Enemy {
public:
	~Enemy();

	void Initialize(Model* model, Vector3& position, const Vector3& velocity);

	void Update();

	void Draw(ViewProjection& viewProjection);

	void SetPlayer(Player* player) { player_ = player; }

	void OnCollision();

	void SetGameScene(GameScene* gameScene) { gameScene_ = gameScene; }

	bool IsDead() const { return isDead_; }

public:
	// 発射間隔
	static const int kFireInterval = 60;
	// 接近フェーズ初期化
	void ANIT();

	Vector3 GetWorldPosition();
	Player* player_ = nullptr;
	Vector3 DifferentialVector;
	Vector3 resultNomalize;
	//const std::list<EnemyBullet*>& GetBullet() const { return enemyBullets_; }
	float radius = 3;
	// ゲームシーン
	GameScene* gameScene_ = nullptr;

private:
	void Approach();

	void Leave();

	void Fire();

private:
	// 行動フェーズ
	enum class Phase {
		Approach, // 接近する
		Leave,    // 離脱する
	};

	WorldTransform worldTransform_;
	Model* model_ = nullptr;
	uint32_t texturehandle_ = 0u;
	// 速度
	Vector3 velocity_;
	// フェーズ
	Phase phase_ = Phase::Approach;
	EnemyBullet* enemyBullet_ = nullptr;
	// 発射タイマー
	int32_t timer_ = 0;
	// デスフラグ
	bool isDead_ = false;

};
