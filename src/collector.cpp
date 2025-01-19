#include "stdafx.h"
#include "resource.h"
#include <helpers/atl-misc.h>
#include <atlbase.h>
#include <string>
#include <sstream>
#include <istream>
#include <iostream>
#include <fstream>
#include "collector.h"
#include <ctime>
#include <time.h>

class Collector : public play_callback_static {

public:
	// Playback callback methods.
	// created callbacks
	static std::string get_local_time() {
		// https://learn.microsoft.com/en-us/windows/win32/api/minwinbase/ns-minwinbase-systemtime
		console::print("DEBUG: get_local_time");
		std::ostringstream lt_ostr;
		std::ostringstream st_ostr;
		std::string st_str;
		std::string lt_str;

		struct tm * s_timeinfo;
		struct tm * l_timeinfo;

		char s_buffer[80];
		char l_buffer[80];

		time_t srawtime;
		time(&srawtime);
		
		s_timeinfo = gmtime(&srawtime);
		
		strftime(s_buffer, sizeof(s_buffer), "%d-%m-%Y %H:%M:%S", s_timeinfo);
		st_str = *s_buffer;

		time_t lrawtime;
		time(&lrawtime);

		l_timeinfo = localtime(&lrawtime);
		
		strftime(l_buffer, sizeof(l_buffer), "%d-%m-%Y %H:%M:%S", l_timeinfo);
		lt_str = *l_buffer;

		/*SYSTEMTIME st, lt;

		GetSystemTime(&st);
		GetLocalTime(&lt);

		console::printf("%d", st.wHour);
		console::printf("%d", lt.wMinute);

		st_ostr << st.wHour << ":" << st.wMinute << ":" << st.wSecond << " " << st.wMonth << "-" << st.wDay << "-" << st.wYear;
		st_str = st_ostr.str();

		lt_ostr << lt.wHour << ":" << lt.wMinute << ":" << lt.wSecond << " " << lt.wMonth << "-" << lt.wDay << "-" << lt.wYear;
		lt_str = lt_ostr.str();

		console::printf("DEBUG: %s", st_str);
		console::printf("DEBUG: %s", lt_str);*/
		
		// console::print(st_str.c_str());
		// console::print(lt_str.c_str());

		return st_str, lt_str;
	}

	static void write_string_to_file(const char* dir_path, const char* filename, std::string content) {
		console::print("DEBUG: write_string_to_file");
		std::ofstream myfile;

		char file_path[MAX_PATH] = "";

		strncat_s(file_path, dir_path, sizeof(file_path));
		console::print("WOAH FILE PATH");
		console::print(file_path);
		console::print("WOAH ENDFILE PATH");
		strncat_s(file_path, filename, sizeof(file_path));

		console::print("WOAH FILE PATH");
		console::print(file_path);
		console::print("WOAH ENDFILE PATH");

		myfile.open(file_path, std::ios_base::app);
		myfile << content;
		myfile.close();
	}

	static bool is_file_empty(const char* dir_path, const char* filename)
	{
		char file_path[MAX_PATH] = "";

		strncat_s(file_path, filename, sizeof(file_path));
		strncat_s(file_path, dir_path, sizeof(file_path));

		std::ifstream myfile(dir_path);

		return myfile.peek() == std::ifstream::traits_type::eof();
	}

	void on_new_track_update() {
		//playback_control::ptr m_playback_control = playback_control::get();
		console::print("DEBUG: on_new_track_update");
		if (cfg_enabled_collection) {
			track_logged = false;
			console::print("DEBUG: COLLECTION IS ENABLED!!! GOGOGO!!!");
			if (tifo.is_empty()) {
				console::print("DEBUG: TIFO IS EMPTY");
				titleformat_compiler::get()->compile_safe_ex(tifo, format_info);
			}

			if (playback_control::get()->playback_format_title(NULL, track_info, tifo, NULL, playback_control::display_level_none)) {
				// Succeeded already
				console::print("DEBUG: PLAYBACK SUCCEEDED");
				playback_state = 1;
				playback_length = playback_control::get()->playback_get_length();
				console::print("DEBUG: I HAVE THE LENGTH:");
				console::print(playback_length);
				when = get_local_time();
			}
			else if (playback_control::get()->is_playing()) {
				// Starting playback but not done opening the first track yet
				console::print("DEBUG: STILL OPENING!!!");
				playback_state = 2;
				playback_time = 0;
			}
			else {
				// Other state
				playback_state = 0;
			}
		}
	};
	void on_time_update() {
		//double playback_time = playback_control::get()->playback_get_position();
		//double playback_length = playback_control::get()->playback_get_length();
		console::print("DEBUG: on_time_update");
		console::print(playback_time);
		console::print(playback_length);
		console::print(cfg_threshold);
		console::print((cfg_threshold / 100.0));
		console::print((playback_time / playback_length));
		console::print((playback_time / playback_length) >= (cfg_threshold / 100.0));

		if ((!track_logged) && (cfg_enabled_collection)) {
			if ((playback_time / playback_length) >= (cfg_threshold / 100.0)) {
				console::print("DEBUG: THRESHOLD PASSED!");
				track_logged = true;
				collect_track_data();
			}
			else {
				console::print("DEBUG: TIME UP! TIME UP!");
				playback_time += 1;
			}
			// playback_time = m_playback_control->playback_get_position();
		}
	};

	// ...
	void collect_track_data() {
		console::print("DEBUG: collect_track_data");
		std::ostringstream sl;
		std::string stats_line;

		console::print(track_info);
		//console::printf("%s", when);

		sl << when << track_info << "\n";
		stats_line = sl.str();

		//console::print(stats_line.c_str());
		console::print(cfg_data_path);
		//console::print(file_name);
		if (is_file_empty(cfg_data_path, file_name)) {
			write_string_to_file(cfg_data_path, file_name, file_header);
		}

		write_string_to_file(cfg_data_path, file_name, stats_line);
		console::print("DEBUG: INFO WRITTEN!");
		// move to on_time_update
		// track_logged = true;
	};

