#include "GameScene.h"
#include "AxisIndicator.h"
#include "TextureManager.h"
#include <cassert>
#include <fstream>


GameScene::GameScene() {}

GameScene::~GameScene() {
	delete model_, delete player_, delete debugCamera_, delete modelSkydome_;

	for (EnemyBullet* bullet : enemyBullets_) {
		delete bullet;
	}

	for (Enemy* enemy : enemys_) {
		delete enemy;
	}

}

void GameScene::Initialize() {

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();

	// テクスチャ

	
	// モデル
	textureHandle_ = TextureManager::Load("uvChecker.png");
	
	model_ = Model::Create();

	// ビュープロジェクション
	viewProjection_.farZ = 600;
	viewProjection_.Initialize();

	// 3Dモデルの生成
	modelSkydome_ = Model::CreateFromOBJ("skydome", true);

	//
	

	// レティクルのテクスチャ
	TextureManager::Load("reticle.png");

	// 敵の速度
	const float kEnemySpeed = 0.1f;
	// 敵の移動
	Vector3 velocity(kEnemySpeed, kEnemySpeed, -kEnemySpeed);
	// 敵のポジション
	Vector3 EnemyPosition = {8.0f, 1.0f, 40.0f};

	// 自キャラの生成
	player_ = new Player();

	Time = 1300;
	TFlag = 0;
	Count = 5;

	Vector3 playerPosition(0, 0, 50);
	// 自キャラの初期化
	player_->Initialize(model_, textureHandle_, playerPosition);

	// レイルカメラの生成
	railCamera_ = new RailCamera();
	// レイルカメラの初期化
	railCamera_->Initialize({0.0f,0.0f,0.0f}, {0.0f, 0.0f, 0.0f});

	// 天球の生成
	skydome_ = new Skydome;
	// 天球の初期化
	skydome_->Initialize(modelSkydome_);

	// デバックカメラの生成
	debugCamera_ = new DebugCamera(1280, 720);

	// 自キャラとレールカメラの親子関係を結ぶ
	player_->SetParent(&railCamera_->GetWorldTransform());

	LoadEnemyPopData();
}

void GameScene::Update() {

	// デバッグカメラの更新
	debugCamera_->Update();

	// レールカメラの更新
	railCamera_->Update();

	UpdateEnemyPopCommand();

	// 自キャラの更新
	player_->Update(viewProjection_);

	for (Enemy* enemy : enemys_) {
		enemy->Update();
	}
	// 天球の更新
	skydome_->Update();

	for (EnemyBullet* bullet : enemyBullets_) {
		bullet->Update();
	}

#ifdef _DEBUG
	if (input_->TriggerKey(DIK_1) && isDebugCameraActive_ == 0) {
		isDebugCameraActive_ = 1;
	} else if (input_->TriggerKey(DIK_1) && isDebugCameraActive_ == 1) {
		isDebugCameraActive_ = 0;
	}
#endif

	// カメラの処理
	if (isDebugCameraActive_ == 1) {
		// デバックカメラの更新
		debugCamera_->Update();
		viewProjection_.matView = debugCamera_->GetViewProjection().matView;
		viewProjection_.matProjection = debugCamera_->GetViewProjection().matProjection;

		// ビュープロジェクション行列の転送
		viewProjection_.TransferMatrix();
	} else {
		viewProjection_.matView = railCamera_->GetViewProjection().matView;
		viewProjection_.matProjection = railCamera_->GetViewProjection().matProjection;
		// ビュープロジェクション行列の更新と転送
		viewProjection_.TransferMatrix();
	}

	// 軸方向の表示を有効にする
	AxisIndicator::GetInstance()->SetVisible(true);
	// 軸方向表示が参照するビュープロジェクションを指定する(アドレス渡し)
	AxisIndicator::GetInstance()->SetTargetViewProjection(&viewProjection_);

	CheckAllCollision();

	// デスフラグの立った弾を削除
	enemyBullets_.remove_if([](EnemyBullet* bullet) {
		if (bullet->IsDead()) {
			delete bullet;
			return true;
		}
		return false;
	});

	// デスフラグの立った敵を削除
	enemys_.remove_if([](Enemy* enemy) {
		if (enemy->IsDead()) {
			delete enemy;
			return true;
		}
		return false;
	});
}

