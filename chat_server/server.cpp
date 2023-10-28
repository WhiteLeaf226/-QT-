#include "server.h"

ChatDataBase *Server::chatdb = new ChatDataBase();
ChatInfo *Server::chatlist = new ChatInfo();

Server::Server(const char *ip, int port)
{
	//创建一个事件集合
	base = event_base_new();  

	//协议族地址结构体
	struct sockaddr_in server_addr;
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;  //接收IPV4地址
	server_addr.sin_port = htons(port);  //PORT
	server_addr.sin_addr.s_addr = inet_addr(IP);  //IP

	//创建监听对象
	listener = evconnlistener_new_bind(base, listener_cb, NULL,
		LEV_OPT_CLOSE_ON_FREE | LEV_OPT_REUSEABLE, 10,
		(struct sockaddr *)&server_addr, sizeof(server_addr));

	if(listener == NULL)
	{
		cout<<"evconnlistener_new_bind error"<<endl;
	}

	cout<<"服务器初始化成功 开始监听客户端"<<endl;
	//启动集合监听
	event_base_dispatch(base);
}

void Server::listener_cb(struct evconnlistener *listener, evutil_socket_t fd,
	struct sockaddr *addr, int socklen, void *arg)
{
	cout<<"接收到了客户端连接，fd = "<<fd<<endl;

	//创工作线程俩处理客户端
	thread client_thread(client_handler, fd);
	
	//线程分离
	client_thread.detach();
}

void Server::client_handler(int fd)
{
	//创建事件集合
	struct event_base *base =  event_base_new();
	
	//创建bufferevent对象
	struct bufferevent *bev = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE);
	if(bev == NULL)
	{
		cout<<"bufferevent_socket_new error"<<endl;
	}
	
	//给bufferevent设置读回调函数和异常回调函数
	bufferevent_setcb(bev, read_cb, NULL, event_cb, NULL);
	//启用读回调函数(在读操作发生时会使用读回调函数)
	bufferevent_enable(bev, EV_READ);

	event_base_dispatch(base);
	
	event_base_free(base);

	cout<<"线程退出，释放集合"<<endl;
}

void Server::read_cb(struct bufferevent *bev, void *ctx)
{
	//存读取的数据
	char buf[1024];
	int size = bufferevent_read(bev, buf, sizeof(buf));
	if(size < 0)
	{
		cout<<"bufferevent_read error"<<endl;
	}

	cout<<buf<<endl;
	
	//解析json对象
	Json::Reader reader;
	//封装json对象
	Json::FastWriter writer;
	//创建json对象
	Json::Value val;

	//把char* 解析成json对象
	if(!reader.parse(buf, val))
	{
		cout<<"服务器解析数据失败"<<endl;
	}

	string cmd = val["cmd"].asString();

	if(cmd == "register")  //注册功能
	{
		server_register(bev, val);
	}
	else if(cmd == "login") //登入
	{
		server_login(bev, val);
	}
	else if(cmd == "add")  //添加好友
	{
		server_add_friend(bev, val);
	}
	else if(cmd == "create_group") //创建群
	{
		server_create_group(bev, val);
	}else if(cmd == "add_group")  //添加群
	{
		server_add_group(bev, val);
	}else if(cmd == "private_chat")  //私聊
	{
		server_private_chat(bev, val);
	}else if(cmd == "group_chat") //群聊
	{
		server_group_chat(bev, val);
	}
	else if(cmd == "get_group_member") //获取群成员
	{
		server_get_group_member(bev, val);
	}
	else if(cmd == "offline")  //用户下线
	{
		server_user_offline(bev, val);
	}
	else if(cmd == "send_file")  //文件传输
	{
		server_send_file(bev, val);
	}
	
}

void Server::event_cb(struct bufferevent *bev, short what, void *ctx)
{	
	
}

Server::~Server()
{
	event_base_free(base);	
}

