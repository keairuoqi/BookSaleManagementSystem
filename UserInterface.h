#pragma once

#include <mysql.h>
#include <string>
#include <vector>
#include <iomanip>
#include <iostream>

using namespace std;

// 输出系统提示
void print_guide(const char* str) {
    cout << "<系统提示> " << str << endl;
}


// 输出表格的边框
void print_table_line(vector<int> widths, int col) {
    for (int i = 0; i < col; ++i) {
        cout << "+-";
        for (int j = 0; j <= widths[i]; ++j) {
            cout << '-';
        }
    }
    cout << "+" << endl;
}


// 输出表格
void print_table(MYSQL_RES* res) {
    int i, j;
    vector<vector<string> > ele;
    vector<string> tele;
    vector<int> widths;
    int col = mysql_num_fields(res);
    unsigned long long row = mysql_num_rows(res);
    MYSQL_FIELD* field = mysql_fetch_fields(res);
    MYSQL_ROW rows;
    for (i = 0; i < col; ++i) {
        string t = field[i].name;
        widths.push_back((int)t.size());
        tele.push_back(t);
    }
    ele.push_back(tele);
    for (i = 1; i <= row; ++i) {
        tele.clear();
        rows = mysql_fetch_row(res);
        for (j = 0; j < col; ++j) {
            if (strlen(rows[j]) > widths[j]) widths[j] = (int)strlen(rows[j]);
            tele.push_back(rows[j]);
        }
        ele.push_back(tele);
    }
    print_table_line(widths, col);
    for (int i = 0; i < col; ++i) {
        cout << "| " << setw(widths[i]) << setiosflags(ios::left) << ele[0][i] << " ";
    }
    cout << "|" << endl;
    print_table_line(widths, col);
    for (int j = 1; j <= row; ++j) {
        for (int i = 0; i < col; ++i) {
            cout << "| " << setw(widths[i]) << setiosflags(ios::left) << ele[j][i] << " ";
        }
        cout << "|" << endl;
    }
    print_table_line(widths, col);
}

// 输入选择
int get_cmd() {
    string temp;
    cout << "<选择操作> ";
    cin >> temp;
    int cmd = temp[0] - '0';
    return cmd;
}

// 输入浮点数信息
float get_info_float(const char* str, const char* key) {
    float price;
    print_guide(str);
    cout << "[" << key << "] ";
    cin >> price;
    return price;
}

// 输入字符串信息
string get_info_string(const char* str, const char* key) {
    string res;
    print_guide(str);
    cout << "[" << key << "] ";
    cin >> res;
    return res;
}

// 输入整数信息
int get_info_int(const char* str, const char* key) {
    int amount;
    print_guide(str);
    cout << "[" << key << "] ";
    cin >> amount;
    return amount;
}

// 在用户操作界面居中输出字符串
void print_center(char ch, string str) {
    size_t half = (60 - str.size()) / 2;
    for (int i = 0; i < half; ++i) 
        cout << ch;
    cout << str;
    for (int i = 0; i < 60 - half - str.size(); ++i) 
        cout << ch;
    cout << endl;
}

// 输出错误报告
void print_error(const char* str) {
    print_center('!', (string)"操作出错!");
    cout << endl;
    cout << "[错误报告] " << str << endl;
    print_guide("请重试。");
    cout << endl;
}

void print_welcome() {
    print_center('*', " 欢迎来到图书销售管理系统! ");
}

void print_bye() {
    cout << endl << endl;
    print_center('*', " 退出管理系统。 ");
    cout << endl << endl << endl;
}

void print_menu() {
    cout << endl << endl;
    print_center('-', " 操作选项 ");
    cout << endl;
    print_center('~', "~");
    cout << "           1 -------------------- 显示图书库存" << endl;
    cout << "           2 -------------------- 显示进货清单" << endl;
    cout << "           3 ---------------------------- 进货" << endl;
    cout << "           4 ------------------------ 出售图书" << endl;
    cout << "           5 ---------------------------- 退货" << endl;
    cout << "           6 ------ 按时间顺序显示销售统计情况" << endl;
    cout << "           7 ---------------- 显示销售量排行榜" << endl;
    cout << "           8 ---------------------------- 退出" << endl<< endl;
    print_center('~', "~");
    cout << endl;
}
