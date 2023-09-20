#include "skydome.h"
#include <cassert>

void Skydome::Initialize(Model* model) {
	assert(model);

	model_ = model;

	// X,Y,Z方向のスケーリングを設定
	worldTransform_.scale_ = {500.0f, 500.0f, 500.0f};

	// X,Y,Z方向の回転を設定
	worldTransform_.rotation_ = {0.0f, 0.0f, 0.0f};

	//// X,Y,Z方向の平行移動を設定
	worldTransform_.translation_ = {0.0f, 0.0f, 0.0f};

	// ワールドトランスフォームの初期化
	worldTransform_.Initialize();
}

void Skydome::Update() { worldTransform_.UpdateMatrix(); }

void Skydome::Draw(const ViewProjection& viewProjection) {
	model_->Draw(worldTransform_, viewProjection);
}
