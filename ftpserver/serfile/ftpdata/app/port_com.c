/*   本程序符合GPL条约
 *   Beneboy 2003-5-16
 */
#include <stdio.h>              // printf
#include <fcntl.h>              // open
#include <string.h>             // bzero
#include <stdlib.h>             // exit
#include <sys/time.h>          // times
#include <sys/types.h>          // pid_t
#include <termios.h>        //termios, tcgetattr(), tcsetattr()
#include <unistd.h>
#include <sys/ioctl.h>          // ioctl


//接收超时
#define TIMEOUT_SEC(buflen,baud) (buflen*20/baud+1)
#define TIMEOUT_USEC 0

typedef struct
{
	char                prompt;      //prompt after reciving data
	int                 baudrate;       //baudrate
	char                databit;         //data bits, 5, 6, 7, 8
	char                debug;   //debug mode, 0: none, 1: debug
	char                echo;    //echo mode, 0: none, 1: echo
	char                fctl;            //flow control, 0: none, 1: hardware, 2: software
	char                tty;         //tty: 0, 1, 2, 3, 4, 5, 6, 7
	char                parity;      //parity 0: none, 1: odd, 2: even
	char                stopbit;         //stop bits, 1, 2
	int                 reserved;   //reserved, must be zero
}portinfo_t;


typedef portinfo_t  *pportinfo_t;

// serial port information
portinfo_t  g_sPortInfo =
{
	'0',                          // print prompt after receiving
	115200,                       // baudrate: 115200
	'8',                          // databit: 8
	'0',                          // debug: off
	'0',                          // echo: off
	'0',                          // flow control: software
	'0',                          // default tty: COM1
	'0',                          // parity: none
	'1',                          // stopbit: 1
	0                             // reserved
};

/*******************************************
 *  波特率转化转换函数
 ********************************************/
int convbaud(unsigned long int baudrate)
{
	switch (baudrate)
	{
		case 2400:
			return B2400;
		case 4800:
			return B4800;
		case 9600:
			return B9600;
		case 19200:
			return B19200;
		case 38400:
			return B38400;
		case 57600:
			return B57600;
		case 115200:
			return B115200;
		case 230400:
			return B230400;
		case 460800:
			return B460800;
		case 921600:
			return B921600;
		default:
			return B9600;
	}
}

/*******************************************
 *  Setup comm attr
 *  fdcom: 串口文件描述符, pportinfo: 待设置的端口信息s
 *
 ********************************************/
int PortSet(int fdcom, const pportinfo_t pportinfo)
{
	struct termios  termios_old, termios_new;
	int             baudrate, tmp;
	char            databit, stopbit, parity, fctl;
	bzero(&termios_old, sizeof(termios_old));
	bzero(&termios_new, sizeof(termios_new));
	cfmakeraw(&termios_new);
	tcgetattr(fdcom, &termios_old);         //get the serial port attributions
	/*------------设置端口属性----------------*/
	//baudrates
	baudrate = convbaud(pportinfo -> baudrate);
	cfsetispeed(&termios_new, baudrate);        //填入串口输入端波特率
	cfsetospeed(&termios_new, baudrate);        //填入串口输出端波特率
	termios_new.c_cflag |= CLOCAL;          //控制模式, 保证程序不会成为端口的占有者
	termios_new.c_cflag |= CREAD;           //控制模式, 使能端口读取输入的数据
	// 控制模式, flow control
	fctl = pportinfo-> fctl;
	switch (fctl)
	{
		case '0':
			{
				termios_new.c_cflag &= ~CRTSCTS;        //no flow control
			}break;
		case '1':
			{
				termios_new.c_cflag |= CRTSCTS;         //hardware flow control
			}break;
		case '2':
			{
				termios_new.c_iflag |= IXON | IXOFF | IXANY;    //software flow control
			}break;
		default:
			{
				break;
			}
	}
	//控制模式, data bits
	termios_new.c_cflag &= ~CSIZE;      //控制模式, 屏蔽字符大小位
	databit = pportinfo -> databit;
	switch (databit)
	{
		case '5':
			termios_new.c_cflag |= CS5;
			//lint -fallthrough
		case '6':
			termios_new.c_cflag |= CS6;
			//lint -fallthrough
		case '7':
			termios_new.c_cflag |= CS7;
			//lint -fallthrough
		default:
			termios_new.c_cflag |= CS8;
	}
	//控制模式 parity check
	parity = pportinfo -> parity;
	switch (parity)
	{
		case '0':
			{
				termios_new.c_cflag &= ~PARENB;     //no parity check
			}break;
		case '1':
			{
				termios_new.c_cflag |= PARENB;      //odd check
				termios_new.c_cflag &= ~PARODD;
			}break;
		case '2':
			{
				termios_new.c_cflag |= PARENB;      //even check
				termios_new.c_cflag |= PARODD;
			}break;
		default:
			{
				break;
			}
	}
	//控制模式, stop bits
	stopbit = pportinfo -> stopbit;
	if (stopbit == '2')
	{
		termios_new.c_cflag |= CSTOPB;  //2 stop bits
	}
	else
	{
		termios_new.c_cflag &= ~CSTOPB; //1 stop bits
	}
	//other attributions default
	termios_new.c_oflag &= ~OPOST;          //输出模式, 原始数据输出
	termios_new.c_cc[VMIN] = 1;         //控制字符, 所要读取字符的最小数量
	termios_new.c_cc[VTIME] = 1;            //控制字符, 读取第一个字符的等待时间,   unit: (1/10)second
	tcflush(fdcom, TCIFLUSH);           //溢出的数据可以接收,但不读
	tmp = tcsetattr(fdcom, TCSANOW, &termios_new);  //设置新属性, TCSANOW: 所由改变立即生效
	tcgetattr(fdcom, &termios_old);
	return(tmp);
}
/*******************************************
 *  Open serial port
 *  tty: 端口号 ttyS0, ttyS1, ....
 *  返回值为串口文件描述符
 ********************************************/
