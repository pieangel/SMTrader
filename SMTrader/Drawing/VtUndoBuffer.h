#pragma once
#include <string>
#include "VtElement.h"
//class VtElement;
enum class BuffOp
{
	Update,
	Insert,
	Delete
};
class BuffElement
{
public:
	VtElement* newE; //Start point
	VtElement* objRef;
	VtElement* oldE; //Start point
	BuffOp op; // U:Update, I:Insert, D:Delete

	BuffElement(VtElement* refe, VtElement* newe, VtElement* olde, BuffOp o)
	{
		objRef = refe;
		oldE = olde;
		newE = newe;
		op = o;
	}
	~BuffElement()
	{
		if (newE != nullptr)
			delete newE;
		if (oldE != nullptr)
			delete oldE;
		//if (objRef != nullptr)
		//	delete objRef;
	}
};

/// <summary>
/// Two Linked List Element
/// </summary>
class BuffObject
{
public:
	BuffElement* elem;
	BuffObject* Next;
	BuffObject* Prec;

	BuffObject(BuffElement* o)
	{
		elem = o;
	}
	~BuffObject()
	{
		delete elem;
	}
};

/// <summary>
/// Undo buffer. (Two Linked List)
/// </summary>
//[Serializable]
class UndoBuffer
{
private:
	int _count;
	bool IsAtEnd;
	BuffObject* Bottom;
	BuffObject* Current;
	BuffObject* Top;
	int _BuffSize = 0;
public:
	UndoBuffer(int i)
	{
		_BuffSize = i;
		_count = 0;
		Top = nullptr;
		Bottom = nullptr;
		Current = nullptr;
		IsAtEnd = true;
	}

	~UndoBuffer()
	{
		ClearBuffer();
	}

	void ClearBuffer()
	{
		BuffObject* next = Top;
		BuffObject* cur = nullptr;
		while (next != nullptr)
		{
			cur = next;
			next = next->Next;
			delete cur;
		}
	}

	int BuffSize() 
	{ 
		return _BuffSize; 
	}

	int Count()
	{
		return _count;
	}

	void Add(BuffElement* o)
	{
		if (o == nullptr)
			return;
		auto g = new BuffObject(o);
		if (_count == 0)
		{
			g->Next = nullptr;
			g->Prec = nullptr;
			Top = g;
			Bottom = g;
			Current = g;
		}
		else
		{
			g->Prec = Current;
			g->Next = nullptr;
			Current->Next = g;
			Top = g;
			Current = g;
			if (_count == 1)
				Bottom->Next = g;
		}

		//this._N_elem = count();
		_count++;
		if (_BuffSize < _count)
		{
			Bottom = Bottom->Next;
			Bottom->Prec = nullptr;
			_count--;
		}
		IsAtEnd = false;
	}

	BuffElement* Undo()
	{
		if (Current != nullptr)
		{
			BuffElement* obj = Current->elem;
			if (Current->Prec != nullptr)
			{
				Current = Current->Prec;
				_count--;
				IsAtEnd = false;
			}
			else
				IsAtEnd = true;
			return obj;
		}
		//this._N_elem = count();
		return nullptr;
	}

	BuffElement* Redo()
	{
		if (Current != nullptr)
		{
			if (!IsAtEnd)
			{
				if (Current->Next != nullptr)
				{
					Current = Current->Next;
					_count++;
				}
			}
			else
				IsAtEnd = false;
			return Current->elem;
		}
		//this._N_elem = count();
		return nullptr;
	}

	bool unDoable()
	{
		return !IsAtEnd;
	}

	bool unRedoable()
	{
		return Current != nullptr && Current->Next != nullptr;
	}
};

class VtUndoBuffer
{
public:
	VtUndoBuffer();
	~VtUndoBuffer();
};

