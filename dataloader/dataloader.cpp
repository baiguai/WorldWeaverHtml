#include <wx/wx.h>
#include <wx/textctrl.h>
#include <wx/filedlg.h>
#include <wx/msgdlg.h>
#include <wx/textfile.h>
#include <wx/timer.h>
#include <wx/filename.h>
#include <string>
#include <fstream>

class DataLoaderFrame : public wxFrame {
public:
    DataLoaderFrame() : wxFrame(nullptr, wxID_ANY, "WorldWeaver DataLoader", 
                                   wxDefaultPosition, wxSize(1200, 800)),
                                   saveTimer(this) {
        SetBackgroundColour(wxColour(0, 0, 0));
        currentFile.Clear();
        SetupUI();
        SetupAccelerators();
        saveTimer.Bind(wxEVT_TIMER, &DataLoaderFrame::OnTimer, this);
    }

private:
    wxTextCtrl* textCtrl;
    wxString currentFile;
    wxStaticText* statusLabel;
    wxTimer saveTimer;
    
    void SetupUI();
    void StyleButton(wxButton* btn);
    void SetupAccelerators();
    void OnOpen(wxCommandEvent& event);
    void OnSelectDB(wxCommandEvent& event);
    void OnUpdateDB(wxCommandEvent& event);
    void SelectDatabase();
    void OnSave(wxCommandEvent& event);
    void OnExit(wxCommandEvent& event);
    void OnTimer(wxTimerEvent& event);
};

