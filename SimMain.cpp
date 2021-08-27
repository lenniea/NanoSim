/////////////////////////////////////////////////////////////////////////////
// Name:        NanoMain.cpp
// Purpose:     Nano Siimulator Main Window
// Author:      Lennie Araki
// Created:     20-Aug-2021
// Copyright:   (c) 2021 Lennie Araki. All Rights Reserved.
// Licence:     
/////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#include "NanoCpu.h"

#define NANO_MEM_WORDS	32768		// Full 32K x 16 (64K Bytes)

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/sizer.h"
#include "wx/gbsizer.h"
#include "wx/statline.h"
#include "wx/listctrl.h"


#include "SimMain.h"

#ifndef wxHAS_IMAGES_IN_RESOURCES
    #include "../sample.xpm"
#endif

typedef struct menu_res
{
	int id;
	const char* szMenu;
	const char* szHelp;
} MENU_ITEM;

// controls and menu constants
enum
{
	ID_FILE_NEW = 100,
	ID_FILE_OPEN,
	ID_FILE_EXIT = wxID_EXIT,

	ID_DEBUG_STEP_INTO = 200,
	ID_DEBUG_STEP_OVER,
	ID_DEBUG_STEP_OUT,
	ID_DEBUG_GO,
	ID_DEBUG_BREAKPT,

	ID_HELP_ABOUT = wxID_ABOUT
};

#define COUNT(a)	(sizeof(a) / sizeof(a[0]))

//
//	WIN32 LIKE MENU RESOURCE
//
MENU_ITEM menuFile[] =
{
	{ ID_FILE_NEW,		"&New", "New Simulation" },
	{ ID_FILE_OPEN,		"&Open...\t^O", "Open File" },
	{ ID_FILE_EXIT, 	"E&xit\tCtrl+Q", "Quit this program" }
};

MENU_ITEM menuDebug[] =
{
	{ ID_DEBUG_STEP_INTO,	"Step Into\tF11", "Step Into the next instruction" },
	{ ID_DEBUG_STEP_OVER, 	"Step Over\tF10", "Step Over the next instruction" },
	{ ID_DEBUG_STEP_OUT,	"Step Out",	"Step Out of the current function" },
	{ 0,					NULL,			NULL },
	{ ID_DEBUG_GO,			"Go\tF5",	"Start or continues execution", },
	{ ID_DEBUG_BREAKPT,		"Breakpoint\tF9",	"Insert or remove breakpoint" }
};

MENU_ITEM menuHelp[] =
{
	{ ID_HELP_ABOUT,		"&About Nano Sim",	"Displays program information, version and Copyright" },
};

typedef struct accel_item
{
	int id;
	int modifier;
	int key;
} ACCEL_ITEM;

ACCEL_ITEM accelTable[] =
{
	{ ID_FILE_NEW, wxACCEL_CTRL, 'N' },
	{ ID_FILE_OPEN, wxACCEL_CTRL, 'O' },
	{ ID_DEBUG_STEP_INTO, wxACCEL_NORMAL, WXK_F11 },
	{ ID_DEBUG_STEP_OVER, wxACCEL_NORMAL, WXK_F10 },
	{ ID_DEBUG_GO, wxACCEL_NORMAL, WXK_F5 },
	{ ID_DEBUG_BREAKPT, wxACCEL_NORMAL, WXK_F9 },
};

//Constructor, sets up virtual report list with 3 columns
MemListCtrl::MemListCtrl(wxWindow* parent, int numItems) :
wxListView(parent, wxID_ANY, wxDefaultPosition, wxSize(200, 100), wxLC_REPORT | wxLC_VIRTUAL | wxLC_SINGLE_SEL){
	// Add first column        
	wxListItem col0;
	col0.SetId(0);
	col0.SetText(_T("Addr"));
	col0.SetWidth(40);
	InsertColumn(0, col0);

	// Add second column 
	wxListItem col1;
	col1.SetId(1);
	col1.SetText(_T("Data"));
	col1.SetWidth(40);
	InsertColumn(1, col1);

	// Add third column      
	wxListItem col2;
	col2.SetId(2);
	col2.SetText(_T("Disassembly"));
	col2.SetWidth(100);
	InsertColumn(2, col2);

	SetItemCount(numItems);
}

//Overload virtual method of wxListView to provide text data for virtual list
wxString MemListCtrl::OnGetItemText(long item, long column) const {
	wxString str = "";
	char szDisAsm[40];
	NANO_ADDR addr = item * 2;
	NANO_INST opc;
	MemReadWord(addr, &opc);
	switch (column) {
	case 0:
		str = str.Format(_("%04x"), item * 2);
		break;
	case 1:
		str = str.Format(_("%04x"), opc);
		break;
	case 2:
		NanoDisAsm(szDisAsm, 20, addr, opc);
		str = str.FromAscii(szDisAsm);
		break;
	default:
		OutputDebugString(_T("Invalid column"));
	}
	return str;
}


// ----------------------------------------------------------------------------
// MyApp
// ----------------------------------------------------------------------------

wxIMPLEMENT_APP(MyApp);

