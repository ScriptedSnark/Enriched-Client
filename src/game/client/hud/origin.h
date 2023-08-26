#ifndef HUD_ORIGIN_H
#define HUD_ORIGIN_H

class CHudOrigin : public CHudElemBase<CHudOrigin>
{
public:
	virtual void Init();
	virtual void VidInit();
	virtual void Draw(float time);
	void UpdateOrigin(const float simorg[3]);

private:
	Vector m_Origin;
};

#endif // HUD_ORIGIN_H