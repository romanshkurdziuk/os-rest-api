#include "crow.h"
#include "model.hpp"
#include <map>
#include <mutex>

// ==========================================
// WINDOWS FIX
// ==========================================
// Windows определяет макрос DELETE, который конфликтует с Crow.
// Мы должны удалить это определение, чтобы использовать crow::HTTPMethod::DELETE
#ifdef DELETE
#undef DELETE
#endif
// ==========================================

// --- IN-MEMORY DATABASE ---
std::map<int, TodoApp::Task> tasks_db;
std::mutex db_mutex;
int next_id = 1;

void populate_test_data() {
    std::lock_guard<std::mutex> lock(db_mutex);
    tasks_db[1] = {1, "Buy Milk", "3.2% fat", "todo"};
    tasks_db[2] = {2, "Learn C++", "Complete OS Colloquium", "in_progress"};
    next_id = 3;
}

int main() {
    populate_test_data();

    crow::SimpleApp app;

    // 1. GET /tasks
    CROW_ROUTE(app, "/tasks")
    .methods(crow::HTTPMethod::GET)
    ([](){
        std::lock_guard<std::mutex> lock(db_mutex);
        std::vector<crow::json::wvalue> result_list;
        for (const auto& [id, task] : tasks_db) {
            result_list.push_back(task.to_json());
        }
        crow::json::wvalue final_json;
        final_json = std::move(result_list);
        return final_json;
    });

    // 2. POST /tasks
    CROW_ROUTE(app, "/tasks")
    .methods(crow::HTTPMethod::POST)
    ([](const crow::request& req){
        auto x = crow::json::load(req.body);
        if (!x) return crow::response(400, "Invalid JSON");

        std::lock_guard<std::mutex> lock(db_mutex);
        
        TodoApp::Task new_task;
        new_task.id = next_id++;
        
        if (x.has("title")) new_task.title = x["title"].s();
        if (x.has("description")) new_task.description = x["description"].s();
        if (x.has("status")) new_task.status = x["status"].s();
        else new_task.status = "todo";

        tasks_db[new_task.id] = new_task;
        return crow::response(201, new_task.to_json());
    });

    // 3. GET /tasks/{id}
    CROW_ROUTE(app, "/tasks/<int>")
    .methods(crow::HTTPMethod::GET)
    ([](int id){
        std::lock_guard<std::mutex> lock(db_mutex);
        if (tasks_db.count(id) == 0) return crow::response(404, "Task not found");
        return crow::response(tasks_db[id].to_json());
    });

    // 4. PUT /tasks/{id}
    CROW_ROUTE(app, "/tasks/<int>")
    .methods(crow::HTTPMethod::PUT)
    ([](const crow::request& req, int id){
        auto x = crow::json::load(req.body);
        if (!x) return crow::response(400, "Invalid JSON");

        std::lock_guard<std::mutex> lock(db_mutex);
        
        if (tasks_db.count(id) == 0) return crow::response(404, "Task not found");

        if (x.has("title")) tasks_db[id].title = x["title"].s();
        if (x.has("description")) tasks_db[id].description = x["description"].s();
        if (x.has("status")) tasks_db[id].status = x["status"].s();

        return crow::response(200, tasks_db[id].to_json());
    });

    // 5. DELETE /tasks/{id}
    CROW_ROUTE(app, "/tasks/<int>")
    .methods(crow::HTTPMethod::DELETE)
    ([](int id){
        std::lock_guard<std::mutex> lock(db_mutex);
        
        if (tasks_db.count(id) == 0) return crow::response(404, "Task not found");

        tasks_db.erase(id);
        return crow::response(204); 
    });

    app.port(18080).multithreaded().run();
    return 0;
}