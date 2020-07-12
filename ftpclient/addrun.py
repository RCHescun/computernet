if __name__ == '__main__':
    app = QtWidgets.QApplication(sys.argv)
    MainWindow = QtWidgets.QMainWindow()
    ui = Ui_FtpFile()
    ui.setupUi(MainWindow)
    MainWindow.show()
    sys.exit(app.exec_())