bool MyApp::OnInit()
{
  if ( !wxApp::OnInit() )
      return false;

  // Create the main frame window
  MyFrame *frame = new MyFrame;
  frame->SetMinClientSize(wxSize(500, 400));

  frame->Show(true);

  return true;
}

// ----------------------------------------------------------------------------
// MyFrame
// ----------------------------------------------------------------------------

typedef enum wxwids
{
	wxID_R0 = 1000,
	wxID_CHECK_INP0 = 1018,
	wdID_CHECK_OUT0 = 1026
} wxIDS;

wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
// File Menu
EVT_MENU(ID_FILE_NEW, MyFrame::OnFileNew)
EVT_MENU(ID_FILE_OPEN, MyFrame::OnFileOpen)
// Debug Menu
EVT_MENU(ID_DEBUG_STEP_OVER, MyFrame::OnDebugStepOver)
EVT_MENU(ID_DEBUG_STEP_INTO, MyFrame::OnDebugStepInto)
EVT_MENU(ID_DEBUG_STEP_OUT, MyFrame::OnDebugStepOut)
EVT_MENU(ID_DEBUG_GO, MyFrame::OnDebugGo)

EVT_MENU(ID_HELP_ABOUT, MyFrame::OnAbout)
EVT_MENU(ID_FILE_EXIT, MyFrame::OnQuit)

wxEND_EVENT_TABLE()

const TCHAR* szRegName[18] =
{
	_T("R0"), _T("R1"), _T("R2"), _T("R3"),
	_T("R4"), _T("R5"), _T("R6"), _T("R7"),
	_T("R8"), _T("R9"), _T("R10"), _T("R11"),
	_T("R12"), _T("R13"), _T("R14"), _T("SP"),
	_T("PFX"), _T("CCR")
};

wxMenu* wxMakeMenu(MENU_ITEM* pMenu, int count)
{
	wxMenu* menu = new wxMenu;
	for (int i = 0; i < count; ++i)
	{
		const char* pszName = pMenu[i].szMenu;
		if (pszName != NULL)
		{
			menu->Append(pMenu[i].id, wxString(pszName), wxString(pMenu[i].szHelp));
		}
		else
		{
			menu->AppendSeparator();
		}
	}
	return menu;
}

// Define my frame constructor
MyFrame::MyFrame()
       : wxFrame(NULL, wxID_ANY, "Nano CPU Simulator")
{
    SetIcon(wxICON(sample));

    // Make a menubar
	wxMenu *file_menu = wxMakeMenu(menuFile, COUNT(menuFile));
	wxMenu* debug_menu = wxMakeMenu(menuDebug, COUNT(menuDebug));
	wxMenu *help_menu = wxMakeMenu(menuHelp, COUNT(menuHelp));

    wxMenuBar *menu_bar = new wxMenuBar;

    menu_bar->Append(file_menu, "&File");
	menu_bar->Append(debug_menu, "&Debug");
    menu_bar->Append(help_menu, "&Help");

    // Associate the menu bar with the frame
    SetMenuBar(menu_bar);

#if wxUSE_STATUSBAR
    CreateStatusBar(2);
    SetStatusText("Nano Simulator");
#endif // wxUSE_STATUSBAR

    wxPanel* p = new wxPanel(this, wxID_ANY);

    // we want to get a dialog that is stretchable because it
    // has a text ctrl in the middle. at the bottom, we have
    // two buttons which.

    wxBoxSizer *topsizer = new wxBoxSizer( wxVERTICAL );

    wxBoxSizer* horizSizer = new wxBoxSizer( wxHORIZONTAL );

	m_memory = new MemListCtrl(p, NANO_MEM_WORDS);
	NanoReset(&m_cpu);

    horizSizer->Add(m_memory, wxSizerFlags(1).Expand().Border(wxALL, 5));
    
    // 2) Layout Register Display in a 4 column x 9 row
    wxGridSizer *gridsizer = new wxFlexGridSizer(9,4, 4, 4);

	for (int c = 0; c < 2; ++c) {
		for (int r = 0; r < 9; ++r) {
            TCHAR id = c * 9 + r;
			// Create static text label to left of value
            gridsizer->Add(new wxStaticText(p, wxID_ANY, szRegName[id]),
                wxSizerFlags().Align(wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL));
			long style = wxBORDER | wxALIGN_CENTER_HORIZONTAL;
			if (id >= ID_IMM) {
				// Last 2 registers are readonly
				style |= wxTE_READONLY;
			}
			// Create value TextCtrl
			m_register[id] = new wxTextCtrl(p, wxID_R0 + id, "",
				wxDefaultPosition, wxDefaultSize, style), 
				wxSizerFlags().Align(wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL);
			;
			gridsizer->Add(m_register[id]);
        }
    }
	horizSizer->Add(gridsizer, wxSizerFlags(1).Expand().Border(wxALL, 5));
    topsizer->Add(horizSizer, wxSizerFlags(1).Proportion(1).Expand().Border(wxALL, 5));

	wxBoxSizer* iosizer = new wxBoxSizer(wxHORIZONTAL);
	// Create I/O checkboxes
	for (int i = 0; i < 16; ++i)
	{
		TCHAR szCheckName[10];
		szCheckName[0] = (i < 10) ? i + '0' : 'A' + i - 10;
		szCheckName[1] = '\0';
		m_iobox[i] = new wxCheckBox(p, wxID_CHECK_INP0 + i, szCheckName);
		iosizer->Add(m_iobox[i]);
	}
	topsizer->Add(iosizer, wxSizerFlags(1).Expand().Border(wxBOTTOM | wxLEFT | wxRIGHT, 5));

    wxTextCtrl* logger = new wxTextCtrl(p, wxID_ANY, "Log.",
                                 wxDefaultPosition, wxDefaultSize,
                                 wxTE_READONLY | wxTE_MULTILINE | wxSUNKEN_BORDER);

	topsizer->Add(logger, wxSizerFlags(1).Proportion(1).Expand().Border(wxALL, 5));

#if wxUSE_STATLINE
    // 3) middle: create wxStaticLine with minimum size (3x3)
    topsizer->Add(
        new wxStaticLine( p, wxID_ANY, wxDefaultPosition, wxSize(3,3), wxHORIZONTAL),
        wxSizerFlags().Expand());
#endif // wxUSE_STATLINE

    p->SetSizer( topsizer );




    // don't allow frame to get smaller than what the sizers tell it and also set
    // the initial size as calculated by the sizers
    topsizer->SetSizeHints( this );
}