	// used
	void on_playback_new_track(metadb_handle_ptr p_track) { on_new_track_update(); }
	void on_playback_starting(playback_control::t_track_command p_command, bool p_paused) { on_new_track_update(); }
	void on_playback_edited(metadb_handle_ptr p_track) { on_new_track_update(); }
	void on_playback_time(double p_time) { on_time_update(); }

	// empty
	void on_playback_stop(play_control::t_stop_reason p_reason) {};
	void on_playback_seek(double p_time) {};
	void on_playback_pause(bool p_state) {};
	void on_volume_change(float p_new_val) {};
	void on_playback_dynamic_info(const file_info& p_info) {};
	void on_playback_dynamic_info_track(const file_info& p_info) {};

	// flags
	virtual unsigned get_flags() { return flag_on_playback_new_track | flag_on_playback_starting | flag_on_playback_edited | flag_on_playback_time; }
	
private:
	
	int playback_state;
	double playback_time;
	double playback_length;
	bool pause_state;
	bool track_logged;
	const char* file_name = "Soni_Collecti.csv";

	// https://wiki.hydrogenaud.io/index.php?title=Foobar2000:Title_Formatting_Reference
	titleformat_object::ptr tifo;
	pfc::string8 track_info;
	pfc::string8 format_info = "%length_ex%,%artist%,%title%,%album%,%album_artist%,%genre%,%date%,%codec%,%_foobar2000_version%";

	std::string when;
	const std::string file_header = "time_local,time_unix,duration,artist,title,album,album_artist,genre,release_year,codec,foobar_version\n";

	//playback_control::ptr m_playback_control = playback_control::get();
};

//static std::string get_local_time() {
//	std::ostringstream st;
//	std::string st_string;
//
//	SYSTEMTIME lt;
//	GetLocalTime(&lt);
//
//	st << lt.wHour << ":" << lt.wMinute << ":" << lt.wSecond << " " << lt.wMonth << "-" << lt.wDay << "-" << lt.wYear;
//	st_string = st.str();
//
//	return st_string;
//}
//
//static void write_string_to_file(const char* dir_path, const char* filename, std::string content) {
//	console::print("DEBUG: write_string_to_file");
//	std::ofstream myfile;
//
//	char file_path[MAX_PATH];
//
//	strncpy_s(file_path, dir_path, sizeof(file_path));
//	strncpy_s(file_path, filename, sizeof(file_path));
//
//	myfile.open(file_path, std::ios_base::app);
//	myfile << content;
//	myfile.close();
//}
//
//static bool is_file_empty(const char* dir_path, const char* filename)
//{
//	char file_path[MAX_PATH];
//
//	strncpy_s(file_path, dir_path, sizeof(file_path));
//	strncpy_s(file_path, filename, sizeof(file_path));
//
//	std::ifstream myfile(file_path);
//
//	return myfile.peek() == std::ifstream::traits_type::eof();
//}

//void on_new_track_update() noexcept {
//
//	console::print("DEBUG: on_new_track_update");
//	if (cfg_enabled_collection) {
//		if (tifo.is_empty()) {
//			titleformat_compiler::get()->compile_safe_ex(tifo, format_info);
//		}
//
//		if (m_playback_control->playback_format_title(NULL, track_info, tifo, NULL, playback_control::display_level_none)) {
//			// Succeeded already
//			playback_state = 1;
//			playback_length = m_playback_control->playback_get_length();
//			when = get_local_time();
//		}
//		else if (m_playback_control->is_playing()) {
//			// Starting playback but not done opening the first track yet
//			playback_state = 2;
//			playback_time = 0;
//		}
//		else {
//			// Other state
//			playback_state = 0;
//		}
//	}
//}

//void Collector::collect_track_data() noexcept {
//	console::print("DEBUG: collect_track_data");
//	std::ostringstream sl;
//	std::string stats_line;
//
//	sl << track_info << when << "\n";
//	stats_line = sl.str();
//
//	if (is_file_empty(cfg_data_path, filename)) {
//		write_string_to_file(cfg_data_path, filename, file_header);
//	}
//
//	write_string_to_file(cfg_data_path, filename, stats_line);
//}

//void Collector::on_time_update() noexcept {
//	console::print("DEBUG: on_time_update");
//	if (cfg_enabled_collection) {
//		if (playback_time / playback_length >= 1 / cfg_threshold) {
//			collect_track_data();
//		}
//		else {
//			playback_time += 1;
//		}
//		// playback_time = m_playback_control->playback_get_position();
//	}
//}

//void Collector::on_pause_update() {
//	if (cfg_enabled_collection) {
//		if (m_playback_control->is_playing()) {
//			playback_state = 1;
//		}
//		else {
//			playback_state = 0;
//		}
//	}
//}

//unsigned Collector::get_flags() {
//
//}
//
//void Collector::on_stopped_update() {
//
//}
//
//void Collector::on_volume_change() {
//
//}
//
//void Collector::on_playback_dynamic_info() {
//
//}
//
//void Collector::on_playback_dynamic_info_track() {
//
//}

//static service_impl_t<Collector> g_collector;

//class myinitquit : public initquit {
//public:
//	void on_init() {
//		playback_control::ptr m_playback_control = playback_control::get();;
//	}
//	void on_quit() {
//		console::print("Sample component: on_quit()");
//	}
//};
//
//static initquit_factory_t<myinitquit> g_myinitquit_factory;

static play_callback_static_factory_t<Collector> collector_g;