#pragma once

// Playback threshold
extern cfg_uint cfg_threshold;

// Path for output .csv file
extern cfg_string cfg_data_path;

// Filename for output .csv file
extern cfg_string cfg_filename;

// Bool for enabling the collection of the playback info/statistics
extern cfg_bool cfg_enabled_collection;

// Bool for enabling console logging
extern cfg_bool cfg_enabled_console_logging;

void write_string_to_file(std::string dir_path, std::string filename, std::string content) {
	std::ofstream myfile;

	std::string file_path = dir_path + filename;
	if (dir_path.back() != '\\') {
		file_path = dir_path + '\\' + filename;
	}
	myfile.open(file_path, std::ios_base::app);
	myfile << content;
	myfile.close();
}

bool is_file_empty(std::string dir_path, std::string filename)
{
	std::string file_path = dir_path + filename;
	if (dir_path.back() != '\\') {
		file_path = dir_path + '\\' + filename;
	}

	std::ifstream myfile(file_path);

	return myfile.peek() == std::ifstream::traits_type::eof();
}

struct times { std::string time_lt; std::string time_st; };

static times get_local_time() {
	// https://learn.microsoft.com/en-us/windows/win32/api/minwinbase/ns-minwinbase-systemtime
	//console::print("DEBUG: get_local_time");
	time_t now_lt = time(0);
	time_t now_st = time(0);
	struct tm tstruct_lt;
	struct tm tstruct_st;
	char buf_lt[80];
	char buf_st[80];

	tstruct_lt = *localtime(&now_lt);
	// Visit http://en.cppreference.com/w/cpp/chrono/c/strftime
	// for more information about date/time format
	strftime(buf_lt, sizeof(buf_lt), "%m-%d-%Y %H:%M:%S", &tstruct_lt);

	tstruct_st = *gmtime(&now_st);
	// Visit http://en.cppreference.com/w/cpp/chrono/c/strftime
	// for more information about date/time format
	strftime(buf_st, sizeof(buf_st), "%m-%d-%Y %H:%M:%S", &tstruct_st);

	return times{ buf_lt, buf_st };
}