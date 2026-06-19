#pragma once

class DamageEffect
{
public:

	DamageEffect(void);
	~DamageEffect(void);

	void Load(void);
	void Init(void);
	void Update(void);
	void Draw(void);

	void SetEffect(int alpha, unsigned int color);

private:

	int alpha_;
	unsigned int color_;
};

