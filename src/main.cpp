/**********  Hashish - Cross platform hasing utility *************/

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

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
#include "wx/dnd.h"

#include "hashish_wdr.h"
#include "crypto/cryptlib.h"
#include "crypto/files.h"

#include "crypto/hex.h"
#include "crypto/rc2.h"
#include "crypto/rc5.h"
#include "crypto/sha.h"
#include "crypto/md2.h"
#include "crypto/md5.h"
#include "crypto/ripemd.h"
#include "crypto/tiger.h"
#include "crypto/base64.h"
#include "crypto/panama.h"
#include "crypto/haval.h"
#include "crypto/crc.h"
#include "crypto/sapphire.h"

#include "gnu_small_trans.xpm"

#include <string>

class MyApp : public wxApp
{
public:
  
  virtual bool OnInit();
};

class MyFrame : public wxFrame
{
public:

  friend class DnDFile;

  MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size);

  void OnQuit(wxCommandEvent& event);
  void OnAbout(wxCommandEvent& event);
  void OnBrowseFiles(wxCommandEvent& event);
  void OnChangeSource(wxCommandEvent& event);
  void OnHash(wxCommandEvent& event);
  void OnHex(wxCommandEvent& event);
  void On64(wxCommandEvent& event);

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

class DnDFile : public wxFileDropTarget
{

public:
  DnDFile(MyFrame *pOwner) { m_pOwner = pOwner; }

  virtual bool OnDropFiles(wxCoord x, wxCoord y,
                             const wxArrayString& filenames);

private:

  MyFrame *m_pOwner;
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

using namespace std;
using namespace CryptoPP;

IMPLEMENT_APP(MyApp)

bool MyApp::OnInit()
{
    MyFrame *frame = new MyFrame(_T("Hashish"),
                                 wxDefaultPosition, wxSize(500, 140));

    frame->Show(TRUE);
    SetTopWindow(frame);

    return TRUE;
}

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(ID_MENU_QUIT,  MyFrame::OnQuit)
    EVT_MENU(ID_MENU_OPEN, MyFrame::OnBrowseFiles)

    EVT_MENU(ID_MENU_HEX, MyFrame::OnHex)
    EVT_MENU(ID_MENU_64, MyFrame::On64)
    EVT_RADIOBOX(ID_HASH_SOURCE, MyFrame::OnChangeSource)
    EVT_BUTTON(ID_BUTTON_BROWSE, MyFrame::OnBrowseFiles)

    EVT_BUTTON(ID_HASH, MyFrame::OnHash)
    
    EVT_MENU(ID_MENU_ABOUT, MyFrame::OnAbout)

END_EVENT_TABLE()


MyFrame::MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
       : wxFrame((wxFrame *)NULL, -1, title, pos, size)
{
  wxIcon appIcon;
  appIcon.CopyFromBitmap(wxBitmap(gnu_small_trans_xpm));
  SetIcon(appIcon); 

  m_menu = MainMenu();
  SetMenuBar(m_menu);
  CreateStatusBar();
 
  wxPanel * pTopPanel = new wxPanel(this);

  pTopPanel->SetDropTarget(new DnDFile(this));

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

  m_listhash->SetStringSelection("SHA-1");

  SetStatusText("Welcome to Hashish.");

}

void MyFrame::OnQuit(wxCommandEvent& event)
{

    Close(TRUE);
}


void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxMessageBox(_T("Hashish\n© 2002 Jesse Lovelace\nBeta 0.4\nGNU License"),
                 _T("About Hashish"), wxOK | wxICON_INFORMATION, this);
}

void MyFrame::OnBrowseFiles(wxCommandEvent& event)
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

void MyFrame::OnChangeSource(wxCommandEvent& event)
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

void MyFrame::OnHash(wxCommandEvent& event)
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
    RIPEMD_160, _TIGER, PANAMA_HASH, _CRC32, SAPPHIRE_HASH };
  
	HashModule * hash;

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
  case(SAPPHIRE_HASH): hash = new SapphireHash; break;
  case(RIPEMD_160): hash = new RIPEMD160; break;
	case(_TIGER): hash = new Tiger; break;
	case(PANAMA_HASH): hash = new PanamaHash<true>; break; // true for big endian
	default: hash = new SHA256;
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

void MyFrame::On64(wxCommandEvent& event)
{
  (m_menu->FindItem(ID_MENU_64))->Check(TRUE);
  (m_menu->FindItem(ID_MENU_HEX))->Check(FALSE);
  m_hex = false;
}

void MyFrame::OnHex(wxCommandEvent& event)
{
  m_hex = true;
  (m_menu->FindItem(ID_MENU_64))->Check(FALSE);
  (m_menu->FindItem(ID_MENU_HEX))->Check(TRUE);

}



