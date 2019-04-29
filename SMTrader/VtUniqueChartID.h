#pragma once
class VtUniqueChartID
{
protected:
	int _id;
	static int _current_id;
public:
	VtUniqueChartID();
	~VtUniqueChartID();
	int Id() { return _id++; }
	void Id(int val) { _id = val; }
};
