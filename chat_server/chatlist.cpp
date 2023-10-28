#include "chatlist.h"

ChatInfo::ChatInfo()
{
	online_user = new list<User>;

	group_info = new list<Group>;

	//往group_info链表中添加群信息	
	mydatabase = new ChatDataBase();
	mydatabase->my_database_connect("chat_group");

	string group_name[MAXNUM];
	int group_num = mydatabase->my_database_get_group_name(group_name);

	for(int i = 0; i < group_num; i++)
	{
		Group g;
		//群名字
		g.group_name = group_name[i];
		//群成员
		g.li = new list<GroupUser>;
		
		string member;
		mydatabase->my_database_get_group_member(group_name[i], member);
		if(member.size() == 0)
		{
			continue;
		}

		int start = 0, end = 0; 
		GroupUser u;
		while((end = member.find('|', start)) != string::npos)
		{
			u.groupuser_name = member.substr(start, end - start);
			g.li->push_back(u);
			start = end + 1;
			u.groupuser_name.clear();
		}
		u.groupuser_name = member.substr(start);
		g.li->push_back(u);
		group_info->push_back(g);
	}

	mydatabase->my_database_disconnect();

	cout<<"初始化链表成功"<<endl;
}

bool ChatInfo::info_group_exist(string group_name)
{
	for(list<Group>::iterator it = group_info->begin();
			it != group_info->end(); ++it)
	{
		if(it->group_name == group_name)
		{
			return true;
		}
	}
	return false;
}

bool ChatInfo::info_user_in_group(string user_name, string group_name)
{
	
	for(list<Group>::iterator it1 = group_info->begin();
			it1 != group_info->end(); ++it1)
	{
		if(it1->group_name == group_name)
		{
			for(list<GroupUser>::iterator it2 = it1->li->begin();
					it2 != it1->li->end(); ++it2)
			{
				if(it2->groupuser_name == user_name)
				{
					return true;
				}
			}
		}
	}
	return false;
}

void ChatInfo::info_group_add_user(string group_name, string user_name)
{
	for(list<Group>::iterator it = group_info->begin();
			it != group_info->end(); ++it)
	{
		if(it->group_name == group_name)
		{
			GroupUser u;
			u.groupuser_name = user_name;
			it->li->push_back(u);
			break;
		}
	}
	
}

struct bufferevent* ChatInfo::info_get_friend_dev(string name)
{
	for(list<User>::iterator it = online_user->begin();
			it != online_user->end(); ++it)
	{
		if(it->user_name == name)
		{
			return it->bev;
		}
	}
	return NULL;
}

string ChatInfo::info_get_group_member(string group_name) 
{
	string member;
	for(list<Group>::iterator it1 = group_info->begin();
			it1 != group_info->end(); ++it1)
	{
		if(it1->group_name == group_name)
		{
			for(list<GroupUser>::iterator it2 = it1->li->begin();
					it2 != it1->li->end(); ++it2)
			{
				member += it2->groupuser_name + "|";
			}
		}
	}
	return member;
}

void ChatInfo::info_add_new_group(string group_name, string user_name)
{
	Group g;
	g.group_name = group_name;
	g.li = new list<GroupUser>;
	group_info->push_back(g);

	GroupUser u;
	u.groupuser_name = user_name;
	g.li->push_back(u);
}


