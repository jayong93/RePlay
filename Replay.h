#pragma once

enum class ReplayDataType { NONE, CREATE_BODY, MOVE_BODY, SELECT_BODY, RESET };

struct ReplayData
{
	ReplayData(ReplayDataType type = ReplayDataType::NONE, long long time = 0, int nByte = 0, void* data = nullptr) : type{ type }, timePoint{ time }, nByte{ nByte }, data{ data } {}
	ReplayData(const ReplayData& d) : type{ d.type }, timePoint{ d.timePoint }, nByte{ d.nByte }, data{ new char[d.nByte] } { memcpy_s(data, nByte, d.data, d.nByte); }
	ReplayData(ReplayData&& d) : type{ d.type }, timePoint{ d.timePoint }, nByte{ d.nByte }, data{ d.data }
	{
		d.type = ReplayDataType::NONE; d.timePoint = 0; d.nByte = 0; d.data = nullptr;
	}
	~ReplayData() { if (data) delete[] data; }
	void operator=(const ReplayData& d)
	{
		type = d.type;
		timePoint = d.timePoint;
		nByte = d.nByte;
		data = new char[nByte];
		memcpy_s(data, nByte, d.data, d.nByte);
	}
	void operator=(ReplayData&& d)
	{
		type = d.type;
		timePoint = d.timePoint;
		nByte = d.nByte;
		data = d.data;
		d.type = ReplayDataType::NONE;
		d.timePoint = 0;
		d.nByte = 0;
		d.data = nullptr;
	}

	ReplayDataType type;
	long long timePoint;
	int nByte;
	void* data;
};