void DataLoaderFrame::SetupUI() {
    wxPanel* panel = new wxPanel(this);
    panel->SetBackgroundColour(wxColour(0, 0, 0));
    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);

    wxPanel* btnPanel = new wxPanel(panel);
    btnPanel->SetBackgroundColour(wxColour(0, 0, 0));
    wxBoxSizer* btnSizer = new wxBoxSizer(wxHORIZONTAL);

    wxButton* openBtn = new wxButton(btnPanel, wxID_ANY, "Open (Ctrl+O)");
    wxButton* selectBtn = new wxButton(btnPanel, wxID_ANY, "Select DB (Ctrl+D)");
    wxButton* updateBtn = new wxButton(btnPanel, wxID_ANY, "Reload File and Select DB (Ctrl+U)");
    wxButton* saveBtn = new wxButton(btnPanel, wxID_ANY, "Save (Ctrl+S)");
    wxButton* exitBtn = new wxButton(btnPanel, wxID_ANY, "Exit (Ctrl+Q)");

    StyleButton(openBtn);
    StyleButton(selectBtn);
    StyleButton(updateBtn);
    updateBtn->SetMinSize(wxSize(230, 30));
    StyleButton(saveBtn);
    StyleButton(exitBtn);

    openBtn->Bind(wxEVT_BUTTON, &DataLoaderFrame::OnOpen, this);
    selectBtn->Bind(wxEVT_BUTTON, &DataLoaderFrame::OnSelectDB, this);
    updateBtn->Bind(wxEVT_BUTTON, &DataLoaderFrame::OnUpdateDB, this);
    saveBtn->Bind(wxEVT_BUTTON, &DataLoaderFrame::OnSave, this);
    exitBtn->Bind(wxEVT_BUTTON, &DataLoaderFrame::OnExit, this);

    btnSizer->Add(openBtn, 0, wxALL, 2);
    btnSizer->Add(selectBtn, 0, wxALL, 2);
    btnSizer->Add(updateBtn, 0, wxALL, 4);
    btnSizer->Add(saveBtn, 0, wxALL, 2);
    btnSizer->Add(exitBtn, 0, wxALL, 2);
    
    statusLabel = new wxStaticText(btnPanel, wxID_ANY, "", wxDefaultPosition, wxDefaultSize);
    statusLabel->SetForegroundColour(wxColour(0, 200, 0));
    btnSizer->Add(statusLabel, 1, wxALL | wxALIGN_CENTER_VERTICAL, 2);
    
    btnPanel->SetSizer(btnSizer);

    sizer->Add(btnPanel, 0, wxEXPAND | wxALL, 5);

    textCtrl = new wxTextCtrl(panel, wxID_ANY, "", 
                               wxDefaultPosition, wxDefaultSize,
                               wxTE_MULTILINE | wxTE_RICH2 | wxTE_DONTWRAP);
    textCtrl->SetFont(wxFont(10, wxFONTFAMILY_TELETYPE, 
                             wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
    textCtrl->SetBackgroundColour(wxColour(0, 0, 0));
    textCtrl->SetForegroundColour(wxColour(255, 255, 255));

    sizer->Add(textCtrl, 1, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 5);
    panel->SetSizer(sizer);
}

void DataLoaderFrame::StyleButton(wxButton* btn) {
    btn->SetBackgroundColour(wxColour(51, 51, 51));
    btn->SetForegroundColour(wxColour(255, 255, 255));
    btn->SetMinSize(wxSize(150, 30));
}

void DataLoaderFrame::SetupAccelerators() {
    wxAcceleratorEntry entries[5];
    entries[0].Set(wxACCEL_CTRL, 'O', 1001);
    entries[1].Set(wxACCEL_CTRL, 'D', 1002);
    entries[2].Set(wxACCEL_CTRL, 'S', 1003);
    entries[3].Set(wxACCEL_CTRL, 'Q', 1004);
    entries[4].Set(wxACCEL_CTRL, 'U', 1005);
    wxAcceleratorTable accel(5, entries);
    SetAcceleratorTable(accel);

    Bind(wxEVT_MENU, &DataLoaderFrame::OnOpen, this, 1001);
    Bind(wxEVT_MENU, &DataLoaderFrame::OnSelectDB, this, 1002);
    Bind(wxEVT_MENU, &DataLoaderFrame::OnSave, this, 1003);
    Bind(wxEVT_MENU, &DataLoaderFrame::OnExit, this, 1004);
    Bind(wxEVT_MENU, &DataLoaderFrame::OnUpdateDB, this, 1005);
}

void DataLoaderFrame::OnOpen(wxCommandEvent& event) {
    wxFileDialog dialog(this, "Select HTML file", "", "",
                       "HTML files (*.html)|*.html|All files (*.*)|*.*",
                       wxFD_OPEN | wxFD_FILE_MUST_EXIST);
    if (dialog.ShowModal() == wxID_CANCEL) return;

    currentFile = dialog.GetPath();
    std::ifstream file(currentFile.ToStdString(), std::ios::binary);
    if (!file.is_open()) {
        wxMessageBox("Failed to open file", "Error", wxOK | wxICON_ERROR);
        return;
    }
    std::string contentStr;
    file.seekg(0, std::ios::end);
    contentStr.reserve(file.tellg());
    file.seekg(0, std::ios::beg);
    contentStr.assign((std::istreambuf_iterator<char>(file)),
                     std::istreambuf_iterator<char>());
    file.close();
    wxString content = wxString::FromUTF8(contentStr.c_str(), contentStr.length());
    textCtrl->SetValue(content);
    SetTitle("DataLoader - " + currentFile);
    
    // Automatically select the database after opening
    SelectDatabase();
}

void DataLoaderFrame::OnSelectDB(wxCommandEvent& event) {
    SelectDatabase();
}

void DataLoaderFrame::OnUpdateDB(wxCommandEvent& event) {
    if (currentFile.IsEmpty()) {
        wxFileDialog dialog(this, "Select HTML file to update database for", "", "",
                           "HTML files (*.html)|*.html|All files (*.*)|*.*",
                           wxFD_OPEN | wxFD_FILE_MUST_EXIST);
        if (dialog.ShowModal() == wxID_CANCEL) return;
        currentFile = dialog.GetPath();
    }

    std::ifstream file(currentFile.ToStdString(), std::ios::binary);
    if (!file.is_open()) {
        wxMessageBox("Failed to open file", "Error", wxOK | wxICON_ERROR);
        if (currentFile.IsEmpty()) currentFile.Clear();
        return;
    }
    std::string contentStr;
    file.seekg(0, std::ios::end);
    contentStr.reserve(file.tellg());
    file.seekg(0, std::ios::beg);
    contentStr.assign((std::istreambuf_iterator<char>(file)),
                     std::istreambuf_iterator<char>());
    file.close();
    wxString content = wxString::FromUTF8(contentStr.c_str(), contentStr.length());
    textCtrl->SetValue(content);
    SetTitle("DataLoader - " + currentFile);

    SelectDatabase();
}


void DataLoaderFrame::SelectDatabase() {
    wxString content = textCtrl->GetValue();
    wxString lowerContent = content.Lower();
    
    // Manual search for "let database"
    long dbPos = -1;
    for (long i = 0; i <= (long)(lowerContent.Length() - 13); i++) {
        if (lowerContent[i] == 'l' && lowerContent[i+1] == 'e' &&
            lowerContent[i+2] == 't' && lowerContent[i+3] == ' ' &&
            lowerContent[i+4] == 'd' && lowerContent[i+5] == 'a' &&
            lowerContent[i+6] == 't' && lowerContent[i+7] == 'a' &&
            lowerContent[i+8] == 'b' && lowerContent[i+9] == 'a' &&
            lowerContent[i+10] == 's' && lowerContent[i+11] == 'e') {
            if (i == 0 || content[i-1] == ' ' || content[i-1] == '\t' ||
                content[i-1] == '\n' || content[i-1] == '\r') {
                dbPos = i;
                break;
            }
        }
    }
    
    if (dbPos == -1) {
        wxMessageBox("Could not find 'let database' in the file.", 
                    "Not Found", wxOK | wxICON_WARNING);
        return;
    }
    
    // Find start of line
    long startPos = dbPos;
    while (startPos > 0) {
        wxChar ch = content[startPos - 1];
        if (ch == '\n' || ch == '\r') break;
        startPos--;
    }
    
    // Find opening '['
    long bracketPos = -1;
    for (long i = dbPos; i < (long)content.Length(); i++) {
        if (content[i] == '[') {
            bracketPos = i;
            break;
        }
    }
    
    if (bracketPos == -1) {
        wxMessageBox("Could not find the database array.", 
                    "Not Found", wxOK | wxICON_WARNING);
        return;
    }
    
    // Find closing ']'
    int bracketCount = 1;
    long closeBracketPos = -1;
    for (long i = bracketPos + 1; i < (long)content.Length(); i++) {
        wxChar ch = content[i];
        if (ch == '[') bracketCount++;
        else if (ch == ']') {
            bracketCount--;
            if (bracketCount == 0) {
                closeBracketPos = i;
                break;
            }
        }
    }
    
    if (closeBracketPos == -1) {
        wxMessageBox("Could not find the end of the database array.", 
                    "Not Found", wxOK | wxICON_WARNING);
        return;
    }
    
    long endPos = closeBracketPos + 1;
    
    // Look for ';'
    for (long i = closeBracketPos + 1; i < (long)content.Length(); i++) {
        wxChar ch = content[i];
        if (ch == ';') {
            endPos = i + 1;
            break;
        }
        if (ch != ' ' && ch != '\t' && ch != '\n' && ch != '\r') break;
    }
    
    // Scroll to top first
    textCtrl->ScrollLines(-textCtrl->GetNumberOfLines());
    textCtrl->SetInsertionPoint(0);
    textCtrl->ShowPosition(0);
    
    // Set selection LAST so it doesn't get cleared
    textCtrl->SetSelection(startPos, endPos);
    textCtrl->SetFocus();
    textCtrl->ShowPosition(endPos);
}

void DataLoaderFrame::OnSave(wxCommandEvent& event) {
    if (currentFile.IsEmpty()) {
        statusLabel->SetLabel("No file is open.");
        return;
    }

    wxString content = textCtrl->GetValue();
    wxFile file;
    if (!file.Create(currentFile, true) && !file.Open(currentFile, wxFile::write)) {
        statusLabel->SetLabel("Failed to save file");
        return;
    }
    if (file.Write(content) == wxInvalidOffset) {
        statusLabel->SetLabel("Failed to write file");
        return;
    }
    file.Close();

    wxFileName fname(currentFile);
    statusLabel->SetLabel("Saved: " + fname.GetFullName());
    saveTimer.Start(3000, wxTIMER_ONE_SHOT);
}

void DataLoaderFrame::OnExit(wxCommandEvent& event) {
    Close(true);
}

void DataLoaderFrame::OnTimer(wxTimerEvent& event) {
    statusLabel->SetLabel("");
}

class DataLoaderApp : public wxApp {
public:
    bool OnInit() override {
        DataLoaderFrame* frame = new DataLoaderFrame();
        frame->Show(true);
        return true;
    }
};

wxIMPLEMENT_APP(DataLoaderApp);
