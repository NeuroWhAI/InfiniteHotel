#pragma once


#include <CodeAdapter\EasyCA.h>

#include "Hotel.h"




class DrawableHotel : public Hotel, public caDraw::Drawable
{
private:
	USING_CA_DRAWING(Graphics);
	USING_CA_DRAWING(Transform);


public:
	DrawableHotel();
	virtual ~DrawableHotel();


protected:
	std::shared_ptr<caDraw::Font> m_font;


protected:
	virtual void onDraw(Graphics& g, const Transform& parentTransform) override;


public:
	void setFont(std::shared_ptr<caDraw::Font> font);
};

