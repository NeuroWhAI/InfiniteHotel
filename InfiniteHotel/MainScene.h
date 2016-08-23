#pragma once


#include <memory>
#include <chrono>

#include <CodeAdapter\EasyCA.h>




class DrawableHotel;


class MainScene : public caUtil::Scene
{
private:
	USING_CA_DRAWING(PointF);

	using system_clock = std::chrono::system_clock;


public:
	MainScene();
	virtual ~MainScene();


protected:
	std::shared_ptr<caDraw::Panel> m_panel;
	std::shared_ptr<caDraw::Font> m_font;


protected:
	std::shared_ptr<caDraw::Panel> m_loadPanel;
	std::shared_ptr<caDraw::DrawableRectangle> m_backRect;
	std::shared_ptr<caUI::Label> m_loadLabel;
	std::shared_ptr<caUI::Button> m_loadButton;
	std::shared_ptr<caUI::Button> m_cancelButton;


protected:
	PointF m_camera;
	std::unique_ptr<DrawableHotel> m_hotel;


protected:
	system_clock::time_point m_beginTime;


public:
	virtual void onInitialize(caDraw::Window& owner) override;
	virtual void onRelease() override;
	virtual void onUpdate(caDraw::Window& owner) override;
	virtual void onDrawBack(caDraw::Graphics& g) override;
	virtual void onDrawFront(caDraw::Graphics& g) override;
};

