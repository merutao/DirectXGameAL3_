#include "Input.h"
#include "Model.h"
#include "PlayerBullet.h"
#include "WorldTransform.h"
#include <Sprite.h>
#include <list>

class Player {

public:
	/// <summary>
	/// デストラクタ
	/// </summary>
	~Player();

	void Initialize(Model* model, uint32_t textureHandle,Vector3 position);

	void Update(ViewProjection& viewProjection);

	void Draw(ViewProjection& viewProjection);

	// ワールド座標を取得
	Vector3 GetWorldPosition();

	const std::list<PlayerBullet*>& GetBullets() const { return bullets_; }

	void OnCollision();

	/// <summary>
	/// 親となるワールドトランスフォーム
	/// </summary>
	/// <param name="parent"></param>
	void SetParent(const WorldTransform* parent);

	/// <summary>
	/// UI描画
	/// </summary>
	void DrawUI();

public:
	float radius = 3;

		bool IsDead() const { return isDead_; }


private:
	WorldTransform worldTransform_;
	Model* model_ = nullptr;
	Model* reticle = nullptr;
	uint32_t texturehandle_ = 0u;
	Input* input_ = nullptr;
	float inputFloat3[3] = {0, 0, 0};
	//  弾
	std::list<PlayerBullet*> bullets_;

	bool isDead_ = false;

	//3Dレティクル用ワールドトランスフォーム
	WorldTransform worldTransform3DReticle_;

	//2Dレティクル用スプライト
	Sprite* sprite2DReticle_ = nullptr;

private:
	void Rotate();
	void Attack();
};