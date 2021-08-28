/////////////////////////////////////////////////////////////////////////////
// Name:        layout.h
// Purpose:     Layout sample
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "NanoCpu.h"

#ifndef wxOVERRIDE
#define wxOVERRIDE
#endif

// Define a new application
class MyApp: public wxApp
{
public:
    MyApp(){}
    bool OnInit() wxOVERRIDE;
};

typedef enum id_regs
{
	ID_R0,	ID_R1,	ID_R2,	ID_R3,	ID_R4,	ID_R5,	ID_R6,	ID_R7,
	ID_R8,	ID_R9,	ID_R10, ID_R11,	ID_R12,	ID_R13,	ID_R14,	ID_R15,
	ID_IMM, ID_CCR,
	REGS
} ID_REGS;

class MemListCtrl : public wxListView
{
public:
	MemListCtrl(wxWindow* parent, int numitems);
	wxString OnGetItemText(long item, long column) const;
};

// the main frame class
class MyFrame : public wxFrame
{
	void UpdateView();
public:
	MyFrame();
	// File Menu
	void OnFileNew(wxCommandEvent& event);
	void OnFileOpen(wxCommandEvent& event);
	void OnFileSave(wxCommandEvent& event);
	// Debug Menu
	void OnDebugStepOver(wxCommandEvent& event);
	void OnDebugStepInto(wxCommandEvent& event);
	void OnDebugStepOut(wxCommandEvent& event);
	void OnDebugGo(wxCommandEvent& event);
	// Help Menu
	void OnAbout(wxCommandEvent& event);
    void OnQuit(wxCommandEvent& event);

private:
	NANO_CPU m_cpu;
	MemListCtrl* m_memory;
	wxTextCtrl* m_register[REGS];
	wxCheckBox* m_iobox[16];
    wxDECLARE_EVENT_TABLE();
};
