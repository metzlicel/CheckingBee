#include <iostream>
#include <wx/wx.h>
#include <wx/splitter.h>
#include <wx/listctrl.h>
#include "spellCheck.h"

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
    void ClearButtonClicked(wxCommandEvent& evt);
    void CheckButtonClicked(wxCommandEvent& evt);
    void ShowSuggestionsList(const wxString& word, const std::vector<wxString>& suggestions);
    void OnInputEnter();

    wxPanel *panel;
    wxStaticText* headlineText;
    wxTextCtrl* inputField;
    wxButton* checkButton;
    wxButton* clearButton;
    wxListCtrl* suggestionsField;
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
    BindEventHandlers();
}

void MyFrame::CreateControls() {
    wxFont headlineFont(wxFontInfo(wxSize(0,36)).Bold());
    wxFont mainFont(wxFontInfo(wxSize(0,14)));

    panel = new wxPanel(this);
    panel->SetFont(mainFont);

    headlineText = new wxStaticText(panel, wxID_ANY, "Checking Bee", wxPoint(0,22), wxSize(800,-1), wxALIGN_CENTER_HORIZONTAL);
    headlineText->SetFont(headlineFont);

    inputField = new wxTextCtrl(panel, wxID_ANY, "", wxPoint(100,80), wxSize(495,60), wxTE_MULTILINE);
    checkButton = new wxButton(panel, wxID_ANY, "Check spelling", wxPoint(600,80), wxSize(130,35));

    suggestionsField = new wxListCtrl(panel, wxID_ANY, wxPoint(100,150), wxSize(600,350), wxLC_REPORT);
    suggestionsField->InsertColumn(0, "Palabra Incorrecta", wxLIST_FORMAT_LEFT, 200);
    suggestionsField->InsertColumn(1, "Sugerencias", wxLIST_FORMAT_LEFT, 400);

    clearButton = new wxButton(panel, wxID_ANY, "Clear", wxPoint(100, 525), wxSize(100,35));
}

void MyFrame::BindEventHandlers() {
    checkButton->Bind(wxEVT_BUTTON, &MyFrame::CheckButtonClicked, this);
    clearButton->Bind(wxEVT_BUTTON, &MyFrame::ClearButtonClicked, this);
}

void MyFrame::ShowSuggestionsList(const wxString& word, const vector<wxString>& suggestions) {
    // Insertar la palabra incorrecta en la primera columna
    if (!suggestions.empty()) {
        long index = suggestionsField->InsertItem(suggestionsField->GetItemCount(), word);


    // Concatenar todas las sugerencias en una sola cadena separada por comas
    wxString suggestionsText;
    for (size_t i = 0; i < suggestions.size(); ++i) {
        suggestionsText += suggestions[i];
        if (i < suggestions.size() - 1) {
            suggestionsText += ", ";
        }
    }

    // Insertar las sugerencias en la segunda columna
    suggestionsField->SetItem(index, 1, suggestionsText);
    }
}

void MyFrame::CheckButtonClicked(wxCommandEvent &evt) {
    suggestionsField->DeleteAllItems();

    wxString text = inputField->GetValue();
    vector<pair<string, vector<string>>> suggestions = checkSpelling(text.ToStdString());

    for (const auto& entry : suggestions) {
        // Convertir el vector de std::string a wxString
        std::vector<wxString> wxSuggestions;
        for (const auto& suggestion : entry.second) {
            wxSuggestions.push_back(wxString::FromUTF8(suggestion));
        }
        ShowSuggestionsList(wxString::FromUTF8(entry.first), wxSuggestions);
    }
}


void MyFrame::ClearButtonClicked(wxCommandEvent &evt) {
    inputField->Clear();
    suggestionsField->DeleteAllItems();
}
