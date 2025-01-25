#include "stdafx.h"
#include "resource.h"
#include <helpers/atl-misc.h>
#include <string>
#include <atlbase.h>
#include <direct.h>
#include <fstream>
#include <sstream>

void copyFile(std::string from, std::string towards) {
	std::ifstream ab(from, std::ios::binary);
	std::ofstream ad(towards, std::ios::binary);

	ad << ab.rdbuf();
}

std::string get_local_time_shvalid(bool britbong) {
	std::ostringstream st;
	std::string st_string;

	SYSTEMTIME lt;
	GetLocalTime(&lt);

	if (britbong) {
		st << lt.wHour << "_" << lt.wMinute << "_" << lt.wSecond << "_" << lt.wDay << "-" << lt.wMonth << "-" << lt.wYear;
	}
	else {
		st << lt.wHour << "_" << lt.wMinute << "_" << lt.wSecond << "_" << lt.wMonth << "-" << lt.wDay << "-" << lt.wYear;
	}
	st_string = st.str();

	return st_string;
}

// These GUIDs identify the variables within our component's configuration file.
static const GUID guid_cfg_threshold = { 0xf1888ebd, 0x5ba3, 0x4d22, { 0xa3, 0x9a, 0x89, 0x5f, 0xda, 0x9e, 0x46, 0xb7 } };

// These GUIDs identify the path and filedialog folderpicker button
static const GUID guid_open_directory = { 0x057575ce, 0x7692, 0x4f4e, { 0x81, 0x7a, 0x09, 0x60, 0x0c, 0x11, 0x4c, 0xcd } };
static const GUID guid_data_path = { 0x44b9a8e5, 0xbf5e, 0x44e2, { 0x96, 0xd6, 0x58, 0xdc, 0xbf, 0x11, 0x41, 0xf5 } };
static const GUID guid_filename = { 0xbbb159fb, 0x5d48, 0x4fc8, { 0x96, 0x75, 0xea, 0x6b, 0xe2, 0xe4, 0xd4, 0x67 } };
static const GUID guid_backup = { 0xa5b22bbf, 0x0c6f, 0x44c5, { 0x9a, 0xb3, 0x85, 0xb4, 0x0e, 0x67, 0xd8, 0x7c } };

// This GUID identifies the checkbox to enable playback statistic collection.
static const GUID guid_enabled_collection = { 0xcfea0555, 0xfa10, 0x4ea7, { 0x9c, 0xab, 0xf0, 0x29, 0x3f, 0x10, 0x36, 0x2c } };

enum {
	default_cfg_threshold = 50,
};

// Default path of directory where the stats are saved
LPCSTR default_data_path = "C:\\Users\\";
LPCSTR default_filename = "soni_collecti";

//
cfg_uint cfg_threshold(guid_cfg_threshold, default_cfg_threshold);

// 
cfg_string cfg_data_path(guid_data_path, default_data_path);
cfg_string cfg_filename(guid_filename, default_filename);

// Checkbox boolean
cfg_bool cfg_enabled_collection(guid_enabled_collection, true);
//bool collector_testor;

// Buffer strings for storing dialog box text and for HasChanged() checks
int bsize = MAX_PATH + 1;

LPSTR buff_path = (LPSTR) calloc(bsize, sizeof(LPSTR));
LPSTR check_buff_path = (LPSTR) calloc(bsize, sizeof(LPSTR));
bool path_discrepancy;

LPSTR buff_filename = (LPSTR) calloc(bsize, sizeof(LPSTR));
LPSTR check_buff_filename = (LPSTR) calloc(bsize, sizeof(LPSTR));
bool filename_discrepancy;

class CMyPreferences : public CDialogImpl<CMyPreferences>, public preferences_page_instance {
public:
	CMyPreferences(preferences_page_callback::ptr callback) : m_callback(callback) {}

	//dialog resource ID
	enum {IDD = IDD_MYPREFERENCES};
	t_uint32 get_state();
	void apply();
	void reset();

