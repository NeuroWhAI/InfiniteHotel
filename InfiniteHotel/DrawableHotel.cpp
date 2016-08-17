#include "DrawableHotel.h"

#include "HotelStat.h"
#include "Unit.h"
#include "Gene.h"




DrawableHotel::DrawableHotel()
{

}


DrawableHotel::~DrawableHotel() = default;

//###########################################################################

void DrawableHotel::onDraw(Graphics& g, const Transform& parentTransform)
{
	auto& textArtist = g.textArtist;


	textArtist->beginDrawString(*m_font);

	textArtist->drawString("Epoch: " + std::to_string(m_epoch),
		8, 8, caDraw::Color::Gray);

	float timePercent = static_cast<float>(m_time) / m_endTime * 100.0f;
	textArtist->drawString("Time: " + std::to_string(timePercent) + "%",
		8, 8 + 20, caDraw::Color::Gray);

	textArtist->drawString("Energy: " + std::to_string(m_totalEnergy),
		8, 8 + 20 * 2, caDraw::Color::Gray);

	textArtist->drawString("Room: " + std::to_string(m_roomList.size()),
		8, 8 + 20 * 3, caDraw::Color::Gray);

	textArtist->drawString("Unit: " + std::to_string(m_hotelStat->getUnitCount()),
		8, 8 + 20 * 4, caDraw::Color::Gray);

	textArtist->drawString("Birth: " + std::to_string(m_hotelStat->getBirth()),
		8, 8 + 20 * 5, caDraw::Color::Gray);

	textArtist->drawString("Death: " + std::to_string(m_hotelStat->getDeath()),
		8, 8 + 20 * 6, caDraw::Color::Gray);

	textArtist->drawString("Max Unit Energy: " + std::to_string(m_hotelStat->getMaxUnitEnergy()),
		8, 8 + 20 * 7, caDraw::Color::Gray);

	textArtist->drawString("Max Unit Score: " + std::to_string(m_hotelStat->getMaxUnitScore()),
		8, 8 + 20 * 8, caDraw::Color::Gray);

	textArtist->drawString("Min Unit Score: " + std::to_string(m_hotelStat->getMinUnitScore()),
		8, 8 + 20 * 9, caDraw::Color::Gray);

	textArtist->drawString("Mutation: " + std::to_string(m_hotelStat->getMutationCount()),
		8, 8 + 20 * 10, caDraw::Color::Gray);

	textArtist->endDrawString();
}

//###########################################################################

void DrawableHotel::setFont(std::shared_ptr<caDraw::Font> font)
{
	m_font = font;
}

