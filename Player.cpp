#include "Player.h"
#include "ImGuiManager.h"
#include "MT.h"
#include "iostream"
#include <cassert>

using namespace std;

void Player::Initialize(Model* model, uint32_t textureHandle, Vector3 position) {
	assert(model);

	model_ = model;
	texturehandle_ = textureHandle;

	worldTransform_.Initialize();

	input_ = Input::GetInstance();

	// X,Y,Z方向のスケーリングを設定
	worldTransform_.scale_ = {1.0f, 1.0f, 1.0f};

	// X,Y,Z方向の回転を設定
	worldTransform_.rotation_ = {0.0f, 0.0f, 0.0f};

	// X,Y,Z方向の平行移動を設定
	worldTransform_.translation_ = position;

	// 3Dレティクルのワールドトランスフォーム初期化
	worldTransform3DReticle_.Initialize();

	// ワールドトランスフォームの初期化
	worldTransform_.Initialize();

	// レティクルの用テクスチャ取得
	uint32_t textureReticle = TextureManager::Load("reticle.png");

	// スプライト生成
	sprite2DReticle_ =
	    Sprite::Create(textureReticle, {640.0f, 360.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.5f, 0.5f});
}

void Player::Update(ViewProjection& viewProjection) {

	
	// キャラクターの移動ベクトル
	Vector3 move = {0, 0, 0};

	// キャラクターの移動速さ
	const float kCharacterSpeed = 0.2f;

	// 押した方向で移動ベクトルを変更(左右)
	if (input_->PushKey(DIK_LEFT)) {
		move.x -= kCharacterSpeed;
	} else if (input_->PushKey(DIK_RIGHT)) {
		move.x += kCharacterSpeed;
	}

	// 押した方向でベクトルを変更(上下)

	if (input_->PushKey(DIK_UP)) {
		move.y += kCharacterSpeed;
	} else if (input_->PushKey(DIK_DOWN)) {
		move.y -= kCharacterSpeed;
	}

	// 移動限界座標
	const float kMoveLimitX = 31;
	const float kMoveLimitY = 19;

	// 範囲を超えない処理
	worldTransform_.translation_.x = max(worldTransform_.translation_.x, -kMoveLimitX);
	worldTransform_.translation_.x = min(worldTransform_.translation_.x, +kMoveLimitX);
	worldTransform_.translation_.y = max(worldTransform_.translation_.y, -kMoveLimitY);
	worldTransform_.translation_.y = min(worldTransform_.translation_.y, +kMoveLimitY);

	// 座標飯銅(ベクトルの加算)
	worldTransform_.translation_.x += move.x;
	worldTransform_.translation_.y += move.y;
	worldTransform_.translation_.z += move.z;

	worldTransform_.UpdateMatrix();

	// キャラクターの座標を画面表示する処理
	ImGui::Begin("Debug1");
	float playerPos[] = {
	    worldTransform_.translation_.x, worldTransform_.translation_.y,
	    worldTransform_.translation_.z};
	ImGui::SliderFloat3("PlayerPos", playerPos, 0, 1280);

	// 処理のままだとSilderFloat3でplayerPosの値を変えているので実際の座標(translation)が
	// 変わっていないのでここで変更する
	worldTransform_.translation_.x = playerPos[0];
	worldTransform_.translation_.y = playerPos[1];
	worldTransform_.translation_.z = playerPos[2];

	ImGui::End();

	Rotate();

	// 時機のワールド座標から3Dレティクルのワールド座標を計算

	// 時機から3Dレティクルへの距離
	const float kDistancePlayerTo3DReticle = 50.0f;
	// 時機から3Dレティクルのオフセット(Z+向き)
	Vector3 offset = {0, 0, 1.0f};
	// 時機のワールド行列の回転を反映
	offset = Multiply(offset, worldTransform_.matWorld_);
	// ベクトルの長さを整える
	offset = Multiply(kDistancePlayerTo3DReticle, Normalize(offset));

	// 3Dレティクルの座標を設定
	worldTransform3DReticle_.translation_ = Add(GetWorldPosition(), offset);
	worldTransform3DReticle_.UpdateMatrix();

	// 3Dレティクルのワールド座標から2Dレティクルのスクリーン座標を計算
	Vector3 PositonReticle = Subtract(
	    {worldTransform3DReticle_.matWorld_.m[3][0], worldTransform3DReticle_.matWorld_.m[3][1],
	    worldTransform3DReticle_.matWorld_.m[3][2]}, GetWorldPosition());

	// ビューポート行列
	Matrix4x4 matViewport =
	    MakeViewPortMatrix(0, 0, WinApp::kWindowWidth, WinApp::kWindowHeight, 0, 1);

	// ビュー行列とプロジェクション行列、ビューポート行列を合成する
	Matrix4x4 matViewProjectionViewport =
	    Multiply(Multiply(viewProjection.matView, viewProjection.matProjection), matViewport);

	// ワールド→スクリーン座標交換(ここで3Dから2Dになる)
	PositonReticle = Transform(PositonReticle, matViewProjectionViewport);
	//sprite2DReticle_->SetPosition(Vector2((float)PositonReticle.x, (float)PositonReticle.y));

#pragma region

	// POINT mousePosition;
	//// マウス座標(スクリーン座標)を取得する
	// GetCursorPos(&mousePosition);

	//// クライアントエリア座標に変換する
	// HWND hwnd = WinApp::GetInstance()->GetHwnd();
	// ScreenToClient(hwnd, &mousePosition);
	//
	// sprite2DReticle_->SetPosition(Vector2((float)mousePosition.x, (float)mousePosition.y));

	// スプライトの現在座標を取得
	Vector2 spritePosition = sprite2DReticle_->GetPosition();

	// ゲームパッドの状態を得る変数
	XINPUT_STATE joyState;

	// ジョイスティック状態取得
	if (Input::GetInstance()->GetJoystickState(0, joyState)) {
		spritePosition.x += (float)joyState.Gamepad.sThumbRX / SHRT_MAX * 5.0f;
		spritePosition.y -= (float)joyState.Gamepad.sThumbRY / SHRT_MAX * 5.0f;
		// スプライトの座標変更を反映
		sprite2DReticle_->SetPosition(spritePosition);
	}

	// ビュープロジェクションビューポート合成行列
	Matrix4x4 matVPV =
	    Multiply(Multiply(viewProjection.matView, viewProjection.matProjection), matViewport);

	Matrix4x4 matInverseVPV = Inverse(matVPV);

	// スクリーン座標
	Vector3 posNear = {
	    (float)sprite2DReticle_->GetPosition().x, (float)sprite2DReticle_->GetPosition().y, 0};

	Vector3 posFar = {
	    (float)sprite2DReticle_->GetPosition().x, (float)sprite2DReticle_->GetPosition().y, 1};

	posNear = Transform(posNear, matInverseVPV);
	posFar = Transform(posFar, matInverseVPV);

	// マウスレイの方向
	 Vector3 mouseDirection = Subtract(posFar, posNear);
	 mouseDirection = Normalize(mouseDirection);

	// カメラから標準オブジェクトの距離
	 const float kDistanceTestObject = 100.0f;

	 worldTransform3DReticle_.translation_ =
	     Add(posNear, Multiply(kDistanceTestObject, mouseDirection));

	 worldTransform3DReticle_.UpdateMatrix();

	ImGui::Begin("Player");
	// ImGui::Text("2DReeticle:(%f,%f)",sprite)
	ImGui::Text("Near:(%+.2f,%+.2f,%+.2f)", posNear.x, posNear.y, posNear.z);
	ImGui::Text("Far:(%+.2f,%+.2f,%+.2f)", posFar.x, posFar.y, posFar.z);
	ImGui::Text(
	    "3Deticle:(%+.2f,%+.2f,%+.2f)", worldTransform3DReticle_.translation_.x,
	    worldTransform3DReticle_.translation_.y, worldTransform3DReticle_.translation_.z);
	ImGui::End();

#pragma endregion

#pragma region

	// XINPUT_STATE joyState;

	// ゲームパッド状態取得
	if (Input::GetInstance()->GetJoystickState(0, joyState)) {
		move.x += (float)joyState.Gamepad.sThumbLX / SHRT_MAX * kCharacterSpeed;
		move.y += (float)joyState.Gamepad.sThumbLY / SHRT_MAX * kCharacterSpeed;
	}

	worldTransform_.translation_ = Add(worldTransform_.translation_, move);
	worldTransform_.UpdateMatrix();

	


#pragma endregion
	Attack();

	for (PlayerBullet* bullet : bullets_) {
		bullet->Update();
	}

	// デスフラグの立った弾を削除
	bullets_.remove_if([](PlayerBullet* bullet) {
		if (bullet->IsDead()) {
			delete bullet;
			return true;
		}
		return false;
	});
}