void Server::server_register(struct bufferevent *bev, Json::Value val)
{
	chatdb->my_database_connect("chat_user");
	
	if(chatdb->my_database_user_exist(val["user"].asString()))
	{
		//用户存在
		Json::Value val;
		val["cmd"] = "register_reply";
		val["result"] = "failure";
		
		string s = Json::FastWriter().write(val);
		
		int size = bufferevent_write(bev, s.c_str(), strlen(s.c_str()));
		if(size < 0)
		{
			cout<<"bufferevent_write error"<<endl;
		}
		
	}
	else
	{
		//用户不存在
		
		//向用户数据库写入用户名和密码
		chatdb->my_database_user_password(val["user"].asString(), val["password"].asString());
		Json::Value val;
		val["cmd"] = "register_reply";
		val["result"] = "success";
		
		string s = Json::FastWriter().write(val);
		
		int size = bufferevent_write(bev, s.c_str(), strlen(s.c_str()));
		if(size < 0)
		{
			cout<<"bufferevent_write error"<<endl;
		}
	}

	//关闭数据库
	chatdb->my_database_disconnect();
}

void Server::server_login(struct bufferevent *bev, Json::Value val)
{
	chatdb->my_database_connect("chat_user");

	//用户名不存在
	if(!chatdb->my_database_user_exist(val["user"].asString()))
	{
		Json::Value val;
		val["cmd"] = "login_replay";
		val["result"] = "user_not_exist";

		string s = Json::FastWriter().write(val);

		int size = bufferevent_write(bev, s.c_str(), strlen(s.c_str()));
		if(size < 0)
		{
			cout<<"bufferevent_write error"<<endl;
		}
		return;
	}
	
	//密码错误
	if(!chatdb->my_database_password_correct(val["user"].asString(), val["password"].asString()))
	{		
		Json::Value val;
		val["cmd"] = "login_replay";
		val["result"] = "password_error";

		string s = Json::FastWriter().write(val);

		int size = bufferevent_write(bev, s.c_str(), strlen(s.c_str()));
		if(size < 0)
		{
			cout<<"bufferevent_write error"<<endl;
		}
		return;
	}

	//用户名密码正确
	
	//向链表中添加用户
	User u = {val["user"].asString(), bev};
	chatlist->online_user->push_back(u);

	//获取好友列表并返回
	string friend_list, group_list;
	chatdb->my_database_get_friend_group(val["user"].asString(), friend_list, group_list);
	//Json::Value val1;
	//val1["cmd"] = "login_replay";
	//val1["result"] = "success";
	//val1["friend"] = friend_list.c_str();
	//val1["group"] = group_list.c_str();

	//string s = Json::FastWriter().write(val1);
	string s1 = "{\"cmd\":\"login_replay\",\"friend\":\"";
	string s2 = friend_list + "\",\"group\":\"";
	string s3 = group_list + "\",\"result\":\"success\"}";
	string s = s1 + s2 + s3;
	
	if(bufferevent_write(bev, s.c_str(), strlen(s.c_str())) < 0)
	{
		cout<<"bufferevent_write error"<<endl;
	}

	//向所有好友发送好友上线提醒
	int start = 0, end = 0, flag = 1;
	string name;
	while(flag)
    {
		end = friend_list.find('|', start);
		if(end == -1)
		{
			name = friend_list.substr(start);
			flag = 0;
		}
		else
		{
			name  = friend_list.substr(start, end - start);
		}
        for(list<User>::iterator it = chatlist->online_user->begin();
				it != chatlist->online_user->end(); ++it)
		{
			if(name == it->user_name)
			{
				Json::Value val2;
				val2["cmd"] = "friend_login";
				val2["friend"] = val["user"];

				string s = Json::FastWriter().write(val2);

				int size = bufferevent_write(it->bev, s.c_str(), strlen(s.c_str()));
				if(size < 0)
				{
					cout<<"bufferevent_write error"<<endl;
				}
				
			}
		}

		start = end + 1;
    }
	
	//关闭数据库
	chatdb->my_database_disconnect();
}

