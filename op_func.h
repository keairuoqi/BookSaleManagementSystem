#pragma once
#pragma warning(disable:4996)

#include <time.h>
#include <iomanip>

MYSQL mysql;
MYSQL_RES* result;

// 建立数据库中四个表
void create_table() {
    // 创建 book 表，属性有 bno、bname、bamount、bsaled、bprice_in、bprice_out、bsupplier ，主码为 bno
    mysql_query(&mysql, "create table books ( bno varchar(16) NOT NULL, bname varchar(64) NOT NULL, bamount int NOT NULL,bsaled int NOT NULL,bprice_in float,bprice_out float NOT NULL,bsupplier varchar(64), primary key(bno) ) engine=innodb default charset=utf8;");
    // 创建 purchase 表，属性有 pno、bno、pnum、bsupplier、bprice_in，主码为 pno，bno 是参照 books 表的外码
    mysql_query(&mysql, "create table purchase ( pno varchar(16) NOT NULL, bno varchar(16) NOT NULL, pnum int NOT NULL,bsupplier varchar(64), bprice_in float, primary key(pno), foreign key(bno) references books(bno)) engine=innodb default charset=utf8;");
    // 创建 sale 表，属性有 sno、bno、snum，主码为 sno，bno 是参照 books 表的外码
    mysql_query(&mysql, "create table sale ( sno varchar(16) NOT NULL, bno varchar(16) NOT NULL, snum int NOT NULL, primary key(sno), foreign key(bno) references books(bno) ) engine=innodb default charset=utf8;");
    // 创建 refund 表，属性有 rno、bno、rnum，主码为 rno，bno 是参照 books 表的外码
    mysql_query(&mysql, "create table refund ( rno varchar(16) NOT NULL, bno varchar(16) NOT NULL, rnum int NOT NULL, primary key(rno), foreign key(bno) references books(bno) )  engine=innodb default charset=utf8;");
}

// 创建三个触发器
void create_trigger() {
    // 进货完成时的触发器
    mysql_query(&mysql, "create trigger purchase_update after insert on purchase for each row begin update books set bamount=bamount+new.pnum where bno=new.bno; update books set bsupplier=new.bsupplier where bno=new.bno; update books set bprice_in=new.bprice_in where bno=new.bno; end");
    // 销售图书完成时的触发器
    mysql_query(&mysql, "create trigger sale_update after insert on sale for each row begin update books set bamount=bamount-new.snum where bno=new.bno; update books set bsaled=bsaled+new.snum where bno=new.bno; end");
    // 退货完成时的触发器
    mysql_query(&mysql, "create trigger refund_update after insert on refund for each row begin update books set bamount=bamount+new.rnum where bno=new.bno; update books set bsaled=bsaled-new.rnum where bno=new.bno; update sale set snum=snum-new.rnum where bno=new.bno; end");
}

// 向 books 表中插入记录
bool insert_book(string bno, string bname, int bamount, int bsaled, float bprice_in, float bprice_out, string bsupplier) {
    char querys[256];
    sprintf_s(querys, "insert into books(bno,bname,bamount,bsaled,bprice_in, bprice_out,bsupplier) value (\"%s\",\"%s\",%d,%d,%f,%f,\"%s\");", bno.c_str(), bname.c_str(), bamount, bsaled, bprice_in, bprice_out, bsupplier.c_str());
    if (mysql_query(&mysql, querys) == 0) {
        return true;
    }
    print_error(mysql_error(&mysql));
    return false;
}

// 向 purchase 表中插入记录
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

// 向 sale 表中插入记录
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

// 向 refund 表中插入记录
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


// 返回时间戳
string get_time_stamp() {
    time_t tt = time(NULL);
    struct tm* stm = localtime(&tt);
    char str[32];
    sprintf_s(str, "%04d%02d%02d%02d%02d%02d", 1900 + stm->tm_year, 1 + stm->tm_mon, stm->tm_mday, stm->tm_hour, stm->tm_min, stm->tm_sec);
    return str;
}

// 显示当前书籍库存
void show_book_table() {
    mysql_query(&mysql, "select * from books order by bno asc;");
    MYSQL_RES* res = mysql_store_result(&mysql);
    if (mysql_num_rows(res) == 0) {
        print_guide("无记录。");
    }
    else {
        print_table(res);
    }
}

// 显示进货清单
void show_purchase_list() {
    mysql_query(&mysql, "select p.pno,b.bno,b.bname,p.pnum,b.bsupplier,b.bprice_in from purchase p join books b on b.bno=p.bno order by pno desc;");
    MYSQL_RES* res = mysql_store_result(&mysql);
    if (mysql_num_rows(res) == 0) {
        print_guide("无记录");
    }
    else {
        print_table(res);
    }
}

