#pragma once

namespace FrameRenderer {

    // 初期化（画像ハンドルの読み込みなど）
    void Load(void);

    // 描画処理（サイズに合わせてExtendDraw）
    void Draw(int x, int y);
    void Draw(int x, int y, int width, int height, int frameOffset);
    void DrawF(float x, float y, float width, float height, float frameOffset);

    // 後始末
    void Release(void);
}