void MyFrame::OnFileNew(wxCommandEvent& WXUNUSED(event))
{
	for (NANO_ADDR a = 0; a < NANO_MEM_WORDS; ++a)
	{
		MemWriteWord(a * 2, a * 17);
	}
	Refresh();
}

void MyFrame::OnFileOpen(wxCommandEvent& WXUNUSED(event))
{
	wxFileDialog* dialog = new wxFileDialog(
		this, _("Choose a file to open"), wxEmptyString, wxEmptyString,
		_("Binary Files (*.bin)|*.bin|Hex Files (*.hex)|*.hex"),
	wxFD_OPEN, wxDefaultPosition);

	// Creates a "open file" dialog with 4 file types
	if (dialog->ShowModal() == wxID_OK) // if the user click "Open" instead of "Cancel"
	{
		wxString path = dialog->GetPath();
		if (path.EndsWith(".bin"))
		{
			FILE* fp;
			NANO_ADDR addr = 0;
			if (fopen_s(&fp, path, "rb") == 0)
			{
				int words;
				do
				{
					NANO_WORD buffer[256];
					words = fread(buffer, sizeof(NANO_WORD), 256, fp);
					for (int i = 0; i < words; ++i)
					{
						MemWriteWord(addr++, buffer[i]);
					}
				} while (words > 0 && addr < NANO_MEM_WORDS);
				Refresh();
			}
		}
		else
		{
			wxMessageBox(".hex files not yet implemented", "ERROR", wxOK | wxCENTRE | wxICON_ERROR);
		}
	}

	// Clean up after ourselves
	dialog->Destroy();
}

void MyFrame::UpdateView(void)
{
	char szValue[10];
	for (int i = 0; i < 16; ++i)
	{
		sprintf(szValue, "%04x", m_cpu.reg[i]);
		m_register[i]->SetValue(szValue);
	}
	sprintf(szValue, "%04x", m_cpu.prefix);
	m_register[16]->SetValue(szValue);
	NANO_WORD ccr = m_cpu.ccr;
	sprintf(szValue, "%c %c %c %c",
		(ccr & NANO_N) ? 'N' : '-',
		(ccr & NANO_C) ? 'C' : '-',
		(ccr & NANO_V) ? 'V' : '-',
		(ccr & NANO_Z) ? 'Z' : '-');
	m_register[17]->SetValue(szValue);
	long index = m_cpu.pc >> 1;
	m_memory->Select(index);
	m_memory->Focus(index);

	Refresh();
}

void MyFrame::OnDebugStepOver(wxCommandEvent& WXUNUSED(event))
{
	NanoSimInst(&m_cpu, NANO_STEP_OVER);
	UpdateView();
}

void MyFrame::OnDebugStepInto(wxCommandEvent& WXUNUSED(event))
{
	NanoSimInst(&m_cpu, NANO_STEP_INTO);
	UpdateView();
}

void MyFrame::OnDebugStepOut(wxCommandEvent& WXUNUSED(event))
{
	NanoSimInst(&m_cpu, NANO_STEP_OUT);
	UpdateView();
}

void MyFrame::OnDebugGo(wxCommandEvent& WXUNUSED(event))
{
	// TODO: Fix this - for now just step 1000 instructions!
	for (int i = 0; i < 10000; ++i)
	{
		NanoSimInst(&m_cpu, NANO_STEP_INTO);
	}
	UpdateView();
}
	
void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    Close(true);
}

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event) )
{
    (void)wxMessageBox("Nano 16-bit CPU Simulator.\n",
            "About Nano Simulator", wxOK|wxICON_INFORMATION);
}
