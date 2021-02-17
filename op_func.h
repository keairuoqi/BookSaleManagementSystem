#pragma once
#pragma warning(disable:4996)

#include <time.h>
#include <iomanip>

MYSQL mysql;
MYSQL_RES* result;

// �������ݿ����ĸ���
void create_table() {
    // ���� book �������� bno��bname��bamount��bsaled��bprice_in��bprice_out��bsupplier ������Ϊ bno
    mysql_query(&mysql, "create table books ( bno varchar(16) NOT NULL, bname varchar(64) NOT NULL, bamount int NOT NULL,bsaled int NOT NULL,bprice_in float,bprice_out float NOT NULL,bsupplier varchar(64), primary key(bno) ) engine=innodb default charset=utf8;");
    // ���� purchase �������� pno��bno��pnum��bsupplier��bprice_in������Ϊ pno��bno �ǲ��� books �������
    mysql_query(&mysql, "create table purchase ( pno varchar(16) NOT NULL, bno varchar(16) NOT NULL, pnum int NOT NULL,bsupplier varchar(64), bprice_in float, primary key(pno), foreign key(bno) references books(bno)) engine=innodb default charset=utf8;");
    // ���� sale �������� sno��bno��snum������Ϊ sno��bno �ǲ��� books �������
    mysql_query(&mysql, "create table sale ( sno varchar(16) NOT NULL, bno varchar(16) NOT NULL, snum int NOT NULL, primary key(sno), foreign key(bno) references books(bno) ) engine=innodb default charset=utf8;");
    // ���� refund �������� rno��bno��rnum������Ϊ rno��bno �ǲ��� books �������
    mysql_query(&mysql, "create table refund ( rno varchar(16) NOT NULL, bno varchar(16) NOT NULL, rnum int NOT NULL, primary key(rno), foreign key(bno) references books(bno) )  engine=innodb default charset=utf8;");
}

// ��������������
void create_trigger() {
    // �������ʱ�Ĵ�����
    mysql_query(&mysql, "create trigger purchase_update after insert on purchase for each row begin update books set bamount=bamount+new.pnum where bno=new.bno; update books set bsupplier=new.bsupplier where bno=new.bno; update books set bprice_in=new.bprice_in where bno=new.bno; end");
    // ����ͼ�����ʱ�Ĵ�����
    mysql_query(&mysql, "create trigger sale_update after insert on sale for each row begin update books set bamount=bamount-new.snum where bno=new.bno; update books set bsaled=bsaled+new.snum where bno=new.bno; end");
    // �˻����ʱ�Ĵ�����
    mysql_query(&mysql, "create trigger refund_update after insert on refund for each row begin update books set bamount=bamount+new.rnum where bno=new.bno; update books set bsaled=bsaled-new.rnum where bno=new.bno; update sale set snum=snum-new.rnum where bno=new.bno; end");
}

// �� books ���в����¼
bool insert_book(string bno, string bname, int bamount, int bsaled, float bprice_in, float bprice_out, string bsupplier) {
    char querys[256];
    sprintf_s(querys, "insert into books(bno,bname,bamount,bsaled,bprice_in, bprice_out,bsupplier) value (\"%s\",\"%s\",%d,%d,%f,%f,\"%s\");", bno.c_str(), bname.c_str(), bamount, bsaled, bprice_in, bprice_out, bsupplier.c_str());
    if (mysql_query(&mysql, querys) == 0) {
        return true;
    }
    print_error(mysql_error(&mysql));
    return false;
}

// �� purchase ���в����¼
bool insert_purchase(string pno, string bno, int pnum, string bsupplier, float bprice_in) {
    char querys[256]; 
    sprintf_s(querys, "insert into purchase(pno,bno,pnum,bsupplier,bprice_in) value (\"%s\",\"%s\",%d,\"%s\",%f);", pno.c_str(), bno.c_str(), pnum, bsupplier.c_str(), bprice_in);
    if (mysql_query(&mysql, querys) == 0) {
        return true;
    }
    else {
        print_error(mysql_error(&mysql));
        return false;
    }
}