// 进货
void purchase() {
    string pno, bno, bname, bsupplier;
    int pnum, bamount, bsaled;
    float bprice_in, bprice_out;
    // 以时间戳作为进货的编号
    pno = get_time_stamp();
    bno = get_info_string("请输入图书编号", "图书编号");
    pnum = get_info_int("请输入进货的数目", "进货数目");
    bsupplier = get_info_string("请输入图书供应商名称", "供应商  ");
    bprice_in = get_info_float("请输入图书的进价", "图书进价");
    // 判断是否已有相同的图书信息记录
    string if_exist = "select * from books where bno=\"";
    if_exist += bno; if_exist += "\"";
    mysql_query(&mysql, if_exist.c_str());
    MYSQL_RES* res = mysql_store_result(&mysql);
    if (mysql_num_rows(res) == 0) {
        //如果没有图书相关信息，进行设置
        print_guide("书店中没有这部书的相关信息，请完善以下信息：");
        bname = get_info_string("请输入此书的名称", "图书名称");
        bprice_out = get_info_float("请输入图书售价", "图书售价");
        bamount = bsaled = 0;

        if (insert_book(bno, bname, bamount, bsaled, bprice_in, bprice_out, bsupplier)) {
            print_guide("成功插入本书。");
        }
        else {
            print_guide("插入书籍失败。");
        }
    }

    if (insert_purchase(pno, bno, pnum, bsupplier, bprice_in)) {
        char str[80];
        sprintf_s(str, "成功进货，以 %.2f 的单价，进货 %d 本 %s，来自供应商 %s.\n" , bprice_in, pnum, bname.c_str(), bsupplier.c_str());
        print_guide(str);
    }
    else {
        print_guide("进货失败。");
    }
    mysql_free_result(res);
}

// 销售图书
void sale() {
    string sno, bno;
    int snum;
    sno = get_time_stamp();
    bno = get_info_string("请输入图书编号", "图书编号");
    snum = get_info_int("请输入销售的数目", "销售数目");
    // 判断库存中是否存在这本书
    string if_exist = "select * from books where bno=\"";
    if_exist += bno; if_exist += "\"";
    mysql_query(&mysql, if_exist.c_str());
    MYSQL_RES* res = mysql_store_result(&mysql);
    if (mysql_num_rows(res) == 0) {
        // 库存中没有
        print_error("书店中没有这本书。");
    }
    else {
        // 判断库存量是否满足需求
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
            // 如果库存量够
            if (insert_sale(sno, bno, snum)) {
                char str[80];
                sprintf_s(str, "成功出售。出售 %d 本编号 %s 的图书。", snum, bno.c_str());
                print_guide(str);
            }
            else {
                print_guide("销售失败。");
            }
        }
        else {
            // 如果库存量不够
            print_error("本书的库存数目不足。");
        }
    }
    mysql_free_result(res);
}

// 退货
void refund() {
    string rno, bno;
    int rnum;
    rno = get_time_stamp();
    bno = get_info_string("请输入退书的编号", "图书编号");
    rnum = get_info_int("请输入退书的数目", "退货数目");

    // 判断是否有这本书的记录
    string if_exist = "select * from books where bno=\"";
    if_exist += bno; if_exist += "\"";
    mysql_query(&mysql, if_exist.c_str());
    MYSQL_RES* res = mysql_store_result(&mysql);
    unsigned long long res_num = mysql_num_rows(res);

    // 判断这本书是否是本店售出的
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
        // 如果书店里没有过这本书的相关记录
        print_error("书店中没有出售过这本书。");
    }
    else if (snum < rnum) {
        // 如果书店里没有卖出过这么多书
        print_error("书店中没有售卖过这么多图书。");
    }
    else {
        // 满足退货要求
        if (insert_refund(rno, bno, rnum)) {
            char str[80];
            sprintf_s(str, "成功退货。退货 %d 本编号 %s 的图书。", rnum, bno.c_str());
            print_guide(str);
        }
        else {
            print_guide("退货失败。");
        }
    }
    mysql_free_result(res);
}


// 按时间顺序显示销售清单
void show_sale_list_by_time() {
    mysql_query(&mysql, "select s.sno,s.snum,b.bno,b.bname,b.bamount,b.bsaled,b.bprice_out from books b join sale s on s.bno=b.bno order by sno desc;");
    MYSQL_RES* res = mysql_store_result(&mysql);
    if (mysql_num_rows(res) == 0) {
        print_guide("无记录。");
    }
    else {
        print_table(res);
    }
}

// 显示销量排行榜单
void show_sale_list_by_amount() {
    mysql_query(&mysql, "select b.bno,b.bname,b.bamount,b.bsaled,b.bprice_out from books b order by bsaled desc;");
    MYSQL_RES* res = mysql_store_result(&mysql);
    if (mysql_num_rows(res) == 0) {
        print_guide("无记录。");
    }
    else {
        print_table(res);
    }
}