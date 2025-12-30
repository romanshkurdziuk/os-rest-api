#include "crow.h"
#include "storage.hpp"

// Windows Fix
#ifdef DELETE
#undef DELETE
#endif

TodoApp::Storage storage;

int main() {
    crow::SimpleApp app;

    // 1. GET /tasks
    CROW_ROUTE(app, "/tasks")
    .methods(crow::HTTPMethod::GET)
    ([](){
        auto tasks = storage.get_all();
        std::vector<crow::json::wvalue> json_list;
        for (const auto& t : tasks) json_list.push_back(t.to_json());
        
        crow::json::wvalue final_json = std::move(json_list);
        return final_json;
    });

    // 2. POST /tasks
    CROW_ROUTE(app, "/tasks")
    .methods(crow::HTTPMethod::POST)
    ([](const crow::request& req){
        auto x = crow::json::load(req.body);
        if (!x) return crow::response(400, "Invalid JSON");

        // FIX: Явное приведение к std::string
        if (!x.has("title") || std::string(x["title"].s()).empty()) {
            return crow::response(400, "Title is required");
        }

        // FIX: Приводим обе части тернарного оператора к std::string
        std::string status = x.has("status") ? std::string(x["status"].s()) : std::string("todo");
        
        TodoApp::Task temp; temp.status = status;
        if (!temp.is_valid_status()) {
            return crow::response(400, "Invalid status");
        }

        std::string desc = x.has("description") ? std::string(x["description"].s()) : std::string("");

        auto task = storage.create(std::string(x["title"].s()), desc, status);
        return crow::response(201, task.to_json());
    });

    // 3. GET /tasks/{id}
    CROW_ROUTE(app, "/tasks/<int>")
    .methods(crow::HTTPMethod::GET)
    ([](int id){
        auto json = storage.get_one_json(id);
        if (json.t() == crow::json::type::Null) return crow::response(404, "Task not found");
        return crow::response(json);
    });

    // 4. PUT /tasks/{id}
    CROW_ROUTE(app, "/tasks/<int>")
    .methods(crow::HTTPMethod::PUT)
    ([](const crow::request& req, int id){
        auto x = crow::json::load(req.body);
        if (!x) return crow::response(400, "Invalid JSON");

        if (x.has("status")) {
            TodoApp::Task temp; 
            // FIX: Явное преобразование
            temp.status = std::string(x["status"].s());
            if (!temp.is_valid_status()) return crow::response(400, "Invalid status");
        }

        if (!storage.update(id, x)) {
            return crow::response(404, "Task not found");
        }
        return crow::response(200, storage.get_one_json(id));
    });

    // 5. DELETE /tasks/{id}
    CROW_ROUTE(app, "/tasks/<int>")
    .methods(crow::HTTPMethod::DELETE)
    ([](int id){
        if (!storage.remove(id)) {
            return crow::response(404, "Task not found");
        }
        return crow::response(204); 
    });

    app.port(18080).multithreaded().run();
    return 0;
}