int PortOpen(pportinfo_t pportinfo, int mode)
{
	return 0;
}
int PortInit(int mode)
{
	return 0;
}
/*******************************************
 *  Close serial port
 ********************************************/
void PortClose(int fdcom)
{
	close(fdcom);
}
/********************************************
 *  send data
 *  fdcom: 串口描述符, data: 待发送数据, datalen: 数据长度
 *  返回实际发送长度
 *********************************************/
int PortSend(int fdcom, char *data, int datalen)
{
	int len = 0;

	len = write(fdcom, data, datalen);  //实际写入的长度
	if (len == datalen)
	{
		return (len);
	}
	else
	{
		tcflush(fdcom, TCOFLUSH);
		return -1;
	}
}
/*******************************************
 *  receive data
 *  返回实际读入的字节数
 *
 ********************************************/
int PortRecv(int fdcom, char *data, int datalen/*, int baudrate*/)
{
#if 0
	int             readlen, fs_sel;
	fd_set          fs_read;
	struct timeval  tv_timeout;
	FD_ZERO(&fs_read);
	FD_SET(fdcom, &fs_read);

	/*RAT 该值应该减小以提高响应速度*/
	tv_timeout.tv_sec = 0;/*TIMEOUT_SEC(datalen, baudrate);*/

	tv_timeout.tv_usec = 200*1000;/*TIMEOUT_USEC;*/

	fs_sel = select(fdcom + 1, &fs_read, NULL, NULL, &tv_timeout);
	if (fs_sel)
	{

		printf("rpmsg: portrecv fdcom =%d\n",fdcom);
		readlen = read(fdcom, data, datalen);
		return(readlen);
	}
	else
	{
		return(0);
	}
#else
	int readlen = read(fdcom, data, datalen);
	return(readlen);
#endif
}

int PortRecvTimeout(int fdcom, char *data, int datalen, int msec)
{
#if 1
	int             readlen, fs_sel;
	fd_set          fs_read;
	struct timeval  tv_timeout;
	FD_ZERO(&fs_read);
	FD_SET(fdcom, &fs_read);

	/*RAT 该值应该减小以提高响应速度*/
	tv_timeout.tv_sec = 0;/*TIMEOUT_SEC(datalen, baudrate);*/

	tv_timeout.tv_usec = msec*1000;/*TIMEOUT_USEC;*/

	fs_sel = select(fdcom + 1, &fs_read, NULL, NULL, &tv_timeout);
	if (fs_sel)
	{

		printf("rpmsg: portrecv fdcom =%d\n",fdcom);
		readlen = read(fdcom, data, datalen);
		return(readlen);
	}
	else
	{
		return(0);
	}
#else
	int readlen = read(fdcom, data, datalen);
	return(readlen);
#endif
}


int wait_at_port_open(const char *strPorName)
{
	int iFdCom = -1;

	iFdCom= open(strPorName, O_RDWR);
	if (iFdCom < 0)
	{
		printf("open serial at port:%s failed, fd:%d\n", strPorName, iFdCom);
		return iFdCom;
	}

	printf("rpmsg: ap port:%s open iFdCom = %d\n", strPorName, iFdCom);//add shideyou
	ioctl(iFdCom, (('R'<<8)|1|(0x4004<<16)), 0x400);
	ioctl(iFdCom, (('R'<<8)|4|(0x4004<<16)), 0);
	PortSet(iFdCom, &g_sPortInfo); 
	return iFdCom;
}

