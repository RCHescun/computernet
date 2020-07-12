# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'ftpser.ui'
#
# Created by: PyQt5 UI code generator 5.14.2
#
# WARNING! All changes made in this file will be lost!

from PyQt5 import QtCore, QtGui, QtWidgets
import sys
import os
from pyftpdlib.authorizers import DummyAuthorizer
from pyftpdlib.handlers import FTPHandler
from pyftpdlib.servers import FTPServer
import threading
import tkinter as tk
from tkinter import filedialog

from PyQt5.QtWidgets import QMainWindow,QApplication,QTextEdit,QAction,QFileDialog
import threading

class MyWindow(QtWidgets.QWidget):  
        def __init__(self):  
            super().__init__()  
            self.myButton = QtWidgets.QPushButton(self)  
 
            self.myButton.clicked.connect(self.msg)  
      
        def msg(self):  #使用infomation信息框  
            reply = QtWidgets.QMessageBox.information(self,  "消息通知",   "创建成功，是否继续创建",  QtWidgets.QMessageBox.Yes|QtWidgets.QMessageBox.No)  

class QMessageBoxApp(QtWidgets.QWidget):
    def __init__(self):
        super().__init__()
        self.initUI()

    def initUI(self):

        self.setGeometry(300, 300, 300, 220)
        self.setWindowTitle('消息盒子')        
        self.show()

    def closeEvent(self, event):

        reply = QtWidgets.QMessageBox.question(self, '信息', '确认退出吗？', 
            QtWidgets.QMessageBox.Yes | QtWidgets.QMessageBox.No, QtWidgets.QMessageBox.No)

        if reply == QtWidgets.QMessageBox.Yes:
            event.accept()
        else:
            event.ignore()


class Application(tk.Frame):
    def __init__(self, master=None):
        super().__init__(master)
        self.master = master
        self.filePath = tk.StringVar()
        self.pack()
        self.create_widgets()
 
    def create_widgets(self):
        # 获取文件
        self.getFile_bt = tk.Button(self)
        self.getFile_bt['width'] = 15
        self.getFile_bt['height'] = 1
        self.getFile_bt["text"] = "打开"
        self.getFile_bt["command"] = self._getFile
        self.getFile_bt.pack(side="top")
 
        # 显示文件路径
        self.filePath_en = tk.Entry(self,  width = 30)
        self.filePath_en.pack(side="top")
 
        self.filePath_en.delete(0, "end")
        self.filePath_en.insert(0, "请选择文件")
 
 
    # 打开文件并显示路径
    def _getFile(self):
        default_dir = r"文件路径"
        self.filePath = tk.filedialog.askopenfilename(title=u'选择文件', initialdir=(os.path.expanduser(default_dir)))
        print(self.filePath)
        self.filePath_en.delete(0, "end")
        self.filePath_en.insert(0, self.filePath)


