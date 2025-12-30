#include "storage.hpp"
#include <fstream>
#include <iostream>

namespace TodoApp {

    Storage::Storage() {
        load_from_disk();
    }

    void Storage::load_from_disk() {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cout << "[STORAGE] No database file found. Starting fresh.\n";
            return;
        }

        std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        if (content.empty()) return;

        auto json_data = crow::json::load(content);
        if (!json_data || json_data.t() != crow::json::type::List) return;

        std::lock_guard<std::mutex> lock(db_mutex);
        tasks_db.clear();
        int max_id = 0;

        for (const auto& item : json_data) {
            Task t;
            t.id = item["id"].i();
            t.title = item["title"].s();
            t.description = item["description"].s();
            t.status = item["status"].s();
            
            tasks_db[t.id] = t;
            if (t.id > max_id) max_id = t.id;
        }
        next_id = max_id + 1;
        std::cout << "[STORAGE] Loaded " << tasks_db.size() << " tasks from disk.\n";
    }

    void Storage::save_to_disk() {
        std::vector<crow::json::wvalue> list;
        for (const auto& [id, task] : tasks_db) {
            list.push_back(task.to_json());
        }
        crow::json::wvalue final_json = std::move(list);

        std::ofstream file(filename);
        if (file.is_open()) {
            file << final_json.dump(); 
        }
    }

    std::vector<Task> Storage::get_all() {
        std::lock_guard<std::mutex> lock(db_mutex);
        std::vector<Task> result;
        for (const auto& [id, task] : tasks_db) {
            result.push_back(task);
        }
        return result;
    }

    crow::json::wvalue Storage::get_one_json(int id) {
        std::lock_guard<std::mutex> lock(db_mutex);
        if (tasks_db.count(id) == 0) return {};
        return tasks_db[id].to_json();
    }

    bool Storage::exists(int id) {
        std::lock_guard<std::mutex> lock(db_mutex);
        return tasks_db.count(id) > 0;
    }

    Task Storage::create(const std::string& title, const std::string& desc, const std::string& status) {
        std::lock_guard<std::mutex> lock(db_mutex);
        Task t;
        t.id = next_id++;
        t.title = title;
        t.description = desc;
        t.status = status;
        
        tasks_db[t.id] = t;
        save_to_disk();
        return t;
    }

    bool Storage::update(int id, const crow::json::rvalue& json_body) {
        std::lock_guard<std::mutex> lock(db_mutex);
        if (tasks_db.count(id) == 0) return false;

        auto& task = tasks_db[id];
        if (json_body.has("title")) task.title = json_body["title"].s();
        if (json_body.has("description")) task.description = json_body["description"].s();
        if (json_body.has("status")) task.status = json_body["status"].s();

        save_to_disk();
        return true;
    }

    bool Storage::remove(int id) {
        std::lock_guard<std::mutex> lock(db_mutex);
        if (tasks_db.count(id) == 0) return false;
        
        tasks_db.erase(id);
        save_to_disk();
        return true;
    }

} // namespace TodoApp