	//WTL message map
	BEGIN_MSG_MAP_EX(CMyPreferences)
		MSG_WM_INITDIALOG(OnInitDialog)
		COMMAND_HANDLER_EX(IDC_THRESHOLD, EN_CHANGE, OnEditChange)
		COMMAND_HANDLER_EX(IDC_PATH, EN_CHANGE, OnEditChange)
		COMMAND_HANDLER_EX(IDC_DIRBROWSE, BN_CLICKED, FolderPickOpen)
		COMMAND_HANDLER_EX(IDC_FILE, EN_CHANGE, OnEditChange)
		COMMAND_HANDLER_EX(IDC_ENABLE_COLLECTION, BN_CLICKED, OnEditChange)
		COMMAND_HANDLER_EX(IDC_BACKUP, BN_CLICKED, DataBackup)
	END_MSG_MAP()
private:
	std::string dirr;
	std::string fill;
	std::string file_path;
	std::string backup_path;
	std::string backup_file_path;
	std::string ext = ".csv";
	BOOL OnInitDialog(CWindow, LPARAM);
	void OnEditChange(UINT, int, CWindow);
	void FolderPickOpen(UINT, int, CWindow);
	void DataBackup(UINT, int, CWindow);
	bool HasChanged();
	void OnChanged();


	HWND handler_data_path;
	HWND handler_checkbox;
	HWND handler_filename;
	HWND handler_backup;
	const preferences_page_callback::ptr m_callback;
};

BOOL CMyPreferences::OnInitDialog(CWindow, LPARAM) {
	//console::print("DEBUG: Preferences window initialized");

	handler_data_path = GetDlgItem(IDC_PATH);
	handler_backup = GetDlgItem(IDC_BACKUP);
	handler_checkbox = GetDlgItem(IDC_ENABLE_COLLECTION);
	handler_filename = GetDlgItem(IDC_FILE);

	CheckDlgButton(IDC_ENABLE_COLLECTION, cfg_enabled_collection);

	SetDlgItemInt(IDC_THRESHOLD, cfg_threshold, FALSE);

	// check if cfg_data_path is NULL or an empty string
	if (cfg_data_path.empty() || cfg_data_path[0] == 0) {
		SetWindowTextA(handler_data_path, default_data_path);
	}
	else {
		SetWindowTextA(handler_data_path, cfg_data_path);
	}

	// Filename
	if (cfg_filename.empty() || cfg_filename[0] == 0) {
		SetWindowTextA(handler_filename, default_filename);
	}
	else {
		SetWindowTextA(handler_filename, cfg_filename);
	}

	GetWindowTextA(handler_data_path, buff_path, bsize);
	GetWindowTextA(handler_data_path, check_buff_path, bsize);
	GetWindowTextA(handler_filename, check_buff_filename, bsize);
	GetWindowTextA(handler_filename, buff_filename, bsize);
	cfg_enabled_collection = IsDlgButtonChecked(IDC_ENABLE_COLLECTION);
	CheckDlgButton(IDC_BACKUP, BST_CHECKED);
	return FALSE;
}

void CMyPreferences::OnEditChange(UINT, int, CWindow) {
	OnChanged();
}

t_uint32 CMyPreferences::get_state() {
	t_uint32 state = preferences_state::resettable;

	// if (HasChanged() == True, state = preferences_state::resettable or preferences_state::changed
	if (HasChanged()) { 
		state |= preferences_state::changed;
		CheckDlgButton(IDC_BACKUP, BST_INDETERMINATE);
	}
	return state;
}

void CMyPreferences::reset() {
	SetDlgItemInt(IDC_THRESHOLD, default_cfg_threshold, FALSE);

	SetWindowTextA(handler_data_path, default_data_path);
	SetWindowTextA(handler_filename, default_filename);
	CheckDlgButton(IDC_ENABLE_COLLECTION, true);

	OnChanged();
	CheckDlgButton(IDC_BACKUP, BST_CHECKED);
}

