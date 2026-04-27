#include <wx/wx.h>
#include <wx/textctrl.h>
#include <wx/filedlg.h>
#include <wx/msgdlg.h>
#include <wx/textfile.h>
#include <regex>
#include <string>
#include <fstream>

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
                                   wxTE_MULTILINE | wxTE_RICH2);
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
        
        // Read file using standard C++ to preserve whitespace
        std::ifstream file(currentFile.ToStdString(), std::ios::binary);
        if (!file.is_open()) {
            wxMessageBox("Failed to open file", "Error", wxOK | wxICON_ERROR);
            return;
        }
        
        // Read entire file into string
        std::string contentStr;
        file.seekg(0, std::ios::end);
        contentStr.reserve(file.tellg());
        file.seekg(0, std::ios::beg);
        contentStr.assign((std::istreambuf_iterator<char>(file)),
                         std::istreambuf_iterator<char>());
        file.close();
        
        // Convert to wxString - FromUTF8 handles null bytes properly
        wxString content = wxString::FromUTF8(contentStr.c_str(), contentStr.length());
        
        textCtrl->SetValue(content);
        textCtrl->SetInsertionPoint(0);
        
        SetTitle("DataLoader - " + currentFile);
    }

    void OnSelectDB(wxCommandEvent& event) {
        wxString content = textCtrl->GetValue();
        
        // Find "let database" (case insensitive)
        wxString lowerContent = content.Lower();
        wxString searchStr = "let database";
        int dbPos = lowerContent.Find(searchStr);
        
        if (dbPos == wxNOT_FOUND) {
            wxMessageBox("Could not find 'let database' in the file.", 
                        "Not Found", wxOK | wxICON_WARNING);
            return;
        }
        
        // Find start of line (including indentation)
        int startPos = dbPos;
        while (startPos > 0) {
            wxChar ch = content[startPos - 1];
            if (ch == '\n' || ch == '\r') {
                break;
            }
            startPos--;
        }
        
        // Find the opening '[' after "let database"
        int bracketPos = content.Find('[', dbPos);
        if (bracketPos == wxNOT_FOUND) {
            wxMessageBox("Could not find the database array.", 
                        "Not Found", wxOK | wxICON_WARNING);
            return;
        }
        
        // Find matching closing ']'
        int bracketCount = 1;
        int endPos = bracketPos + 1;
        while (endPos < (int)content.Length()) {
            wxChar ch = content[endPos];
            if (ch == '[') bracketCount++;
            else if (ch == ']') {
                bracketCount--;
                if (bracketCount == 0) {
                    endPos++; // Include the ']'
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
        
        // Find semicolon after closing ']'
        int semiPos = endPos;
        while (semiPos < (int)content.Length()) {
            wxChar ch = content[semiPos];
            if (ch == ';') {
                endPos = semiPos + 1; // Include the ';'
                break;
            }
            if (!isspace(ch)) break;
            semiPos++;
        }
        
        // Select from start of line to end (including semicolon)
        textCtrl->SetSelection(startPos, endPos);
        textCtrl->ShowPosition(startPos);
        textCtrl->SetFocus();
    }

    void OnSave(wxCommandEvent& event) {
        if (currentFile.IsEmpty()) {
            wxMessageBox("No file is open.", "No File", wxOK | wxICON_WARNING);
            return;
        }

        wxString content = textCtrl->GetValue();
        
        // Write using wxFile for better handling of large content
        wxFile file;
        if (!file.Create(currentFile, true) && !file.Open(currentFile, wxFile::write)) {
            wxMessageBox("Failed to save file", "Error", wxOK | wxICON_ERROR);
            return;
        }
        
        if (file.Write(content) == wxInvalidOffset) {
            wxMessageBox("Failed to write file", "Error", wxOK | wxICON_ERROR);
            return;
        }
        
        file.Close();

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
