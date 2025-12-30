#pragma once
#include <map>
#include <mutex>
#include <string>
#include <vector>
#include "crow.h"
#include "model.hpp"

namespace TodoApp {

    class Storage {
    private:
        const std::string filename = "db.json";
        std::map<int, Task> tasks_db;
        int next_id = 1;
        std::mutex db_mutex;

        // Внутренние методы
        void load_from_disk();
        void save_to_disk();

    public:
        Storage(); // Конструктор

        // CRUD методы
        std::vector<Task> get_all();
        crow::json::wvalue get_one_json(int id);
        bool exists(int id);
        Task create(const std::string& title, const std::string& desc, const std::string& status);
        bool update(int id, const crow::json::rvalue& json_body);
        bool remove(int id);
    };

}