void GameScene::Draw() {

	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

#pragma region 背景スプライト描画

	// 背景スプライト描画前処理
	Sprite::PreDraw(commandList);
	/// <summary>
	/// ここに背景スプライトの描画処理を追加できる
	/// </summary>
	
	// スプライト描画後処理
	Sprite::PostDraw();
	// 深度バッファクリア
	dxCommon_->ClearDepthBuffer();
#pragma endregion

#pragma region 3Dオブジェクト描画
	// 3Dオブジェクト描画前処理
	Model::PreDraw(commandList);

	/// <summary>
	/// ここに3Dオブジェクトの描画処理を追加できる
	/// </summary>

	// title_->Draw(viewProjection_);

	player_->Draw(viewProjection_);

	// 敵の描画
	for (Enemy* enemy : enemys_) {
		enemy->Draw(viewProjection_);
	}

	// 天球の描画
	skydome_->Draw(viewProjection_);

	for (EnemyBullet* bullet : enemyBullets_) {
		bullet->Draw(viewProjection_);
	}

	// 3Dオブジェクト描画後処理
	Model::PostDraw();
#pragma endregion

#pragma region 前景スプライト描画

	// 前景スプライト描画前処理
	Sprite::PreDraw(commandList);
	

	player_->DrawUI();

	
	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}

void GameScene::CheckAllCollision() {
	// 判定対象AとBの座標
	Vector3 posA, posB;
	// 自弾リストを取得
	const std::list<PlayerBullet*>& playerBullets = player_->GetBullets();
	// 敵リストの取得
	// const std::list<Enemy*>& enemys = enemy_->SetGameScene();

#pragma region
	// 自キャラと敵弾の当たり判定

	// 自キャラの座標
	posA = player_->GetWorldPosition();

	// 自キャラと敵弾全ての当たり判定
	for (EnemyBullet* bullet : enemyBullets_) {
		// 敵弾の座標
		posB = bullet->GetWorldPosition();

		float Hit = (posA.x - posB.x) * (posA.x - posB.x) + (posA.y - posB.y) * (posA.y - posB.y) +
		            (posA.z - posB.z) * (posA.z - posB.z);

		float Radius = (player_->radius + bullet->radius) * (player_->radius + bullet->radius);

		if (Hit <= Radius) {
			// 自キャラの衝突自コールバックを呼び出す
			player_->OnCollision();
			// 敵弾の衝突時コールバックを呼び出す
			bullet->OnCollision();
			Count -= 1;
			if (Count <= 0)
			{
				Over();
			}
		}
	}

#pragma endregion

#pragma region
	// 自弾と敵キャラの当たり判定

	for (PlayerBullet* bullet : playerBullets) {
		for (Enemy* enemy : enemys_) {
			// 敵の判定
			posA = enemy->GetWorldPosition();
			// 自弾の座標
			posB = bullet->GetWorldPosition();

			float Hit = (posA.x - posB.x) * (posA.x - posB.x) +
			            (posA.y - posB.y) * (posA.y - posB.y) +
			            (posA.z - posB.z) * (posA.z - posB.z);

			float Radius = (bullet->radius + enemy->radius) * (bullet->radius + enemy->radius);

			if (Hit <= Radius) {
				// 敵の衝突自コールバックを呼び出す
				enemy->OnCollision();
				// 自弾の衝突時コールバックを呼び出す
				bullet->OnCollision();
			}
		}
	}

#pragma endregion

#pragma
	// 自弾と敵弾の当たり判定

	// 敵の判定
	// posA = enemy_->GetWorldPosition();

	for (PlayerBullet* playerBullet : playerBullets) {
		posA = playerBullet->GetWorldPosition();
		for (EnemyBullet* enemyBullet : enemyBullets_) {
			// 自弾の座標
			posB = enemyBullet->GetWorldPosition();

			float Hit = (posA.x - posB.x) * (posA.x - posB.x) +
			            (posA.y - posB.y) * (posA.y - posB.y) +
			            (posA.z - posB.z) * (posA.z - posB.z);

			float Radius = (playerBullet->radius + enemyBullet->radius) *
			               (playerBullet->radius + enemyBullet->radius);

			if (Hit <= Radius) {
				// 自弾の衝突自コールバックを呼び出す
				playerBullet->OnCollision();
				// 敵弾の衝突時コールバックを呼び出す
				enemyBullet->OnCollision();
			}
		}
	}

#pragma endregion
}

