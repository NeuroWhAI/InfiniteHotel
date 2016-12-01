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
	caDraw::PanelPtr m_panel;
	caDraw::FontPtr m_font;
	caUI::ButtonPtr m_feedEnergyButton;
	caUI::ButtonPtr m_takeEnergyButton;


protected:
	caDraw::PanelPtr m_loadPanel;
	caDraw::DrawableRectanglePtr m_backRect;
	caUI::LabelPtr m_loadLabel;
	caUI::ButtonPtr m_loadButton;
	caUI::ButtonPtr m_cancelButton;


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


public:
	void enable(bool bEnable);
};

