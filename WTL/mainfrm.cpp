// MainFrm.cpp : implementation of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <AtlCtrlx.h>				// for CWaitCursor

#include "resource.h"

#include "aboutdlg.h"
#include "IntelHex.h"
#include "NanoSimView.h"
#include "MainFrm.h"

BOOL CMainFrame::PreTranslateMessage(MSG* pMsg)
{
	if(CFrameWindowImpl<CMainFrame>::PreTranslateMessage(pMsg))
		return TRUE;

	return m_view.PreTranslateMessage(pMsg);
}

BOOL CMainFrame::OnIdle()
{
	UIUpdateToolBar();
	return FALSE;
}

LRESULT CMainFrame::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	// create command bar window
	HWND hWndCmdBar = m_CmdBar.Create(m_hWnd, rcDefault, NULL, ATL_SIMPLE_CMDBAR_PANE_STYLE);
	// attach menu
	m_CmdBar.AttachMenu(GetMenu());
	// load command bar images
	m_CmdBar.LoadImages(IDR_MAINFRAME);
	// remove old menu
	SetMenu(NULL);

	HWND hWndToolBar = CreateSimpleToolBarCtrl(m_hWnd, IDR_MAINFRAME, FALSE, ATL_SIMPLE_TOOLBAR_PANE_STYLE);

	CreateSimpleReBar(ATL_SIMPLE_REBAR_NOBORDER_STYLE);
	AddSimpleReBarBand(hWndCmdBar);
	AddSimpleReBarBand(hWndToolBar, NULL, TRUE);

	CreateSimpleStatusBar();

	m_hWndClient = m_view.Create(m_hWnd);

	UIAddToolBar(hWndToolBar);
	UISetCheck(ID_VIEW_TOOLBAR, 1);
	UISetCheck(ID_VIEW_STATUS_BAR, 1);

	// register object for message filtering and idle updates
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->AddMessageFilter(this);
	pLoop->AddIdleHandler(this);

	return 0;
}

LRESULT CMainFrame::OnFileExit(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	PostMessage(WM_CLOSE);
	return 0;
}

LRESULT CMainFrame::OnFileNew(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: add code to initialize document

	return 0;
}

BOOL CMainFrame::DoFileOpen(const char* filename)
{
	FILE* fp = fopen(filename, "r");
	if (fp != NULL)
	{
		TCHAR szLine[HEX_LINE_BUF];
		while (fgets(szLine, sizeof(szLine), fp) != NULL)
		{
			HEX_RECORD hexrec;

			int result = ParseIntelHex(szLine, &hexrec);
			if (result == HEX_DONE)
				break;
			if (result == HEX_DATA)
			{
				MemCopyBytes((NANO_ADDR) hexrec.addr, hexrec.buffer, hexrec.length);
			}
		}
		fclose(fp);
		return TRUE;
	}
	return FALSE;
}

const TCHAR szFilter[] = 
{
	"Hex files (*.hex)\0*.hex\0All files (*.*)\0*.*\0"
};

LRESULT CMainFrame::OnFileOpen(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CFileDialog dlg(/*bOpen=*/TRUE, /*szDefExt=*/ NULL, /*pszFileName=*/NULL, /*dwFlags=*/ 0, szFilter, m_hWnd);
	if (dlg.DoModal() == IDOK)
	{
		CWaitCursor wait;
		DoFileOpen(dlg.m_ofn.lpstrFile);
	}
	return 0;
}

LRESULT CMainFrame::OnFileNewWindow(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	::PostThreadMessage(_Module.m_dwMainThreadID, WM_USER, 0, 0L);
	return 0;
}

LRESULT CMainFrame::OnViewToolBar(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	static BOOL bVisible = TRUE;	// initially visible
	bVisible = !bVisible;
	CReBarCtrl rebar = m_hWndToolBar;
	int nBandIndex = rebar.IdToIndex(ATL_IDW_BAND_FIRST + 1);	// toolbar is 2nd added band
	rebar.ShowBand(nBandIndex, bVisible);
	UISetCheck(ID_VIEW_TOOLBAR, bVisible);
	UpdateLayout();
	return 0;
}

LRESULT CMainFrame::OnViewStatusBar(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	BOOL bVisible = !::IsWindowVisible(m_hWndStatusBar);
	::ShowWindow(m_hWndStatusBar, bVisible ? SW_SHOWNOACTIVATE : SW_HIDE);
	UISetCheck(ID_VIEW_STATUS_BAR, bVisible);
	UpdateLayout();
	return 0;
}

LRESULT CMainFrame::OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CAboutDlg dlg;
	dlg.DoModal();
	return 0;
}

LRESULT CMainFrame::OnDebugStepInto(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
    m_view.Step(NANO_STEP_INTO);
	return 0;
}

LRESULT CMainFrame::OnDebugStepOver(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
    m_view.Step(NANO_STEP_OVER);
	return 0;
}

LRESULT CMainFrame::OnDebugStepOut(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
    m_view.Step(NANO_STEP_OUT);
	return 0;
}

LRESULT CMainFrame::OnDebugShowNext(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	m_view.ShowNext();
	return 0;
}

LRESULT CMainFrame::OnDebugGo(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
    return 0;
}

LRESULT CMainFrame::OnDebugBreakpoint(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
    return 0;
}

