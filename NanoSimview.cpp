// NanoSimView.cpp : implementation of the CNanoSimView class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"

#include "NanoSimView.h"

BOOL CNanoSimView::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_CHAR)
	{
		HWND hEdit = GetDlgItem(IDC_TEXT_OUT);
		if (GetFocus() == hEdit)
		{
			TCHAR szText[2];
			szText[0] = pMsg->wParam;
			szText[1] = '\0';
			SendMessage(hEdit, EM_REPLACESEL, FALSE, (LPARAM) szText);
		}
		return TRUE;
	}
	return CWindow::IsDialogMessage(pMsg);
}

LRESULT CNanoSimView::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    m_list = GetDlgItem(IDC_MEMORY);
    for (NANO_ADDR a = 0; a < 32768; ++a)
    {
#ifdef _DEBUG
        MemWriteWord(a << 1, a << 1);
#endif
        m_list.AddString(NULL);
    }
    NanoReset(&m_cpu);
    SetView();
	return 0;
}

LRESULT CNanoSimView::OnDrawItem(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    LPDRAWITEMSTRUCT lpDIS = (LPDRAWITEMSTRUCT) lParam;
    NANO_ADDR addr = lpDIS->itemID * 2;
    NANO_INST inst;
    char szText[80];

	MemReadWord(addr, &inst);	// ignore number of cycles

    const BOOL bSel = lpDIS->itemState & ODS_SELECTED;
    const COLORREF rgbText = ::GetSysColor(bSel ? COLOR_HIGHLIGHTTEXT : COLOR_WINDOWTEXT);
    const COLORREF rgbBack = ::GetSysColor(bSel ? COLOR_HIGHLIGHT : COLOR_WINDOW);

    CDCHandle dc(lpDIS->hDC);
    RECT rect;
    rect = lpDIS->rcItem;

    int iSaveDC = dc.SaveDC();
    dc.SetTextColor(rgbText);
    dc.SetBkColor(rgbBack);
    int width = lpDIS->rcItem.right + lpDIS->rcItem.left;
    int div = lpDIS->rcItem.left + MulDiv(width, 45, 100);

    // Draw address and hex opcode on left side
    rect.right = div;
    int count = wsprintf(szText, NANO_SZADDR ":%04x  ", addr, inst);
    dc.ExtTextOut(rect.left, rect.top, ETO_OPAQUE, &rect, szText, count, NULL);

    // Draw disassembly on right side
    rect.left = div;
    rect.right = lpDIS->rcItem.right;
    count = NanoDisAsm(addr, szText);
    dc.ExtTextOut(rect.left, rect.top, ETO_OPAQUE, &rect, szText, count, NULL);

    // Draw focus rectangle
    rect.left = lpDIS->rcItem.left;
    if (lpDIS->itemState & ODS_FOCUS)
    {
        dc.DrawFocusRect(&rect);
    }
    dc.RestoreDC(iSaveDC);

	return 0;
}

int ConvDigit(int ch)
{
    if (ch >= '0' && ch <= '9')
        return ch - '0';
    ch = tolower(ch);
    if (ch >= 'a' && ch <= 'z')
        return ch - 'a' + 10;
    return -1;
}

unsigned long ParseLong(char* pText, BOOL* bError, int base)
{
    unsigned long l = 0;
    *bError = FALSE;

    int ch;

    /* Skip leading blanks */
    do
        ch = *pText++;
    while (ch == ' ');

    /* Convert digits */
    while (ch != '\0')
    {
        int digit = ConvDigit(ch);
        if (digit < 0 || digit >= base)
        {
            *bError = TRUE;
            break;
        }
        l = (l * base) + digit;
        ch = *pText++;
    }
    return l;
}

BOOL CNanoSimView::GetDlgWord(UINT id, NANO_WORD* pWord, BOOL bHex)
{
    char szText[80];
	BOOL result = GetDlgItemText(id, szText, sizeof(szText));
	if (result)
    {
        BOOL bError;
        NANO_WORD w = ParseLong(szText, &bError, bHex ? 16 : 10);
        if (!bError)
            *pWord = w;
	}
	return result;
}

BOOL CNanoSimView::SetDlgWord(UINT id, NANO_WORD w, BOOL bHex)
{
    TCHAR szText[16];
    int length = wsprintf(szText, bHex ? NANO_SZADDR : "%u", w);
    return SetDlgItemText(id, szText);
}

void CNanoSimView::GetView()
{
    for (int r = 0; r < 16; ++r)
    {
        GetDlgWord(IDC_R0 + r, &m_cpu.reg[r], TRUE);
    }
    m_cpu.pc = m_list.GetCurSel() << 1;

    /* Update Switch inputs */
    for (int bit = 0; bit < 8; ++bit)
    {
        if (IsDlgButtonChecked(IDC_SW0 + bit))
            sw_inp |= (1 << bit);
        else
            sw_inp &= ~(1 << bit);
    }
}

void CNanoSimView::SetView()
{
    for (int r = 0; r < 16; ++r)
    {
        SetDlgWord(IDC_R0 + r, m_cpu.reg[r], TRUE);
    }
    SetDlgWord(IDC_PREFIX, m_cpu.prefix, TRUE);

    TCHAR szText[10];
    NANO_WORD ccr = m_cpu.ccr;
    wsprintf(szText, "%c %c %c %c",
        (ccr & NANO_N) ? 'N' : '-',
        (ccr & NANO_C) ? 'C' : '-',
        (ccr & NANO_V) ? 'V' : '-',
        (ccr & NANO_Z) ? 'Z' : '-');
    SetDlgItemText(IDC_CCR, szText);
    m_list.SetCurSel(m_cpu.pc >> 1);
    /* Update LED outputs */
    for (int bit = 0; bit < 8; ++bit)
    {
        if (led_out & (1 << bit))
            CheckDlgButton(IDC_LED0 + bit, BST_CHECKED);
        else
            CheckDlgButton(IDC_LED0 + bit, BST_UNCHECKED);
    }
	/* Update Cycle Count */
	SetDlgItemInt(IDC_CYCLES, m_cpu.cycles);
}

void CNanoSimView::Step(NANO_STEP step)
{
    GetView();
    NanoSimInst(&m_cpu, step);
    SetView();
}

void CNanoSimView::ShowNext(void)
{
	int index = m_list.GetCurSel();
	m_list.SetCurSel(index);
}