void Server::server_add_friend(struct bufferevent *bev, Json::Value val)
{
	chatdb->my_database_connect("chat_user");
	
	//添加好友不存在
	if(!chatdb->my_database_user_exist(val["friend"].asString()))
	{	
		Json::Value val;
		val["cmd"] = "add_reply";
		val["result"] = "user_not_exist";

		string s = Json::FastWriter().write(val);

		int size = bufferevent_write(bev, s.c_str(), strlen(s.c_str()));
		if(size < 0)
		{
			cout<<"bufferevent_write error"<<endl;
		}
		return;
	}

	//已经是好友
	if(chatdb->my_database_is_friend(val["user"].asString(), 
			val["friend"].asString()))
	{
		Json::Value val;
		val["cmd"] = "add_reply";
		val["result"] = "already_friend";
		string s = Json::FastWriter().write(val);
		int size = bufferevent_write(bev, s.c_str(), 
				strlen(s.c_str()));
		if(size < 0)
		{
			cout<<"bufferevent_write error"<<endl;
		}

		return;
	}

	//不是好友
	//修改双方的数据库
	chatdb->my_database_add_new_friend(val["user"].asString(),
			val["friend"].asString());
	chatdb->my_database_add_new_friend(val["friend"].asString(),
			val["user"].asString());
	
	//回复客户端添加成功
	Json::Value val1;
	val1["cmd"] = "add_reply";
	val1["result"] = "success";
	val1["friend"] = val["friend"];
	string s = Json::FastWriter().write(val1);
	int size = bufferevent_write(bev, s.c_str(), 
			strlen(s.c_str()));
	if(size < 0)
	{
		cout<<"bufferevent_write error"<<endl;
	}

	//回复对方，多了一个好友
	for(list<User>::iterator it = chatlist->online_user->begin();
			it != chatlist->online_user->end(); ++it)
	{
		if(val["friend"]  == it->user_name)
		{	
			Json::Value val2;
			val2["cmd"] = "add_friend_reply";
			val2["result"] = val["user"];
			string s = Json::FastWriter().write(val2);
			int size = bufferevent_write(it->bev, s.c_str(), 
					strlen(s.c_str()));
			if(size < 0)
			{	
				cout<<"bufferevent_write error"<<endl;
			}
		}

	}				

	//关闭数据库
    chatdb->my_database_disconnect();
}

void Server::server_create_group(struct bufferevent *bev, Json::Value val)
{
	//连接群信息数据库
	chatdb->my_database_connect("chat_group");

	//判断群是否存在
	if(chatdb->my_database_group_exist(val["group"].asString()))
	{
			
		Json::Value val1;
		val1["cmd"] = "create_group_reply";
		val1["result"] = "group_exist";
		string s = Json::FastWriter().write(val1);
		int size = bufferevent_write(bev, s.c_str(), 
				strlen(s.c_str()));
		if(size < 0)
		{	
			cout<<"bufferevent_write error"<<endl;
		}
		return;
	}

	//群不存在，把群信息写入数据库
	chatdb->my_database_add_new_group(val["group"].asString(),
			val["user"].asString());
	chatdb->my_database_disconnect();
	//修改用户表群信息
	chatdb->my_database_connect("chat_user");
	chatdb->my_database_user_add_group(val["user"].asString(),
			val["group"].asString());
	//修改群链表
	chatlist->info_add_new_group(val["group"].asString(), 
			val["user"].asString());

	Json::Value val2;
	val2["cmd"] = "create_group_reply";
	val2["result"] = "success";
	val2["group"] = val["group"];
	string s = Json::FastWriter().write(val2);
	int size = bufferevent_write(bev, s.c_str(), 
			strlen(s.c_str()));
	if(size < 0)
	{	
		cout<<"bufferevent_write error"<<endl;
	}

	//关闭数据库
	chatdb->my_database_disconnect();
}

