#pragma once
class VtUniqueID
{
protected:
	int _id;
	static int _current_id;
public:
	VtUniqueID();
	~VtUniqueID();
	int Id() { return _id++; }
};

