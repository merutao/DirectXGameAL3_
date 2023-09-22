#include "RailCamera.h"
#include "ImGuiManager.h"

/// <summary>
/// 初期化
/// </summary>
void RailCamera::Initialize(Vector3 position, Vector3 rotation) {
	

	//// X,Y,Z方向の平行移動を設定
	worldTransform_.translation_ = position;

	// X,Y,Z方向の回転を設定
	worldTransform_.rotation_ = rotation;

	// ワールドトランスフォームの初期化
	viewProjection_.Initialize();
};

/// <summary>
/// 更新処理
/// </summary>
void RailCamera::Update() {

	// キャラクターの移動ベクトル

	/*worldTransform_.translation_.x += move.x;
	worldTransform_.translation_.y += move.y;*/
	worldTransform_.translation_.z += 0.1f;

	/*worldTransform_.rotation_.x += move.x;
	worldTransform_.rotation_.y += move.y;*/
	// worldTransform_.rotation_.z += move.z;

	worldTransform_.matWorld_ = MakeAffineMatrix(
	    {1.0f, 1.0f, 1.0f}, worldTransform_.rotation_, worldTransform_.translation_);

	viewProjection_.matView = Inverse(worldTransform_.matWorld_);

	// カメラの座標を画面表示する処理
	/*ImGui::Begin("Camera");
	float CameraPos[] = {
	    worldTransform_.translation_.x, worldTransform_.translation_.y,
	    worldTransform_.translation_.z};

	ImGui::SliderFloat3("PlayerPos", CameraPos, 0, 1280);

	float CameraRot[] = {
	    worldTransform_.rotation_.x, worldTransform_.rotation_.y, worldTransform_.rotation_.z};

	ImGui::SliderFloat3("PlayerPos", CameraRot, 0, 1280);

	ImGui::End();*/
};
