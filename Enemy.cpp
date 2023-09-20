#include "Enemy.h"
//#include "Player.h"
#include "GameScene.h"

Enemy::~Enemy() {

}

void Enemy::Initialize(Model* model, Vector3& position, const Vector3& velocity) {
	assert(model);

	model_ = model;
	texturehandle_ = TextureManager::Load("tex1.png");

	// ワールドトランスフォームの初期化
	worldTransform_.Initialize();
	// 引数で受け取った初期座標をセット
	worldTransform_.translation_ = position;
	// 引数で受け取った速度をメンバ変数に代入
	velocity_ = velocity;
	//接近フェーズ初期化
}

void Enemy::Update() {

	worldTransform_.UpdateMatrix();

	switch (phase_) {
	case Phase::Approach:
	default:

		Approach();

		break;
	case Phase::Leave:

		Leave();

		break;
	}

	ANIT();

};

void Enemy::Draw(ViewProjection& viewProjection) {
	model_->Draw(worldTransform_, viewProjection, texturehandle_);
};

void Enemy::Approach() {
	// 移動(ベクトルを加算)
	worldTransform_.translation_.z += velocity_.z;

	if (worldTransform_.translation_.z < -0.0f) {
		phase_ = Phase::Leave;
	}
}

void Enemy::Leave() {
	// 移動(ベクトルを加算)
	worldTransform_.translation_.x -= velocity_.x;
	worldTransform_.translation_.y += velocity_.y;
	worldTransform_.translation_.z += velocity_.z;
}


void Enemy::Fire() {
	assert(player_);

	// 弾の速度
	const float kBulletSpeed = 0.8f;
	Vector3 velocity(0, 0, -kBulletSpeed);

	//自キャラの座標取得
	player_->GetWorldPosition();
	//敵チャラの座標取得
	GetWorldPosition();
	//敵キャラ→自キャラの差分ベクトルを求める
	DifferentialVector = {
	    player_->GetWorldPosition().x -GetWorldPosition().x,
	    player_->GetWorldPosition().y -GetWorldPosition().y,
	    player_->GetWorldPosition().z -GetWorldPosition().z
	};

	velocity = Normalize(DifferentialVector);

	// 弾を生成し、初期化
	EnemyBullet* newBullet = new EnemyBullet();
	newBullet->Initialize(model_, worldTransform_.translation_, velocity);

	gameScene_->AddEnemyBullet(newBullet);
	
} 

void Enemy::ANIT() {
	//発射タイマーカウントダウン
	timer_--;
	//指定時間に達した
	if (timer_ <= 0) {
		// 弾の発射
		Fire();
		//発射タイマーを初期化
		timer_ = kFireInterval;
	}
}

Vector3 Enemy::GetWorldPosition() {
	Vector3 worldPos;
	// ワールド行列の平行移動成分を取得(ワールド座標)
	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];

	return worldPos;
}

void Enemy::OnCollision() { 
  	isDead_ = true;
}


