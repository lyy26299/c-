#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <memory>
#include <fstream>
#include <sstream>
#include <filesystem>

using namespace std;
class Employee {
protected:
    string name;
    int id;

public:
    Employee(string name, int id) : name(name), id(id) {}

    virtual float calculateSalary() = 0; // 纯虚函数，用于计算工资
    virtual void displayInfo() = 0; // 显示员工信息
    string getName() const { return name; }
    int getId() const { return id; }
};

class Teacher : public Employee {
private:
    map<string, int> courses; // 课程名称和课时数

public:
    // 修改后的构造函数
    Teacher(string name, int id, const map<string, int>& courses)
        : Employee(name, id), courses(courses) {}

    void addCourse(string courseName, int hours) {
        courses[courseName] = hours;
    }

    float calculateSalary() override {
        float salary = 0;
        for (auto& course : courses) {
            salary += course.second * 1000; // 每门课1000元
        }
        return salary;
    }

    const map<string, int>& getCourses() const {
        return courses;
    }
    void displayInfo() override {
        cout << "Teacher: " << name << ", ID: " << id << ", Courses: ";
        for (auto& course : courses) {
            cout << course.first << " (" << course.second << " hours), ";
        }
        cout << endl;
    }
};

class Administrator : public Employee {
private:
    string position; // 职务

public:
    Administrator(string name, int id, string position) : Employee(name, id), position(position) {}

    float calculateSalary() override {
        return 5000; // 固定工资
    }
    string getPosition() const {
        return position;
    }
    void displayInfo() override {
        cout << "Administrator: " << name << ", ID: " << id << ", Position: " << position << endl;
    }
};

class Logistics : public Employee {
private:
    int workingDays; // 工作日数

public:
    Logistics(string name, int id, int days) : Employee(name, id), workingDays(days) {}

    void setWorkingDays(int days) {
        workingDays = days;
    }

    float calculateSalary() override {
        return workingDays * 100; // 每个工作日100元
    }

    void displayInfo() override {
        cout << "Logistics: " << name << ", ID: " << id << ", Working Days: " << workingDays << ", Salary: " << calculateSalary() << endl;
    }
    int getWorkingDays() const {
        return workingDays;
    }
};


class StaffManagementSystem {
private:
    vector<shared_ptr<Employee>> employees;

public:
    void addTeacher(const string& name, int id, const map<string, int>& courses) {
        employees.push_back(make_shared<Teacher>(name, id, courses));
    }

    void addAdministrator(const string& name, int id, const string& position) {
        employees.push_back(make_shared<Administrator>(name, id, position));
    }

    void addLogistics(const string& name, int id, int workingDays) {
        employees.push_back(make_shared<Logistics>(name, id, workingDays));
    }

    void displayAllInfo() {
        for (const auto& emp : employees) {
            emp->displayInfo();
        }
    }

    void calculateAllSalaries() {
        for (auto& emp : employees) {
            float salary = emp->calculateSalary();
            cout << "员工 " << emp->getName() << " (ID: " << emp->getId() << ") 的工资: " << salary << endl;
        }
    }
    void saveToFile() {
        string filename = "employ.csv";
        ofstream outFile(filename);
        if (!outFile) {
            cerr << "无法打开或创建文件 " << filename << endl;
            return;
        }
        for (const auto& emp : employees) {
            // 先写入类型，然后是ID和姓名
            if (auto t = dynamic_pointer_cast<Teacher>(emp)) {
                outFile << "Teacher," << emp->getId() << "," << emp->getName() << ",";
                // 写入课程信息
                for (const auto& course : t->getCourses()) {
                    outFile << course.first << ":" << course.second << ";";
                }
            }
            else if (auto a = dynamic_pointer_cast<Administrator>(emp)) {
                outFile << "Administrator," << emp->getId() << "," << emp->getName() << ",";
                // 写入职务信息
                outFile << a->getPosition();
            }
            else if (auto l = dynamic_pointer_cast<Logistics>(emp)) {
                outFile << "Logistics," << emp->getId() << "," << emp->getName() << ",";
                // 写入工作日数
                outFile << l->getWorkingDays();
            }
            outFile << endl;
        }

        outFile.close();
    }

    void loadFromFile() {
        string filename = "employ.csv";
        ifstream inFile(filename);

        // 检查文件是否成功打开
        if (!inFile) {
            cout << "文件 " << filename << " 不存在，将创建一个新文件。" << endl;
            return;
        }

        string line, type, name, position, courseName, temp;
        int id, hours, workingDays;
        map<string, int> courses;

        while (getline(inFile, line)) {
            stringstream ss(line);
            getline(ss, type, ',');
            ss >> id;
            ss.ignore(); // 忽略逗号
            getline(ss, name, ',');

            if (type == "Teacher") {
                courses.clear();
                getline(ss, temp, ','); // 获取课程信息部分
                stringstream courseStream(temp);
                while (getline(courseStream, courseName, ':')) {
                    courseStream >> hours;
                    courseStream.ignore(); // 忽略分号
                    courses[courseName] = hours;
                    if (courseStream.peek() == ';') courseStream.ignore();
                }
                addTeacher(name, id, courses);
            }
            else if (type == "Administrator") {
                getline(ss, position);
                addAdministrator(name, id, position);
            }
            else if (type == "Logistics") {
                ss >> workingDays;
                addLogistics(name, id, workingDays);
            }
        }

        inFile.close();
    }

};

int main() {
    StaffManagementSystem sms;
    int choice;
    sms.loadFromFile();
    do {
        cout << "\n教职工管理系统" << endl;
        cout << "1. 添加教师" << endl;
        cout << "2. 添加管理人员" << endl;
        cout << "3. 添加后勤人员" << endl;
        cout << "4. 显示所有员工信息" << endl;
        cout << "5. 计算并显示所有员工工资" << endl;
        cout << "0. 退出" << endl;
        cout << "请输入您的选择: ";
        cin >> choice;

        string name, position, courseName;
        int id, workingDays, hours;
        map<string, int> courses;

        switch (choice) {
        case 1: // 添加教师
            cout << "输入教师姓名: ";
            cin >> name;
            cout << "输入教师工号: ";
            cin >> id;
            cout << "输入授课信息（课程名 课时数），输入 'done' 结束: ";
            while (true) {
                cin >> courseName;
                if (courseName == "done") break;
                cin >> hours;
                courses[courseName] = hours;
            }
            sms.addTeacher(name, id, courses);
            break;
        case 2: // 添加管理人员
            cout << "输入管理人员姓名: ";
            cin >> name;
            cout << "输入管理人员工号: ";
            cin >> id;
            cout << "输入管理人员职务: ";
            cin >> position;
            sms.addAdministrator(name, id, position);
            break;
        case 3: // 添加后勤人员
            cout << "输入后勤人员姓名: ";
            cin >> name;
            cout << "输入后勤人员工号: ";
            cin >> id;
            cout << "输入工作日数: ";
            cin >> workingDays;
            sms.addLogistics(name, id, workingDays);
            break;
        case 4: // 显示所有员工信息
            sms.displayAllInfo();
            break;
        case 5: // 计算并显示所有员工工资
            sms.calculateAllSalaries();
            sms.displayAllInfo();
            break;
        case 0: // 退出
            cout << "退出系统。" << endl;
            sms.saveToFile();
            break;
        default:
            cout << "无效选择，请重新输入。" << endl;
        }
    } while (choice != 0);

    return 0;
}