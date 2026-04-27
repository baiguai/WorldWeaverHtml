#include <wx/wx.h>
#include <wx/textctrl.h>
#include <wx/filedlg.h>
#include <wx/msgdlg.h>
#include <regex>
#include <fstream>
#include <sstream>
#include <cctype>

class DataLoaderFrame : public wxFrame {
public:
    DataLoaderFrame() : wxFrame(nullptr, wxID_ANY, "WorldWeaver DataLoader", 
                                   wxDefaultPosition, wxSize(1200, 800)) {
        SetBackgroundColour(wxColour(0, 0, 0));
        currentFile.Clear();
        SetupUI();
        SetupAccelerators();
    }

private:
    wxTextCtrl* textCtrl;
    wxString currentFile;

    void SetupUI() {
        wxPanel* panel = new wxPanel(this);
        panel->SetBackgroundColour(wxColour(0, 0, 0));

        wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);

        wxPanel* btnPanel = new wxPanel(panel);
        btnPanel->SetBackgroundColour(wxColour(0, 0, 0));
        wxBoxSizer* btnSizer = new wxBoxSizer(wxHORIZONTAL);

        wxButton* openBtn = new wxButton(btnPanel, wxID_ANY, "Open (Ctrl+O)");
        wxButton* selectBtn = new wxButton(btnPanel, wxID_ANY, "Select DB (Ctrl+D)");
        wxButton* saveBtn = new wxButton(btnPanel, wxID_ANY, "Save (Ctrl+S)");
        wxButton* exitBtn = new wxButton(btnPanel, wxID_ANY, "Exit (Ctrl+Q)");

        StyleButton(openBtn);
        StyleButton(selectBtn);
        StyleButton(saveBtn);
        StyleButton(exitBtn);

        openBtn->Bind(wxEVT_BUTTON, &DataLoaderFrame::OnOpen, this);
        selectBtn->Bind(wxEVT_BUTTON, &DataLoaderFrame::OnSelectDB, this);
        saveBtn->Bind(wxEVT_BUTTON, &DataLoaderFrame::OnSave, this);
        exitBtn->Bind(wxEVT_BUTTON, &DataLoaderFrame::OnExit, this);

        btnSizer->Add(openBtn, 0, wxALL, 2);
        btnSizer->Add(selectBtn, 0, wxALL, 2);
        btnSizer->Add(saveBtn, 0, wxALL, 2);
        btnSizer->Add(exitBtn, 0, wxALL, 2);
        btnPanel->SetSizer(btnSizer);

        sizer->Add(btnPanel, 0, wxEXPAND | wxALL, 5);

        textCtrl = new wxTextCtrl(panel, wxID_ANY, "", 
                                   wxDefaultPosition, wxDefaultSize,
                                   wxTE_MULTILINE);
        textCtrl->SetFont(wxFont(10, wxFONTFAMILY_TELETYPE, 
                                 wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
        textCtrl->SetBackgroundColour(wxColour(0, 0, 0));
        textCtrl->SetForegroundColour(wxColour(255, 255, 255));

        sizer->Add(textCtrl, 1, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 5);
        panel->SetSizer(sizer);
    }

    void StyleButton(wxButton* btn) {
        btn->SetBackgroundColour(wxColour(51, 51, 51));
        btn->SetForegroundColour(wxColour(255, 255, 255));
        btn->SetMinSize(wxSize(150, 30));
    }

    void SetupAccelerators() {
        wxAcceleratorEntry entries[4];
        entries[0].Set(wxACCEL_CTRL, 'O', 1001);
        entries[1].Set(wxACCEL_CTRL, 'D', 1002);
        entries[2].Set(wxACCEL_CTRL, 'S', 1003);
        entries[3].Set(wxACCEL_CTRL, 'Q', 1004);

        wxAcceleratorTable accel(4, entries);
        SetAcceleratorTable(accel);

        Bind(wxEVT_MENU, &DataLoaderFrame::OnOpen, this, 1001);
        Bind(wxEVT_MENU, &DataLoaderFrame::OnSelectDB, this, 1002);
        Bind(wxEVT_MENU, &DataLoaderFrame::OnSave, this, 1003);
        Bind(wxEVT_MENU, &DataLoaderFrame::OnExit, this, 1004);
    }

    void OnOpen(wxCommandEvent& event) {
        wxFileDialog dialog(this, "Select HTML file", "", "",
                           "HTML files (*.html)|*.html|All files (*.*)|*.*",
                           wxFD_OPEN | wxFD_FILE_MUST_EXIST);

        if (dialog.ShowModal() == wxID_CANCEL) return;

        currentFile = dialog.GetPath();
        std::ifstream file(currentFile.ToStdString());
        if (!file.is_open()) {
            wxMessageBox("Failed to open file", "Error", wxOK | wxICON_ERROR);
            return;
        }

        std::stringstream buffer;
        buffer << file.rdbuf();
        textCtrl->SetValue(buffer.str());
        file.close();

        SetTitle("DataLoader - " + currentFile);
    }

    void OnSelectDB(wxCommandEvent& event) {
        wxString content = textCtrl->GetValue();
        std::string stdContent(content.ToStdString());

        std::regex startRegex(R"((let\s+database\s*=\s*\[))", std::regex::icase);
        std::smatch match;

        if (!std::regex_search(stdContent, match, startRegex)) {
            wxMessageBox("Could not find 'let database = [' in the file.", 
                        "Not Found", wxOK | wxICON_WARNING);
            return;
        }

        size_t startPos = match.position(1);
        size_t openBracketPos = startPos + match.str(1).rfind('[');

        int bracketCount = 1;
        size_t endPos = openBracketPos + 1;
        while (endPos < stdContent.length()) {
            char ch = stdContent[endPos];
            if (ch == '[') bracketCount++;
            else if (ch == ']') {
                bracketCount--;
                if (bracketCount == 0) {
                    endPos++;
                    break;
                }
            }
            endPos++;
        }

        if (bracketCount != 0) {
            wxMessageBox("Could not find the end of the database array.", 
                        "Not Found", wxOK | wxICON_WARNING);
            return;
        }

        size_t semiPos = endPos;
        while (semiPos < stdContent.length()) {
            if (stdContent[semiPos] == ';') {
                endPos = semiPos + 1;
                break;
            }
            if (!isspace(stdContent[semiPos])) break;
            semiPos++;
        }

        textCtrl->SetSelection(startPos, endPos);
        textCtrl->ShowPosition(startPos);
        textCtrl->SetFocus();
    }

    void OnSave(wxCommandEvent& event) {
        if (currentFile.IsEmpty()) {
            wxMessageBox("No file is open.", "No File", wxOK | wxICON_WARNING);
            return;
        }

        std::ofstream file(currentFile.ToStdString());
        if (!file.is_open()) {
            wxMessageBox("Failed to save file", "Error", wxOK | wxICON_ERROR);
            return;
        }

        file << textCtrl->GetValue().ToStdString();
        file.close();

        wxMessageBox("File saved successfully:\n" + currentFile, 
                     "Saved", wxOK | wxICON_INFORMATION);
    }

    void OnExit(wxCommandEvent& event) {
        Close(true);
    }
};

class DataLoaderApp : public wxApp {
public:
    bool OnInit() override {
        DataLoaderFrame* frame = new DataLoaderFrame();
        frame->Show(true);
        return true;
    }
};

wxIMPLEMENT_APP(DataLoaderApp);
