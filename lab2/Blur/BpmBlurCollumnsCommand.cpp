#include "BpmBlurCollumnsCommand.h"
#include <numeric>

BpmBlurCollumnsCommand::BpmBlurCollumnsCommand(const bitmap_image& fromImage, bitmap_image& toImage, uint32_t indexBegin, uint32_t indexEnd)
	: m_fromImage(fromImage)
	, m_toImage(toImage)
	, m_indexBegin(indexBegin)
	, m_indexEnd(indexEnd)
{
}

void BpmBlurCollumnsCommand::Execute()
{
	uint32_t maxHeight = m_fromImage.height();

	for (uint32_t collumnIndex = m_indexBegin; collumnIndex < m_indexEnd; collumnIndex++)
	{
		for (uint32_t height = 0; height < maxHeight; ++height)
		{
			BlurPixel(collumnIndex, height);
		}
	}
}

const uint32_t PIXEL_SEARCH_RADIUS = 50;

void BpmBlurCollumnsCommand::BlurPixel(uint32_t widthIndex, uint32_t heightIndex) const
{
	std::vector<rgb_t> pixelsInRadius = GetPixelsInRadius(widthIndex, heightIndex, PIXEL_SEARCH_RADIUS);

	uint32_t totalR = 0;
	uint32_t totalG = 0;
	uint32_t totalB = 0;

	for (const auto& pixel : pixelsInRadius)
	{
		totalR += pixel.red;
		totalG += pixel.green;
		totalB += pixel.blue;
	}

	rgb_t avaragePixel = {
		static_cast<uint8_t>(totalR / pixelsInRadius.size()),
		static_cast<uint8_t>(totalG / pixelsInRadius.size()),
		static_cast<uint8_t>(totalB / pixelsInRadius.size())
	};
	m_toImage.set_pixel(widthIndex, heightIndex, avaragePixel);
}

std::vector<rgb_t> BpmBlurCollumnsCommand::GetPixelsInRadius(uint32_t widthIndex, uint32_t heightIndex, uint32_t radius) const
{
	uint32_t minSearchWidthBegin = static_cast<uint32_t>(std::max(0ll, static_cast<long long>(widthIndex) - radius));
	uint32_t maxSearchWidthEnd = std::min(m_fromImage.width(), widthIndex + radius + 1);

	uint32_t minSearchHeightBegin = static_cast<uint32_t>(std::max(0ll, static_cast<long long>(heightIndex) - radius));
	uint32_t maxSearchHeightEnd = std::min(m_fromImage.height(), heightIndex + radius + 1);

	std::vector<rgb_t> raduisPixels;
	for (uint32_t i = minSearchWidthBegin; i < maxSearchWidthEnd; ++i)
	{
		for (uint32_t j = minSearchHeightBegin; j < maxSearchHeightEnd; ++j)
		{
			raduisPixels.push_back(m_fromImage.get_pixel(i, j));
		}
	}

	return raduisPixels;
}
