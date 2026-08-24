#include "FrameRenderer.h"

#include<DxLib.h>

namespace FrameRenderer 
{
	int frame_ = -1;

	static constexpr int FRAME_SIZE_WID = 200;
	static constexpr int FRAME_SIZE_HIG = 50;

	void Load(void)
	{
		// âÊëúÇÃì«Ç›çûÇ›
		frame_ = LoadGraph("Data/Image/Common/Frame.png");
	}

	void Draw(int x, int y)
	{
		DrawExtendGraph(x,
			y ,
			x + FRAME_SIZE_WID,
			y + FRAME_SIZE_HIG,
			frame_, true);
	}

	void Draw(int x, int y, int width, int height,int frameOffset)
	{
		DrawExtendGraph(x - frameOffset,
			y - frameOffset,
			x + width + frameOffset,
			y + height + frameOffset,
			frame_, true);
	}

	void DrawF(float x, float y, float width, float height,float frameOffset)
	{
		DrawExtendGraphF(x - frameOffset,
			y - frameOffset,
			x + width + frameOffset,
			y + height + frameOffset,
			frame_, true);
	}

	void Release(void)
	{
		DeleteGraph(frame_);
	}
}