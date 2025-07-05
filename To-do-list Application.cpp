#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <algorithm>
#include <iomanip>
#include <cctype>
#include <sstream>
#include <ctime>

using namespace std;

void centerText(const string& text, int width = 80) {
    int padding = (width - text.length()) / 2;
    if (padding > 0) cout << string(padding, ' ');
    cout << text;
}

string getCenteredInput(const string& prompt) {
    centerText(prompt);
    cout << "\n";
    centerText("> ");
    string input;
    getline(cin, input);
    return input;
}

struct Task {
    string description;
    string category;
    bool completed;
    time_t creationTime;
};

class TodoList {
private:
    vector<Task> tasks;
    string filename;

    string getCurrentDateTime() {
        time_t now = time(nullptr);
        tm* localTime = localtime(&now);
        char buffer[20];
        strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", localTime);
        return string(buffer);
    }

    void saveToFile() {
        if (filename.empty()) return;

        ofstream file(filename);
        if (file.is_open()) {
            for (const auto& task : tasks) {
                file << task.description << "|" 
                     << task.category << "|"
                     << task.completed << "|"
                     << task.creationTime << "\n";
            }
            file.close();
            centerText("Tasks saved to " + filename + " at " + getCurrentDateTime() + "\n\n");
        } else {
            centerText("Error: Could not save to file " + filename + "\n\n");
        }
    }

    void loadFromFile() {
        if (filename.empty()) return;

        ifstream file(filename);
        if (file.is_open()) {
            tasks.clear();
            string line;
            while (getline(file, line)) {
                stringstream ss(line);
                string part;
                vector<string> parts;
                
                while (getline(ss, part, '|')) {
                    parts.push_back(part);
                }
                
                if (parts.size() >= 4) {
                    Task task;
                    task.description = parts[0];
                    task.category = parts[1];
                    task.completed = (parts[2] == "1");
                    task.creationTime = stol(parts[3]);
                    tasks.push_back(task);
                }
            }
            file.close();
            centerText("Tasks loaded from " + filename + "\n\n");
        } else {
            centerText("New todo list created. File will be saved when you add tasks.\n\n");
        }
    }

public:
    TodoList() = default;

    void setFilename(const string& file) {
        filename = file;
        if (!filename.empty()) {
            loadFromFile();
        }
    }

    string getFilename() const {
        return filename;
    }

    void addTask(const string& description, const string& category) {
        if (description.empty()) {
            centerText("Error: Task description cannot be empty\n\n");
            return;
        }

        Task task;
        task.description = description;
        task.category = category;
        task.completed = false;
        task.creationTime = time(nullptr);
        tasks.push_back(task);
        saveToFile();
    }

    void displayTasks(const string& categoryFilter = "all", bool showCompleted = false) const {
        int taskNumber = 1;
        centerText("=== TO-DO LIST (" + (filename.empty() ? "No file" : filename) + ") ===\n\n");
        
        if (tasks.empty()) {
            centerText("No tasks found.\n\n");
            return;
        }

        for (const auto& task : tasks) {
            if ((categoryFilter == "all" || task.category == categoryFilter) &&
                (showCompleted || !task.completed)) {
                
                centerText(to_string(taskNumber) + ". ");
                cout << (task.completed ? "[X] " : "[ ] ");
                cout << task.description;
                
                if (!task.category.empty()) {
                    cout << " (" << task.category << ")";
                }
                
                char timeStr[20];
                strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M", localtime(&task.creationTime));
                cout << " - " << timeStr << "\n";
                
                taskNumber++;
            }
        }
        
        if (taskNumber == 1) {
            centerText("No matching tasks found.\n");
        }
        cout << "\n";
    }

    bool markTaskCompleted(int index) {
        if (index < 1 || index > tasks.size()) {
            centerText("Error: Invalid task number\n\n");
            return false;
        }
        
        tasks[index-1].completed = true;
        saveToFile();
        centerText("Task marked as completed.\n\n");
        return true;
    }

    bool removeTask(int index) {
        if (index < 1 || index > tasks.size()) {
            centerText("Error: Invalid task number\n\n");
            return false;
        }
        
        tasks.erase(tasks.begin() + index - 1);
        saveToFile();
        centerText("Task removed successfully.\n\n");
        return true;
    }

    vector<string> getCategories() const {
        vector<string> categories;
        for (const auto& task : tasks) {
            if (!task.category.empty() && 
                find(categories.begin(), categories.end(), task.category) == categories.end()) {
                categories.push_back(task.category);
            }
        }
        return categories;
    }

    size_t taskCount() const {
        return tasks.size();
    }
};

