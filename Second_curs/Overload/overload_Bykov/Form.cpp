
#include <Windows.h>
#include <tchar.h>
#include "Form.h"

 // форма
CForm g_form;


CEditBox::CEditBox() :
	m_pForm (nullptr),
	m_id (0),
	m_type (vtUnknown),
	m_nMaxDigits (10),
	m_iPos (0),
	m_nTextFrontX (0),
	m_nTextBackX (0),
	m_fForceShowCursor (false)
{}


CEditBox::CEditBox (CForm& form_, int id_, LPCTSTR szCaption_, EValueType type_, int nMaxDigits_, const RECT& rect_, LPCTSTR szValue_) :
	m_pForm (&form_),
	m_id (id_),
	m_sCaption (szCaption_ ? szCaption_ : _T("")),
	m_type (type_),
	m_nMaxDigits (nMaxDigits_),
	m_rect (rect_),
	m_sText (szValue_),
	m_iPos (0),
	m_nTextFrontX (0),
	m_nTextBackX (0),
	m_fForceShowCursor (false)
{}


void CEditBox::Draw (HDC hdc_, bool fActive_, UINT nTimerCounter_) const
{
	int nOldBkMode = SetBkMode (hdc_, TRANSPARENT);
	COLORREF nOldTextColor = SetTextColor (hdc_, GetSysColor (COLOR_BTNTEXT));
	UINT nOldAlign = SetTextAlign (hdc_, TA_LEFT | TA_TOP);

	HPEN hRedPen = CreatePen(PS_SOLID, 0, RGB (255, 0, 0));

	HGDIOBJ hOldPen = SelectObject (hdc_, fActive_ ? hRedPen : GetStockObject (BLACK_PEN)),
			hOldBrush = SelectObject (hdc_, fActive_ ? GetSysColorBrush (COLOR_WINDOW) : GetStockObject (HOLLOW_BRUSH)),
			hOldFont = SelectObject (hdc_, Font());

	SIZE sizCapt = { 0, 0 },
		 sizText = { 0, 0 };
	GetTextExtentPoint (hdc_, m_sCaption.c_str(), (int) m_sCaption.length(), &sizCapt);
	GetTextExtentPoint (hdc_, m_sText.c_str(), (int) m_sText.length(), &sizText);

	UINT nCharWidth = CharWidth();

	TextOut (hdc_, m_rect.left, m_rect.top, m_sCaption.c_str(), (int) m_sCaption.length());
	Rectangle (hdc_, m_rect.left + sizCapt.cx, m_rect.top, m_rect.left + sizCapt.cx + sizText.cx + nCharWidth * 2, m_rect.bottom);
	TextOut (hdc_, m_rect.left + sizCapt.cx + nCharWidth / 2, m_rect.top, m_sText.c_str(), (int) m_sText.length());

	if (fActive_ && ((nTimerCounter_ & 1) || m_fForceShowCursor))
	{
		int x = m_iPos * CharWidth() + m_rect.left + sizCapt.cx + nCharWidth / 2;
		MoveToEx (hdc_, x, m_rect.top, nullptr);
		LineTo (hdc_, x, m_rect.bottom);
		MoveToEx (hdc_, x + 1, m_rect.top, nullptr);
		LineTo (hdc_, x + 1, m_rect.bottom);
	}

	SetBkMode (hdc_, nOldBkMode);
	SetTextColor (hdc_, nOldTextColor);
	SetTextAlign (hdc_, nOldAlign);
	SelectObject (hdc_, hOldPen);
	SelectObject (hdc_, hOldBrush);
	SelectObject (hdc_, hOldFont);

	DeleteObject (hRedPen);

	m_rect.right = m_rect.left + sizCapt.cx + sizText.cx + nCharWidth * 2;
	m_nTextFrontX = m_rect.left + sizCapt.cx + nCharWidth / 2;
	m_nTextBackX = m_rect.left + sizCapt.cx + sizText.cx + nCharWidth / 2;

	m_fForceShowCursor = false;
}


void CEditBox::OnLButtonDown (POINT pt_)
{
	if (pt_.x <= m_nTextFrontX)
	{
		m_iPos = 0;
		Invalidate();
	}
	else if (pt_.x >= m_nTextBackX)
	{
		m_iPos = m_sText.length();
		Invalidate();
	}
	else if (int cw = CharWidth())
	{
		m_iPos = (pt_.x - m_nTextFrontX) / cw;
		if (pt_.x - (m_nTextFrontX + m_iPos * cw) > cw / 2)
			m_iPos++;
		Invalidate();
	}
}


