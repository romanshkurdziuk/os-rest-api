#include "crow.h"
#include "model.hpp"
#include <map>
#include <mutex>

// --- IN-MEMORY DATABASE ---
// Используем map для быстрого поиска по ID.
// Защищаем данные мьютексом, так как Crow многопоточный!
std::map<int, TodoApp::Task> tasks_db;
std::mutex db_mutex;
int next_id = 1;

// Функция для заполнения тестовыми данными (Mock Data)
void populate_test_data() {
    std::lock_guard<std::mutex> lock(db_mutex);
    tasks_db[next_id] = {next_id++, "Buy Milk", "3.2% fat", "todo"};
    tasks_db[next_id] = {next_id++, "Learn C++", "Complete OS Colloquium", "in_progress"};
}

int main() {
    // Заполним базу парой задач
    populate_test_data();

    crow::SimpleApp app;

    // --- ENDPOINTS ---

    // 1. GET /tasks - Получить все задачи
    CROW_ROUTE(app, "/tasks")
    .methods(crow::HTTPMethod::GET)
    ([](){
        // Блокируем доступ к БД на время чтения
        std::lock_guard<std::mutex> lock(db_mutex);

        // Создаем JSON-массив
        std::vector<crow::json::wvalue> result_list;
        
        for (const auto& [id, task] : tasks_db) {
            result_list.push_back(task.to_json());
        }

        // Crow автоматически превратит vector<wvalue> в JSON-список
        crow::json::wvalue final_json;
        final_json = std::move(result_list);
        
        return final_json;
    });

    // Запуск сервера
    app.port(18080).multithreaded().run();
    
    return 0;
}