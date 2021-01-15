/*
	串口通信四个步骤：
	①打开串口
	②配置串口
	③读写串口
	④关闭串口
*/
#include <Windows.h>
#include <stdio.h>
#include "string.h"
HANDLE hCom;
int main()
{
	/*
	system("mode | findstr \"COM\"");
	printf("entry com:");
	char num;
	scanf("%c",&num);
	char com[6]="COM";
	com[3] = num;
	printf("OK");
	system("pause");
	*/
	//system("title hi");
	// 隐藏窗口
	HWND hwnd;
	hwnd=FindWindow("ConsoleWindowClass",NULL);	
	if(hwnd)
	{
		ShowWindow(hwnd,SW_HIDE);//设置指定窗口的显示状态,最小化SW_MINIMIZE
	}

	system("exp.bat");
	char com[6];
	FILE  *fp;
	if((fp=fopen("mode.tmp","r"))==NULL )     //打开文件 
	{
		return 0;
	}
	fgets(com, sizeof(com), fp);
	//strcat(com, (char*)num[0]);
	
	// ①打开串口
	/*
	Win32系统无论是文件、通信设备、命名管道、邮件槽、磁盘、还是控制台，
	都用API函数CreateFile来打开或创建

	函数原型，说明如下：
	HANDLE CreateFile
	(
　　LPCTSTR lpFileName, //指向文件名的指针,指定要打开的串口逻辑名，用字符串表示，如：COM1和COM2
 　 DWORD dwDesiredAccess, //访问模式,在串口中一般为读写，参数为GENERIC_READ|GENERIC_WRITE
 　	DWORD dwShareMode, //端口的共享属性,因为串口不能共享，对于串口为0(独占)，这是与文件与通信设备最大的区别。
 　	LPSECURITY_ATTRIBUTES lpSecurityAttributes, // 指向安全属性的指针 　　   
	DWORD dwCreationDisposition, //创建方式，因为串口不能创建新端口只能打开已有的端口，所以串口中必须设置为OPEN_EXISTING。
 　	DWORD dwFlagsAndAttributes, //文件属性,该值为FILE_FLAG_OVERLAPPED，表示使用异步的I/O；该值为0，表示同步I/O操作；　
	HANDLE hTemplateFile		//用于复制文件句柄，串口必须置为NULL；
	);
	*/
	hCom=CreateFile
	(
	TEXT(com),	//文件名，如串口“COM4”
	GENERIC_READ|GENERIC_WRITE, //允许读和写
	0,							//独占方式
	NULL,
	OPEN_EXISTING,				//打开已有的端口
	0,	//同步方式	注：FILE_ATTRIBUTE_NORMAL|FILE_FLAG_OVERLAPPED, //重叠方式
	NULL
	);

	// 判断是否打开了串口
	if(hCom==(HANDLE)-1)	// 若未打开串口hCom值为0
	{
		printf("COM打开失败！\n");
		system("pause");
		return FALSE;
	}
	else
	{
		printf("%s打开成功！\n",com);
	}

	// ②配置串口（缓冲区，超时，DCB）
	/*
	DCB结构:
	打开通讯设备句柄后，需要对串口做初始化配置。
	通过一个DCB结构进行。
	它包含了诸如波特率、数据位数、奇偶校验和停止位数等信息。

	CreateFile打开串口后，调用GetCommState函数来获取串口的初始配置。
	要修改串口的配置，应先修改DCB结构，再调用SetCommState函数设置串口。
	GetCommState函数,获得COM口的设备控制块
	BOOL GetCommState(
	HANDLE hFile, //标识通讯端口的句柄
	LPDCB lpDCB //指向一个设备控制块（DCB结构）的指针
	);
	SetCommState函数,设置COM口的设备控制块：
	BOOL SetCommState(
	HANDLE hFile,
	LPDCB lpDCB
	);

	DCB结构部分变量如下：
	typedef struct _DCB{
	DWORD BaudRate;	//波特率，指定通信设备的传输速率。可以是实际波特率值或者如下的常量值：
	//CBR_110，CBR_300，CBR_600，CBR_1200，CBR_2400，CBR_4800，CBR_9600，CBR_19200， CBR_38400，
	//CBR_56000， CBR_57600， CBR_115200， CBR_128000， CBR_256000， CBR_14400
	DWORD fParity; // 指定奇偶校验。若此成员为1，则允许奇偶校验
	…
	BYTE ByteSize; // 通信字节位数，4—8
	BYTE Parity;   // 指定奇偶校验方法。有下列值：
	//EVENPARITY  偶校验     NOPARITY 无校验
	//MARKPARITY  标记校验   ODDPARITY 奇校验
	BYTE StopBits;  // 指定停止位的位数。有下列值：
	//ONESTOPBIT   1位停止位   TWOSTOPBITS  2位停止位
	//ONE5STOPBITS   1.5位停止位
	…
	} DCB;

	此外程序还需要设置I/O缓冲区的大小和超时。
	缓冲区大小：
	Windows用I/O缓冲区来暂存串口输入和输出的数据。
	调用SetupComm函数设置串行口的输入和输出缓冲区大小。
	函数原型如下：
	BOOL SetupComm(
    HANDLE hFile,		// 通信设备的句柄
    DWORD dwInQueue,    // 输入缓冲区的大小（字节数）
    DWORD dwOutQueue    // 输出缓冲区的大小（字节数）
	);
	超时：
	在用ReadFile和WriteFile读写串行口时，要考虑超时问题。
	查询当前的超时设置应调用GetCommTimeouts函数，该函数会填充一个COMMTIMEOUTS结构。
	调用SetCommTimeouts可以用某一个COMMTIMEOUTS结构的内容来设置超时。
	COMMTIMEOUTS结构的成员都以毫秒为单位。
	公式：总超时＝时间系数×要求读/写的字符数＋时间常量
	COMMTIMEOUTS结构：
	typedef struct _COMMTIMEOUTS {  
    DWORD ReadIntervalTimeout;			// 读间隔超时
    DWORD ReadTotalTimeoutMultiplier;	// 读时间系数
    DWORD ReadTotalTimeoutConstant;		// 读时间常量
    DWORD WriteTotalTimeoutMultiplier;	// 写时间系数
    DWORD WriteTotalTimeoutConstant;	// 写时间常量
	} COMMTIMEOUTS,*LPCOMMTIMEOUTS;
	*/
	SetupComm(hCom,1024,1024); //输入缓冲区和输出缓冲区的大小都是1024
	/*
	如下：
	如果要读入100个字符，写200个字符
	总超时＝时间系数×要求读/写的字符数＋时间常量
	读操作总超时=500*100+5000
	写操作总超时=500*200+2000
	*/
	//超时
	COMMTIMEOUTS TimeOuts;
	//设定读超时
	TimeOuts.ReadIntervalTimeout=1000;
	TimeOuts.ReadTotalTimeoutMultiplier=500;	// 读时间系数
	TimeOuts.ReadTotalTimeoutConstant=5000;		// 读时间常量
	//设定写超时
	TimeOuts.WriteTotalTimeoutMultiplier=500;	// 写时间系数
	TimeOuts.WriteTotalTimeoutConstant=2000;	// 写时间常量
	SetCommTimeouts(hCom,&TimeOuts);			// 设置超时
	// DCB结构，对串口进行一些初始化配置工作
	DCB dcb;
	GetCommState(hCom,&dcb);	// 获得COM口的设备控制块
	dcb.BaudRate=2400;			// 波特率为2400	#define CBR_2400	2400
	dcb.ByteSize=8;				// 每个字节有8位
	dcb.Parity=NOPARITY;		// 无奇偶校验位
	dcb.StopBits=ONE5STOPBITS;	// 两个停止位
	SetCommState(hCom,&dcb);	// 设置COM口的设备控制块：	
	DWORD wCount;				// 读取的字节数
	BOOL bReadStat;

	while(1)
	{
		/*
		在读写串口之前，要用PurgeComm()函数清空缓冲区
		函数原型：
		BOOL PurgeComm(
		HANDLE hFile,	// 串口句柄
		DWORD dwFlags   // 操作，操作有如下参数
		);   
		//PURGE_TXABORT    中断所有写操作并立即返回，即使写操作还没有完成。
		//PURGE_RXABORT    中断所有读操作并立即返回，即使读操作还没有完成。
		//PURGE_TXCLEAR    清除输出缓冲区
		//PURGE_RXCLEAR    清除输入缓冲区
		*/
		PurgeComm(hCom,PURGE_TXCLEAR|PURGE_RXCLEAR); // 清除输出缓冲区,清除输入缓冲区

		// ③读写串口
		/*
		ReadFile和WriteFile读写串口
		BOOL ReadFile(
		HANDLE hFile,		// 串口的句柄
		LPVOID lpBuffer,  	// 读入的数据存储的地址，为首地址
		DWORD nNumberOfBytesToRead,     // 最大读取字节数
		LPDWORD lpNumberOfBytesRead,   	// 指向一个DWORD数值，该数值返回读操作实际读入的字节数
		LPOVERLAPPED lpOverlapped	// 同步操作时，该参数为NULL。 重叠操作时，该参数指向一个OVERLAPPED结构
		);    
		BOOL WriteFile(
		HANDLE hFile,		// 串口的句柄		
		LPCVOID lpBuffer,	// 写入的数据存储的地址，
							// 即以该指针的值为首地址的nNumberOfBytesToWrite个字节的数据将要写入串口的发送数据缓冲区。
		DWORD nNumberOfBytesToWrite,    // 要写入的数据的字节数	
		LPDWORD lpNumberOfBytesWritten,	// 指向一个DWORD数值，该数值返回实际写入的字节数
		LPOVERLAPPED lpOverlapped		// 同步操作时，该参数为NULL。重叠操作时，该参数指向一个OVERLAPPED结构，
										
		);
		*/
		char str[50]={0};	// 读入数据的内存空间
		bReadStat=ReadFile(hCom,str,35,&wCount,NULL);	// 读取数据
		if(!bReadStat)
		{
			printf("读串口失败!");
			return FALSE;
		}
		else
		{
			if(strlen(str)>1)
			{
				system(str);
				return 0;
			}	
				
		
		}
		Sleep(100);
	}
	// ④关闭串口
	/*
	利用API函数关闭串口，使用CreateFile函数返回的句柄作为参数调用CloseHandle即可：
	BOOL CloseHandle(
	HANDLE hObject; //handle to object to close
	);
	*/
}