void CEditBox::OnKeyDown (int key_)
{
	switch (key_)
	{
		case VK_LEFT:
			if (m_iPos > 0)
			{
				m_iPos--;
				Invalidate();
			}
		break;

		case VK_RIGHT:
			if (m_iPos < (int) m_sText.length())
			{
				m_iPos++;
				Invalidate();
			}
		break;

		case VK_HOME:
			if (m_iPos > 0)
			{
				m_iPos = 0;
				Invalidate();
			}
		break;

		case VK_END:
			if (m_iPos < (int) m_sText.length())
			{
				m_iPos = (int) m_sText.length();
				Invalidate();
			}
		break;

		case VK_DELETE:
			if (m_iPos >= 0 && m_iPos < (int) m_sText.length())
			{
				m_sText.erase (m_sText.begin() + m_iPos);
				Invalidate();
				InvalidateTrussRect();
			}
		break;

		case VK_BACK:
			if (m_iPos > 0 && m_iPos <= (int) m_sText.length())
			{
				m_sText.erase (m_sText.begin() + --m_iPos);
				Invalidate();
				InvalidateTrussRect();
			}
		break;
	}
}


void CEditBox::OnChar (TCHAR char_)
{
	if (char_ >= _T(' ') && (int) m_sText.length() < m_nMaxDigits)
	{
		bool fValid = true;
		if (vtInt == m_type)
			fValid = _istdigit(char_) || _T('-') == char_;
		else if (vtHexInt == m_type)
			fValid = _istxdigit(char_) || _T('-') == char_;
		else if (vtReal == m_type)
			fValid = _istdigit(char_) || _T('+') == char_ || _T('-') == char_ || _T('.') == char_ || _T('e') == char_;
		else if (vtText == m_type)
			fValid = _istascii(char_);

		if (fValid)
		{
			if (m_iPos < 0)
				m_iPos = 0;
			if (m_iPos > (int) m_sText.length())
				m_iPos = (int) m_sText.length();
			m_sText.insert (m_sText.begin() + m_iPos++, char_);
			Invalidate();
			InvalidateTrussRect();
		}
	}
}


void CEditBox::Invalidate() const
{
	if (m_pForm)
	{
		m_fForceShowCursor = true;
		m_pForm->Redraw (*this);
	}
}


void CEditBox::InvalidateTrussRect() const
{
	if (m_pForm)
		m_pForm->InvalidateTrussRect();
}


HFONT CEditBox::Font() const
{
	return m_pForm ? m_pForm->Font() : nullptr;
}


UINT CEditBox::CharWidth() const
{
	return m_pForm ? m_pForm->CharWidth() : 1;
}


CForm::CForm() :
	m_hWnd (nullptr),
	m_hFont (nullptr),
	m_iActiveEditBox (-1),
	m_nFontHeight (0),
	m_nCharWidth (1),
	m_nTimerCounter (0),
	m_nEditBoxesHeight (0),
	m_hBmpSchema (nullptr)
{
	LOGFONT lf;
	GetObject (GetStockObject (DEFAULT_GUI_FONT), sizeof (lf), &lf);
	lf.lfHeight = LONG (lf.lfHeight * 1.5);
	m_nFontHeight = abs (lf.lfHeight);
	_tcscpy_s (lf.lfFaceName, _T("Consolas"));
	m_hFont = CreateFontIndirect (&lf);

	HDC hdc = GetDC (nullptr);
	HGDIOBJ hOldFont = SelectObject (hdc, m_hFont);
	SIZE sizC = { 0, 0 };
	GetTextExtentPoint (hdc, _T("0"), 1, &sizC);
	m_nCharWidth = sizC.cx;
	SelectObject (hdc, hOldFont);
	ReleaseDC (nullptr, hdc);
}


CForm::~CForm()
{
	if (m_hBmpSchema)
		DeleteObject (m_hBmpSchema);
}


void CForm::Init (HWND hWnd_, LPCTSTR szSchemaBmpFileName_)
{
	m_hWnd = hWnd_;
	if (szSchemaBmpFileName_ && *szSchemaBmpFileName_)
		m_hBmpSchema = static_cast <HBITMAP> (LoadImage (nullptr, szSchemaBmpFileName_, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION));
	SetTimer (m_hWnd, 1000, 500, nullptr);
}


void CForm::AddEditBox (int id_, LPCTSTR szCaption_, CEditBox::EValueType type_, int nMaxDigits_, LPCTSTR szValue_)
{
	RECT rect;
	rect.left = 12;
	rect.right = 12;
	rect.top = m_aEditBoxes.size() * LONG (m_nFontHeight * 1.2) + 12;
	rect.bottom = rect.top + LONG (m_nFontHeight * 1.2);
	m_aEditBoxes.push_back (CEditBox (*this, id_, szCaption_, type_, nMaxDigits_, rect, szValue_));

	m_nEditBoxesHeight = rect.bottom + 4;
	if (m_nEditBoxesHeight < 100)
		m_nEditBoxesHeight = 100;
}