// �� sale ���в����¼
bool insert_sale(string sno, string bno, int snum) {
    char querys[256];
    sprintf_s(querys, "insert into sale(sno,bno,snum) value (\"%s\",\"%s\",%d);", sno.c_str(), bno.c_str(), snum);
    if (mysql_query(&mysql, querys) == 0) {
        return true;
    }
    else {
        print_error(mysql_error(&mysql));
        return false;
    }
}

// �� refund ���в����¼
bool insert_refund(string rno, string bno, int rnum) {
    char querys[256];
    sprintf_s(querys, "insert into refund(rno,bno,rnum) value (\"%s\",\"%s\",%d);", rno.c_str(), bno.c_str(), rnum);
    if (mysql_query(&mysql, querys) == 0) {
        return true;
    }
    else {
        print_error(mysql_error(&mysql));
        return false;
    }
}


// ����ʱ���
string get_time_stamp() {
    time_t tt = time(NULL);
    struct tm* stm = localtime(&tt);
    char str[32];
    sprintf_s(str, "%04d%02d%02d%02d%02d%02d", 1900 + stm->tm_year, 1 + stm->tm_mon, stm->tm_mday, stm->tm_hour, stm->tm_min, stm->tm_sec);
    return str;
}

// ��ʾ��ǰ�鼮���
void show_book_table() {
    mysql_query(&mysql, "select * from books order by bno asc;");
    MYSQL_RES* res = mysql_store_result(&mysql);
    if (mysql_num_rows(res) == 0) {
        print_guide("�޼�¼��");
    }
    else {
        print_table(res);
    }
}

// ��ʾ�����嵥
void show_purchase_list() {
    mysql_query(&mysql, "select p.pno,b.bno,b.bname,p.pnum,b.bsupplier,b.bprice_in from purchase p join books b on b.bno=p.bno order by pno desc;");
    MYSQL_RES* res = mysql_store_result(&mysql);
    if (mysql_num_rows(res) == 0) {
        print_guide("�޼�¼");
    }
    else {
        print_table(res);
    }
}

// ����
void purchase() {
    string pno, bno, bname, bsupplier;
    int pnum, bamount, bsaled;
    float bprice_in, bprice_out;
    // ��ʱ�����Ϊ�����ı��
    pno = get_time_stamp();
    bno = get_info_string("������ͼ����", "ͼ����");
    pnum = get_info_int("�������������Ŀ", "������Ŀ");
    bsupplier = get_info_string("������ͼ�鹩Ӧ������", "��Ӧ��  ");
    bprice_in = get_info_float("������ͼ��Ľ���", "ͼ�����");
    // �ж��Ƿ�������ͬ��ͼ����Ϣ��¼
    string if_exist = "select * from books where bno=\"";
    if_exist += bno; if_exist += "\"";
    mysql_query(&mysql, if_exist.c_str());
    MYSQL_RES* res = mysql_store_result(&mysql);
    if (mysql_num_rows(res) == 0) {
        //���û��ͼ�������Ϣ����������
        print_guide("�����û���ⲿ��������Ϣ��������������Ϣ��");
        bname = get_info_string("��������������", "ͼ������");
        bprice_out = get_info_float("������ͼ���ۼ�", "ͼ���ۼ�");
        bamount = bsaled = 0;

        if (insert_book(bno, bname, bamount, bsaled, bprice_in, bprice_out, bsupplier)) {
            print_guide("�ɹ����뱾�顣");
        }
        else {
            print_guide("�����鼮ʧ�ܡ�");
        }
    }

    if (insert_purchase(pno, bno, pnum, bsupplier, bprice_in)) {
        char str[80];
        sprintf_s(str, "�ɹ��������� %.2f �ĵ��ۣ����� %d �� %s�����Թ�Ӧ�� %s.\n" , bprice_in, pnum, bname.c_str(), bsupplier.c_str());
        print_guide(str);
    }
    else {
        print_guide("����ʧ�ܡ�");
    }
    mysql_free_result(res);
}

