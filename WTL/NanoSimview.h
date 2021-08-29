// NanoSimView.h : interface of the CNanoSimView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_NANOSIMVIEW_H__87A472C4_A272_410C_B4F2_314ECD7DDB2A__INCLUDED_)
#define AFX_NANOSIMVIEW_H__87A472C4_A272_410C_B4F2_314ECD7DDB2A__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "NanoCpu.h"

class CNanoSimView : public CDialogImpl<CNanoSimView>
{
public:
	enum { IDD = IDD_NANOSIM_FORM };

	BOOL PreTranslateMessage(MSG* pMsg);

	BEGIN_MSG_MAP(CNanoSimView)
	MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
	MESSAGE_HANDLER(WM_DRAWITEM, OnDrawItem)
	END_MSG_MAP()

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)
	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnDrawItem(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	void Step(NANO_STEP step);
	void ShowNext(void);
protected:
	void GetView(void);
	void SetView(void);
    NANO_CPU m_cpu;
    BOOL CNanoSimView::GetDlgWord(UINT id, NANO_WORD* w, BOOL bHex);
	BOOL SetDlgWord(UINT id, NANO_WORD w, BOOL bHex);
    CListBox m_list;
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NANOSIMVIEW_H__87A472C4_A272_410C_B4F2_314ECD7DDB2A__INCLUDED_)
