#pragma once
#include "../Global/VtDefine.h"
class VtSignal
{
public:
	VtSignal();
	~VtSignal();
	double DateTime() const { return _DateTime; }
	void DateTime(double val) { _DateTime = val; }
	VtPositionType SignalType() const { return _SignalType; }
	void SignalType(VtPositionType val) { _SignalType = val; }
	double Price() const { return _Price; }
	void Price(double val) { _Price = val; }
	VtDate Date() const { return _Date; }
	void Date(VtDate val) { _Date = val; }
	VtTime Time() const { return _Time; }
	void Time(VtTime val) { _Time = val; }
private:
	/// <summary>
	/// ��ȣ�� �߻��� �ð�
	/// </summary>
	double _DateTime;
	VtDate _Date;
	VtTime _Time;
	/// <summary>
	/// ��ȣ�� ���� : �ŵ� / �ż�
	/// </summary>
	VtPositionType _SignalType;
	/// <summary>
	/// ��ȣ�� �߻��� ����
	/// </summary>
	double _Price;
};

