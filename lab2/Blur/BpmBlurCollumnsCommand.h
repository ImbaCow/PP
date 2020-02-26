#pragma once
#include "bitmap_image.hpp"
#include <vector>

class BpmBlurCollumnsCommand
{
public:
	BpmBlurCollumnsCommand(const bitmap_image& fromImage, bitmap_image& toImage, uint32_t indexBegin, uint32_t indexEnd);

	void Execute();

private:
	const bitmap_image& m_fromImage;
	bitmap_image& m_toImage;

	uint32_t m_indexBegin;
	uint32_t m_indexEnd;

	void BlurPixel(uint32_t width, uint32_t height) const;
	std::vector<rgb_t> GetPixelsInRadius(uint32_t widthIndex, uint32_t heightIndex, uint32_t radius) const;
};

