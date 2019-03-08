#pragma once
class VtUniqueOrderID
{
protected:
	int _id;
	static int _current_id;
public:
	VtUniqueOrderID();
	~VtUniqueOrderID();
	static int GetID()
	{
		return _current_id++;
	}
};

