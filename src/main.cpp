/**********  Hashish - Cross platform hasing utility *************/
// --*-c++-*--
/*
  
    Hashish - Cross platform hasing utility
    Copyright (C) 2003  Jesse Lovelace, A. S. Logic Systems Co.
	jllovela@ncsu.edu
 
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include "wx/wx.h"
#include "wx/app.h"
#include "wx/frame.h"
#include "wx/utils.h"
#include "wx/menu.h"
#include "wx/msgdlg.h"
#include "wx/textdlg.h"
#include "wx/listbox.h"
#include "wx/filedlg.h"
#include "wx/choicdlg.h"
#include "wx/file.h"

#if (!defined (__WXMAC__)) && (!defined (__WXX11__))
#include "wx/dnd.h"
#endif

#include "wx/clipbrd.h"
#ifdef WIN32
#include "wx/msw/taskbar.h"
#endif
#include "hashish_wdr.h"

/* cryptoPP includes */
#include "cryptopp/misc.h"
#include "cryptopp/files.h"
#include "cryptopp/crc.h"
#include "cryptopp/hex.h"
#include "cryptopp/sha.h"
#include "cryptopp/md2.h"
#include "cryptopp/md5.h"
#include "cryptopp/ripemd.h"
#include "cryptopp/tiger.h"
#include "cryptopp/base64.h"
#include "cryptopp/panama.h"
#include "cryptopp/haval.h"
/* end cryptopp includes */

#include "gnu_small_trans.xpm"

#include <string>
#include <stdexcept>

using namespace std;
using namespace CryptoPP;

enum 
{   
    ID_TASKBAR_EXIT = 19000,
    ID_TASKBAR_RESTORE,
    ID_TASKBAR_LOAD
};
class HashFrame;
#ifdef WIN32
class HashTaskBar: public wxTaskBarIcon
{
public:
	HashTaskBar() {}
	virtual void OnLButtonUp(wxEvent&);
	virtual void OnRButtonUp(wxEvent&);
	void OnLButtonDClick(wxEvent&);
	void OnMenuRestore(wxCommandEvent&);
	void OnMenuExit(wxCommandEvent&);

	void setFrame(HashFrame * frame) { m_frame = frame; }

private:
	HashFrame * m_frame;

	DECLARE_EVENT_TABLE()
};
#endif

class MyApp : public wxApp
{
public:
    HashFrame * m_frame;

    virtual bool OnInit();

#ifdef WIN32
protected:
    HashTaskBar m_taskBarIcon;
#endif
};

class HashFrame : public wxFrame
{
public:
#if (!defined (__WXMAC__)) && (!defined (__WXX11__))
	friend class DnDFile;
#endif
	HashFrame(const wxString& title, const wxPoint& pos, const wxSize& size);

	void OnQuit(wxCommandEvent& event);
	void OnAbout(wxCommandEvent& event);
	void OnBrowseFiles(wxCommandEvent& event);
	void OnChangeSource(wxCommandEvent& event);
	void OnHash(wxCommandEvent& event);
	void OnHex(wxCommandEvent& event);
	void On64(wxCommandEvent& event);
	void OnCopy(wxCommandEvent& event);
	void OnPaste(wxCommandEvent& event);
	void OnCut(wxCommandEvent& event);
	void OnClear(wxCommandEvent& event);

private:

	bool m_hex;
	wxTextCtrl * m_filename;
	wxTextCtrl * m_string;
	wxTextCtrl * m_hashout;

	wxChoice * m_listhash;
	wxButton * m_browse;

	wxRadioBox * m_rbox;
	wxMenuBar * m_menu;
		
	DECLARE_EVENT_TABLE()
};

#if (!defined (__WXMAC__)) && (!defined (__WXX11__))
class DnDFile : public wxFileDropTarget
{

public:
    DnDFile(HashFrame *pOwner) { m_pOwner = pOwner; }

