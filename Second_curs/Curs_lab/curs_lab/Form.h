
#pragma once

#include <vector>
#include <string>


#ifdef _UNICODE
	typedef std::wstring tstring;
#else
	typedef std::string tstring;
#endif

 //
 // область отсечения модели
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
 // поле редактирования
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

	 // форма-владелец
	class CForm* m_pForm;

	 // идентификатор
	int m_id;

	 // строки комментария и значения
	tstring m_sCaption, m_sText;

	 // тип значения
	EValueType m_type;

	 // допустимая длина строки
	int m_nMaxDigits;

	 // область комментария и значения
	mutable RECT m_rect;

	 // координаты x начала и конца строки
	mutable int m_nTextFrontX, m_nTextBackX;

	 // позиция курсора в строке
	int m_iPos;

	  // при очередной перерисовке показать курсор вне зависимости от таймера
	mutable bool m_fForceShowCursor;

public:

	CEditBox();
	CEditBox (CForm& form_, int id_, LPCTSTR szCaption_, EValueType type_, int nMaxDigits_, const RECT& rect_, LPCTSTR szValue_);

	 // отрисовка
	void Draw (HDC hdc_, bool fActive_, UINT nTimerCounter_) const;

	 // -- обработчики сообщений --
	void OnLButtonDown (POINT pt_);
	void OnKeyDown (int key_);
	void OnChar (TCHAR char_);

	 // обновление
	void Invalidate() const;
	void InvalidateTrussRect() const;

	 // -- получение значений полей и параметров --

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
 // форма - набор полей редактирования и рисунок
 //
class CForm
{
protected:

	 // окно
	HWND m_hWnd;

	 // шрифт
	HFONT m_hFont;

	 // данные о полях редактирования
	std::vector <CEditBox> m_aEditBoxes;

	 // активное поле редактирования
	int m_iActiveEditBox;

	 // параметры шрифта, счётчик таймера
	UINT m_nFontHeight, m_nCharWidth, m_nTimerCounter;

	 // высота области ввода значений
	int m_nEditBoxesHeight;

	 // BMP схемы
	HBITMAP m_hBmpSchema;

public:

	CForm();
	~CForm();

	 // инициализация
	void Init (HWND hWnd_, LPCTSTR szSchemaBmpFileName_);

	 // добавление поля редактирования
	void AddEditBox (int id_, LPCTSTR szCaption_, CEditBox::EValueType type_, int nMaxDigits_, LPCTSTR szValue_);

	 // получение поля редактирования
	const CEditBox* GetEditBox (int id_) const;

	 // -- обработчики сообщений --
	void OnLButtonDown (WPARAM wParam_, LPARAM lParam_);
	void OnKeyDown (WPARAM wParam_, LPARAM lParam_);
	void OnChar (WPARAM wParam_, LPARAM lParam_);
	void OnPaint (WPARAM wParam_, LPARAM lParam_);
	void OnTimer (WPARAM wParam_, LPARAM lParam_);
	void OnGetMinMaxInfo (WPARAM wParam_, LPARAM lParam_);

	 // перерисовка поля редактирования
	void Redraw (const CEditBox& item_) const;

	 // получение прямоугольника для вывода изображения фермы
	void GetTrussRect (RECT& rect_);

	 // обновление изображения фермы
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


 // получение HWND окна
HWND GetFormHwnd();

 // получение шрифта формы
HFONT GetFormFont();

 // добавление в форму поля редактирования
void AddFormEditBox (int id_, LPCTSTR szCaption_, CEditBox::EValueType type_, int nMaxDigits_, LPCTSTR szValue_);

 // -- получение значения из поля редактирования --
bool GetEditValue (int id_, int& nValue_);
bool GetEditHexValue (int id_, int& nValue_);
bool GetEditValue (int id_, double& dValue_);

 // -- рисование фермы --
void SetModelViewBox (ModelViewBox& MinMax_, double x_min_, double y_min_, double x_max_, double y_max_);
void DrawTruss (HDC hdc_, const RECT& rect_);

 // выбор имени файла, экспорт изображения в emf
void SelectEmfFileName (TCHAR acFileName_ [/*MAX_PATH*/]);
void CreateEmfFile();

 // -- обработчики сообщений формы --
void FormOn_WM_CREATE (HWND hWnd_, WPARAM wParam_, LPARAM lParam_, LPCTSTR szSchemaBmpFileName_);
void FormOn_WM_LBUTTONDOWN (WPARAM wParam_, LPARAM lParam_);
void FormOn_WM_KEYDOWN (WPARAM wParam_, LPARAM lParam_);
void FormOn_WM_CHAR (WPARAM wParam_, LPARAM lParam_);
void FormOn_WM_PAINT (WPARAM wParam_, LPARAM lParam_);
void FormOn_WM_TIMER (WPARAM wParam_, LPARAM lParam_);
void FormOn_WM_GETMINMAXINFO (WPARAM wParam_, LPARAM lParam_);
