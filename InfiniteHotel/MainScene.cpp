#include "MainScene.h"

#include <fstream>

#include "DrawableHotel.h"




MainScene::MainScene()
	: m_camera(PointF(0, 20))
	, m_hotel(std::make_unique<DrawableHotel>())
{

}


MainScene::~MainScene()
{

}

//###########################################################################

void MainScene::onInitialize(caDraw::Window& owner)
{
	auto winSize = owner.getSize();


	m_panel = caFactory->createPanel();
	m_panel->transform.position = { 0, 0 };
	m_panel->size = static_cast<caDraw::SizeF>(winSize);
	addPanel(m_panel);

	m_font = caFactory->createFont();
	m_font->loadFromFile("IH Resources/NanumGothic.ttf");
	m_font->setCharacterSize(18);
	m_font->setStyle(caDraw::FontStyles::Bold);

	m_feedEnergyButton = canew<caUI::Button>();
	m_feedEnergyButton->setFont(m_font);
	m_feedEnergyButton->setSize({ 128, 36 });
	m_feedEnergyButton->setPosition({ 8, (float)winSize.height - 36 - 8 });
	m_feedEnergyButton->setText(L"에너지 증가");
	m_feedEnergyButton->WhenClick += [&hotel = m_hotel]
	(const caUI::TouchEventArgs& args)
	{
		hotel->addEnergy(10000.0);
	};

	m_takeEnergyButton = canew<caUI::Button>();
	m_takeEnergyButton->setFont(m_font);
	m_takeEnergyButton->setSize({ 128, 36 });
	m_takeEnergyButton->setPosition({ 8 + 128 + 8, (float)winSize.height - 36 - 8 });
	m_takeEnergyButton->setText(L"에너지 감소");
	m_takeEnergyButton->WhenClick += [&hotel = m_hotel]
	(const caUI::TouchEventArgs& args)
	{
		hotel->addEnergy(-10000.0);
	};

	m_panel->addDrawable(m_feedEnergyButton);
	m_panel->addUpdatable(m_feedEnergyButton);

	m_panel->addDrawable(m_takeEnergyButton);
	m_panel->addUpdatable(m_takeEnergyButton);


	m_loadPanel = caFactory->createPanel();
	m_loadPanel->setVisible(false);
	m_loadPanel->size = { 534, 150 };
	m_loadPanel->transform.position = {
		winSize.width / 2 - m_loadPanel->size.width / 2,
		winSize.height / 2 - m_loadPanel->size.height / 2
	};
	addPanel(m_loadPanel);

	m_backRect = canew<caDraw::DrawableRectangle>();
	m_backRect->setLocation(-2, -2);
	m_backRect->setSize(m_loadPanel->size.width, m_loadPanel->size.height);
	m_backRect->fillColor = caDraw::Color(250, 250, 250);
	m_backRect->edgeColor = caDraw::Color(128, 128, 128, 128);
	m_backRect->edgeWidth = 4.0f;

	m_loadLabel = canew<caUI::Label>();
	m_loadLabel->setFont(m_font);
	m_loadLabel->setSize({ 300, 36 });
	m_loadLabel->setPosition({ 22, 20 });
	m_loadLabel->setText(L"기록된 정보로 시작하시겠습니까?");

	m_loadButton = canew<caUI::Button>();
	m_loadButton->setFont(m_font);
	m_loadButton->setSize({ 128, 36 });
	m_loadButton->setPosition({ 250, 100 });
	m_loadButton->setText(L"예");
	m_loadButton->WhenClick += [&hotel = m_hotel, &panel = m_loadPanel]
	(const caUI::TouchEventArgs& args)
	{
		panel->setVisible(false);
		hotel->setVisible(true);

		std::ifstream fr("save.htl");
		if (fr.is_open())
		{
			hotel->readFrom(fr);


			fr.close();
		}
	};

	m_cancelButton = canew<caUI::Button>();
	m_cancelButton->setFont(m_font);
	m_cancelButton->setSize({ 128, 36 });
	m_cancelButton->setPosition({ 250 + 128 + 16, 100 });
	m_cancelButton->setText(L"아니오");
	m_cancelButton->WhenClick += [&hotel = m_hotel, &panel = m_loadPanel]
	(const caUI::TouchEventArgs& args)
	{
		panel->setVisible(false);
		hotel->setVisible(true);

		hotel->initialize(1024, 1024);
	};

	m_loadPanel->addDrawable(m_backRect);

	m_loadPanel->addDrawable(m_loadLabel);
	m_loadPanel->addUpdatable(m_loadLabel);

	m_loadPanel->addDrawable(m_loadButton);
	m_loadPanel->addUpdatable(m_loadButton);

	m_loadPanel->addDrawable(m_cancelButton);
	m_loadPanel->addUpdatable(m_cancelButton);


	std::ifstream fr("save.htl");
	if (fr.is_open())
	{
		m_loadPanel->setVisible(true);
		m_hotel->setVisible(false);


		fr.close();
	}
	else
	{
		m_hotel->initialize(1024, 1024);
	}

	m_hotel->setFont(m_font);
}


void MainScene::onRelease()
{
	std::ofstream fw("save.htl");
	if (fw.is_open())
	{
		m_hotel->writeTo(fw);


		fw.close();
	}
}


void MainScene::onUpdate(caDraw::Window& owner)
{
	m_beginTime = system_clock::now();


	if (caKeyboard->isKeyPressed(caSys::Keys::LShift))
	{
		if (caKeyboard->isKeyDown(caSys::Keys::Escape))
		{
			reserveNextScene(nullptr);
		}

		if (caKeyboard->isKeyDown(caSys::Keys::Home))
		{
			m_hotel->showExtinctGene(false);
		}
		else if (caKeyboard->isKeyDown(caSys::Keys::End))
		{
			m_hotel->showExtinctGene(true);
		}
	}

	if (caKeyboard->isKeyPressed(caSys::Keys::Up))
	{
		m_camera.move(0, 8);
	}
	else if (caKeyboard->isKeyPressed(caSys::Keys::Down))
	{
		m_camera.move(0, -8);
	}

	if (caKeyboard->isKeyPressed(caSys::Keys::Left))
	{
		m_camera.move(8, 0);
	}
	else if (caKeyboard->isKeyPressed(caSys::Keys::Right))
	{
		m_camera.move(-8, 0);
	}

	if (caKeyboard->isKeyPressed(caSys::Keys::Space))
	{
		m_camera.setLocation(0, 0);
	}


	m_hotel->update();
}


void MainScene::onDrawBack(caDraw::Graphics& g)
{
	auto cam = m_panel->transform;
	cam.position += m_camera;


	m_hotel->draw(g, cam);
}


void MainScene::onDrawFront(caDraw::Graphics& g)
{
	auto endTime = system_clock::now();
	std::chrono::duration<double> duration = endTime - m_beginTime;

	double fps = 1.0 / duration.count();


	auto& textArtist = g.textArtist;


	textArtist->beginDrawString(*m_font);

	textArtist->drawString(std::to_string(fps) + "fps",
		8, 8, caDraw::Color::Red);

	textArtist->endDrawString();
}

//###########################################################################

void MainScene::enable(bool bEnable)
{
	m_feedEnergyButton->setEnabled(bEnable);
	m_takeEnergyButton->setEnabled(bEnable);
}