void CMyPreferences::apply() {
	cfg_threshold = GetDlgItemInt(IDC_THRESHOLD, NULL, FALSE);

	GetWindowTextA(handler_data_path, buff_path, bsize);

	if (cfg_threshold > 100 || cfg_threshold < 1) {
		SetDlgItemInt(IDC_THRESHOLD, default_cfg_threshold, FALSE);
		cfg_threshold = GetDlgItemInt(IDC_THRESHOLD, NULL, FALSE);
	}

	if (PathFileExistsA(buff_path)) {
		SetWindowTextA(handler_data_path, buff_path);
	}
	else {
		SetWindowTextA(handler_data_path, default_data_path);
	}

	GetWindowTextA(handler_data_path, buff_path, bsize);
	GetWindowTextA(handler_data_path, check_buff_path, bsize);
	GetWindowTextA(handler_filename, check_buff_filename, bsize);
	GetWindowTextA(handler_filename, buff_filename, bsize);

	cfg_data_path = buff_path;
	cfg_filename = buff_filename;

	cfg_enabled_collection = IsDlgButtonChecked(IDC_ENABLE_COLLECTION);
	OnChanged();
	CheckDlgButton(IDC_BACKUP, BST_CHECKED);
}

bool CMyPreferences::HasChanged() {
	// returns whether our dialog content is different from the current configuration (whether the apply button should be enabled or not)
	GetWindowTextA(handler_data_path, check_buff_path, bsize);
	GetWindowTextA(handler_filename, check_buff_filename, bsize);
	if (strcmp(buff_path, check_buff_path) == 0) {
		path_discrepancy = false;
	}
	else {
		path_discrepancy = true;
	}

	if (strcmp(buff_filename, check_buff_filename) == 0) {
		filename_discrepancy = false;
	}
	else {
		filename_discrepancy = true;
	}

	//collector_testor = IsDlgButtonChecked(IDC_ENABLE_COLLECTION);
	
	return IsDlgButtonChecked(IDC_ENABLE_COLLECTION) != cfg_enabled_collection || GetDlgItemInt(IDC_THRESHOLD, NULL, FALSE) != cfg_threshold || path_discrepancy || filename_discrepancy;
}

void CMyPreferences::OnChanged() {
	// tell the host that our state has changed to enable/disable the apply button appropriately.
	m_callback->on_state_changed();
}

void CMyPreferences::FolderPickOpen(UINT, int, CWindow) {

	if ( !ModalDialogPrologue() ) return;
	
	const HWND wndParent = core_api::get_main_window();
	modal_dialog_scope scope(wndParent);
	uBrowseForFolder(wndParent, "Choose Playback Statistics Output Folder", cfg_data_path);

	SetWindowTextA(handler_data_path, cfg_data_path);
	GetWindowTextA(handler_data_path, buff_path, bsize);
	GetWindowTextA(handler_data_path, check_buff_path, bsize);

}

void CMyPreferences::DataBackup(UINT, int, CWindow) {
	dirr = cfg_data_path.toString();
	fill = cfg_filename.toString();

	file_path = dirr + fill;
	backup_path = dirr + "backups";
	if (dirr.back() != '\\') {
		file_path = dirr + '\\' + fill + ext;
		backup_path = dirr + '\\' + "backups";
	}

	if (PathFileExistsA(file_path.c_str())) {
		if (!PathFileExistsA(backup_path.c_str())) {
			_mkdir(backup_path.c_str());
		}
		backup_file_path = backup_path + '\\' + fill + '_' + get_local_time_shvalid(false) + ext;
		copyFile(file_path, backup_file_path);
	}
	else {
		console::error("No data file found to backup");
	}
}

class preferences_page_myimpl : public preferences_page_impl<CMyPreferences> {
	// preferences_page_impl<> helper deals with instantiation of our dialog; inherits from preferences_page_v3.
public:
	const char * get_name() {return "Soni Collecti";}
	GUID get_guid() {
		static const GUID guid = { 0x683297f1, 0x41d4, 0x44a0, { 0x8a, 0xc0, 0x35, 0xb5, 0x96, 0xca, 0xb2, 0x57 } };
		return guid;
	}
	GUID get_parent_guid() {return guid_tools;}
};

static preferences_page_factory_t<preferences_page_myimpl> g_preferences_page_myimpl_factory;