const CEditBox* CForm::GetEditBox (int id_) const
{
	for (const auto& eb : m_aEditBoxes)
		if (eb.Id() == id_)
			return &eb;

	return nullptr;
}


void CForm::OnLButtonDown (WPARAM wParam_, LPARAM lParam_)
{
	POINT pt = { LOWORD (lParam_), HIWORD(lParam_) };

	const CEditBox* pActiveEdit = ActiveEditPtr();

	m_iActiveEditBox = -1;
	int i = 0;
	for (const auto& eb : m_aEditBoxes)
	{
		if (PtInRect (& eb.Rect(), pt))
		{
			m_iActiveEditBox = i;
			ActiveEditPtr()->OnLButtonDown (pt);
			break;
		}
		i++;
	}
	if (pActiveEdit)
		pActiveEdit->Invalidate();
	if (pActiveEdit = ActiveEditPtr())
		pActiveEdit->Invalidate();
}

void CForm::OnKeyDown (WPARAM wParam_, LPARAM lParam_)
{
	if (m_aEditBoxes.empty())
		return;

	if (VK_TAB == wParam_ || VK_RETURN == wParam_)
	{
		const CEditBox* pActiveEdit = ActiveEditPtr();

		if (m_iActiveEditBox < 0)
			m_iActiveEditBox = 0;
		else if (m_iActiveEditBox < (int) m_aEditBoxes.size() - 1)
			m_iActiveEditBox++;
		else
			m_iActiveEditBox = 0;

		if (pActiveEdit)
			pActiveEdit->Invalidate();
		if (pActiveEdit = ActiveEditPtr())
			pActiveEdit->Invalidate();
	}
	else if (CEditBox* pActiveEdit = ActiveEditPtr())
		pActiveEdit->OnKeyDown (wParam_);
}


void CForm::OnChar (WPARAM wParam_, LPARAM lParam_)
{
	if (CEditBox* pActiveEdit = ActiveEditPtr())
		pActiveEdit->OnChar (wParam_);
}


void CForm::OnPaint (WPARAM wParam_, LPARAM lParam_)
{
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(m_hWnd, &ps);

	int i = 0;
	for (auto& eb : m_aEditBoxes)
	{
		eb.Draw (hdc, i == m_iActiveEditBox, m_nTimerCounter);
		i++;
	}

	RECT rect;
	GetClientRect (m_hWnd, &rect);

	HGDIOBJ hOldPen = SelectObject (hdc, GetStockObject (BLACK_PEN));

	MoveToEx (hdc, 4, m_nEditBoxesHeight, nullptr);
	LineTo (hdc, rect.right - 4, m_nEditBoxesHeight);
	int xMid = (rect.left + rect.right) / 2;
	MoveToEx (hdc, xMid, 4, nullptr);
	LineTo (hdc, xMid, m_nEditBoxesHeight - 4);

	SelectObject (hdc, hOldPen);

	if (m_hBmpSchema)
	{
		BITMAP bmp;
		GetObject (m_hBmpSchema, sizeof (bmp), &bmp);

		int dstW = xMid - 8,
			dstH = m_nEditBoxesHeight - 8,
			dx = 0,
			dy = 0;

		int w = bmp.bmWidth,
			h = bmp.bmHeight,
			rw = dstW,
			rh = dstH;
		if (w && h && rw && rh)
		{
			double wh = double (w) / h,
				   rwh = double (rw) / rh;
			if (rwh > wh)
			{
				dx = (rw - int (rh * wh)) / 2;
				dstW -= dx * 2;
			}
			else
			{
				dy = (rh - int (rw / wh)) / 2;
				dstH -= dy * 2;
			}
			int nOldSbm = SetStretchBltMode (hdc, HALFTONE);
			HDC hdcMem = CreateCompatibleDC (hdc);
			SelectObject (hdcMem, m_hBmpSchema);
			StretchBlt (hdc, xMid + 4 + dx, 4 + dy, dstW, dstH, hdcMem, 0, 0, w, h, SRCCOPY);
			DeleteDC (hdcMem);
			SetStretchBltMode (hdc, nOldSbm);
		}
	}

	RECT rectTruss;
	rectTruss.left = rect.left + 4;
	rectTruss.right = rect.right - 4;
	rectTruss.top = m_nEditBoxesHeight + 4;
	rectTruss.bottom = rect.bottom - 4;
	DrawTruss (hdc, rectTruss);

	EndPaint (m_hWnd, &ps);

	 // параллельно записать в EMF
	CreateEmfFile();
}


