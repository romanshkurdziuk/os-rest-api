#include "crow.h"
#include "model.hpp"
#include <map>
#include <mutex>

// --- IN-MEMORY DATABASE ---
std::map<int, TodoApp::Task> tasks_db;
std::mutex db_mutex;
int next_id = 1;

void populate_test_data() {
    std::lock_guard<std::mutex> lock(db_mutex);
    
    // Явно создаем задачи с конкретными ID
    tasks_db[1] = {1, "Buy Milk", "3.2% fat", "todo"};
    tasks_db[2] = {2, "Learn C++", "Complete OS Colloquium", "in_progress"};
    
    // Обновляем счетчик для будущих задач (которые добавим через POST)
    next_id = 3;
}

int main() {
    populate_test_data();

    crow::SimpleApp app;

    // ---------------------------------------------------------
    // 1. GET /tasks (Получить список)
    // ---------------------------------------------------------
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

    // ---------------------------------------------------------
    // 2. GET /tasks/{id} (Получить одну задачу)
    // ---------------------------------------------------------
    // <int> означает, что URL должен содержать число
    CROW_ROUTE(app, "/tasks/<int>")
    .methods(crow::HTTPMethod::GET)
    ([](int id){
        std::lock_guard<std::mutex> lock(db_mutex);
        
        if (tasks_db.count(id) == 0) {
            return crow::response(404, "Task not found");
        }

        return crow::response(tasks_db[id].to_json());
    });

    // ---------------------------------------------------------
    // 3. POST /tasks (Создать задачу)
    // ---------------------------------------------------------
    CROW_ROUTE(app, "/tasks")
    .methods(crow::HTTPMethod::POST)
    ([](const crow::request& req){
        // Парсим JSON из тела запроса
        auto x = crow::json::load(req.body);
        
        if (!x) {
            return crow::response(400, "Invalid JSON");
        }

        // Блокируем БД для записи
        std::lock_guard<std::mutex> lock(db_mutex);
        
        // Создаем новую задачу
        TodoApp::Task new_task;
        new_task.id = next_id++;
        
        // Безопасно извлекаем данные (если полей нет, будут пустые строки)
        if (x.has("title")) new_task.title = x["title"].s();
        if (x.has("description")) new_task.description = x["description"].s();
        if (x.has("status")) new_task.status = x["status"].s();
        else new_task.status = "todo"; // Значение по умолчанию

        // Сохраняем
        tasks_db[new_task.id] = new_task;

        // Возвращаем 201 Created и созданный объект
        return crow::response(201, new_task.to_json());
    });

    // Запуск сервера
    app.port(18080).multithreaded().run();
    
    return 0;
}