#include <mysql/mysql.h>
#include <stdio.h>
#include <stdlib.h>

int main() {

    MYSQL *conn;
    MYSQL_RES *res;
    MYSQL_ROW row;

    char server[] = "10.8.4.121";//192.168.2.107
    char user[] = "root";
    char password[] = "123456";
    char database[] = "lingsheng-test";

    conn = mysql_init(NULL);
    if (mysql_real_connect(conn, server, user, password, database, 0, NULL, 0)) {
        printf("%s\n", mysql_error(conn));
    } 

    char sql[128] = {0};
    sprintf(sql, "insert into user_info2(`name`,title,money)values('king', '', 3);");
    if (mysql_query(conn, sql)) {
        printf("%s\n", mysql_error(conn));
    }


    if (mysql_query(conn, "select * from user_info2")) {
        printf("%s\n", mysql_error(conn));
    }

    res = mysql_use_result(conn);
    while ((row = mysql_fetch_row(res)) != NULL) {
        printf("%s\t", row[0]);
        printf("%s\t", row[1]);
        printf("%s\t", row[2]);
        printf("%s\n", row[3]);
    }

    mysql_free_result(res);
    mysql_close(conn);

    return 0;

}