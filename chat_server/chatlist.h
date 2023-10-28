#ifndef __CHATLIST_H__
#define __CHATLIST_H__

#include <event.h>
#include <list>
#include "chat_database.h"

using namespace std;

#define MAXNUM 1024  //最大群数量

//在线用户链表成员
struct User
{
	string user_name;
	struct bufferevent *bev;
};


//群链表成员
struct GroupUser
{
	string groupuser_name;
};
struct Group
{
	string group_name;
	list<GroupUser> *li;
};

typedef struct User User;
typedef struct Group Group;
typedef struct GroupUser GroupUser;

class Server;

class ChatInfo
{
	friend class Server;
public:
	ChatInfo();
	
	//判断群是否存在
	bool info_group_exist(string);
	//判断用户是否在群内
	bool info_user_in_group(string, string);
	//更新群链表
	void info_group_add_user(string, string);
	//获取聊天对象的bufferevent
	struct bufferevent* info_get_friend_dev(string);
	//获取群成员
	string info_get_group_member(string);
	//添加新群
	void info_add_new_group(string, string);
		
	~ChatInfo();
private:
	list<User> *online_user;   //在线用户
	list<Group> *group_info;   //群信息
	ChatDataBase *mydatabase;  //数据库对象
};


#endif
