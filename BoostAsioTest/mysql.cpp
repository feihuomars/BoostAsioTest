
//初始化数据库
#include "mysql.h"



mysqlDatabase::mysqlDatabase()
{
	mysql_real_connect1();
}
int mysqlDatabase::mysql_library_init1()
{
	if (0 == mysql_library_init(0, NULL, NULL))
	{
	}
	else
	{
		cout << "数据库初始化失败" << endl;
		return -1;
	}
}

//初始化数据结构
int mysqlDatabase::mysql_init1()
{
	if (NULL != mysql_init(&mydata)) {
		cout << "mysql_init() succeed" << endl;
	}
	else {
		cout << "mysql_init() failed" << endl;
		return -1;
	}
}

//设置额外的连接选项，这里设置字符集，否则无法处理中文
int mysqlDatabase::mysql_options1()
{
	if (0 == mysql_options(&mydata, MYSQL_SET_CHARSET_NAME, "gbk")) {
		cout << "mysql_options() succeed" << endl;
	}
	else {
		cout << "mysql_options() failed" << endl;
		return -1;
	}
}

//连接数据库
int mysqlDatabase::mysql_real_connect1()
{
	mysql_library_init1();
	mysql_init1();
	mysql_options1();
	if (NULL != mysql_real_connect(&mydata, "localhost", "root", "123456", "video_center", 3306, NULL, 0))
		//这里的地址，用户名，密码，端口可以根据自己本地的情况更改
	{
		cout << "mysql_real_connect() succeed" << endl;
	}
	else {
		cout << "mysql_real_connect() failed" << endl;
		return -1;
	}
}

//将数据插入数据库中的纠错信息表
bool mysqlDatabase::err_insert(int logid, string date, string position, string imgpath)
{
	char c[8];
	sprintf_s(c, "%d", logid);
	string Logid(c);
	//sqlstr = "select date_format(date, '%Y-%c-%d %h:%i:%s') from erroritem where id = 3";
	sqlstr = "insert into erroritem(logid,date,pos,img)values('" + Logid + "'," + "str_to_date('"+ date+"', '%Y-%m-%d %h:%i:%s')" + ",'" + position + "','" + imgpath + "');";
	//sqlstr = "insert into erroritem(logid,date,pos,img)values('"  + Logid + "'," + "str_to_date('08.09.2008 08:09:30', '%m.%d.%Y %h:%i:%s')" + ",'" + position + "','" + imgpath + "');";
	//sqlstr = "insert into erroritem(id,logid,date,pos,img)values(3,3,'2017-12-12','电子科技大学','c:\\file');";
	//cout << sqlstr.c_str()<<endl;
	if (0 == mysql_query(&mydata, sqlstr.c_str())) {
//		result = mysql_store_result(&mydata);
//	row = mysql_fetch_row(result);
//		cout << row[0]<< endl;
		cout << "mysql_query() insert data succeed" << endl;
		return true;
	}
	else {
		cout << "mysql_query() insert data failed" << endl;
		mysql_close(&mydata);
		return false;
	}
}

//将数据插入数据库中的纠错信息表
bool mysqlDatabase::log_insert(string imgpath) {
	sqlstr = "insert into logitem(date,img)values(now(),'"+imgpath+"' );";
	if (0 == mysql_query(&mydata, sqlstr.c_str())) {
		cout << "mysql_query() insert data succeed" << endl;
		return true;
	}
	else {
		cout << sqlstr << endl;
		//cout << "mysql_query() insert data failed" << endl;                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                      l;
		mysql_close(&mydata);
		return false;
	}
}

//将检索指令插入到query表中
bool mysqlDatabase::query_insert(string pic_id,string pic_path, string begin_time, string end_time) {
	
	sqlstr = "insert into query(pic_id, pic_path, begin_time, end_time) values('" + pic_id + "', '" + pic_path + "', str_to_date('" + begin_time + "','%Y-%m-%d*%H:%i:%s'), str_to_date('" + end_time + "','%Y-%m-%d*%H:%i:%s'));";
	if (0 == mysql_query(&mydata, sqlstr.c_str())) {
		cout << "mysql_query() insert data succeed" << endl;
		return true;
	}
	else {
		cout << sqlstr << endl;
		cout << "mysql_query() insert data failed" << endl;
		mysql_close(&mydata);
		return false;
	}
}

/*
查找结果表中最后一条记录
*/
bool mysqlDatabase::result_retrieve()
{
	sqlstr = "select * from result order by result_id desc LIMIT 1;";

	if (0 == mysql_query(&mydata, sqlstr.c_str()))
	{
		//一次性取得数据集
		result = mysql_store_result(&mydata);
		while (row = mysql_fetch_row(result))
		{
			//将第二到第四列放入vector
			result_results.push_back(row[1]);	//图片id
			result_results.push_back(row[2]);	//图片路径
			result_results.push_back(row[3]);	//人脸出现时间
			result_results.push_back(row[4]);	//人脸消失时间
		}
		mysql_free_result(result);
		return true;
	}
	else
	{
		cout << sqlstr<<"查询失败" << endl;
		return false;
	}
}
 
/*
查找地址表中的地址名
*/
bool mysqlDatabase::position_retrieve()
{
	sqlstr = "select * from position order by position_id desc LIMIT 1;";

	if (0 == mysql_query(&mydata, sqlstr.c_str()))
	{
		//一次性取得数据集
		result = mysql_store_result(&mydata);
		while (row = mysql_fetch_row(result))
		{
			//地址结果转为string
			position_result = row[1];
			
		}
		mysql_free_result(result);
		return true;
	}
	else
	{
		cout << sqlstr << "查询失败" << endl;
		return false;
	}
}


bool mysqlDatabase::logItemdeleteById(int id) {
	char c[8];
	sprintf_s(c, "%d", id);
	string ID(c);
	sqlstr = "delete from logitem where id = '" + ID + "';";
	string sqlstr2 = "delete from pathitem where logid = '" + ID + "';";
	string sqlstr3="delete from erroritem where logid = '" + ID + "';";
		if (0 == mysql_query(&mydata, sqlstr.c_str()) && 0 == mysql_query(&mydata, sqlstr2.c_str())) {
			mysql_query(&mydata, sqlstr3.c_str());
			return true;
	}
		return false;
}

bool mysqlDatabase::updateLogItemStatus(int id) {
	char c[8];
	sprintf_s(c, "%d", id);
	string ID(c);
	sqlstr = "UPDATE logitem SET status = 1 WHERE id ='" + ID + "';";
	if (0 == mysql_query(&mydata, sqlstr.c_str())) {
		return true;
	}
	cout << sqlstr;
	return false;
}



//删除数据表中的全部数据
void mysqlDatabase::deleteAllData()
{
	sqlstr = "TRUNCATE TABLE table_hash;";
	if (0 == mysql_query(&mydata, sqlstr.c_str()))
	{
		//cout << "aaaaaaa" << endl;
	}
	else
	{
		cout << "数据删除失败，请重新再试~！~" << endl;
	}
}
