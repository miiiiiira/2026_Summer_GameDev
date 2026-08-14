#pragma once

#include "../Component.h"

class FlashEffect :public Component
{
public:

	FlashEffect(void);
	~FlashEffect(void)override;

	void Init(void)override;
	void Update(void)override;
	void Draw2D(void)override;

	void SetEffect(int alpha, unsigned int color);

private:

	int alpha_;
	unsigned int color_;
};

