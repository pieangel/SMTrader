#pragma once
#include "VtHandle.h"
class VtPointHandle :
	public VtHandle
{
private:
	VtElement* _Parent;
	VtPoint*    _LinkedPoint;
public:
	VtPointHandle();
	virtual ~VtPointHandle();
	VtPointHandle(VtElement* e, RedimStatus o, VtPoint* pt);
	VtPointHandle(XYChart* chart, VtElement* e, RedimStatus o, VtPoint* pt);
	virtual void RePosition(VtElement* e);
	virtual void RePosition(XYChart* chart, VtElement* e);
	virtual void Draw(int startID, int zoomStartIndex, DrawArea* d, XYChart* chart, int dx, int dy);
	/*
	virtual void SyncPoint()
	{
		if (_LinkedPoint != nullptr)
		{
			_LinkedPoint->x = Location0.x;
			_LinkedPoint->y = Location0.y;
		}
	}
	*/
	virtual void Move(int x, int y);
	virtual void Move(XYChart* chart, int x, int y);
	VtPoint* LinkedPoint() const { return _LinkedPoint; }
	void LinkedPoint(VtPoint* val) { _LinkedPoint = val; }
};

