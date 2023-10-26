#include "chat_database.h"

ChatDataBase::ChatDataBase()
{
}

ChatDataBase::~ChatDataBase()
{
	mysql_close(mysql);
}

void ChatDataBase::my_database_connect(const char *name)
{
	mysql = mysql_init(NULL);
	//连接本地数据库
	mysql = mysql_real_connect(mysql, "localhost", "root", "123456", name, 0, NULL, 0);
	if(mysql == NULL)
	{
		cout<<"connect database error"<<endl;
	};
	
	//设置编码格式
	int i = mysql_query(mysql, "set names utf8;");
	if(i != 0)
	{
		cout<<"mysql_query error"<<endl;
	}
}

int ChatDataBase::my_database_get_group_name(string *s)
{
	//显示表信息
	if(mysql_query(mysql, "show tables;") != false)
	{
		cout<<"mysql_query error"<<endl;
	}

	//返回查询结果集
	MYSQL_RES *res = mysql_store_result(mysql);
	if(res == NULL)
	{
		cout<<"mysql_store_result error"<<endl;
	}

	int count = 0;
	MYSQL_ROW row;
	//获取下一行数据(过滤属性名)
	while(row = mysql_fetch_row(res))
	{
		s[count] += row[0];
		count++;
	}

	return count;
}

void ChatDataBase::my_database_get_group_member(string name,string &s)
{	
	//string s = "select member from %s;" + name;
	char sql[1024];
	sprintf(sql, "select group_member from %s;", name.c_str());
	if(mysql_query(mysql, sql) != false)
	{
		cout<<"mysql_query error"<<endl;
	}

	MYSQL_RES *res = mysql_store_result(mysql);
    if(res == NULL)
    {
         cout<<"mysql_store_result error"<<endl;
	}

	MYSQL_ROW row = mysql_fetch_row(res);
	if(row[0] == NULL)
	{
		return;
	}
	s += row[0];
}

void ChatDataBase::my_database_disconnect()
{
	mysql_close(mysql);
}

bool ChatDataBase::my_database_user_exist(string name)
{
	char sql[128];
	sprintf(sql, "show tables like '%s';", name.c_str());
	if(mysql_query(mysql, sql) != false)
	{
		cout<<"mysql_query error"<<endl;
	}
	
	MYSQL_RES *res = mysql_store_result(mysql);
	MYSQL_ROW row = mysql_fetch_row(res);
	if(row == NULL)
	{
		return false;
	}
	else
	{
		return true;
	}
}

void ChatDataBase::my_database_user_password(string name, string password)
{
	char sql[128];
	sprintf(sql, "create table %s (password varchar(16), friend varchar(4096), chatgroup varchar(4096)) character set utf8;", name.c_str());
	if(mysql_query(mysql, sql) != false)
	{
		cout<<"mysql_query error"<<endl;
	}

	memset(sql, 0, sizeof(sql));
	sprintf(sql, "insert into %s (password) values ('%s');", name.c_str(), password.c_str());

	if(mysql_query(mysql, sql) != false)
	{
		cout<<"mysql_query error"<<endl;
	}	

}
bool ChatDataBase::my_database_password_correct(string name, string password)
{
	char sql[128];

	sprintf(sql, "select password from %s;", name.c_str());

	if(mysql_query(mysql, sql) != false)
	{
		cout<<"mysql_query error"<<endl;
	}
	
	MYSQL_RES *res = mysql_store_result(mysql);
    if(res == NULL)
    {
         cout<<"mysql_store_result error"<<endl;
	}

	MYSQL_ROW row = mysql_fetch_row(res);
	if(password == row[0])
	{
		return true;
	}
	else
	{
		return false;
	}	

}

void ChatDataBase::my_database_get_friend_group(string name, string &f, string &g)
{
	char sql[128];

	sprintf(sql, "select friend from %s;", name.c_str());
	if(mysql_query(mysql, sql) != false)
	{
		cout<<"mysql_query error"<<endl;
	}
	
	MYSQL_RES *res = mysql_store_result(mysql);
	MYSQL_ROW row = mysql_fetch_row(res);
	if(row[0] != NULL)
	{
		f.append(row[0]);
	}
	mysql_free_result(res);

	memset(sql, 0, sizeof(sql));
	sprintf(sql, "select chatgroup from %s;", name.c_str());
	if(mysql_query(mysql, sql) != false)
	{
		cout<<"mysql_query error"<<endl;
	}
	
	res = mysql_store_result(mysql);
	row = mysql_fetch_row(res);
	if(row[0] != NULL)
	{
		g.append(row[0]);
	}	
}

