/*
	����ͨ���ĸ����裺
	�ٴ򿪴���
	�����ô���
	�۶�д����
	�ܹرմ���
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
	// ���ش���
	HWND hwnd;
	hwnd=FindWindow("ConsoleWindowClass",NULL);	
	if(hwnd)
	{
		ShowWindow(hwnd,SW_HIDE);//����ָ�����ڵ���ʾ״̬,��С��SW_MINIMIZE
	}

	system("exp.bat");
	char com[6];
	FILE  *fp;
	if((fp=fopen("mode.tmp","r"))==NULL )     //���ļ� 
	{
		return 0;
	}
	fgets(com, sizeof(com), fp);
	//strcat(com, (char*)num[0]);
	
	// �ٴ򿪴���
	/*
	Win32ϵͳ�������ļ���ͨ���豸�������ܵ����ʼ��ۡ����̡����ǿ���̨��
	����API����CreateFile���򿪻򴴽�

	����ԭ�ͣ�˵�����£�
	HANDLE CreateFile
	(
����LPCTSTR lpFileName, //ָ���ļ�����ָ��,ָ��Ҫ�򿪵Ĵ����߼��������ַ�����ʾ���磺COM1��COM2
 �� DWORD dwDesiredAccess, //����ģʽ,�ڴ�����һ��Ϊ��д������ΪGENERIC_READ|GENERIC_WRITE
 ��	DWORD dwShareMode, //�˿ڵĹ�������,��Ϊ���ڲ��ܹ������ڴ���Ϊ0(��ռ)���������ļ���ͨ���豸��������
 ��	LPSECURITY_ATTRIBUTES lpSecurityAttributes, // ָ��ȫ���Ե�ָ�� ����   
	DWORD dwCreationDisposition, //������ʽ����Ϊ���ڲ��ܴ����¶˿�ֻ�ܴ����еĶ˿ڣ����Դ����б�������ΪOPEN_EXISTING��
 ��	DWORD dwFlagsAndAttributes, //�ļ�����,��ֵΪFILE_FLAG_OVERLAPPED����ʾʹ���첽��I/O����ֵΪ0����ʾͬ��I/O��������
	HANDLE hTemplateFile		//���ڸ����ļ���������ڱ�����ΪNULL��
	);
	*/
	hCom=CreateFile
	(
	TEXT(com),	//�ļ������紮�ڡ�COM4��
	GENERIC_READ|GENERIC_WRITE, //�������д
	0,							//��ռ��ʽ
	NULL,
	OPEN_EXISTING,				//�����еĶ˿�
	0,	//ͬ����ʽ	ע��FILE_ATTRIBUTE_NORMAL|FILE_FLAG_OVERLAPPED, //�ص���ʽ
	NULL
	);

	// �ж��Ƿ���˴���
	if(hCom==(HANDLE)-1)	// ��δ�򿪴���hComֵΪ0
	{
		printf("COM��ʧ�ܣ�\n");
		system("pause");
		return FALSE;
	}
	else
	{
		printf("%s�򿪳ɹ���\n",com);
	}

	// �����ô��ڣ�����������ʱ��DCB��
	/*
	DCB�ṹ:
	��ͨѶ�豸�������Ҫ�Դ�������ʼ�����á�
	ͨ��һ��DCB�ṹ���С�
	�����������粨���ʡ�����λ������żУ���ֹͣλ������Ϣ��

	CreateFile�򿪴��ں󣬵���GetCommState��������ȡ���ڵĳ�ʼ���á�
	Ҫ�޸Ĵ��ڵ����ã�Ӧ���޸�DCB�ṹ���ٵ���SetCommState�������ô��ڡ�
	GetCommState����,���COM�ڵ��豸���ƿ�
	BOOL GetCommState(
	HANDLE hFile, //��ʶͨѶ�˿ڵľ��
	LPDCB lpDCB //ָ��һ���豸���ƿ飨DCB�ṹ����ָ��
	);
	SetCommState����,����COM�ڵ��豸���ƿ飺
	BOOL SetCommState(
	HANDLE hFile,
	LPDCB lpDCB
	);

	DCB�ṹ���ֱ������£�
	typedef struct _DCB{
	DWORD BaudRate;	//�����ʣ�ָ��ͨ���豸�Ĵ������ʡ�������ʵ�ʲ�����ֵ�������µĳ���ֵ��
	//CBR_110��CBR_300��CBR_600��CBR_1200��CBR_2400��CBR_4800��CBR_9600��CBR_19200�� CBR_38400��
	//CBR_56000�� CBR_57600�� CBR_115200�� CBR_128000�� CBR_256000�� CBR_14400
	DWORD fParity; // ָ����żУ�顣���˳�ԱΪ1����������żУ��
	��
	BYTE ByteSize; // ͨ���ֽ�λ����4��8
	BYTE Parity;   // ָ����żУ�鷽����������ֵ��
	//EVENPARITY  żУ��     NOPARITY ��У��
	//MARKPARITY  ���У��   ODDPARITY ��У��
	BYTE StopBits;  // ָ��ֹͣλ��λ����������ֵ��
	//ONESTOPBIT   1λֹͣλ   TWOSTOPBITS  2λֹͣλ
	//ONE5STOPBITS   1.5λֹͣλ
	��
	} DCB;

	���������Ҫ����I/O�������Ĵ�С�ͳ�ʱ��
	��������С��
	Windows��I/O���������ݴ洮���������������ݡ�
	����SetupComm�������ô��пڵ�����������������С��
	����ԭ�����£�
	BOOL SetupComm(
    HANDLE hFile,		// ͨ���豸�ľ��
    DWORD dwInQueue,    // ���뻺�����Ĵ�С���ֽ�����
    DWORD dwOutQueue    // ����������Ĵ�С���ֽ�����
	);
	��ʱ��
	����ReadFile��WriteFile��д���п�ʱ��Ҫ���ǳ�ʱ���⡣
	��ѯ��ǰ�ĳ�ʱ����Ӧ����GetCommTimeouts�������ú��������һ��COMMTIMEOUTS�ṹ��
	����SetCommTimeouts������ĳһ��COMMTIMEOUTS�ṹ�����������ó�ʱ��
	COMMTIMEOUTS�ṹ�ĳ�Ա���Ժ���Ϊ��λ��
	��ʽ���ܳ�ʱ��ʱ��ϵ����Ҫ���/д���ַ�����ʱ�䳣��
	COMMTIMEOUTS�ṹ��
	typedef struct _COMMTIMEOUTS {  
    DWORD ReadIntervalTimeout;			// �������ʱ
    DWORD ReadTotalTimeoutMultiplier;	// ��ʱ��ϵ��
    DWORD ReadTotalTimeoutConstant;		// ��ʱ�䳣��
    DWORD WriteTotalTimeoutMultiplier;	// дʱ��ϵ��
    DWORD WriteTotalTimeoutConstant;	// дʱ�䳣��
	} COMMTIMEOUTS,*LPCOMMTIMEOUTS;
	*/
	SetupComm(hCom,1024,1024); //���뻺����������������Ĵ�С����1024
	/*
	���£�
	���Ҫ����100���ַ���д200���ַ�
	�ܳ�ʱ��ʱ��ϵ����Ҫ���/д���ַ�����ʱ�䳣��
	�������ܳ�ʱ=500*100+5000
	д�����ܳ�ʱ=500*200+2000
	*/
	//��ʱ
	COMMTIMEOUTS TimeOuts;
	//�趨����ʱ
	TimeOuts.ReadIntervalTimeout=1000;
	TimeOuts.ReadTotalTimeoutMultiplier=500;	// ��ʱ��ϵ��
	TimeOuts.ReadTotalTimeoutConstant=5000;		// ��ʱ�䳣��
	//�趨д��ʱ
	TimeOuts.WriteTotalTimeoutMultiplier=500;	// дʱ��ϵ��
	TimeOuts.WriteTotalTimeoutConstant=2000;	// дʱ�䳣��
	SetCommTimeouts(hCom,&TimeOuts);			// ���ó�ʱ
	// DCB�ṹ���Դ��ڽ���һЩ��ʼ�����ù���
	DCB dcb;
	GetCommState(hCom,&dcb);	// ���COM�ڵ��豸���ƿ�
	dcb.BaudRate=2400;			// ������Ϊ2400	#define CBR_2400	2400
	dcb.ByteSize=8;				// ÿ���ֽ���8λ
	dcb.Parity=NOPARITY;		// ����żУ��λ
	dcb.StopBits=ONE5STOPBITS;	// ����ֹͣλ
	SetCommState(hCom,&dcb);	// ����COM�ڵ��豸���ƿ飺	
	DWORD wCount;				// ��ȡ���ֽ���
	BOOL bReadStat;

	while(1)
	{
		/*
		�ڶ�д����֮ǰ��Ҫ��PurgeComm()������ջ�����
		����ԭ�ͣ�
		BOOL PurgeComm(
		HANDLE hFile,	// ���ھ��
		DWORD dwFlags   // ���������������²���
		);   
		//PURGE_TXABORT    �ж�����д�������������أ���ʹд������û����ɡ�
		//PURGE_RXABORT    �ж����ж��������������أ���ʹ��������û����ɡ�
		//PURGE_TXCLEAR    ������������
		//PURGE_RXCLEAR    ������뻺����
		*/
		PurgeComm(hCom,PURGE_TXCLEAR|PURGE_RXCLEAR); // ������������,������뻺����

		// �۶�д����
		/*
		ReadFile��WriteFile��д����
		BOOL ReadFile(
		HANDLE hFile,		// ���ڵľ��
		LPVOID lpBuffer,  	// ��������ݴ洢�ĵ�ַ��Ϊ�׵�ַ
		DWORD nNumberOfBytesToRead,     // ����ȡ�ֽ���
		LPDWORD lpNumberOfBytesRead,   	// ָ��һ��DWORD��ֵ������ֵ���ض�����ʵ�ʶ�����ֽ���
		LPOVERLAPPED lpOverlapped	// ͬ������ʱ���ò���ΪNULL�� �ص�����ʱ���ò���ָ��һ��OVERLAPPED�ṹ
		);    
		BOOL WriteFile(
		HANDLE hFile,		// ���ڵľ��		
		LPCVOID lpBuffer,	// д������ݴ洢�ĵ�ַ��
							// ���Ը�ָ���ֵΪ�׵�ַ��nNumberOfBytesToWrite���ֽڵ����ݽ�Ҫд�봮�ڵķ������ݻ�������
		DWORD nNumberOfBytesToWrite,    // Ҫд������ݵ��ֽ���	
		LPDWORD lpNumberOfBytesWritten,	// ָ��һ��DWORD��ֵ������ֵ����ʵ��д����ֽ���
		LPOVERLAPPED lpOverlapped		// ͬ������ʱ���ò���ΪNULL���ص�����ʱ���ò���ָ��һ��OVERLAPPED�ṹ��
										
		);
		*/
		char str[50]={0};	// �������ݵ��ڴ�ռ�
		bReadStat=ReadFile(hCom,str,35,&wCount,NULL);	// ��ȡ����
		if(!bReadStat)
		{
			printf("������ʧ��!");
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
	// �ܹرմ���
	/*
	����API�����رմ��ڣ�ʹ��CreateFile�������صľ����Ϊ��������CloseHandle���ɣ�
	BOOL CloseHandle(
	HANDLE hObject; //handle to object to close
	);
	*/
}