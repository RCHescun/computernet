# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'FtpFile_.ui'
#
# Created by: PyQt5 UI code generator 5.13.0
#
# WARNING! All changes made in this file will be lost!

import os
import sys
from PyQt5 import QtCore, QtGui, QtWidgets
from PyQt5.Qt import *
from ftplib import FTP
import logging
import threading
from random import randint

class FTPmethod(object):
    def __init__(self):
        self.slm = QStringListModel()
        self.ftp = FTP()
        self.select_file = ""
        self.file_list = []
    def gettable(self):  # 获取目录结构
        dir_del = []

        table_dir = {}
        ftp.dir('.', dir_del.append)  # 根据文件目录，进行拆分。
        for name in dir_del:
            strname = str(name)
            temp = strname.split(' ')
            attrname = []
            for j in range(0,len(temp)):
                if temp[j] != '':
                    attrname.append(temp[j])
            m = ' '.join(attrname[8:])
            if strname.startswith('d'):
                #print('文件夹:', m)
                table_dir[m] = '文件夹'
            else:
                #print('文件:', m)
                table_dir[m] = '文件'
        return table_dir  # 字典，key值是文件名，value是类型
    def ftpconnect(self,ip, port,  username, passwd):
        
        #ftp.set_debuglevel(2)  # 打开调试级别2，显示详细信息
        ftp.connect(ip, port)  # 连接的ftp服务器ip和port
        ftp.login(username, passwd)  # 连接的用户名和密码
        print(ftp.getwelcome())  # 打印出欢迎信息。
        return ftp

    def showdownloadfile(self):
        #print("当前目录下的文件:文件夹名字："+ftp.pwd())
        mu_dir = gettable(ftp)
        #print("当前文件夹： "+ftp.pwd()+"  可供下载的文件：")
        behind_name = []
        for name in mu_dir:
            if mu_dir[name] == "文件":
                behind_name.append(name)
                #print(name)
        return behind_name

    def showfolderfile(self):
        #print("当前目录下的文件:文件夹名字："+ftp.pwd())
        mu_dir = gettable(ftp)
        #print("当前文件夹：  "+ftp.pwd()+"   可供索引的文件夹：")
        folder_name = []
        for name in mu_dir:
            if mu_dir[name] == "文件夹":
                folder_name.append(name)
                #print(name)
        return folder_name

    def randomdownload(self, localpath, str):
        # 当前目录下可供下载的文件名字
        behind_name = showdownloadfile(ftp)
        #print("当前文件夹： "+ftp.pwd()+"  可供下载的文件：")
        #print(behind_name)
        # 随机数
        while len(behind_name) == 0:  # 说明没有可以下载的文件
            # 切换文件夹
            changetable(ftp,str)
            behind_name = showdownloadfile(ftp)
        file_random = randint(0, len(behind_name)-1)
        filename = behind_name[file_random]
        print("当前目录为：  "+ftp.pwd())
        print("下载文件为 "+filename)
        # 设置下载数据
        bufsize = 1024  # 设置缓冲区大小
        new_path = localpath+filename  # 设置下载路径

        fp = open(new_path, 'wb')
        ftp.retrbinary('RETR ' + filename, fp.write, bufsize)  # 下载FTP文件。FTP命令 RETR filename
        fp.close()
        print(filename+' 下载成功')


    def downloadfiles(self, localpath, str):  # 根目录不能继续向下，没有目录不能索引（这个可以自动实现）。
        # 在当前目录下随机下载几个文件
        behind_name = showdownloadfile(ftp)
        #folder_name = showfolderfile(ftp)
        while len(behind_name) == 0:  # 说明没有可以下载的文件
            # 切换文件夹
            changetable(ftp, str)
            behind_name = showdownloadfile(ftp)

        randomb = randint(0, len(behind_name)-1)
        while randomb:  # 在这个文件夹下随机下载几个
            randomdownload(ftp, localpath, str)
            randomb -= 1
        # 切换目录下载（可能向下或者向上）
        #viewtable(ftp)

    def gettable(self):  # 获取目录结构
        dir_del = []

        table_dir = {}
        ftp.dir('.', dir_del.append)  # 根据文件目录，进行拆分。
        for name in dir_del:
            strname = str(name)
            temp = strname.split(' ')
            attrname = []
            for j in range(0,len(temp)):
                if temp[j] != '':
                    attrname.append(temp[j])
            m = ' '.join(attrname[8:])
            if strname.startswith('d'):
                #print('文件夹:', m)
                table_dir[m] = '文件夹'
            else:
                #print('文件:', m)
                table_dir[m] = '文件'
        return table_dir  # 字典，key值是文件名，value是类型

    def viewtable(self,str):
        #gettable(ftp)  # 获取当前目录
        changtimes = randint(1, 4)  # 随机切换几次目录
        while changtimes:
            changtimes -= 1
            folder_name = showfolderfile(ftp)
            print("当前目录下为：  "+ftp.pwd())
            print("目录结构：")
            print(gettable(ftp))
            if len(folder_name) > 0:  # 有可供索引的文件夹
                if ftp.pwd() == str:  # 为根目录，只能向下索引
                    print("向下索引后目录为：")
                    randomf = randint(0, len(folder_name)-1)
                    path = ftp.pwd()+folder_name[randomf]
                    ftp.cwd(path)
                    print("当前目录下为：  "+ftp.pwd())
                    print("目录结构：")
                    print(gettable(ftp))

                else:  # 向上向下索引都可以
                    downorup = randint(0, 1)
                    if downorup:  # 向下索引
                        print("向下索引后目录为：")
                        randomf = randint(0, len(folder_name)-1)
                        path = ftp.pwd()+folder_name[randomf]
                        ftp.cwd(path)
                        print("当前目录下为：  "+ftp.pwd())
                        print("目录结构：")
                        print(gettable(ftp))

                    else:
                        print("向上索引后，目录为：")
                        path = '../'
                        ftp.cwd(path)
                        print("当前目录下为：  "+ftp.pwd())
                        print("目录结构：")
                        print(gettable(ftp))
            else:  # 没有可供索引的文件夹，可以向上索引。
                if ftp.pwd() != str:
                    print("向上索引后，目录为：")
                    path = '../'
                    ftp.cwd(path)
                    print("当前目录下为：  "+ftp.pwd())
                    print("目录结构：")
                    print(gettable(ftp))

    def changetable(self,str):
        folder_name = showfolderfile(ftp)
        if len(folder_name) > 0:  # 有可供索引的文件夹
            if ftp.pwd() == str:  # 为根目录，只能向下索引
                randomf = randint(0, len(folder_name)-1)
                path = ftp.pwd()+folder_name[randomf]
                ftp.cwd(path)

            else:  # 向上向下索引都可以
                downorup = randint(0, 1)
                if downorup:  # 向下索引
                    randomf = randint(0, len(folder_name)-1)
                    path = ftp.pwd()+folder_name[randomf]
                    ftp.cwd(path)

                else:
                    path = '../'
                    ftp.cwd(path)
        else:  # 没有可供索引的文件夹，可以向上索引。
            if ftp.pwd() != str:
                path = '../'
                ftp.cwd(path)
    def ftpmain(self,ip, port, username, passwd, localpath, userModel):
        # 1.连接到ftp服务器
        ftp = ftpconnect(ip, port,  username, passwd)
        # 2.开始交互
        str = ftp.pwd()
        #print(str)
        # usermodel 也可以是一个列表序列，让一个用户，执行多个操作。
        cmdtimes = randint(1, 5)  # 这随机多次执行用户模式。
        #downloadtimes = 3  # 多个文件多次下载
        while cmdtimes:
            cmdtimes -= 1
            if userModel == 1:  # 浏览模式
                viewtable(ftp, str)
                changetable(ftp, str)
                continue
            if userModel == 2:  # 下载模式
                randomdownload(ftp, localpath, str)
                changetable(ftp, str)
                continue
            if userModel == 3:  # 同时下载多个文件模式
                #while downloadtimes:
                downloadfiles(ftp, localpath, str)
                changetable(ftp, str)
                #downloadtimes -= 1
                continue
            else:  # 错误处理机制
                print('无法识别传递的模式编号：')
                break

        ftp.quit()