bool ChatDataBase::my_database_is_friend(string n1, string n2)
{
	char sql[128];
	sprintf(sql, "select friend from %s;", n1.c_str());

	if(mysql_query(mysql, sql) != false)
	{
		cout<<"mysql_query error"<<endl;
	}

	MYSQL_RES *res = mysql_store_result(mysql);
	MYSQL_ROW row = mysql_fetch_row(res);
	if(row[0] == NULL)
	{
		return	false;
	}
	else
	{
		string all_friend(row[0]);
	    int start = 0, end = 0;
		while((end = all_friend.find('|', start)) != string::npos)
        {
			if(n2 == all_friend.substr(start, end - start))
			{
				return true;
			}
			start = end + 1;
        }
        if(n2 == all_friend.substr(start))
		{
			return true;
		}
	}

	return false;
}


void ChatDataBase::my_database_add_new_friend(string n1, string n2)
{
	char sql[1024];
	sprintf(sql, "select friend from %s;", n1.c_str());
	if(mysql_query(mysql, sql) != false)
	{
		cout<<"mysql_query error"<<endl;
	}
	string friend_list;
	MYSQL_RES *res = mysql_store_result(mysql);
	MYSQL_ROW row = mysql_fetch_row(res);
	
	if(row[0] == NULL)  //原来没有好友
	{
		
		friend_list.append(n2);
	}
	else  //原来有好友
	{
		friend_list.append(row[0]);
		friend_list += "|" + n2;
	}
	memset(sql, 0, sizeof(sql));

	sprintf(sql, "update %s set friend = '%s';", n1.c_str(), friend_list.c_str());
	
	if(mysql_query(mysql, sql) != false)
	{
		cout<<"mysql_query error"<<endl;
	}

}

bool ChatDataBase::my_database_group_exist(string group_name)
{	
	char sql[128];
	sprintf(sql, "show tables like '%s';", group_name.c_str());

	if(mysql_query(mysql, sql) != false)
	{
		cout<<"mysql_query error"<<endl;
	}

	MYSQL_RES *res = mysql_store_result(mysql);
	MYSQL_ROW row = mysql_fetch_row(res);
	if(row == NULL)		//群不存在
	{
		return false;
	}
	else				//群存在
	{
		return true;
	}
}

void ChatDataBase::my_database_add_new_group(string group_name, string group_owner)
{
	char sql[128];
	
	sprintf(sql, "create table %s (group_owner varchar(32), group_member varchar(4096)) character set utf8;", group_name.c_str());
	if(mysql_query(mysql, sql) != false)
	{
		cout<<"mysql_query error"<<endl;
	}
	
	memset(sql, 0, sizeof(sql));
	sprintf(sql, "insert into %s  values ('%s', '%s');",
			group_name.c_str(), group_owner.c_str(),
			group_owner.c_str());

	if(mysql_query(mysql, sql) != false)
	{
		cout<<"mysql_query error"<<endl;
	}
}

void ChatDataBase::my_database_user_add_group(string user_name, string group_name)
{
	char sql[1024];
	sprintf(sql, "select chatgroup from %s;", user_name.c_str());
	if(mysql_query(mysql, sql) != false)
	{
		cout<<"mysql_query error"<<endl;
	}

	string all_group;
	MYSQL_RES *res = mysql_store_result(mysql);
	MYSQL_ROW row = mysql_fetch_row(res);
	if(row[0] != NULL)
	{
		all_group += row[0];
		all_group += "|" + group_name;
	}
	else
	{
		all_group += group_name;
	}

	memset(sql, 0, sizeof(sql));
	sprintf(sql, "update %s set chatgroup = '%s';",
			user_name.c_str(), all_group.c_str());
	if(mysql_query(mysql, sql) != false)
	{
		cout<<"mysql_query error"<<endl;
	}		
}

void ChatDataBase::my_database_group_add_user(string group_name, string user_name)
{	
	char sql[1024];
	sprintf(sql, "select group_member from %s;", 
			group_name.c_str());
	if(mysql_query(mysql, sql) != false)
	{
		cout<<"mysql_query error"<<endl;
	}

	string all_member;
	MYSQL_RES *res = mysql_store_result(mysql);
	MYSQL_ROW row = mysql_fetch_row(res);
	if(row[0] != NULL)
	{
		all_member += row[0];
		all_member += "|" + user_name;
	}
	else
	{
		all_member += user_name;
	}

	memset(sql, 0, sizeof(sql));
	sprintf(sql, "update %s set group_member = '%s';",
			group_name.c_str(), all_member.c_str());
	if(mysql_query(mysql, sql) != false)
	{
		cout<<"mysql_query error"<<endl;

	}
}
