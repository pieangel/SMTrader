#pragma once
enum  class RedimStatus
{
	Idle = -1,
	None = 0,
	MovePoint,
	LineMove,
	Poly,
	NewPoint,
	MoveSelected,
	Rotate,
	Zoom,
	N,
	NE,
	E,
	SE,
	S,
	SW,
	W,
	NW,
	MoveUpDown,
	MoveLeftRight,
	START,
	END
};

enum class EditOption
{
	None = 0,
	Select,
	Ellipse,
	Line,
	Poly,
	Pen,
	Rect,
	RoundRect,
	Arc,
	SimpleText,
	RichText,
	Image,
	AcquireImage,
	MultiLine,
	Fibo,
	HLine,
	VLine,
	HLineOrder,
	LineOrder
};

typedef EditOption ElementType;

enum class EditStatus
{
	None,
	DrawRect, // 새로운 객체를 생성하려고 하는 상태
	SelectRect, // 객체 선택하려고 하는 상태
	Redim, // 객체를 이동하거나 수정하려고 하는 상태
};

/// <summary>
/// All known operations that can be mapped to keys for keyboard handling
/// </summary>
enum class KeyOperation
{
	None = 0,                   // no operation. for good enumeration habits, it is wise to have a default do-nothing operation.
	SelectMode,           // fall back to 'select' mode.  This is handy if 'Sticky mode' is on, and you want to get rid of it.  A good key to bind it to, would be 'Keys.Escape' for example
	DeSelect,             // clear the selection
	RectMode,
	LineMode,
	EllipseMode,
	RoundRectangleMode,
	SimpleTextMode,
	RichTextMode,
	ImageMode,
	AcquireImageMode,
	ArcMode,
	PolyMode,
	PenMode,
	WaveMode,
	FiboMode,
	VLineMode,
	HLineMode,
	Load,
	Save,
	LoadObjects,
	SaveSelectedObjects,
	PrintPreview,
	Print,
	GridOff,
	Grid3,
	Grid5,
	Grid8,
	Grid10,
	Grid12,
	Grid15,
	Grid20,
	GridCustom,
	Undo,
	Redo,
	Group,
	UnGroup,
	PolyMerge,
	PolyDeletePoints,
	PolyExtendPoints,
	PolyMirrorX,
	PolyMirrorY,
	PolyMirrorXY,
	ToFront,
	ToBack,
	DeleteSelected,
	CopySelected,
};

const int HandleHeight = 5;
const int HandleWidth = 5;
const int HandleRadius = 5;
const int HandleDynamic = 5;
const int SelectedRadius = 10;
const int minDist = 10;
const int UndoBuffSize = 100;
const int chartTextOffSetY = 3;
const int chartTextOffSetX = -2;
const int TextDefaultMargin = 5;
const int DefaultChartDataSize = 300;

static double Ratio[6] = { 0.0, 23.6, 38.2, 50.0, 61.8, 100.0 };