// ����ͼ��
void sale() {
    string sno, bno;
    int snum;
    sno = get_time_stamp();
    bno = get_info_string("������ͼ����", "ͼ����");
    snum = get_info_int("���������۵���Ŀ", "������Ŀ");
    // �жϿ�����Ƿ�����Ȿ��
    string if_exist = "select * from books where bno=\"";
    if_exist += bno; if_exist += "\"";
    mysql_query(&mysql, if_exist.c_str());
    MYSQL_RES* res = mysql_store_result(&mysql);
    if (mysql_num_rows(res) == 0) {
        // �����û��
        print_error("�����û���Ȿ�顣");
    }
    else {
        // �жϿ�����Ƿ���������
        int field_num = mysql_num_fields(res);
        MYSQL_FIELD* fields = mysql_fetch_fields(res);
        MYSQL_ROW row = mysql_fetch_row(res);
        int bamount = 0;
        for (int i = 0; i < field_num; ++i) {
            if (strcmp(fields[i].name, "bamount") == 0) {
                bamount = atoi(row[i]);
                break;
            }
        }
        if (bamount >= snum) {
            // ����������
            if (insert_sale(sno, bno, snum)) {
                char str[80];
                sprintf_s(str, "�ɹ����ۡ����� %d ����� %s ��ͼ�顣", snum, bno.c_str());
                print_guide(str);
            }
            else {
                print_guide("����ʧ�ܡ�");
            }
        }
        else {
            // ������������
            print_error("����Ŀ����Ŀ���㡣");
        }
    }
    mysql_free_result(res);
}

// �˻�
void refund() {
    string rno, bno;
    int rnum;
    rno = get_time_stamp();
    bno = get_info_string("����������ı��", "ͼ����");
    rnum = get_info_int("�������������Ŀ", "�˻���Ŀ");

    // �ж��Ƿ����Ȿ��ļ�¼
    string if_exist = "select * from books where bno=\"";
    if_exist += bno; if_exist += "\"";
    mysql_query(&mysql, if_exist.c_str());
    MYSQL_RES* res = mysql_store_result(&mysql);
    unsigned long long res_num = mysql_num_rows(res);

    // �ж��Ȿ���Ƿ��Ǳ����۳���
    string if_enough = "select * from sale where bno=\"";
    if_enough += bno; if_enough += "\"";
    mysql_query(&mysql, if_enough.c_str());
    res = mysql_store_result(&mysql);
    unsigned long long res_num_sale = mysql_num_rows(res);
    int snum = 0;
    if (res_num_sale != 0) {
        int field_num = mysql_num_fields(res);
        MYSQL_FIELD* fields = mysql_fetch_fields(res);
        MYSQL_ROW row = mysql_fetch_row(res);
        for (int i = 0; i < field_num; ++i) {
            if (strcmp(fields[i].name, "snum") == 0) {
                snum = atoi(row[i]);
                break;
            }
        }
    }
    if (res_num == 0) {
        // ��������û�й��Ȿ�����ؼ�¼
        print_error("�����û�г��۹��Ȿ�顣");
    }
    else if (snum < rnum) {
        // ��������û����������ô����
        print_error("�����û����������ô��ͼ�顣");
    }
    else {
        // �����˻�Ҫ��
        if (insert_refund(rno, bno, rnum)) {
            char str[80];
            sprintf_s(str, "�ɹ��˻����˻� %d ����� %s ��ͼ�顣", rnum, bno.c_str());
            print_guide(str);
        }
        else {
            print_guide("�˻�ʧ�ܡ�");
        }
    }
    mysql_free_result(res);
}


// ��ʱ��˳����ʾ�����嵥
void show_sale_list_by_time() {
    mysql_query(&mysql, "select s.sno,s.snum,b.bno,b.bname,b.bamount,b.bsaled,b.bprice_out from books b join sale s on s.bno=b.bno order by sno desc;");
    MYSQL_RES* res = mysql_store_result(&mysql);
    if (mysql_num_rows(res) == 0) {
        print_guide("�޼�¼��");
    }
    else {
        print_table(res);
    }
}

// ��ʾ�������а�
void show_sale_list_by_amount() {
    mysql_query(&mysql, "select b.bno,b.bname,b.bamount,b.bsaled,b.bprice_out from books b order by bsaled desc;");
    MYSQL_RES* res = mysql_store_result(&mysql);
    if (mysql_num_rows(res) == 0) {
        print_guide("�޼�¼��");
    }
    else {
        print_table(res);
    }
}