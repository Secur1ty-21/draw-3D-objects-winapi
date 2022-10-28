
#pragma once

#include <vector>
#include <string>


#ifdef _UNICODE
	typedef std::wstring tstring;
#else
	typedef std::string tstring;
#endif

 //
 // ������� ��������� ������
 //
struct ModelViewBox
{
	double x_min, y_min, x_max, y_max;

	ModelViewBox (double x_min_ = 0.0, double y_min_ = 0.0, double x_max_ = 1.0, double y_max_ = 1.0) :
		x_min (x_min_),
		y_min (y_min_),
		x_max (x_max_),
		y_max (y_max_)
	{}
};

 //
 // ���� ��������������
 //
class CEditBox
{
public:

	enum EValueType
	{
		vtUnknown,
		vtInt,
		vtHexInt,
		vtReal,
	};

protected:

	 // �����-��������
	class CForm* m_pForm;

	 // �������������
	int m_id;

	 // ������ ����������� � ��������
	tstring m_sCaption, m_sText;

	 // ��� ��������
	EValueType m_type;

	 // ���������� ����� ������
	int m_nMaxDigits;

	 // ������� ����������� � ��������
	mutable RECT m_rect;

	 // ���������� x ������ � ����� ������
	mutable int m_nTextFrontX, m_nTextBackX;

	 // ������� ������� � ������
	int m_iPos;

	  // ��� ��������� ����������� �������� ������ ��� ����������� �� �������
	mutable bool m_fForceShowCursor;

public:

	CEditBox();
	CEditBox (CForm& form_, int id_, LPCTSTR szCaption_, EValueType type_, int nMaxDigits_, const RECT& rect_, LPCTSTR szValue_);

	 // ���������
	void Draw (HDC hdc_, bool fActive_, UINT nTimerCounter_) const;

	 // -- ����������� ��������� --
	void OnLButtonDown (POINT pt_);
	void OnKeyDown (int key_);
	void OnChar (TCHAR char_);

	 // ����������
	void Invalidate() const;
	void InvalidateTrussRect() const;

	 // -- ��������� �������� ����� � ���������� --

	CForm* FormPtr() const
	{
		return m_pForm;
	}
	
	int Id() const
	{
		return m_id;
	}

	const tstring& Caption() const
	{
		return m_sCaption;
	}

	const tstring& Text() const
	{
		return m_sText;
	}

	EValueType Type() const
	{
		return m_type;
	}

	int MaxDigits() const
	{
		return m_nMaxDigits;
	}

	const RECT& Rect() const
	{
		return m_rect;
	}

	int Pos() const
	{
		return m_iPos;
	}

	HFONT Font() const;
	UINT CharWidth() const;
};

 //
 // ����� - ����� ����� �������������� � �������
 //
class CForm
{
protected:

	 // ����
	HWND m_hWnd;

	 // �����
	HFONT m_hFont;

	 // ������ � ����� ��������������
	std::vector <CEditBox> m_aEditBoxes;

	 // �������� ���� ��������������
	int m_iActiveEditBox;

	 // ��������� ������, ������� �������
	UINT m_nFontHeight, m_nCharWidth, m_nTimerCounter;

	 // ������ ������� ����� ��������
	int m_nEditBoxesHeight;

	 // BMP �����
	HBITMAP m_hBmpSchema;

public:

	CForm();
	~CForm();

	 // �������������
	void Init (HWND hWnd_, LPCTSTR szSchemaBmpFileName_);

	 // ���������� ���� ��������������
	void AddEditBox (int id_, LPCTSTR szCaption_, CEditBox::EValueType type_, int nMaxDigits_, LPCTSTR szValue_);

	 // ��������� ���� ��������������
	const CEditBox* GetEditBox (int id_) const;

	 // -- ����������� ��������� --
	void OnLButtonDown (WPARAM wParam_, LPARAM lParam_);
	void OnKeyDown (WPARAM wParam_, LPARAM lParam_);
	void OnChar (WPARAM wParam_, LPARAM lParam_);
	void OnPaint (WPARAM wParam_, LPARAM lParam_);
	void OnTimer (WPARAM wParam_, LPARAM lParam_);
	void OnGetMinMaxInfo (WPARAM wParam_, LPARAM lParam_);

	 // ����������� ���� ��������������
	void Redraw (const CEditBox& item_) const;

	 // ��������� �������������� ��� ������ ����������� �����
	void GetTrussRect (RECT& rect_);

	 // ���������� ����������� �����
	void InvalidateTrussRect();

	HWND Hwnd() const
	{
		return m_hWnd;
	}

	HFONT Font() const
	{
		return m_hFont;
	}

	CEditBox* ActiveEditPtr() const
	{
		return const_cast <CEditBox *> (m_iActiveEditBox >= 0 && m_iActiveEditBox < (int) m_aEditBoxes.size() ? & m_aEditBoxes [m_iActiveEditBox] : nullptr);
	}

	UINT CharWidth() const
	{
		return m_nCharWidth;
	}
};


 // ��������� HWND ����
HWND GetFormHwnd();

 // ��������� ������ �����
HFONT GetFormFont();

 // ���������� � ����� ���� ��������������
void AddFormEditBox (int id_, LPCTSTR szCaption_, CEditBox::EValueType type_, int nMaxDigits_, LPCTSTR szValue_);

 // -- ��������� �������� �� ���� �������������� --
bool GetEditValue (int id_, int& nValue_);
bool GetEditHexValue (int id_, int& nValue_);
bool GetEditValue (int id_, double& dValue_);

 // -- ��������� ����� --
void SetModelViewBox (ModelViewBox& MinMax_, double x_min_, double y_min_, double x_max_, double y_max_);
void DrawTruss (HDC hdc_, const RECT& rect_);

 // ����� ����� �����, ������� ����������� � emf
void SelectEmfFileName (TCHAR acFileName_ [/*MAX_PATH*/]);
void CreateEmfFile();

 // -- ����������� ��������� ����� --
void FormOn_WM_CREATE (HWND hWnd_, WPARAM wParam_, LPARAM lParam_, LPCTSTR szSchemaBmpFileName_);
void FormOn_WM_LBUTTONDOWN (WPARAM wParam_, LPARAM lParam_);
void FormOn_WM_KEYDOWN (WPARAM wParam_, LPARAM lParam_);
void FormOn_WM_CHAR (WPARAM wParam_, LPARAM lParam_);
void FormOn_WM_PAINT (WPARAM wParam_, LPARAM lParam_);
void FormOn_WM_TIMER (WPARAM wParam_, LPARAM lParam_);
void FormOn_WM_GETMINMAXINFO (WPARAM wParam_, LPARAM lParam_);