void displayMainMenu() {
    centerText("===== TO-DO LIST MANAGER =====\n\n");
    centerText("1. Create/Open Todo List\n");
    centerText("2. Exit\n\n");
}

void displayTodoMenu(const string& filename) {
    centerText("===== TODO LIST: " + (filename.empty() ? "No file" : filename) + " =====\n\n");
    centerText("1. Add New Task\n");
    centerText("2. View All Tasks\n");
    centerText("3. Mark Task as Completed\n");
    centerText("4. Remove Task\n");
    centerText("5. Filter by Category\n");
    centerText("6. View Completed Tasks\n");
    centerText("7. Change Todo List File\n");
    centerText("8. Back to Main Menu\n\n");
}

int main() {
    TodoList todoList;
    bool running = true;
    bool inTodoList = false;

    while (running) {
        if (!inTodoList) {
            displayMainMenu();
            string choice = getCenteredInput("Enter your choice (1-2):");

            if (choice == "1") {
                string filename = getCenteredInput("Enter filename for your todo list (e.g., mylist.txt):");
                
                if (filename.empty()) {
                    centerText("Error: Filename cannot be empty\n\n");
                    continue;
                }

            
                if (filename.find('.') == string::npos) {
                    filename += ".txt";
                }

                todoList.setFilename(filename);
                inTodoList = true;
                centerText("Todo list '" + filename + "' is ready!\n\n");
            }
            else if (choice == "2" || choice == "exit" || choice == "quit") {
                running = false;
                centerText("Goodbye! Your tasks have been saved.\n\n");
            }
            else {
                centerText("Invalid choice. Please try again.\n\n");
            }
        } 
        else {
            displayTodoMenu(todoList.getFilename());
            string choice = getCenteredInput("Enter your choice (1-8):");

            if (choice == "1") {
                string description = getCenteredInput("Enter task description:");
                string category = getCenteredInput("Enter category (optional):");
                todoList.addTask(description, category);
            }
            else if (choice == "2") {
                todoList.displayTasks();
            }
            else if (choice == "3") {
                todoList.displayTasks();
                if (todoList.taskCount() > 0) {
                    string input = getCenteredInput("Enter task number to mark as completed:");
                    try {
                        int index = stoi(input);
                        todoList.markTaskCompleted(index);
                    } catch (...) {
                        centerText("Error: Please enter a valid number\n\n");
                    }
                }
            }
            else if (choice == "4") {
                todoList.displayTasks();
                if (todoList.taskCount() > 0) {
                    string input = getCenteredInput("Enter task number to remove:");
                    try {
                        int index = stoi(input);
                        todoList.removeTask(index);
                    } catch (...) {
                        centerText("Error: Please enter a valid number\n\n");
                    }
                }
            }
            else if (choice == "5") {
                vector<string> categories = todoList.getCategories();
                if (categories.empty()) {
                    centerText("No categories found. Add tasks with categories first.\n\n");
                    continue;
                }

                centerText("Available categories:\n");
                for (size_t i = 0; i < categories.size(); i++) {
                    centerText(to_string(i+1) + ". " + categories[i] + "\n");
                }
                centerText(to_string(categories.size()+1) + ". All Categories\n\n");
                
                string input = getCenteredInput("Select category (1-" + to_string(categories.size()+1) + "):");
                try {
                    int catChoice = stoi(input);
                    if (catChoice >= 1 && catChoice <= categories.size()) {
                        todoList.displayTasks(categories[catChoice-1]);
                    } else if (catChoice == categories.size()+1) {
                        todoList.displayTasks();
                    } else {
                        centerText("Error: Invalid category number\n\n");
                    }
                } catch (...) {
                    centerText("Error: Please enter a valid number\n\n");
                }
            }
            else if (choice == "6") {
                todoList.displayTasks("all", true);
            }
            else if (choice == "7") {
                string filename = getCenteredInput("Enter new filename (e.g., work_tasks.txt):");
                
                if (filename.empty()) {
                    centerText("Error: Filename cannot be empty\n\n");
                    continue;
                }

           
                if (filename.find('.') == string::npos) {
                    filename += ".txt";
                }

                todoList.setFilename(filename);
                centerText("Switched to todo list: " + filename + "\n\n");
            }
            else if (choice == "8") {
                inTodoList = false;
                centerText("Returning to main menu.\n\n");
            }
            else if (choice == "exit" || choice == "quit") {
                running = false;
                centerText("Goodbye! Your tasks have been saved.\n\n");
            }
            else {
                centerText("Invalid choice. Please try again.\n\n");
            }
        }
    }

    return 0;
}