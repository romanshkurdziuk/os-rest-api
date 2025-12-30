#pragma once
#include <string>
#include <vector>
#include "crow.h" // Чтобы использовать crow::json

namespace TodoApp {

    // Структура задачи
    struct Task {
        int id;
        std::string title;
        std::string description;
        std::string status; 

        crow::json::wvalue to_json() const {
            crow::json::wvalue json;
            json["id"] = id;
            json["title"] = title;
            json["description"] = description;
            json["status"] = status;
            return json;
        }
    };

}