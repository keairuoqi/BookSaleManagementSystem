#include<mysql.h>
#include<stdio.h>
#include<stdlib.h>
#include<winsock.h>
#include<Windows.h>
#include "userInterface.h"
#include "op_func.h"


#pragma warning(disable:4996)

extern MYSQL mysql;
extern MYSQL_RES* result;
char username[16] = "root";
char password[16] = "root";
char database[16] = "booksale";



int main(int argc, char** argv) {
	print_welcome();

	mysql_init(&mysql);
	mysql_options(&mysql, MYSQL_SET_CHARSET_NAME, "gbk");
	if (NULL == mysql_real_connect(&mysql, "localhost", username, password, database, 3306, 0, 0)) {
		print_error(mysql_error(&mysql));
		return -1;
	}

	create_table();
	create_trigger();

	while (1) {
		print_menu();
		int cmd = get_cmd();
		switch (cmd) {
		case 1:
			show_book_table();
			break;
		case 2:
			show_purchase_list();
			break;
		case 3:
			purchase();
			break;
		case 4:
			sale();
			break;
		case 5:
			refund();
			break;
		case 6:
			show_sale_list_by_time();
			break;
		case 7:
			show_sale_list_by_amount();
			break;
		case 8:
			print_bye();
			return 0;
			break;
		default:
			print_error("请输入菜单中的序号！");
			break;
		}
	}
	mysql_close(&mysql);//关闭mysql
	result = mysql_store_result(&mysql);
	mysql_free_result(result);
	system("pause");
	return 0;
}
