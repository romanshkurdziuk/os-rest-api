#pragma once
#include <string>
#include <vector>
#include <set> // Важно для std::set
#include "crow.h"

namespace TodoApp {

    struct Task {
        int id;
        std::string title;
        std::string description;
        std::string status; // "todo", "in_progress", "done"

        // Конвертация В JSON
        crow::json::wvalue to_json() const {
            crow::json::wvalue json;
            json["id"] = id;
            json["title"] = title;
            json["description"] = description;
            json["status"] = status;
            return json;
        }

        // --- ВАЛИДАЦИЯ (Этого метода у тебя не хватало) ---
        bool is_valid_status() const {
            static const std::set<std::string> valid_statuses = {"todo", "in_progress", "done"};
            return valid_statuses.count(status) > 0;
        }
    };

}