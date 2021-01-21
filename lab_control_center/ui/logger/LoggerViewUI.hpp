// MIT License
// 
// Copyright (c) 2020 Lehrstuhl Informatik 11 - RWTH Aachen University
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
// 
// This file is part of cpm_lab.
// 
// Author: i11 - Embedded Software, RWTH Aachen University

#pragma once

#include "defaults.hpp"
#include <algorithm>
#include <atomic>
#include <cassert>
#include <chrono>
#include <ctime>
#include <future>
#include <map>
#include <memory>
#include <mutex>
#include <sstream>
#include <string>
#include <thread>
#include <gtkmm/builder.h>
#include <gtkmm.h>
#include <glib.h>

#include "LogLevelSetter.hpp"
#include "LoggerModelRecord.hpp"
#include "TimerTrigger.hpp"

#include "cpm/AsyncReader.hpp"
#include "cpm/get_topic.hpp"
#include "cpm/ParticipantSingleton.hpp"

#include "src/LogStorage.hpp"

/**
 * \brief UI Class for the Logs Tab in the LCC. Show most recent logs (->max_log_amount), is connected to LogStorage & allows for search in more than most_recent_logs
 * using Regex, allows to set the log_level for the whole domain
 * \ingroup lcc_ui
 */
class LoggerViewUI {
private:
    //! GTK builder for the UI
    Glib::RefPtr<Gtk::Builder> ui_builder;
    //! Parent element of the UI, which can be integrated into another UI element, e.g. a window
    Gtk::Box* parent;
    //! TreeView that contains the log messages
    Gtk::TreeView* logs_treeview;
    //! Header for the treeview
    Gtk::Label* logs_label_header;
    //! Makes logs_treeview scrollable
    Gtk::ScrolledWindow* logs_scrolled_window;
    //! Enables / disables automatic scrolling in logs_treeview
    Gtk::CheckButton* autoscroll_check_button;
    //! Allows to search for log entries
    Gtk::SearchEntry* logs_search_entry;
    //! Set the type / column within which to search (log ID, log timestamp, log message content, all of them)
    Gtk::ComboBoxText* logs_search_type;
    //! Set the log level within the network & to be displayed
    Gtk::ComboBoxText* log_level_combobox;
    //! Displays warnings depending on the selected log level, e.g. that it might cause performance issues
    Gtk::Label* label_log_status;

    //! Max. amount of logs shown in logs_treeview at the same time (for performance reasons)
    const long max_log_amount = 100;

    /**
     * \brief Callback function for log_level_combobox.
     * Sets the new log level. Its also sent within the network to other participants, so they stop logging now irrelevant messages.
     * Resets the log entries and reloads to only show those that are relevent with the new log level.
     */
    void on_log_level_changed();
    //! Currently set log level
    std::atomic_ushort log_level;
    //! To tell the UI that a log level was changed & perform the required updates in the UI thread
    std::atomic_bool log_level_changed{false};
    //! Labels for log_level_combobox (the string vector is transformed to the ustring vector in the settings, where we also add the selection options to the UI)
    std::vector<std::string> log_labels = {"0 - No logs", "1 - Critical errors", "2 - All Errors", "3 - Verbose"};
    //! Vector used so that we can infer the level number from the index (no need to transform the ustring to a number)
    std::vector<Glib::ustring> log_level_labels; 

    //! TreeView Layout
    LoggerModelRecord log_record;
    //! Status storage for the UI, contains all shown logs
    Glib::RefPtr<Gtk::ListStore> log_list_store;

    //UI update functions and objects
    /**
     * \brief Function called by ui_thread to periodically activate the dispatcher that in turn calls GTK's UI thread, to perform UI updates
     */
    void update_ui();
    /**
     * \brief Callback for GTK's UI dispatcher, all UI changes done during runtime should be performed within this function
     */
    void dispatcher_callback();
    //! To communicate between the current thread and GTK's UI thread
    Glib::Dispatcher ui_dispatcher;
    //! UI thread that periodically ativates the GTK dispatcher to perform an update in the UI
    std::thread ui_thread;
    //! Tells ui_thread if it should still be running
    std::atomic_bool run_thread;

    /**
     * \brief Callback function to enable autoscroll if autoscroll_check_button is used
     * \param allocation Irrelevant parameter that is part of the callback
     */
    void on_size_change_autoscroll(Gtk::Allocation& allocation);

    //! Object that holds (new) log data, also data that is not currently shown in the UI
    std::shared_ptr<LogStorage> log_storage;

    //Filtering and filter types
    //! For filtering, ID column
    Glib::ustring type_id_ustring = "ID";
    //! For filtering, log message column
    Glib::ustring type_content_ustring = "Content";
    //! For filtering, timestamp column
    Glib::ustring type_timestamp_ustring = "Timestamp";
    //! For filtering, all columns
    Glib::ustring type_all_ustring = "All";
    /**
     * \brief Callback for a filter type change. Start search if a filter is applied (text in logs_search_entry).
     */
    void on_filter_type_changed();
    /**
     * \brief Disable searching and the filter. Called by logs_search_entry if cleared.
     */
    void stop_search();
    /**
     * \brief Search was changed, called by logs_search_entry if the entry changes.
     * Continue with new one if a filter is applied (text in logs_search_entry), else stop the search.
     */
    void search_changed();
    std::atomic_bool filter_active;
    //Promise and future for search thread
    std::promise<std::vector<Log>> search_promise;
    std::future<std::vector<Log>> search_future;
    //Extra mutex because promise can be reset right after the UI thread checked if a future exists
    std::mutex promise_reset_mutex;
    std::atomic_bool search_thread_running;
    std::atomic_bool search_reset; //Set after search was finished, to retrieve all old log messages in the UI thread again
    std::thread search_thread;
    /**
     * \brief 
     */
    void start_new_search_thread();
    /**
     * \brief 
     */
    void kill_search_thread();

    //Add log entry to UI (only call from UI thread!)
    void add_log_entry(const Log& entry);

    //Delete old logs
    void delete_old_logs(const long max_amount);
    void reset_list_store();

    //Callback for tooltip
    bool tooltip_callback(int x, int y, bool keyboard_tooltip, const Glib::RefPtr<Gtk::Tooltip>& tooltip);

    //Check for scroll event to turn off automatic scrolling
    bool scroll_callback(GdkEventScroll* scroll_event);

    //Variable for the reset action (is performed within the UI)
    std::atomic_bool reset_logs;

public:
    LoggerViewUI(std::shared_ptr<LogStorage> logStorage);
    ~LoggerViewUI();
    Gtk::Widget* get_parent();

    //Might be called from outside, e.g. when a new 'simulation' is run
    void reset();
};