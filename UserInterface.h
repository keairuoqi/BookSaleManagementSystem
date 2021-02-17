#pragma once

#include <mysql.h>
#include <string>
#include <vector>
#include <iomanip>
#include <iostream>

using namespace std;

// ���ϵͳ��ʾ
void print_guide(const char* str) {
    cout << "<ϵͳ��ʾ> " << str << endl;
}


// ������ı߿�
void print_table_line(vector<int> widths, int col) {
    for (int i = 0; i < col; ++i) {
        cout << "+-";
        for (int j = 0; j <= widths[i]; ++j) {
            cout << '-';
        }
    }
    cout << "+" << endl;
}


// ������
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

// ����ѡ��
int get_cmd() {
    string temp;
    cout << "<ѡ�����> ";
    cin >> temp;
    int cmd = temp[0] - '0';
    return cmd;
}

// ���븡������Ϣ
float get_info_float(const char* str, const char* key) {
    float price;
    print_guide(str);
    cout << "[" << key << "] ";
    cin >> price;
    return price;
}

// �����ַ�����Ϣ
string get_info_string(const char* str, const char* key) {
    string res;
    print_guide(str);
    cout << "[" << key << "] ";
    cin >> res;
    return res;
}

// ����������Ϣ
int get_info_int(const char* str, const char* key) {
    int amount;
    print_guide(str);
    cout << "[" << key << "] ";
    cin >> amount;
    return amount;
}

// ���û����������������ַ���
void print_center(char ch, string str) {
    size_t half = (60 - str.size()) / 2;
    for (int i = 0; i < half; ++i) 
        cout << ch;
    cout << str;
    for (int i = 0; i < 60 - half - str.size(); ++i) 
        cout << ch;
    cout << endl;
}

// ������󱨸�
void print_error(const char* str) {
    print_center('!', (string)"��������!");
    cout << endl;
    cout << "[���󱨸�] " << str << endl;
    print_guide("�����ԡ�");
    cout << endl;
}

void print_welcome() {
    print_center('*', " ��ӭ����ͼ�����۹���ϵͳ! ");
}

void print_bye() {
    cout << endl << endl;
    print_center('*', " �˳�����ϵͳ�� ");
    cout << endl << endl << endl;
}

void print_menu() {
    cout << endl << endl;
    print_center('-', " ����ѡ�� ");
    cout << endl;
    print_center('~', "~");
    cout << "           1 -------------------- ��ʾͼ����" << endl;
    cout << "           2 -------------------- ��ʾ�����嵥" << endl;
    cout << "           3 ---------------------------- ����" << endl;
    cout << "           4 ------------------------ ����ͼ��" << endl;
    cout << "           5 ---------------------------- �˻�" << endl;
    cout << "           6 ------ ��ʱ��˳����ʾ����ͳ�����" << endl;
    cout << "           7 ---------------- ��ʾ���������а�" << endl;
    cout << "           8 ---------------------------- �˳�" << endl<< endl;
    print_center('~', "~");
    cout << endl;
}