    virtual bool OnDropFiles(wxCoord x, wxCoord y,
                             const wxArrayString& filenames);

private:

    HashFrame *m_pOwner;
};

bool DnDFile::OnDropFiles(wxCoord, wxCoord, const wxArrayString& filenames)
{
    // size_t nFiles = filenames.GetCount();
    m_pOwner->m_filename->SetValue(filenames[0]);

    if ((m_pOwner->m_rbox->GetSelection() != 0))
        return TRUE;

    wxCommandEvent myevent(wxEVT_COMMAND_BUTTON_CLICKED,ID_HASH);

    m_pOwner->ProcessEvent(myevent);

    return TRUE;
}

#endif //!WXMAC && !WXX11

using namespace std;
using namespace CryptoPP;

IMPLEMENT_APP(MyApp)

bool MyApp::OnInit()
{
	m_frame = new HashFrame(_T("Hashish"), wxDefaultPosition, wxSize(500, 140));

#ifdef WIN32
	wxIcon appIcon;
	appIcon.CopyFromBitmap(wxBitmap(gnu_small_trans_xpm));
	m_taskBarIcon.SetIcon(appIcon, wxT("Hashish"));
	m_taskBarIcon.setFrame(m_frame);
#endif
   
	m_frame->Show(TRUE);
	SetTopWindow(m_frame);

	return TRUE;
}

BEGIN_EVENT_TABLE(HashFrame, wxFrame)
    EVT_MENU(ID_MENU_QUIT,  HashFrame::OnQuit)
    EVT_MENU(ID_MENU_OPEN, HashFrame::OnBrowseFiles)

    EVT_MENU(ID_MENU_HEX, HashFrame::OnHex)
    EVT_MENU(ID_MENU_64, HashFrame::On64)
    EVT_RADIOBOX(ID_HASH_SOURCE, HashFrame::OnChangeSource)
    EVT_BUTTON(ID_BUTTON_BROWSE, HashFrame::OnBrowseFiles)

    EVT_BUTTON(ID_HASH, HashFrame::OnHash)
    
    EVT_MENU(ID_TASKBAR_LOAD, HashFrame::OnBrowseFiles)
    EVT_MENU(ID_MENU_ABOUT, HashFrame::OnAbout)
    EVT_MENU(ID_MENU_EDIT_CUT, HashFrame::OnCut)
    EVT_MENU(ID_MENU_EDIT_PASTE, HashFrame::OnPaste)
    EVT_MENU(ID_MENU_EDIT_COPY, HashFrame::OnCopy)
    EVT_MENU(ID_MENU_EDIT_CLEAR, HashFrame::OnClear)
END_EVENT_TABLE()


HashFrame::HashFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
       : wxFrame((wxFrame *)NULL, -1, title, pos, size, wxMINIMIZE_BOX |
		       wxSYSTEM_MENU | wxCAPTION)
{
    wxIcon appIcon;
    appIcon.CopyFromBitmap(wxBitmap(gnu_small_trans_xpm));
    SetIcon(appIcon); 

    m_menu = MainMenu();
    SetMenuBar(m_menu);
    CreateStatusBar();
     
    wxPanel * pTopPanel = new wxPanel(this);
#if (!defined __WXMAC__) && (!defined (__WXX11__))
    pTopPanel->SetDropTarget(new DnDFile(this));
#endif

    wxSizer * pPanelSizer = MainDiag(pTopPanel, true,  true);  
    pPanelSizer->SetSizeHints(this);
    SetSize(pPanelSizer->GetMinSize());

    m_browse = (wxButton *)FindWindow(ID_BUTTON_BROWSE);
    m_filename = (wxTextCtrl *)FindWindow(ID_FILE_NAME);
    m_string = (wxTextCtrl *)FindWindow(ID_STRING);
    m_hashout = (wxTextCtrl *)FindWindow(ID_HASHOUT);
    m_listhash = (wxChoice *)FindWindow(ID_LIST_HASH);

    m_rbox = (wxRadioBox *)FindWindow(ID_HASH_SOURCE);

    m_hex = true;
    (m_menu->FindItem(ID_MENU_64))->Check(FALSE);
    (m_menu->FindItem(ID_MENU_HEX))->Check(TRUE);

    m_listhash->SetSelection(0);

    SetStatusText("Welcome to Hashish.");

}