void Server::server_add_group(struct bufferevent *bev, Json::Value val)
{
	//判断群是否存在
   	if(!chatlist->info_group_exist(val["group"].asString()))
	{	
		Json::Value val1;
		val1["cmd"] = "add_group_reply";
		val1["result"] = "group_not_exist";
		string s = Json::FastWriter().write(val1);
		int size = bufferevent_write(bev, s.c_str(), 
				strlen(s.c_str()));
		if(size < 0)
		{	
			cout<<"bufferevent_write error"<<endl;
		}
		return;
	}
	//判断用户是否在群里
	if(chatlist->info_user_in_group(val["user"].asString(),
			val["group"].asString()))
	{
		Json::Value val1;
		val1["cmd"] = "add_group_reply";
		val1["result"] = "user_in_group";
		string s = Json::FastWriter().write(val1);
		int size = bufferevent_write(bev, s.c_str(), 
				strlen(s.c_str()));
		if(size < 0)
		{	
			cout<<"bufferevent_write error"<<endl;
		}
		return;
	}

	//修改数据库(用户表，群表)
	//修改用户表
	chatdb->my_database_connect("chat_user");
	chatdb->my_database_user_add_group(val["user"].asString(),
			val["group"].asString());
	//关闭数据库
	chatdb->my_database_disconnect();

	//修改群表
	chatdb->my_database_connect("chat_group");
	chatdb->my_database_group_add_user(val["group"].asString(),
			val["user"].asString());
	//关闭数据库
	chatdb->my_database_disconnect();

	//修改链表
	chatlist->info_group_add_user(val["group"].asString(),
			val["user"].asString());	

	//回复客户端
	Json::Value val2;
	val2["cmd"] = "add_group_reply";
	val2["result"] = "success";
	val2["group"] = val["group"];
	string s = Json::FastWriter().write(val2);
	int size = bufferevent_write(bev, s.c_str(), 
			strlen(s.c_str()));
	if(size < 0)
	{	
		cout<<"bufferevent_write error"<<endl;
	}
}

void Server::server_private_chat(struct bufferevent *bev, Json::Value val)
{
	struct bufferevent *to_bev = chatlist->info_get_friend_dev(
			val["user_to"].asString());
	
	//如果对方不在线
	if(to_bev == NULL)
	{
		Json::Value val1;
		val1["cmd"] = "private_chat_reply";
		val1["result"] = "offline";
		string s = Json::FastWriter().write(val1);
		int size = bufferevent_write(bev, s.c_str(), 
				strlen(s.c_str()));
		if(size < 0)
		{	
			cout<<"bufferevent_write error"<<endl;
		}
		return;
	}

	//对方在线
	
	//将收到的数据转发过去
	string s = Json::FastWriter().write(val);
	int size = bufferevent_write(to_bev, s.c_str(),
		strlen(s.c_str()));
	if(size < 0)
	{
		cout<<"bufferevent_write error"<<endl;
    }

	//回复客户端发送成功
	Json::Value val2;
	val2["cmd"] = "private_chat_reply";
	val2["result"] = "success";
	s = Json::FastWriter().write(val2);
	size = bufferevent_write(bev, s.c_str(), 
			strlen(s.c_str()));
	if(size < 0)
	{	
		cout<<"bufferevent_write error"<<endl;
	}
}