class Ui_MainWindow(object):
    def __init__(self):
        self.authorizer = DummyAuthorizer()
        self.anonymauthorizer = DummyAuthorizer()
        self.select_file = ""
        self.file_list = []
        self.handler = FTPHandler
        self.anonymauthorizer.add_anonymous(os.getcwd()) #这里是允许匿名用户
        self.myshow=MyWindow() 

    def setupUi(self, MainWindow):
        MainWindow.setObjectName("FTPSERVER")
        MainWindow.resize(300, 331)
        self.centralwidget = QtWidgets.QWidget(MainWindow)
        self.centralwidget.setObjectName("centralwidget")
        self.checkBox = QtWidgets.QCheckBox(self.centralwidget)
        self.checkBox.setGeometry(QtCore.QRect(10, 230, 151, 23))
        self.checkBox.setObjectName("checkBox")
        self.pushButton = QtWidgets.QPushButton(self.centralwidget)
        self.pushButton.setGeometry(QtCore.QRect(10, 260, 211, 41))
        self.pushButton.setObjectName("pushButton")
        self.label = QtWidgets.QLabel(self.centralwidget)
        self.label.setGeometry(QtCore.QRect(10, 90, 67, 17))
        self.label.setObjectName("label")
        self.label_2 = QtWidgets.QLabel(self.centralwidget)
        self.label_2.setGeometry(QtCore.QRect(20, 10, 67, 17))
        self.label_2.setObjectName("label_2")
        self.lineEdit = QtWidgets.QLineEdit(self.centralwidget)
        self.lineEdit.setGeometry(QtCore.QRect(70, 10, 181, 25))
        self.lineEdit.setObjectName("lineEdit")
        self.label_3 = QtWidgets.QLabel(self.centralwidget)
        self.label_3.setGeometry(QtCore.QRect(10, 50, 67, 17))
        self.label_3.setObjectName("label_3")
        self.lineEdit_2 = QtWidgets.QLineEdit(self.centralwidget)
        self.lineEdit_2.setGeometry(QtCore.QRect(70, 50, 71, 25))
        self.lineEdit_2.setObjectName("lineEdit_2")
        self.lineEdit_3 = QtWidgets.QLineEdit(self.centralwidget)
        self.lineEdit_3.setGeometry(QtCore.QRect(70, 90, 111, 25))
        self.lineEdit_3.setObjectName("lineEdit_3")
        self.lineEdit_4 = QtWidgets.QLineEdit(self.centralwidget)
        self.lineEdit_4.setGeometry(QtCore.QRect(70, 130, 113, 25))
        self.lineEdit_4.setObjectName("lineEdit_4")
        self.label_4 = QtWidgets.QLabel(self.centralwidget)
        self.label_4.setGeometry(QtCore.QRect(20, 130, 51, 20))
        self.label_4.setObjectName("label_4")
        self.lineEdit_5 = QtWidgets.QLineEdit(self.centralwidget)
        self.lineEdit_5.setGeometry(QtCore.QRect(80, 170, 201, 25))
        self.lineEdit_5.setObjectName("lineEdit_5")
        self.pushButton_2 = QtWidgets.QPushButton(self.centralwidget)
        self.pushButton_2.setGeometry(QtCore.QRect(10, 170, 61, 25))
        self.pushButton_2.setObjectName("pushButton_2")
        MainWindow.setCentralWidget(self.centralwidget)
        self.statusbar = QtWidgets.QStatusBar(MainWindow)
        self.statusbar.setObjectName("statusbar")
        MainWindow.setStatusBar(self.statusbar)

        self.retranslateUi(MainWindow)
        QtCore.QMetaObject.connectSlotsByName(MainWindow)

    def retranslateUi(self, MainWindow):
        _translate = QtCore.QCoreApplication.translate
        MainWindow.setWindowTitle(_translate("MainWindow", "MainWindow"))
        self.checkBox.setText(_translate("MainWindow", "是否允许匿名用户"))
        self.pushButton.setText(_translate("MainWindow", "创建"))
        self.label.setText(_translate("MainWindow", "用户名："))
        self.label_2.setText(_translate("MainWindow", "域名："))
        self.label_3.setText(_translate("MainWindow", "端口号："))
        self.label_4.setText(_translate("MainWindow", "密码："))
        self.pushButton_2.setText(_translate("MainWindow", "选择路经"))

        self.lineEdit.setText("127.0.0.1")
        self.lineEdit_2.setText("2333")
        self.lineEdit_3.setText("user")
        self.lineEdit_4.setText("123456")
        self.lineEdit_5.setText("D:\sharedir")    


        self.pushButton.clicked.connect(self.creat_button)
        self.pushButton_2.clicked.connect(self.select_button)
    
    def run_ftpserver(self,usr,pwd,seldir):
        
        host = self.lineEdit.text()
        port = int(self.lineEdit_2.text())
        # 是否允许匿名用户
        if(self.checkBox.isChecked()):    
            try:
                # 添加用户 参数：username,password,允许的路径、权限
                self.anonymauthorizer.add_user(usr,pwd,seldir,perm='elradfmwMT') 
                self.handler.authorizer = self.anonymauthorizer
            except Exception as e:
                print(e)
        else:
            try:
                # 添加用户 参数：username,password,允许的路径、权限
                self.authorizer.add_user(usr,pwd,seldir,perm='elradfmwMT') 
                self.handler.authorizer = self.authorizer
            except Exception as e:
                print(e)

        self.handler.banner = 'pyftplib based ftpd ready.'
        server = FTPServer((host, port), self.handler)


        # 设定一个限制连接
        
        server.max_cons = 256
        server.max_cons_per_ip = 5
        # 开启服务器
        server.serve_forever()
         

    def creat_button(self):
        # host = self.lineEdit.text()
        # port = int(self.lineEdit_2.text())
        usr = self.lineEdit_3.text()
        pwd = self.lineEdit_4.text()
        seldir = self.lineEdit_5.text()

        try:
            t1 =threading.Thread(target=self.run_ftpserver,args=(usr,pwd,seldir))
            t1.start()
            self.myshow.msg() 
        except Exception as e:
            print(e)
            print("Open the failure")

    def select_button(self):
        #当窗口非继承QtWidgets.QDialog时，self可替换成 None
        try:
            directory = QFileDialog.getExistingDirectory()
            self.lineEdit_5.setText(directory)  
        except Exception as e:
            print(e)

if __name__ == '__main__':
    # tk().withdraw()
    app = QtWidgets.QApplication(sys.argv)
    MainWindow = QtWidgets.QMainWindow()
    ui = Ui_MainWindow()
    ui.setupUi(MainWindow)
    MainWindow.show()
    sys.exit(app.exec_())
