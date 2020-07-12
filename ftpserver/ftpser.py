#!/usr/bin/python3
#coding:utf-8
import os
from pyftpdlib.authorizers import DummyAuthorizer
from pyftpdlib.handlers import FTPHandler
from pyftpdlib.servers import FTPServer

def main():
    #实例化用户授权管理
    authorizer = DummyAuthorizer()
    authorizer.add_user('user','123456',"/home/rch/Desktop/computernet/ftpserver/serfile",perm='elradfmwMT') #添加用户 参数：username,password,允许的路径、权限
    #authorizer.add_anonymous(os.getcwd()) #这里是允许匿名用户

    #实例化FTPHandle
    handler = FTPHandler
    handler.authorizer = authorizer

    #设定一个客户端链接时的标语
    handler.banner = 'pyftplib based ftpd ready.'

    # handler.masquerade_address = '192.168.88.2'
    # address = ('192.168.88.2', 21) #FTP 一般使用21、20端口
    server = FTPServer(('127.0.0.1', 2777), handler)  #FTP服务器实例
    #设定一个限制连接
    server.max_cons = 256
    server.max_cons_per_ip = 5

    #开启服务器
    server.serve_forever()
    print("服务开启")
if __name__ == '__main__':
    main()