void HashFrame::OnQuit(wxCommandEvent& event)
{
    Close(TRUE);
}


void HashFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxMessageBox(_T("Hashish 1.1\n© 2003 Jesse Lovelace\n"
			    "\nGNU General Public License\n\n"
			    "Visit the Hashish website:\n"
			    "http://hashish.sf.net"),
                 _T("About Hashish"), wxOK | wxICON_INFORMATION, this);
}

void HashFrame::OnCopy(wxCommandEvent& event)
{
    if (wxTheClipboard->Open())
    {
        wxTheClipboard->SetData( new wxTextDataObject(m_hashout->GetValue()));
        wxTheClipboard->Close();
    }
}
void HashFrame::OnPaste(wxCommandEvent& event)
{
    if (wxTheClipboard->Open())
    {
        if (wxTheClipboard->IsSupported( wxDF_TEXT ))
        {
            wxTextDataObject data;
            wxTheClipboard->GetData( data );
            m_string->SetValue(data.GetText());
        }     
        wxTheClipboard->Close();
    }

}
void HashFrame::OnCut(wxCommandEvent& event)
{
  if (wxTheClipboard->Open())
    {
        wxTheClipboard->SetData( new wxTextDataObject(m_hashout->GetValue()));
        wxTheClipboard->Close();
    }
    m_hashout->Clear();
}

void HashFrame::OnClear(wxCommandEvent& event)
{
   m_hashout->Clear();
}

void HashFrame::OnBrowseFiles(wxCommandEvent& event)
{

  if(m_rbox->GetSelection() != 0)
    return;

  wxString s = wxFileSelector("Choose a file to hash-ish");
  if (!s.IsEmpty())
  {
   if (m_filename)
     m_filename->SetValue(s);
  }

}

void HashFrame::OnChangeSource(wxCommandEvent& event)
{
    if(m_rbox->GetSelection() == 0)
    {
        m_string->Enable(FALSE);
        m_browse->Enable(TRUE);
        m_filename->Enable(TRUE);
        (m_menu->FindItem(ID_MENU_OPEN))->Enable(TRUE);
    }
    else
    {
        m_browse->Enable(FALSE);
        m_filename->Enable(FALSE);
        m_string->Enable(TRUE);
        (m_menu->FindItem(ID_MENU_OPEN))->Enable(FALSE);
    }

}