void Server::server_group_chat(struct bufferevent *bev, Json::Value val)
{
	for(list<Group>::iterator it1 = chatlist->group_info->begin();
			it1 != chatlist->group_info->end(); ++it1)
	{
		if(val["group"].asString() == it1->group_name)
		{
			for(list<GroupUser>::iterator it2 = it1->li->begin();
					it2 != it1->li->end(); ++it2)
			{
				struct bufferevent *to_bev = 
					chatlist->info_get_friend_dev(it2->groupuser_name);
				if(to_bev != NULL)
				{
					string s = Json::FastWriter().write(val);
					int size = bufferevent_write(to_bev, s.c_str(),
						strlen(s.c_str()));
					if(size < 0)
					{
						cout<<"bufferevent_write error"<<endl;
					}
				}
			}
		}
	}
	
	//回复客户端发送成功
	Json::Value val1;
	val1["cmd"] = "group_chat_reply";
	val1["result"] = "success";
	string s = Json::FastWriter().write(val1);
	int size = bufferevent_write(bev, s.c_str(), 
			strlen(s.c_str()));
	if(size < 0)
	{	
		cout<<"bufferevent_write error"<<endl;
	}
}

 void Server::server_get_group_member(struct bufferevent *bev,Json::Value val)
{
	string member = chatlist->info_get_group_member(val["group"].asString());
	
	//回复客户端群成员
	//Json::Value val1;
	//val1["cmd"] = "get_group_member_reply";
	//val1["result"] = member;
	//string s = Json::FastWriter().write(val1);
	string s1 = "{\"cmd\":\"get_group_member_reply\",\"member\":\"";
	string s2 = member + "\",\"group\":\"";
	string s3 = val["group"].asString() + "\"}";
	string s = s1 + s2 + s3;
	int size = bufferevent_write(bev, s.c_str(), 
			strlen(s.c_str()));
	if(size < 0)
	{	
		cout<<"bufferevent_write error"<<endl;
	}
}

void Server::server_user_offline(struct bufferevent *bev, Json::Value val)
{	
	//向链表中删除用户
	for(list<User>::iterator it = chatlist->online_user->begin();
			it != chatlist->online_user->end(); ++it)
	{
		if(it->user_name == val["user"].asString())
		{
			chatlist->online_user->erase(it);
			break;
		}
	}

	//获取好友列表并返回
	chatdb->my_database_connect("chat_user");

	string friend_list, group_list;
	string name, s;
	chatdb->my_database_get_friend_group(val["user"].asString(), friend_list, group_list);

	//向所有好友发送好友下线提醒
	int start = 0, end = 0, flag = 1;
    while(flag)
    {
		end = friend_list.find('|', start);
		if(end == -1)
		{
			name = friend_list.substr(start);
			flag = 0;
		}
		else
		{
			name  = friend_list.substr(start, end - start);
		}
        for(list<User>::iterator it = chatlist->online_user->begin();
				it != chatlist->online_user->end(); ++it)
		{
			if(name == it->user_name)
			{
				Json::Value val1;
				val1["cmd"] = "friend_offline";
				val1["friend"] = val["user"];

				string s = Json::FastWriter().write(val1);

				int size = bufferevent_write(it->bev, s.c_str(),
						strlen(s.c_str()));
				if(size < 0)
				{
					cout<<"bufferevent_write error"<<endl;
				}
				
			}
		}

		start = end + 1;
    }

	//关闭数据库
	chatdb->my_database_disconnect();
}

void Server::send_flie_handler(int length, int port, int *f_fd, int *t_fd)
{
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd == -1)
	{
		cout<<"socket fd error"<<endl;
		return;
	}

	int opt = 1;
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

	//接收缓冲区
	int nRecvBuf = MAXSIZE;
	setsockopt(sockfd, SOL_SOCKET, SO_RCVBUF,
			(const char*)&nRecvBuf, sizeof(int));
	//发送缓冲区
	int nSendBuf = MAXSIZE;
	setsockopt(sockfd, SOL_SOCKET, SO_SNDBUF,
			(const char*)&nSendBuf, sizeof(int));

	struct sockaddr_in server_addr, client_addr;
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;  //接收IPV4地址
	server_addr.sin_port = htons(port);  //PORT
	server_addr.sin_addr.s_addr = inet_addr(IP);  //IP
	bind(sockfd, (struct sockaddr *)&server_addr, 
			sizeof(server_addr));
	listen(sockfd, 10);
	//接收发送客户端的连接请求
	int len = sizeof(client_addr);
	*f_fd = accept(sockfd, (struct sockaddr *)&client_addr,
			(socklen_t *)&len);
	int from_fd = *f_fd;
	//接受接收客户端的连接请求
	*t_fd = accept(sockfd, (struct sockaddr *)&client_addr,
			(socklen_t *)&len);
	int to_fd = *t_fd;
	char buf[MAXSIZE];
	size_t size, sum = 0;
	while(1)
	{
		size = recv(from_fd, buf, MAXSIZE, 0);
		if(size <= 0 || size > MAXSIZE)
			break; 
		sum += size;
		send(to_fd, buf, size, 0);
		if(sum >= length)
		{
			cout<<"发送完毕"<<endl;
			break;
		}
		memset(buf, 0, MAXSIZE);
	}

	close(from_fd);
	close(to_fd);
	close(sockfd);
}

