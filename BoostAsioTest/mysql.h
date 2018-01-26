//#pragma once
#pragma execution_character_set("utf-8")
#pragma comment(lib, "libmysql.lib")

#include<vector>
#include<string>
#include <mysql.h>
#include <iostream>

using namespace std;
class mysqlDatabase {
public:
	//构造函数
	mysqlDatabase();
	//将hash值和图片帧数ID插入数据库
	bool err_insert( int logid, string date, string position, string imgpath);
	bool log_insert(string imgpath);
	bool query_insert(string pic_id, string pic_path, string begin_time, string end_time);
	//根据日期来查询记录
	bool logItemQueryByDate(string date);
	//删除数据表中的全部数据
	void deleteAllData();
	//按照图片来查询数据
	bool result_retrieve();
	//查找地址
	bool position_retrieve();
	//按照id来删除记录
	bool logItemdeleteById(int id);
	//当纠错出现时更改logitem表的status字段
	bool updateLogItemStatus(int id);

	//将结果表查询结果存放于一个vector中
	vector<string> result_results;
	//地址表查询结果存放在string中
	string position_result;

private:
	//数据库结构
	MYSQL mydata;
	//SQL操作的字符串
	string sqlstr;
	//SQL操作的结果集
	MYSQL_RES *result;
	//SQL操作的结果集的行数
	MYSQL_ROW row;
	
	//初始化数据库
	int mysql_library_init1();
	//初始化数据结构
	int mysql_init1();
	//设置额外的连接选项，这里设置字符集，否则无法处理中文
	int mysql_options1();
	//连接数据库
	int mysql_real_connect1();
};