void HashFrame::OnHash(wxCommandEvent& event)
{

    if ((m_rbox->GetSelection() == 0))
    {
        if (!wxFile::Exists(m_filename->GetValue()))
        {
            wxLogError("That file does not exist, please choose another.");
            return;
        }
    }

    SetStatusText("Hashing...");
    enum { _SHA = 0, SHA_256, SHA_384, SHA_512, MD_2, MD_5, _HAVAL, HAVAL_3, HAVAL_4, HAVAL_5,
        RIPEMD_160, _TIGER, 
#ifndef __WXMAC__ 
		PANAMA_HASH,
#endif 
		_CRC32 };
    
    HashTransformation * hash;

    switch (m_listhash->GetSelection())
    {
    case(_SHA): hash = new SHA; break;
    case(SHA_256): hash = new SHA256; break;
    case(SHA_384): hash = new SHA384; break;
    case(SHA_512): hash = new SHA512; break;
    case(MD_2): hash = new MD2; break;
    case(MD_5): hash = new MD5; break;
    case(_HAVAL): hash = new HAVAL; break;
    case(HAVAL_3): hash = new HAVAL3; break;
    case(HAVAL_4): hash = new HAVAL4; break;
    case(HAVAL_5): hash = new HAVAL5; break;
    case(_CRC32): hash = new CRC32; break;
    case(RIPEMD_160): hash = new RIPEMD160; break;
    case(_TIGER): hash = new Tiger; break;
#ifndef __WXMAC__
#ifdef IS_LITTLE_ENDIAN
    case(PANAMA_HASH): hash = new PanamaHash<LittleEndian>; break; // true for big endian
#else
    case(PANAMA_HASH): hash = new PanamaHash<BigEndian>; break;
#endif
#endif
    default: throw runtime_error("Invalid Selection");
    }

    string outstring;

    HashFilter * hashFilter;

    if (m_hex)
        hashFilter = new HashFilter(*hash, new HexEncoder(new StringSink(outstring)));
    else
        hashFilter = new HashFilter(*hash, new Base64Encoder(new StringSink(outstring), false));
    
    if ((m_rbox->GetSelection() == 0))
        FileSource file1(m_filename->GetValue().c_str(), true, new Redirector(*hashFilter, false));
    else
        StringSource string1(m_string->GetValue().c_str(), true, new Redirector(*hashFilter, false));
    
    hashFilter->MessageEnd();

    delete hashFilter;
    delete hash;

    m_hashout->SetValue(outstring.c_str());
    SetStatusText("Hashing...Done!");
}

void HashFrame::On64(wxCommandEvent& event)
{
    (m_menu->FindItem(ID_MENU_64))->Check(TRUE);
    (m_menu->FindItem(ID_MENU_HEX))->Check(FALSE);
    m_hex = false;
}

void HashFrame::OnHex(wxCommandEvent& event)
{
    m_hex = true;
    (m_menu->FindItem(ID_MENU_64))->Check(FALSE);
    (m_menu->FindItem(ID_MENU_HEX))->Check(TRUE);

}

#ifdef WIN32
BEGIN_EVENT_TABLE(HashTaskBar, wxTaskBarIcon)
    EVT_MENU(ID_TASKBAR_RESTORE, HashTaskBar::OnMenuRestore)
    EVT_MENU(ID_TASKBAR_EXIT,    HashTaskBar::OnMenuExit)
END_EVENT_TABLE()

void HashTaskBar::OnMenuRestore(wxCommandEvent& )
{
    m_frame->Show(TRUE);
}

void HashTaskBar::OnMenuExit(wxCommandEvent& )
{
    m_frame->Close(TRUE);

    // Nudge wxWindows into destroying the dialog, since
    // with a hidden window no messages will get sent to put
    // it into idle processing.
    wxGetApp().ProcessIdle();
}


void HashTaskBar::OnLButtonUp(wxEvent&)
{
#if 0
    wxMenu      menu;

    menu.Append(ID_TASKBAR_RESTORE, "&Restore Hashish");
    menu.Append(ID_TASKBAR_LOAD, "&Load file to hash");
    menu.Append(ID_MENU_ABOUT, "&About Hashish");
    menu.AppendSeparator();
    menu.Append(ID_TASKBAR_EXIT,  "E&xit");

    PopupMenu(&menu);
#endif
}

void HashTaskBar::OnRButtonUp(wxEvent&)
{
    wxMenu      menu;

    menu.Append(ID_TASKBAR_RESTORE, "&Restore Hashish");
    menu.Append(ID_TASKBAR_LOAD, "&Load file to hash");
    menu.Append(ID_MENU_ABOUT, "&About Hashish");
    menu.AppendSeparator();
    menu.Append(ID_TASKBAR_EXIT,  "E&xit");

    PopupMenu(&menu);
}

void HashTaskBar::OnLButtonDClick(wxEvent&)
{
    if (m_frame->IsShown())
        m_frame->Show(FALSE);
    else
        m_frame->Show(TRUE);
}
#endif