void Server::server_send_file(struct bufferevent *bev, Json::Value val)
{
	struct bufferevent *to_bev = chatlist->info_get_friend_dev(
			val["to_user"].asString());
	
	//客户端不存在
	if(to_bev == NULL)
	{
		Json::Value val1;
		val1["cmd"] = "send_file_reply";
		val1["result"] = "offline";
		string s = Json::FastWriter().write(val1);
		int size = bufferevent_write(bev, s.c_str(), 
				strlen(s.c_str()));
		if(size < 0)
		{	
			cout<<"bufferevent_write error"<<endl;
		}
		return;
	}

	//启动新线程，创建文件服务器
	int port = 10020;
	int from_fd = 0, to_fd = 0;
	thread send_file_thread(send_flie_handler, 
			val["length"].asInt(), port, &from_fd, &to_fd);
	//线程分离
	send_file_thread.detach();
	

	//返回端口号给发送客户端
	Json::Value val2;
	val2["cmd"] = "send_file_port_reply";
	val2["port"] = port;
	val2["filename"] = val["filename"];
	val2["length"] = val["length"];
	string s = Json::FastWriter().write(val2);
	//int size = bufferevent_write(bev, s.c_str(), 
	//		strlen(s.c_str()));
	int size = send(bev->ev_read.ev_fd, s.c_str(),
			strlen(s.c_str()), 0);
	if(size < 0)
	{	
		cout<<"bufferevent_write error"<<endl;
	}
	
	//连接服务器超时
	int count = 0;
	while(from_fd <= 0)
	{
		count++;
		usleep(100000);
		//10s
		if(count == 100)
		{
			//取消线程
			pthread_cancel(send_file_thread.native_handle());
			
			Json::Value val1;
			val1["cmd"] = "send_file_reply";
			val1["result"] = "timeout";
			string s = Json::FastWriter().write(val1);
			int size = bufferevent_write(bev, s.c_str(), 
					strlen(s.c_str()));
			if(size < 0)
			{	
				cout<<"bufferevent_write error"<<endl;
			}
			return;
		}
	}
	//返回端口号给接收客户端
	Json::Value val3;
	val3["cmd"] = "recv_file_port_reply";
	val3["port"] = port;
	val3["filename"] = val["filename"];
	val3["length"] = val["length"];
	s = Json::FastWriter().write(val3);
	//size = bufferevent_write(to_bev, s.c_str(), 
	//		strlen(s.c_str()));
	size = send(to_bev->ev_read.ev_fd, s.c_str(),
			strlen(s.c_str()), 0);
	if(size < 0)
	{	
		cout<<"bufferevent_write error"<<endl;
	}
	count = 0;
	while(to_fd <= 0)
	{
		count++;
		usleep(100000);
		//10s
		if(count == 100)
		{
			//取消线程
			pthread_cancel(send_file_thread.native_handle());
			
			Json::Value val1;
			val1["cmd"] = "send_file_reply";
			val1["result"] = "timeout";
			string s = Json::FastWriter().write(val1);
			int size = bufferevent_write(bev, s.c_str(), 
					strlen(s.c_str()));
			if(size < 0)
			{	
				cout<<"bufferevent_write error"<<endl;
			}
			return;
		}
	}
}	