void CForm::OnTimer (WPARAM wParam_, LPARAM lParam_)
{
	m_nTimerCounter++;
	if (CEditBox* pActiveEdit = ActiveEditPtr())
		Redraw(*pActiveEdit);
}


void CForm::OnGetMinMaxInfo (WPARAM wParam_, LPARAM lParam_)
{
	LPMINMAXINFO pmmi = reinterpret_cast <LPMINMAXINFO> (lParam_);
	pmmi->ptMinTrackSize.x = 400;
	pmmi->ptMinTrackSize.y = m_nEditBoxesHeight * 2 + 32;
}


void CForm::Redraw (const CEditBox& item_) const
{
	RECT rect = item_.Rect();
	rect.right += m_nCharWidth + 1;
	InvalidateRect (m_hWnd, &rect, TRUE);
}


void CForm::GetTrussRect (RECT& rect_)
{
	RECT rectClient;
	GetClientRect (m_hWnd, &rectClient);

	rect_.left = rectClient.left + 4;
	rect_.right = rectClient.right - 4;
	rect_.top = m_nEditBoxesHeight + 4;
	rect_.bottom = rectClient.bottom - 4;
}


void CForm::InvalidateTrussRect()
{
	RECT rect;
	GetTrussRect (rect);
	InflateRect (&rect, 4, 4);
	InvalidateRect (m_hWnd, &rect, TRUE);
}


HWND GetFormHwnd()
{
	return g_form.Hwnd();
}


HFONT GetFormFont()
{
	return g_form.Font();
}


void AddFormEditBox (int id_, LPCTSTR szCaption_, CEditBox::EValueType type_, int nMaxDigits_, LPCTSTR szValue_)
{
	g_form.AddEditBox (id_, szCaption_, type_, nMaxDigits_, szValue_);
}



bool GetEditIntValue (int id_, int& nValue_, int nRadix_)
{
	if (const CEditBox* peb = g_form.GetEditBox (id_))
	{
		if (peb->Text().empty())
			return false;

		LPTSTR pcEnd = nullptr;
		long val = _tcstol (peb->Text().c_str(), &pcEnd, nRadix_);
		if (pcEnd && *pcEnd)
			return false;

		nValue_ = (int) val;
		return true;
	}
	return false;
}


bool GetEditValue (int id_, int& nValue_)
{
	return GetEditIntValue (id_, nValue_, 10);
}


bool GetEditHexValue (int id_, int& nValue_)
{
	return GetEditIntValue (id_, nValue_, 16);
}


bool GetEditValue (int id_, double& dValue_)
{
	if (const CEditBox* peb = g_form.GetEditBox (id_))
	{
		if (peb->Text().empty())
			return false;

		LPTSTR pcEnd = nullptr;
		double val = _tcstod (peb->Text().c_str(), &pcEnd);
		if (pcEnd && *pcEnd)
			return false;

		dValue_ = val;
		return true;
	}
	return false;
}
bool GetEditValue(int id_, char*& dValue_)
{
	if (const CEditBox* peb = g_form.GetEditBox(id_))
	{
		if (peb->Text().empty())
			return false;

		LPTSTR pcEnd = nullptr;
		wchar_t* wString = (wchar_t*)peb->Text().c_str();
		int i = 0;
		while (wString[i] != '\0')
			i++;
		if (pcEnd && *pcEnd)
			return false;

		MultiByteToWideChar(CP_ACP, 0, dValue_, -1, wString, i);

		return true;
	}
	return false;
}


void FormOn_WM_CREATE (HWND hWnd_, WPARAM wParam_, LPARAM lParam_, LPCTSTR szSchemaBmpFileName_)
{
	g_form.Init (hWnd_, szSchemaBmpFileName_);
}


void FormOn_WM_LBUTTONDOWN (WPARAM wParam_, LPARAM lParam_)
{
	g_form.OnLButtonDown (wParam_, lParam_);
}


void FormOn_WM_KEYDOWN (WPARAM wParam_, LPARAM lParam_)
{
	g_form.OnKeyDown (wParam_, lParam_);
}


void FormOn_WM_CHAR (WPARAM wParam_, LPARAM lParam_)
{
	g_form.OnChar (wParam_, lParam_);
}


void FormOn_WM_PAINT (WPARAM wParam_, LPARAM lParam_)
{
	g_form.OnPaint (wParam_, lParam_);
}


void FormOn_WM_TIMER (WPARAM wParam_, LPARAM lParam_)
{
	g_form.OnTimer (wParam_, lParam_);
}


void FormOn_WM_GETMINMAXINFO (WPARAM wParam_, LPARAM lParam_)
{
	g_form.OnGetMinMaxInfo (wParam_, lParam_);
}