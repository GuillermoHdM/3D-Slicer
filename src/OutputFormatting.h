#pragma once
#include <vector>
#include <cstdint>
struct RGB565
{
	void SetR(uint8_t red);
	void SetG(uint8_t green);
	void SetB(uint8_t blue);
	uint16_t GetValue() const;

private:
	uint16_t Value = 0;
};

struct RLEgoo
{
	RLEgoo(const std::vector<uint8_t>& image);

	const std::vector<uint8_t>& GetData() const;
	uint32_t GetDataSize() const;

private:
	std::vector<uint8_t> Data;
};

