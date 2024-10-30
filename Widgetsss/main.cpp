#include <iostream>
#include <wx/wx.h>
#include <wx/splitter.h>
#include <wx/listctrl.h>

class MyApp : public wxApp {
public:
    virtual bool OnInit();
};

wxIMPLEMENT_APP(MyApp);

class MyFrame : public wxFrame {
public:
    MyFrame(const wxString &title);
private:
    void CreateControls();
    void BindEventHandlers();
    void OnAddButtonClicked();
    void CheckInput();

    wxPanel *panel;
    wxStaticText* headlineText;
    wxTextCtrl* inputField;
    wxButton* checkButton;
    wxButton* clearButton;
    wxListView* suggestionsField;
};

bool MyApp::OnInit() {
    MyFrame* myFrame = new MyFrame("SpellCheck");
    myFrame->SetClientSize(800,600);
    myFrame->Center();
    myFrame->Show();
    return true;
}
MyFrame::MyFrame(const wxString &title) : wxFrame(nullptr, wxID_ANY, title) {
    CreateControls();
}
void MyFrame::CreateControls() {
    wxFont headlineFont(wxFontInfo(wxSize(0,36)).Bold());
    wxFont mainFont(wxFontInfo(wxSize(0,14)));

    panel = new wxPanel(this);
    panel->SetFont(mainFont);

    headlineText = new wxStaticText(panel, wxID_ANY, "Checking Bee", wxPoint(0,22), wxSize(800,-1), wxALIGN_CENTER_HORIZONTAL);
    headlineText->SetFont(headlineFont);

    inputField = new wxTextCtrl(panel, wxID_ANY, "", wxPoint(100,80), wxSize(495,60));
    checkButton = new wxButton(panel, wxID_ANY, "Check spelling", wxPoint(600,80), wxSize(130,35));
    suggestionsField = new wxListView(panel, wxID_ANY, wxPoint(100,150), wxSize(600,350));
    clearButton = new wxButton(panel, wxID_ANY, "Clear", wxPoint(100, 525), wxSize(100,35));
}
