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
	void on_new_track_update() {
		playback_time = 0;
		playback_state = 0;
		//playback_control::ptr m_playback_control = playback_control::get();
		//console::print("DEBUG: on_new_track_update");
		if (cfg_enabled_collection) {
			track_logged = false;
	
			if (tifo.is_empty()) {
				//console::print("DEBUG: TIFO IS EMPTY");
				titleformat_compiler::get()->compile_safe_ex(tifo, format_info);
			}

			if (playback_control::get()->playback_format_title(NULL, track_info, tifo, NULL, playback_control::display_level_none) && playback_state != 1) {
				// Succeeded already
				//console::print("DEBUG: PLAYBACK SUCCEEDED");
				titleformat_compiler::get()->compile_safe_ex(tpath, "%path%");
				playback_control::get()->playback_format_title(NULL, track_path, tpath, NULL, playback_control::display_level_none);
				console::printf("Logging %s", track_path.c_str());

				playback_state = 1;
				playback_length = playback_control::get()->playback_get_length();
				times current_times = get_local_time();
				when_lt = current_times.time_lt;
				when_st = current_times.time_st;
			}
			else if (playback_control::get()->is_playing()) {
				// Starting playback but not done opening the first track yet
				//console::print("DEBUG: STILL OPENING!!!");
				playback_state = 2;
				playback_time = 0;
			}
			else {
				// Other state
				playback_state = 0;
			}
		}
		else { track_logged = true; }
	};

	void on_track_info_edited() {
		// Called when the track info is edited during playback (?)
		// This appears to happen with the foo_playcount component (when the play count gets updated)
		if (cfg_enabled_collection) {
			if (playback_state) {
				// Rewrites track_info with the (assumedly) changed info
				playback_control::get()->playback_format_title(NULL, track_path, tpath, NULL, playback_control::display_level_none);
			}
		}
	};

	void on_seek() {
		if (cfg_enabled_collection) {
			if (playback_time == 0.0) {
				track_logged = false;
			}
			//console::print("what do you seek");
		}
	}

	void on_time_update() {
		//double playback_time = playback_control::get()->playback_get_position();
		//double playback_length = playback_control::get()->playback_get_length();
		//console::print("DEBUG: on_time_update");
		//console::print(playback_time);

		if ((!track_logged) && (cfg_enabled_collection)) {
			if ((playback_time / playback_length) >= (cfg_threshold / 100.0)) {
				//console::print("DEBUG: THRESHOLD PASSED!");
				track_logged = true;
				collect_track_data();
				console::printf("Logged %s", track_path.c_str());
			}
			else {
				//console::print("DEBUG: TIME UP! TIME UP!");
				playback_time += 1;
			}
			//playback_time = m_playback_control->playback_get_position();
		}
	};

	// ...
	void collect_track_data() {
		//console::print("DEBUG: collect_track_data");
		std::ostringstream sl;
		std::string stats_line;

		sl << when_lt << "," << when_st << "," << track_info << "\n";
		stats_line = sl.str();

		if (is_file_empty(cfg_data_path.toString(), cfg_filename.toString() + file_ext)) {
			write_string_to_file(cfg_data_path.toString(), cfg_filename.toString() + file_ext, file_header);
		}

		write_string_to_file(cfg_data_path.toString(), cfg_filename.toString() + file_ext, stats_line);
		//console::print("DEBUG: INFO WRITTEN!");
	};

	// called
	void on_playback_new_track(metadb_handle_ptr p_track) { on_new_track_update(); }
	void on_playback_starting(playback_control::t_track_command p_command, bool p_paused) { on_new_track_update(); }
	void on_playback_edited(metadb_handle_ptr p_track) { on_track_info_edited(); };
	void on_playback_time(double p_time) { on_time_update(); }

	// empty
	void on_playback_stop(play_control::t_stop_reason p_reason) {};
	void on_playback_seek(double p_time) { on_seek(); };
	void on_playback_pause(bool p_state) {};
	void on_volume_change(float p_new_val) {};
	void on_playback_dynamic_info(const file_info & p_info) {};
	void on_playback_dynamic_info_track(const file_info & p_info) {};

	// flags
	virtual unsigned get_flags() { return flag_on_playback_new_track | flag_on_playback_starting | flag_on_playback_edited | flag_on_playback_time | flag_on_playback_seek; }
	
private:
	
	int playback_state;
	double playback_time;
	double playback_length;
	bool pause_state;
	bool track_logged;
	const std::string file_header = "time_local,time_gmt,duration,artist,title,album,album_artist,genre,release_year,codec,foobar_version\n";
	std::string file_ext = ".csv";
	std::string when_st;
	std::string when_lt;

	// https://wiki.hydrogenaud.io/index.php?title=Foobar2000:Title_Formatting_Reference
	titleformat_object::ptr tifo;
	titleformat_object::ptr tpath;
	
	pfc::string8 track_info;
	pfc::string8 track_path;
	const char * format_info =
		"%length_ex%," // extended length (hh:mm:ss.msms)
		"$insert($insert(%artist%,$char(34),$len(%artist%)),$char(34),0)," // artist
		"$insert($insert($replace(%title%,$char(34),$char(92)$char(34)),$char(34),$len(%title%)),$char(34),0)," // title
		"$insert($insert(%album%,$char(34),$len(%album%)),$char(34),0)," // album
		"$insert($insert(%album_artist%,$char(34),$len(%album_artist%)),$char(34),0)," // album artist
		"$insert($insert(%genre%,$char(34),$len(%genre%)),$char(34),0)," // genre
		"%date%," // year of release
		"%codec%," // file codec
		"%_foobar2000_version%"; // foobar version
		//"$insert($insert($insert($insert(%artist%,$char(45),$len(%artist%)),%album%,$add($len(%artist%),1)),$char(34),$add($add($len(%artist%),$len(%album%)),3)),$char(34),0),"
	
	
	//pfc::string8 format_info = "%length_ex%,%artist%,%title%,%album%,%album_artist%,%genre%,%date%,%codec%,%_foobar2000_version%";
	
};

static play_callback_static_factory_t<Collector> collector_g;