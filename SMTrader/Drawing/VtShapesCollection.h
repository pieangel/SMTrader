#pragma once
#include <list>
#include <vector>
#include "VtElement.h"
#include "VtBaseTracker.h"
#include "VtUndoBuffer.h"
#include <string>

const int UndoBufferSize = 100;
class XYChart;
class CChartViewer;
struct VtOrder;
class VtShapesCollection
{
public:
	VtShapesCollection();
	~VtShapesCollection();

	std::list<VtElement*> List;
	VtElement* SelectedElement = nullptr;
	VtBaseTracker* Tracker = nullptr;

	bool IsEmpty()
	{
		return (List.size() > 0);
	}
	void DeSelect();
	VtElement* GetCopyOfSelectedElement();
	void CopySelected(int dx, int dy);
	void RemoveElement(VtElement* ele);
	void ClearElements();
	void MovePoint(int dx, int dy);
	void Move(int dx, int dy);
	void EndMove();
	void Redim(int x, int y, RedimStatus redim);
	// For the specific chart
	void MovePoint(XYChart* chart, int dx, int dy);
	void Move(XYChart* chart, int dx, int dy);
	void EndMove(XYChart* chart);
	void Click(XYChart* chart, int x, int y);
	std::list<VtElement*>* GetSelectedList();
	void Click(int x, int y);
	static void SanitizeRect(int& sx, int& sy, int& tmpX, int& tmpY);
	void MultiSelect(int startX, int startY, int endX, int endY);

	void AddMultiLine(int x, int y, int x1, int y1, VtColor penC, VtColor fillC, int penWidth, std::vector<VtPoint> pointList);
	void AddRect(int x, int y, int x1, int y1, VtColor penC, VtColor fillC, int penWidth, bool filled);
	void AddLine(int x, int y, int x1, int y1, VtColor penC, int penWidth);
	void AddVLine(int x, VtColor penC, int penWidth);
	void AddHLine(int y, VtColor penC, int penWidth);
	void AddEllipse(int x, int y, int x1, int y1, VtColor penC, VtColor fillC, int penWidth, bool filled);
	void AddFibo(int x, int y, int x1, int y1, VtColor penC, int penWidth);
	// For the specific chart
	void AddMultiLine(XYChart* chart, int x, int y, int x1, int y1, VtColor penC, VtColor fillC, int penWidth, std::vector<VtPoint> pointList);
	void AddRect(XYChart* chart, int x, int y, int x1, int y1, VtColor penC, VtColor fillC, int penWidth, bool filled);
	void AddLine(XYChart* chart, int x, int y, int x1, int y1, VtColor penC, int penWidth);
	void AddVLine(XYChart* chart, int x, VtColor penC, int penWidth);
	void AddHLine(XYChart* chart, int y, VtColor penC, int penWidth);
	void AddEllipse(XYChart* chart, int x, int y, int x1, int y1, VtColor penC, VtColor fillC, int penWidth, bool filled);
	void AddFibo(XYChart* chart, int x, int y, int x1, int y1, VtColor penC, int penWidth);

	void DrawUnselected(int startID, int zoomStartIndex, DrawArea* d, XYChart* chart, int dx, int dy);
	void DrawSelected(int startID, int zoomStartIndex, DrawArea* d, XYChart* chart, int dx, int dy);

	void PixelToValue(XYChart* chart);
	void ValueToPixel(XYChart* chart);
	void PixelToValue(int startID, int zoomStartIndex, XYChart* chart);
	void ValueToPixel(int startID, int zoomStartIndex, XYChart* chart);

	void SaveDimension(CChartViewer * a_pChartViewer, int dataSize, XYChart* chart);
	bool CheckChangeDimension(CChartViewer * a_pChartViewer, int dataSize, XYChart* chart);
	void DeleteSelectedObject();
	void DeleteAllObjects();
	VtElement* AddHLineByOrder(XYChart* chart, int y, VtColor penC, int penWidth);
	VtElement* FindObjectByOrder(VtOrder* order);
	VtElement* FindObjectByOrderNo(std::string orderNo);
private:
	void DeleteElement(VtElement* ele);
	double xmin, ymin, xmax, ymax;
	int startIndex = 0;

	UndoBuffer* undoB = nullptr;
	void InitUndoBuff()
	{
		undoB = new UndoBuffer(UndoBufferSize);
	}

public:

	bool UndoEnabled()
	{
		return undoB->unDoable();
	}

	bool RedoEnabled()
	{
		return undoB->unRedoable();
	}

	void storeDo(BuffOp option, VtElement* e)
	{
		/*
		VtElement* olde = nullptr;
		if (e->UndoElement() != nullptr)
			olde = e->UndoElement()->Copy();
		VtElement* newe = e->Copy();
		auto buff = new BuffElement(e, newe, olde, option);
		undoB->Add(buff);
		e->UndoElement(e->Copy());
		*/
	}

	void Undo()
	{
		BuffElement* buff = undoB->Undo();
		if (buff != nullptr)
		{
			switch (buff->op)
			{
			case BuffOp::Update:
				buff->objRef->CopyFrom(buff->oldE);
				break;
			case BuffOp::Insert:
				//buff.objRef.CopyFrom(buff.oldE);
				List.remove(buff->objRef);
				break;
			case BuffOp::Delete:
				//buff.objRef.CopyFrom(buff.oldE);
				List.push_back(buff->objRef);
				break;
			default:
				break;
			}
		}
	}

	void Redo()
	{
		BuffElement* buff = undoB->Redo();
		if (buff != nullptr)
		{
			switch (buff->op)
			{
			case BuffOp::Update:
				buff->objRef->CopyFrom(buff->newE);
				break;
			case BuffOp::Insert:
				//buff.objRef.CopyFrom(buff.oldE);
				List.push_back(buff->objRef);
				break;
			case BuffOp::Delete:
				//buff.objRef.CopyFrom(buff.oldE);
				List.remove(buff->objRef);
				break;
			default:
				break;
			}
		}
	}
};