// bullet_の解放
Player::~Player() {
	for (PlayerBullet* bullet : bullets_) {
		delete bullet;
	}

	delete sprite2DReticle_;
}

void Player::Draw(ViewProjection& viewProjection) {

	model_->Draw(worldTransform_, viewProjection, texturehandle_);

	for (PlayerBullet* bullet : bullets_) {
		bullet->Draw(viewProjection);
	}

	model_->Draw(worldTransform3DReticle_, viewProjection);
}

void Player::Rotate() {
	// 回転速さ[ラジアン/frame]
	const float kRotSpeed = 0.02f;

	// 押した方向で移動ベクトルを変更
	if (input_->PushKey(DIK_A)) {
		worldTransform_.rotation_.y -= kRotSpeed;
	} else if (input_->PushKey(DIK_D)) {
		worldTransform_.rotation_.y += kRotSpeed;
	}
}

void Player::Attack() {

	if (input_->TriggerKey(DIK_SPACE)) {

		// 弾の速度
		const float kBulletSpeed = 1.0f;
		Vector3 velocity(0, 0, kBulletSpeed);

		// 速度ベクトルを時機向きに合わせて回転させる
		velocity = TransformNormal(velocity, worldTransform_.matWorld_);

		velocity = Subtract(
		    {worldTransform3DReticle_.matWorld_.m[3][0], worldTransform3DReticle_.matWorld_.m[3][1],
		     worldTransform3DReticle_.matWorld_.m[3][2]},
		    GetWorldPosition());
		velocity = Multiply(kBulletSpeed, Normalize(velocity));

		// 弾を生成し、初期化
		PlayerBullet* newBullet = new PlayerBullet();
		newBullet->Initialize(model_, GetWorldPosition(), velocity);
		// 弾を登録する
		bullets_.push_back(newBullet);
	}

	XINPUT_STATE joyState;

	// ゲームパッド未接続なら何もせず抜ける
	if (!Input::GetInstance()->GetJoystickState(0, joyState)) {
		return;
	}

	// Rトリガーを押していたら
	if (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) {
		// 弾の速度
		const float kBulletSpeed = 1.0f;
		Vector3 velocity(0, 0, kBulletSpeed);

		// 速度ベクトルを時機向きに合わせて回転させる
		velocity = TransformNormal(velocity, worldTransform_.matWorld_);

		velocity = Subtract(
		    {worldTransform3DReticle_.matWorld_.m[3][0], worldTransform3DReticle_.matWorld_.m[3][1],
		     worldTransform3DReticle_.matWorld_.m[3][2]},
		    GetWorldPosition());

		velocity = Multiply(kBulletSpeed, Normalize(velocity));
		// 弾を生成し、初期化
		PlayerBullet* newBullet = new PlayerBullet();
		newBullet->Initialize(model_, GetWorldPosition(), velocity);

		// 弾を登録する
		bullets_.push_back(newBullet);
	}
}

Vector3 Player::GetWorldPosition() {
	// ワールド座標を入れる変数
	Vector3 worldPos;

	// ワールド行列の平行移動成分を取得(ワールド座標)
	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];

	return worldPos;
}

void Player::SetParent(const WorldTransform* parent) { worldTransform_.parent_ = parent; }

void Player::OnCollision() { isDead_ = true; }

void Player::DrawUI() { sprite2DReticle_->Draw(); }
