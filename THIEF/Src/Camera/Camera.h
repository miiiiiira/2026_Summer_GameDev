#pragma once
#include <DxLib.h>

class ActorBase;

class Camera
{

public:

	// カメラの初期座標
	static constexpr VECTOR DERFAULT_POS = { 0.0f, 200.0f, -500.0f };

	// カメラの初期角度
	static constexpr VECTOR DERFAULT_ANGLES = { 
		0.0f, 0.0f, 0.0f
	};

	// 追従対象からカメラへの相対座標
	static constexpr VECTOR FOLLOW_CAMERA_LOCAL_POS_STANDING = { 0.0f, 178.0f, 0.0f };
	static constexpr VECTOR FOLLOW_CAMERA_LOCAL_POS_CROUCHING = { 0.0f, 90.0f, 0.0f };

	// 追従対象から注視点への相対座標
	static constexpr VECTOR FOLLOW_TARGET_LOCAL_POS = { 0.0f, 0.0f, 300.0f };

	// カメラのクリップ範囲
	static constexpr float VIEW_NEAR = 20.0f;
	static constexpr float VIEW_FAR = 5000.0f;
	
	// カメラのX回転上限度角
	static constexpr float LIMIT_X_UP_RAD = 90.0f * (DX_PI_F / 180.0f);
	static constexpr float LIMIT_X_DW_RAD = -90.0f * (DX_PI_F / 180.0f);

	// 視点のしきい値(マウス)
	static constexpr float THRESHOLD = 1.1f;

	// カメラモード
	enum class MODE
	{
		NONE,
		FIXED_POINT,	// 定点カメラ
		FREE,			// フリーモード
		FOLLOW,			// 追従モード
	};

	// コンストラクタ
	Camera(void);

	// デストラクタ
	~Camera(void);

	// 初期化
	void Init(void);

	// 更新
	void Update(void);

	// 描画前のカメラ設定
	void SetBeforeDraw(void);
	void SetBeforeDrawFixedPoint(void);
	void SetBeforeDrawFree(void);
	void SetBeforeDrawFollow(void);

	// デバッグ用描画
	void DrawDebug(void);

	// 解放
	void Release(void);

	// 座標の取得
	const VECTOR& GetPos(void) const { return pos_; }

	// 角度の取得
	const VECTOR& GetAngle(void) const { return angle_; }
	
	// 注視点の取得
	const VECTOR& GetTargetPos(void) const { return targetPos_; }

	// カメラモードの変更
	void ChangeMode(MODE mode);

	// 追従相手の設定
	void SetFollow(ActorBase* follow);

private:

	// 追従相手
	ActorBase* follow_;

	// カメラモード
	MODE mode_;

	// カメラの位置
	VECTOR pos_;

	// カメラの角度
	VECTOR angle_;
	
	// 注視点
	VECTOR targetPos_;
	
	// ２D状でのマウスの位置
	int mousePosX_;
	int mousePosY_;

	// カメラ操作		true = 視点操作(上下)上限有り / false = 視点操作(上下)上限なし
	void ProcessRot(bool isLimit);

	// カメラ回転(キーボード)
	void RotKeyboard(bool isLimit);

	// カメラ回転(ゲームパッド)
	void RotGamePad(bool isLimit);

	// カメラ回転(マウス)
	void RotMouse(bool isLimit);
};