void GameScene::AddEnemyBullet(EnemyBullet* enemyBullet) {

	// 弾
	// std::list<EnemyBullet*> enemyBullets_;

	// リストに登録する
	enemyBullets_.push_back(enemyBullet);
}

void GameScene::LoadEnemyPopData() {
	// ファイルを開く
	std::ifstream file;
	file.open("Resources/enemyPop.csv");
	assert(file.is_open());

	// ファイルの内容を文字列ストリームにコピー
	enemyPopCommands << file.rdbuf();

	// ファイルを閉じる
	file.close();
}

void GameScene::UpdateEnemyPopCommand() {

	// 待機処理
	if (standFlag) {
		standTime--;
		if (standTime <= 0) {
			// 待機完了
			standFlag = false;
		}
		return;
	}

	// 1行分の文字列を入れる変数
	std::string line;

	// コマンド実行ループ
	while (getline(enemyPopCommands, line)) {
		// １行分の文字列をストリームに変換して解析しやすくする
		std::istringstream line_stream(line);

		std::string word;
		//,区切りで行の先頭文字列を取得
		getline(line_stream, word, ',');

		//"//"から始まる行はコメント
		if (word.find("//") == 0) {
			// コメント行を飛ばす
			continue;
		}

		// POPコマンド
		if (word.find("POP") == 0) {
			// x座標
			getline(line_stream, word, ',');
			float x = (float)std::atof(word.c_str());

			// y座標
			getline(line_stream, word, ',');
			float y = (float)std::atof(word.c_str());

			// z座標
			getline(line_stream, word, ',');
			float z = (float)std::atof(word.c_str());

			// 敵を発生させる
			EnemyOccurrence(Vector3(x, y, z), {0, 0, 0});

		} // WAITコマンド
		else if (word.find("WAIT") == 0) {
			getline(line_stream, word, ',');

			// 待ち時間
			int32_t waitTime = atoi(word.c_str());

			// 待機時間
			standFlag = true;
			standTime = waitTime;

			// コマンドループを抜ける
			break; // 重要。なぜ重要か考えてみよう
		}
	}
}

void GameScene::EnemyOccurrence(Vector3 position, Vector3 velocity) {
	Enemy* enemy = new Enemy();

	// 初期化
	enemy->Initialize(model_, position, velocity);
	// 敵キャラに自キャラのアドレスを渡す
	enemy->SetPlayer(player_);
	// 敵キャラにゲームシーン渡す
	enemy->SetGameScene(this);
	// リストに登録
	enemys_.push_back(enemy);
}






void GameScene::Timer() { 

	if (TFlag == 0) {
		Time--;
	}
	if (Time <= 0) {
		TFlag = 1;
	}
	if (TFlag == 1)
	{
		Clear();
	}
}

void GameScene::Clear() {

	NowGameScene = NowGameScene::Clear;
	
}

void GameScene::Over()
{ 
	NowGameScene = NowGameScene::Over;
	
}

void GameScene::TitleProc() { 
	if (input_->TriggerKey(DIK_SPACE)) {
	
		NowGameScene = NowGameScene::Play;

		return;
	}
}

void GameScene::ClearProc()
{
	
	if (input_->TriggerKey(DIK_SPACE)) {
		Initialize();
		NowGameScene = NowGameScene::Title;


	}
}

void GameScene::OverProc()
{
	
	if (input_->TriggerKey(DIK_SPACE)) {
		Initialize();
		NowGameScene = NowGameScene::Title;

	
	}
}

void GameScene::TitleDraw() {  }

