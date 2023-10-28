#ifndef __CHAT_DATABASE_H__
#define __CHAT_DATABASE_H__

#include <mysql/mysql.h>
#include <iostream>
#include <string>
#include <cstring>
#include <stdio.h>

using namespace std;

class ChatDataBase
{
public:
	ChatDataBase();

	//连接数据库
	void my_database_connect(const char *databasename);
	//获取群
	int my_database_get_group_name(string *);
	//群成员
	void my_database_get_group_member(string ,string &);
	//判断用户是否存在
	bool my_database_user_exist(string);
	//创建用户表
	void my_database_user_password(string, string);
	//关闭数据库
	void my_database_disconnect();
	//判断密码是否正确
	bool my_database_password_correct(string, string);
	//获取好友和群
	void my_database_get_friend_group(string, string&, string&);
	//判断是不是好友
	bool my_database_is_friend(string, string);
	//添加好友
	void my_database_add_new_friend(string, string);
	//判断群是否存在
	bool my_database_group_exist(string);
	//创建群
	void my_database_add_new_group(string, string);
	//用户表添加新群
	void my_database_user_add_group(string, string);
	//群表添加新用户
	void my_database_group_add_user(string, string);

	~ChatDataBase();
private:
	MYSQL *mysql;
};

#endif