class Ui_FtpFile(object):
    def __init__(self):
        self.slm = QStringListModel()
        self.ftp = FTP()
        self.select_file = ""
        self.file_list = []
        self.ftpmethod=FTPmethod()

    def setupUi(self, FtpFile):
        FtpFile.setObjectName("FtpFile")
        FtpFile.resize(364, 402)
        self.layoutWidget = QtWidgets.QWidget(FtpFile)
        self.layoutWidget.setGeometry(QtCore.QRect(10, 20, 341, 361))
        self.layoutWidget.setObjectName("layoutWidget")
        self.verticalLayout_2 = QtWidgets.QVBoxLayout(self.layoutWidget)
        self.verticalLayout_2.setContentsMargins(0, 0, 0, 0)
        self.verticalLayout_2.setObjectName("verticalLayout_2")
        self.horizontalLayout = QtWidgets.QHBoxLayout()
        self.horizontalLayout.setObjectName("horizontalLayout")
        self.label = QtWidgets.QLabel(self.layoutWidget)
        self.label.setObjectName("label")
        self.horizontalLayout.addWidget(self.label)
        self.lineEdit = QtWidgets.QLineEdit(self.layoutWidget)
        self.lineEdit.setObjectName("lineEdit")
        self.horizontalLayout.addWidget(self.lineEdit)
        self.label_5 = QtWidgets.QLabel(self.layoutWidget)
        self.label_5.setObjectName("label_5")
        self.horizontalLayout.addWidget(self.label_5)
        self.lineEdit_4 = QtWidgets.QLineEdit(self.layoutWidget)
        self.lineEdit_4.setObjectName("lineEdit_4")
        self.horizontalLayout.addWidget(self.lineEdit_4)
        self.verticalLayout_2.addLayout(self.horizontalLayout)
        self.horizontalLayout_2 = QtWidgets.QHBoxLayout()
        self.horizontalLayout_2.setObjectName("horizontalLayout_2")
        self.label_2 = QtWidgets.QLabel(self.layoutWidget)
        self.label_2.setObjectName("label_2")
        self.horizontalLayout_2.addWidget(self.label_2)
        self.lineEdit_2 = QtWidgets.QLineEdit(self.layoutWidget)
        self.lineEdit_2.setObjectName("lineEdit_2")
        self.horizontalLayout_2.addWidget(self.lineEdit_2)
        self.label_3 = QtWidgets.QLabel(self.layoutWidget)
        self.label_3.setObjectName("label_3")
        self.horizontalLayout_2.addWidget(self.label_3)
        self.lineEdit_3 = QtWidgets.QLineEdit(self.layoutWidget)
        self.lineEdit_3.setObjectName("lineEdit_3")
        self.horizontalLayout_2.addWidget(self.lineEdit_3)
        self.verticalLayout_2.addLayout(self.horizontalLayout_2)
        self.label_4 = QtWidgets.QLabel(self.layoutWidget)
        self.label_4.setObjectName("label_4")
        self.verticalLayout_2.addWidget(self.label_4)
        self.horizontalLayout_3 = QtWidgets.QHBoxLayout()
        self.horizontalLayout_3.setObjectName("horizontalLayout_3")
        self.listView = QtWidgets.QListView(self.layoutWidget)
        self.listView.setObjectName("listView")
        self.horizontalLayout_3.addWidget(self.listView)
        self.verticalLayout = QtWidgets.QVBoxLayout()
        self.verticalLayout.setObjectName("verticalLayout")
        self.pushButton = QtWidgets.QPushButton(self.layoutWidget)
        self.pushButton.setObjectName("pushButton")
        self.verticalLayout.addWidget(self.pushButton)
        self.pushButton_1 = QtWidgets.QPushButton(self.layoutWidget)
        self.pushButton_1.setObjectName("pushButton")
        self.verticalLayout.addWidget(self.pushButton_1)
        self.pushButton_2 = QtWidgets.QPushButton(self.layoutWidget)
        self.pushButton_2.setObjectName("pushButton_2")
        self.verticalLayout.addWidget(self.pushButton_2)
        self.pushButton_3 = QtWidgets.QPushButton(self.layoutWidget)
        self.pushButton_3.setEnabled(False)
        self.pushButton_3.setObjectName("pushButton_3")
        self.verticalLayout.addWidget(self.pushButton_3)
        self.pushButton_4 = QtWidgets.QPushButton(self.layoutWidget)
        self.pushButton_4.setObjectName("pushButton_4")
        self.verticalLayout.addWidget(self.pushButton_4)
        self.horizontalLayout_3.addLayout(self.verticalLayout)
        self.verticalLayout_2.addLayout(self.horizontalLayout_3)

        self.retranslateUi(FtpFile)
        QtCore.QMetaObject.connectSlotsByName(FtpFile)

    def retranslateUi(self, FtpFile):
        _translate = QtCore.QCoreApplication.translate
        FtpFile.setWindowTitle(_translate("FtpFile", "FtpFile"))
        self.label.setText(_translate("FtpFile", "域名："))
        self.label_5.setText(_translate("FtpFile", "端口号："))
        self.label_2.setText(_translate("FtpFile", "登录名："))
        self.label_3.setText(_translate("FtpFile", "口令："))
        self.label_4.setText(_translate("FtpFile", "文件目录列表"))
        self.pushButton.setText(_translate("FtpFile", "查询"))
        self.pushButton_1.setText(_translate("FtpFile", "上一层"))
        self.pushButton_2.setText(_translate("FtpFile", "上传"))
        self.pushButton_3.setText(_translate("FtpFile", "下载"))
        self.pushButton_4.setText(_translate("FtpFile", "退出"))
        self.lineEdit.setText("127.0.0.1")
        self.lineEdit_4.setText("21")
        self.lineEdit_2.setText("user")
        self.lineEdit_3.setText("123456")
        self.pushButton.clicked.connect(self.connect_button)
        self.pushButton_1.clicked.connect(self.back_button)
        self.pushButton_2.clicked.connect(self.upload_button)
        self.pushButton_3.clicked.connect(self.download_button)

        self.listView.setModel(self.slm)
        self.listView.clicked.connect(self.select_item)
        self.listView.doubleClicked.connect(self.cd_button)

    def connect_button(self):
        host = self.lineEdit.text()
        port = int(self.lineEdit_4.text())
        usr = self.lineEdit_2.text()
        pwd = self.lineEdit_3.text()
        try:
            self.ftp.connect(host,port,timeout=10)
        except:
            logging.warning('network connect time out')
            return 1001
        try:
            self.ftp.login(usr,pwd)
        except:
            logging.warning("username or password error")
            return 1002

        self.file_list = self.ftp.nlst() # 当前目录的所有文件列表
        self.slm.setStringList(self.file_list)
        # print(self.file_list)
        # file_list2 = self.ftp.sendcmd("pwd")    # 当前目录位置
        # print(file_list2)

        return 1000

    def select_item(self, qModelIndex):
        self.select_file = self.file_list[qModelIndex.row()]
        # print(self.select_file)
        if "." in self.select_file:     # 如果是文件，则可下载
            self.pushButton_3.setEnabled(True)
        else:                           # 否则是文件夹，不能下载
            self.pushButton_3.setEnabled(False)

    def cd_button(self):
        self.ftp.cwd(self.select_file)
        self.file_list = self.ftp.nlst()  # 刷新当前目录的所有文件列表
        self.slm.setStringList(self.file_list)

    def back_button(self):
        self.ftp.cwd("..")
        self.file_list = self.ftp.nlst()  # 刷新当前目录的所有文件列表
        self.slm.setStringList(self.file_list)

    def upload_button(self):
        t = threading.Thread(target=self.t,args=())
        t.start()

    def t(self):
        file_path, filer_ = QFileDialog.getOpenFileName()
        print(file_path)
        file_name = os.path.split(file_path)[1]
        print(file_name)
        self.ftp.storbinary('stor '+file_name, open(file_path, 'rb'))

    def download_button(self):
        self.ftp.retrbinary('retr '+self.select_file, open(self.select_file, 'wb').write)
        print("download succeed")

if __name__ == '__main__':
    app = QApplication(sys.argv)
    MainWindow = QMainWindow()
    ui = Ui_FtpFile()
    ui.setupUi(MainWindow)
    MainWindow.show()
    sys.exit(app.exec_())