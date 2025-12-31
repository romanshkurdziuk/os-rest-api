# C++ REST API (Todo List Service)

![C++](https://img.shields.io/badge/C++-17-blue.svg?style=flat&logo=c%2B%2B)
![Platform](https://img.shields.io/badge/Platform-Windows-0078D6.svg?style=flat&logo=windows)
![Build](https://img.shields.io/badge/Build-CMake-064F8C.svg?style=flat&logo=cmake)
![License](https://img.shields.io/badge/License-MIT-green.svg)

A high-performance RESTful API for task management developed in C++17. The project implements a layered architecture (MVC) using the Crow microframework and features persistent data storage with thread-safe operations.

## Key Features

*   **Full CRUD Operations**: Create, Read, Update, and Delete tasks via HTTP methods.
*   **Data Persistence**: Tasks are automatically saved to `db.json` and loaded upon server restart.
*   **Input Validation**: Strict validation of incoming JSON data (status codes, required fields) to ensure data integrity.
*   **Thread Safety**: Implementation of `std::mutex` for safe concurrent access to the storage.
*   **Dependency Management**: Automated dependency fetching (Crow, Asio) via CMake `FetchContent`.
*   **Separation of Concerns**: Logic is decoupled into Model, Storage, and Routing layers.

## API Reference

The API runs on port `18080` by default.

| Method | Endpoint | Description |
| :--- | :--- | :--- |
| `GET` | `/tasks` | Retrieve a list of all tasks. |
| `POST` | `/tasks` | Create a new task. |
| `GET` | `/tasks/{id}` | Retrieve a specific task by ID. |
| `PUT` | `/tasks/{id}` | Update an existing task (partial updates allowed). |
| `DELETE` | `/tasks/{id}` | Remove a task permanently. |

### Data Model

**Task Object Structure:**

```json
{
  "id": 1,
  "title": "Complete Project",
  "description": "Write documentation and unit tests",
  "status": "in_progress"
}
```

*Valid status values:* `todo`, `in_progress`, `done`.

## Technology Stack

*   **Language**: C++17
*   **Web Framework**: [Crow](https://github.com/CrowCpp/Crow) (v1.2.0)
*   **Networking**: Asio (Standalone)
*   **Build System**: CMake 3.14+
*   **Format**: JSON

## Installation and Build

This project requires a C++ compiler (MSVC, GCC, or Clang) and CMake installed.

1.  **Clone the repository:**
    ```bash
    git clone https://github.com/YOUR_USERNAME/todo_api_cpp.git
    cd todo_api_cpp
    ```

2.  **Configure the project:**
    ```bash
    mkdir build
    cd build
    cmake ..
    ```

3.  **Build:**
    ```bash
    cmake --build .
    ```

4.  **Run:**
    *   Windows: `.\Debug\todo_server.exe`
    *   Linux/macOS: `./todo_server`

The server will start at `http://localhost:18080`.

## Usage Examples (PowerShell)

**Create a Task:**
```powershell
Invoke-RestMethod -Method Post -Uri "http://localhost:18080/tasks" `
  -ContentType "application/json" `
  -Body '{"title": "Refactor Code", "description": "Separate storage logic", "status": "todo"}'
```

**Get All Tasks:**
```powershell
Invoke-RestMethod -Uri "http://localhost:18080/tasks"
```

**Update Status:**
```powershell
Invoke-RestMethod -Method Put -Uri "http://localhost:18080/tasks/1" `
  -ContentType "application/json" `
  -Body '{"status": "done"}'
```

**Delete Task:**
```powershell
Invoke-RestMethod -Method Delete -Uri "http://localhost:18080/tasks/1"
```

## Project Structure

*   `src/main.cpp` - Entry point and HTTP routing configuration.
*   `src/model.hpp` - Data structures and JSON serialization logic.
*   `src/storage.hpp` - Interface for the persistence layer.
*   `src/storage.cpp` - Implementation of file I/O operations and database management.
*   `CMakeLists.txt` - Build configuration and dependency management.