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
	
	bool info_group_exist(string);
	bool info_user_in_group(string, string);
	void info_group_add_user(string, string);
	struct bufferevent* info_get_friend_dev(string);
	string info_get_group_member(string);
	void info_add_new_group(string, string);
		
	~ChatInfo();
private:
	list<User> *online_user;   //在线用户
	list<Group> *group_info;   //群信息
	ChatDataBase *mydatabase;  //数据库对象
};


#endif
