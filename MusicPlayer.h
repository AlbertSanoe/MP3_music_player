#include <stdio.h>
#include <io.h>
#include <Windows.h>
#include <string.h>
#include <stdlib.h>
#include <string>
#include <conio.h>
#include <mmsystem.h>
#include <time.h>
#include <stdarg.h>
#define LENGTH 500

#pragma comment(lib,"winmm.lib")
HANDLE hOutput, hOutBuf, hSet;                   //����̨��Ļ���������
COORD coord;
DWORD bytes = 0;
char PrintfBuff[LENGTH];
HANDLE  hOut[2];                                 //�����������ľ��
HANDLE handle_out = GetStdHandle(STD_OUTPUT_HANDLE);
//��ȡϵͳĬ�ϻ��������



enum { FATAL, ERROR_, WARNING, INFO, DEBUG };    //log��־��¼���� һ����FATAL ERROR_ WARNING INFO DEBUG ����


//�����б��Ӧ����Ķ���
typedef struct MusicNode {
	int ReId;                 //���ID������1~n�ϸ����
	char name[100];           //��������
	struct MusicNode* next;   //nextָ��
}MusicNode, * LinkNode;


struct lrc {
	int time;                 //ÿ���ʶ�Ӧ���ʱ��
	char lyric[100];          //�������
}LRC[1000];                                      //LRC[i].lyric��ʾ��IDInput�׸�ĵ�i�и��
												 //ͬһʱ��ֻ��ȡһ�׸�ĸ��


int LinesNumber;                                 //LinesNumber��ʾ��IDInput�׸��������

LinkNode head[100] = { NULL };                   //100���赥��ʼ��Ϊ������
const char* Mode[4] = { "0", "˳�򲥷�","����ѭ��","�������" };

char _PATH[100] = { NULL };                      //�û��Զ���mp3�ļ��Լ��赥txt�ļ�����·��
char alls[1000][100];                            //�������и���������alls���棬����׺
char listname[100][100];                         //�Զ��岥���б����ƣ�����ʱ������׺������ʱ����׺         
												 //�½��赥��Ҫ����listname,ͨ��addone��Ӹ���

int ListSize[100];                               //ListSize[i]��ʾ��i���赥�ж����׸�
int ListNumber = 0;                              //�赥������

int MusicNumber = 0;                             //���и�������

int Mci_Flag = 0;                                //�ж��Ƿ����MusicOpen 0��ʾδ���ú���
int Bro_Flag = -1;                               //����/��ͣ 0��ʾ��ͣ 1��ʾ���� -1��ʾδ������
int ReId = -1;                                   //�ڵ�ǰ�赥�е����id
int ListFlag = -1;                               //��ǰ���Ÿ������ڸ赥�ı��
int BackFlag = -1;                               //���ڽ���������˵�ʱ�����޷����ŵ�����
int ModeFlag = 1;                                //����ģʽ 1˳�򲥷ţ�Ĭ��) 2����ѭ�� 3�������
int p = 1;                                       //����ȷ����ǰ����������ر�����ʱ��1������һ�δӸ�ʵ�һ�п�ʼ��ʾ
int LyricFlag = 1;                               //����p����ֵ����LyricFlag����¼��ǰ�ǵڼ��и��
char BroName[100];                               //��ǰ���ŵĸ�������
int IDInput = -1;                                //Ҫ���ŵĸ����ڵ�ǰ�б�����
int LyricShowFlag;                               //0��ʾ��ǰ�򿪵����׸�û��lrc��1��ʾ��
bool BuffSwitch;                                 //������ת����־


//��־��¼����
void log_record(int error_level, const char* format, ...)
{
	va_list args;
	FILE* fp = NULL;
	char time_str[32];
	char file_name[256];

	va_start(args, format);
	time_t time_log = time(NULL);
	strftime(file_name, sizeof(file_name), "%Y-%m-%d_log_history.log", localtime(&time_log));


	if ((fp = fopen(file_name, "a+")) != NULL)
	{
		strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", localtime(&time_log));
		if (error_level == (int)FATAL)
		{
			fprintf(fp, "[%s]-[%s]-[%s] :>   ", time_str, "FATAL", __FILE__);
			vfprintf(fp, format, args);
			fprintf(fp, "\n");

		}
		if (error_level == (int)ERROR_)
		{
			fprintf(fp, "[%s]-[%s]-[%s] :>   ", time_str, "ERROR", __FILE__);
			vfprintf(fp, format, args);
			fprintf(fp, "\n");

		}
		else if (error_level == (int)WARNING)
		{
			fprintf(fp, "[%s]-[%s]-[%s] :> ", time_str, "WARNINGs", __FILE__);
			vfprintf(fp, format, args);
			fprintf(fp, "\n");
		}
		else if (error_level == (int)INFO)
		{
			fprintf(fp, "[%s]-[%s]-[%s] :>      ", time_str, "INFO", __FILE__);
			vfprintf(fp, format, args);
			fprintf(fp, "\n");
		}
		else if (error_level == (int)DEBUG)
		{
			fprintf(fp, "[%s]-[%s]-[%s] :>    ", time_str, "DEBUG", __FILE__);
			vfprintf(fp, format, args);
			fprintf(fp, "\n");
		}
		fclose(fp);
	}

	va_end(args);
}



void initialalls();                              //ɨ�豾��mp3�ļ�����������Ϣ���浽alls������

												 //���ַ�����path����alls��ͬʱ����MusicNumber
inline void addalls(const char* path)
{
	strcpy(alls[++MusicNumber], path);
}
void addone(const char* name, int ListFlag);     //������Ϊname�ĸ�������ӵ�head[LishFlag]β��
void PrintList(int ListFlag);                    //��head[ListFlag]�еĸ�����ӡ
void SaveList(int ListFlag);                     //��������ʱ��ʼ���赥 �������ʱ�ٱ���赥��������������ԭtxt�ĵ�
char* GetName(int ReId, int ListFlag);           //����head[ListFlag]�ĵ�ReId�׸�����ƣ���֤ReId��Խ��
void initiallist();                              //�򿪳���ʱ��ȡtxt������list
void addlist(int n, const char* txtname);        //��txtname���Ƶ��ļ���ȡ��Ϣ�浽head[n]��
void deleteone(int n, int ListFlag);             //ɾ��head[ListFlag]�е�n�׸�
char* GetPrev();                                 //��֤ReId��Խ�磬������һ�׸�����ƣ�ÿ�η���ʱ��strcpy������һ���ַ�����
char* GetNext();                                 //��֤ReId��Խ�磬������һ�׸�����ƣ����ı�ReId,IDInput�ȵ�ֵ
bool exist(const char* path);                    //�ж��������·��Ϊpath���ļ��Ƿ����
void Readlrc();                                  //���ݵ�ǰ��IDInput��ListFlag��ȡ�����Ϣ������LRC�ṹ��
int DeleteList(int n);                           //ɾ�����Ϊn�ĸ赥��ɾ���ɹ�����0�����򷵻�-1
void Play();

//���ݸ�����ǰ���Ž��ȣ����ַ����鴢��������Ž���������ʾ���������ظ�������׵�ַ
char* Getprogress(int music_alltime, int music_timeing_ms);
char* Getvolumeline(int vol);                    //���ַ����鴢�浱ǰ��������ʾ���������ظ�������׵�ַ



/*********************************************************
ʵ��һЩ��mci��صĺ���
*********************************************************/

//��ʾ����xxx.lib����⣬���߱�������Ҫ�õ�xxx.lib��
//���ڹ���������д������xxx.lib��Ч��һ����
//�������ַ���д�� ���������ʹ����Ĵ����ʱ��Ͳ��������ù���settings�ˡ�

#define  CTRLCMD_SIZE 256

//���豸
void MusicOpen(const char* pPath)
{
	char CtrlCmd[CTRLCMD_SIZE];

	char pathindex[100] = { NULL };
	strcpy(pathindex, _PATH);

	strcat(pathindex, pPath);
	sprintf(CtrlCmd, "open \"%s\" alias mymusic", pathindex);//alias����
	mciSendString(CtrlCmd, NULL, 0, NULL);                   //��һ��MCI���豸����������һ���ַ���
	Mci_Flag = 1;
}

//��������
void MusicPlay()
{
	mciSendString("setaudio mymusic volume to 500", NULL, 0, NULL);
	mciSendString("play mymusic", NULL, 0, NULL);
}

//ֹͣ����
void MusicStop()
{
	mciSendString("stop mymusic", NULL, 0, NULL);
}

//�ر�����
void MusicClose()
{
	char c[100];
	int i = mciSendString("close mymusic", NULL, 0, NULL);//�����ɹ����ã��򷵻�0������ʧ�ܷ��ش������
	if (i)                                                //���ú���ʧ��
	{
		mciGetErrorString(i, c, 100);                     //���ڷ���һ�����������ı�������C���������շ��ص��ı������Ļ�����ָ��
		printf("%s\n", c);
	}
	Mci_Flag = 0;
}

//��ͣ����
void MusicPause()
{
	char c[100];
	int i = mciSendStringA("pause mymusic", NULL, 0, NULL);
	if (i)
	{
		mciGetErrorString(i, c, 100);
		printf("%s\n", c);
	}
}

//���ص�ǰ������С
int MusicGetVolume(void)
{
	char c[100];
	int v0 = 0;
	mciSendString("status mymusic volume", c, sizeof(c), NULL);
	v0 = atoi(c);    //cһ��Զָ�룬��ָ����Ӧ�ó��򷵻ص��ַ�����������sizeof(c)��ʾ�������Ĵ�С
	return v0;       //atoi���ַ���ת�����ͣ�������const char* �������������Ϊʲôһ���ַ��������������ַ���
}

//��������
void MusicSetVolumeUp(void)
{
	int volume = 0;
	char cmd[100];
	volume = MusicGetVolume();
	if (volume + 100 <= 1000)
	{
		wsprintf(cmd, "setaudio mymusic volume to %d", volume + 100);//��һϵ�е��ַ�����ֵ���뵽������
		mciSendString(cmd, NULL, 0, NULL);
	}
	else
	{
		mciSendString("setaudio mymusic volume to 1000", NULL, 0, NULL);
	}
}

//��С����
void MusicSetVolumeDown(void)
{
	int volume = 0;
	char cmd[100];
	volume = MusicGetVolume();
	if (volume - 100 >= 0)
	{
		wsprintf(cmd, "setaudio mymusic volume to %d", volume - 100);//��һϵ�е��ַ�����ֵ���뵽������
		mciSendString(cmd, NULL, 0, NULL);
	}
	else
	{
		mciSendString("setaudio mymusic volume to 0", NULL, 0, NULL);
	}
}

//���5��
void MusicFastForward(void)
{
	char c[100];
	char cmd[100];
	char c1[100];
	int iCurrent, i;
	mciSendString("pause mymusic", NULL, 0, NULL);
	mciSendString("status mymusic position", c, sizeof(c), NULL);
	iCurrent = atoi(c);                                      //��ǰ����λ��

	wsprintf(cmd, "seek mymusic to %d", iCurrent + 5000);
	i = mciSendString(cmd, NULL, 0, NULL);
	if (i)
	{
		mciGetErrorString(i, c1, 100);
		printf("%s\n", c1);
	}
	else   printf("%d\n", mciSendString("play mymusic", NULL, 0, NULL));
}

//����5�� Ĭ�Ͽ�����˺Ϸ�
void MusicFastBackward(void)
{
	char c[100];
	char cmd[100];
	char c1[100];
	int iCurrent, i;
	mciSendString("pause mymusic", NULL, 0, NULL);
	mciSendString("status mymusic position", c, sizeof(c), NULL);//��atoi��ʾ��ǰλ������ȡ������ţ�
	iCurrent = atoi(c);                                          //��ǰ����λ��

	wsprintf(cmd, "seek mymusic to %d", iCurrent - 5000);        //��ǰ����λ��-5000
	i = mciSendString(cmd, NULL, 0, NULL);
	if (i)
	{
		mciGetErrorString(i, c1, 100);
		printf("%s\n", c1);
	}
	else
	{
		mciSendString("play mymusic", NULL, 0, NULL);
	}
}

//�������֣��Ӿ���·������
void Play()
{
	if (1 == Mci_Flag)                             //1��ʾ���ù�MusicOpen����
	{
		MusicClose();
		Bro_Flag = -1;             //-1��ʾδ������
		log_record(ERROR_, "LINE:  %d   music open failed", __LINE__ - 2);
	}
	MusicOpen(BroName);
	Readlrc();
	MusicPlay();
	Bro_Flag = 1;                                  //1��ʾ��ǰ����״̬�ǲ���
	system("cls");
}

//���ֵĲ���/��ͣ
void MusicPauseBroadcast(void)
{
	if (Bro_Flag == 1)             //1��ʾ��ǰ����״̬�ǲ���
	{
		MusicPause();
		log_record(INFO, "LINE:  %d   music pause", __LINE__ - 1);
		Bro_Flag = 0;
	}
	else if (Bro_Flag == 0)        //0��ʾ��ǰ����״̬����ͣ
	{
		MusicPlay();
		log_record(INFO, "LINE:  %d   music play", __LINE__ - 1);
		Bro_Flag = 1;
	}
	else
	{
		printf("���ú���ʧ�ܣ�����δ�������ļ�\n");
		log_record(ERROR_, "LINE:  %d   func call failed", __LINE__ - 1);
		return;
	}
}




//˫��������ʼ��
void DoubleBuffInitial()
{
	//�����µĿ���̨������
	hOutBuf = CreateConsoleScreenBuffer(
		GENERIC_WRITE,           //������̿�����������д����
		FILE_SHARE_WRITE,        //���建�����ɹ���дȨ��
		NULL,
		CONSOLE_TEXTMODE_BUFFER,
		NULL
	);
	hOutput = CreateConsoleScreenBuffer(
		GENERIC_WRITE,           //������̿�����������д����
		FILE_SHARE_WRITE,        //���建�����ɹ���дȨ��
		NULL,
		CONSOLE_TEXTMODE_BUFFER,
		NULL
	);
	hOut[0] = hOutBuf;
	hOut[1] = hOutput;

	//���������������Ĺ��
	CONSOLE_CURSOR_INFO cursor;
	cursor.bVisible = FALSE;
	cursor.dwSize = sizeof(cursor);

	SetConsoleCursorInfo(hOutput, &cursor);
	SetConsoleCursorInfo(hOutBuf, &cursor);
}

//˫������ʾ���Ž���
void DoubleBuffSet(int music_timeing_ms, int music_alltime, int music_timeing, char* PreSong, char* NextSong)
{
	if (LyricShowFlag)
	{
		memset(PrintfBuff, 0, sizeof PrintfBuff);
		for (int i = 0; i < strlen(BroName) - 4; i++)
		{
			PrintfBuff[i] = BroName[i];

		}
		coord.X = 60;
		coord.Y = 1;
		WriteConsoleOutputCharacterA(hSet, PrintfBuff, LENGTH, coord, &bytes);

		for (p = 1; music_timeing_ms >= LRC[p].time; p++);

		LyricFlag = --p;


		memset(PrintfBuff, 0, sizeof PrintfBuff);
		if (LyricFlag >= 5)
		{
			strcpy(PrintfBuff, LRC[LyricFlag - 4].lyric);
		}
		coord.Y += 2;
		WriteConsoleOutputCharacterA(hSet, PrintfBuff, LENGTH, coord, &bytes);


		memset(PrintfBuff, 0, sizeof PrintfBuff);
		if (LyricFlag >= 4)
		{
			strcpy(PrintfBuff, LRC[LyricFlag - 3].lyric);
		}
		coord.Y += 2;
		WriteConsoleOutputCharacterA(hSet, PrintfBuff, LENGTH, coord, &bytes);

		memset(PrintfBuff, 0, sizeof PrintfBuff);
		if (LyricFlag >= 3)
		{
			strcpy(PrintfBuff, LRC[LyricFlag - 2].lyric);
		}
		coord.Y += 2;
		WriteConsoleOutputCharacterA(hSet, PrintfBuff, LENGTH, coord, &bytes);



		memset(PrintfBuff, 0, sizeof PrintfBuff);
		if (LyricFlag >= 2)
		{
			strcpy(PrintfBuff, LRC[LyricFlag - 1].lyric);
		}
		coord.Y += 2;
		WriteConsoleOutputCharacterA(hSet, PrintfBuff, LENGTH, coord, &bytes);



		memset(PrintfBuff, 0, sizeof PrintfBuff);
		strcpy(PrintfBuff, LRC[LyricFlag].lyric);
		coord.Y += 2;
		WriteConsoleOutputCharacterA(hSet, PrintfBuff, LENGTH, coord, &bytes);

		strcpy(PrintfBuff, "��");
		coord.X = 58;
		WriteConsoleOutputCharacterA(hSet, PrintfBuff, 2, coord, &bytes);
		coord.X = 60;



		memset(PrintfBuff, 0, sizeof PrintfBuff);
		if (LyricFlag + 1 <= LinesNumber)
		{
			strcpy(PrintfBuff, LRC[LyricFlag + 1].lyric);
		}
		coord.Y += 2;
		WriteConsoleOutputCharacterA(hSet, PrintfBuff, LENGTH, coord, &bytes);


		memset(PrintfBuff, 0, sizeof PrintfBuff);
		if (LyricFlag + 2 <= LinesNumber)
		{
			strcpy(PrintfBuff, LRC[LyricFlag + 2].lyric);
		}
		coord.Y += 2;
		WriteConsoleOutputCharacterA(hSet, PrintfBuff, LENGTH, coord, &bytes);


		memset(PrintfBuff, 0, sizeof PrintfBuff);
		if (LyricFlag + 3 <= LinesNumber)
		{
			strcpy(PrintfBuff, LRC[LyricFlag + 3].lyric);
		}
		coord.Y += 2;
		WriteConsoleOutputCharacterA(hSet, PrintfBuff, LENGTH, coord, &bytes);


		memset(PrintfBuff, 0, sizeof PrintfBuff);
		if (LyricFlag + 4 <= LinesNumber)
		{
			strcpy(PrintfBuff, LRC[LyricFlag + 4].lyric);
		}
		coord.Y += 2;
		WriteConsoleOutputCharacterA(hSet, PrintfBuff, LENGTH, coord, &bytes);


	}
	else {
		for (coord.Y = 1; coord.Y < 22; coord.Y += 2)
		{
			memset(PrintfBuff, 0, sizeof PrintfBuff);
			WriteConsoleOutputCharacterA(hSet, PrintfBuff, LENGTH, coord, &bytes);
		}
		strcpy(PrintfBuff, "��");
		coord.X = 58;
		coord.Y = 10;
		WriteConsoleOutputCharacterA(hSet, PrintfBuff, 2, coord, &bytes);
		memset(PrintfBuff, 0, sizeof PrintfBuff);
		strcpy(PrintfBuff, "δ�ҵ�����ļ�");
		coord.X = 60;
		WriteConsoleOutputCharacterA(hSet, PrintfBuff, LENGTH, coord, &bytes);

	}

	char progressline[101] = { NULL };
	strcpy(progressline, Getprogress(music_alltime, music_timeing_ms));
	coord.X = 18;
	coord.Y = 23;
	WriteConsoleOutputCharacterA(hSet, progressline, 100, coord, &bytes);

	memset(PrintfBuff, 0, sizeof PrintfBuff);
	sprintf(PrintfBuff, "Time:%d:%02d-%d:%02d", music_timeing / 60, music_timeing % 60, music_alltime / 60, music_alltime % 60); // ��ʾ��ǰʱ�����ʱ�䣬����������ǰ������ģʽ
	coord.X = 0;
	coord.Y = 23;
	WriteConsoleOutputCharacterA(hSet, PrintfBuff, 15, coord, &bytes);

	memset(PrintfBuff, 0, sizeof PrintfBuff);
	strcat(PrintfBuff, "��ǰ������        ");
	char volumeline[13] = { NULL };
	strcpy(volumeline, Getvolumeline(MusicGetVolume()));
	strcat(PrintfBuff, volumeline);
	coord.X = 0;
	coord.Y = 25;
	WriteConsoleOutputCharacterA(hSet, PrintfBuff, strlen(PrintfBuff), coord, &bytes);


	coord.X = 0;
	coord.Y = 1;
	memset(PrintfBuff, 0, sizeof PrintfBuff);
	strcpy(PrintfBuff, "����:");
	WriteConsoleOutputCharacterA(hSet, PrintfBuff, strlen(PrintfBuff), coord, &bytes);

	coord.X = 5;

	memset(PrintfBuff, 0, sizeof PrintfBuff);
	strcpy(PrintfBuff, "1��ͣ\\����");
	WriteConsoleOutputCharacterA(hSet, PrintfBuff, strlen(PrintfBuff), coord, &bytes);


	coord.Y += 2;
	memset(PrintfBuff, 0, sizeof PrintfBuff);
	sprintf(PrintfBuff, "2��һ�ף�%s)", PreSong);
	WriteConsoleOutputCharacterA(hSet, PrintfBuff, strlen(PrintfBuff), coord, &bytes);


	coord.Y += 2;
	memset(PrintfBuff, 0, sizeof PrintfBuff);
	sprintf(PrintfBuff, "3��һ�ף�%s)", NextSong);
	WriteConsoleOutputCharacterA(hSet, PrintfBuff, strlen(PrintfBuff), coord, &bytes);


	coord.Y += 2;
	memset(PrintfBuff, 0, sizeof PrintfBuff);
	strcpy(PrintfBuff, "4ֹͣ");
	WriteConsoleOutputCharacterA(hSet, PrintfBuff, strlen(PrintfBuff), coord, &bytes);


	coord.Y += 2;
	memset(PrintfBuff, 0, sizeof PrintfBuff);
	strcpy(PrintfBuff, "5��ӵ��ҵĸ赥");
	WriteConsoleOutputCharacterA(hSet, PrintfBuff, strlen(PrintfBuff), coord, &bytes);


	coord.Y += 2;
	memset(PrintfBuff, 0, sizeof PrintfBuff);
	strcpy(PrintfBuff, "6��������");
	WriteConsoleOutputCharacterA(hSet, PrintfBuff, strlen(PrintfBuff), coord, &bytes);


	coord.Y += 2;
	memset(PrintfBuff, 0, sizeof PrintfBuff);
	strcpy(PrintfBuff, "7��С����");
	WriteConsoleOutputCharacterA(hSet, PrintfBuff, strlen(PrintfBuff), coord, &bytes);



	coord.Y += 2;
	memset(PrintfBuff, 0, sizeof PrintfBuff);
	strcpy(PrintfBuff, "8���5s");
	WriteConsoleOutputCharacterA(hSet, PrintfBuff, strlen(PrintfBuff), coord, &bytes);


	coord.Y += 2;
	memset(PrintfBuff, 0, sizeof PrintfBuff);
	strcpy(PrintfBuff, "9����5s");
	WriteConsoleOutputCharacterA(hSet, PrintfBuff, strlen(PrintfBuff), coord, &bytes);



	coord.Y += 2;
	memset(PrintfBuff, 0, sizeof PrintfBuff);
	strcpy(PrintfBuff, "0�������˵�");
	WriteConsoleOutputCharacterA(hSet, PrintfBuff, strlen(PrintfBuff), coord, &bytes);


	memset(PrintfBuff, 0, sizeof PrintfBuff);
	//sprintf(PrintfBuff, "����������ָ�ע������ָ���Ƿ���Ч��");
	coord.X = 1;
	coord.Y = 27;
	//WriteConsoleOutputCharacterA(hSet, PrintfBuff, LENGTH, coord, &bytes);
	SetConsoleActiveScreenBuffer(hSet);



	BuffSwitch = !BuffSwitch;
	hSet = hOut[BuffSwitch];
}



//���ù��λ��
static void  SetPos(int  x, int  y)
{

	COORD  point = { x ,  y };                   //���Ҫ���õ�λ��x,y

	SetConsoleCursorPosition(handle_out, point); //���ù��λ��
}

//��ʾ����
void note(int x, int y)
{
	SetPos(x, y++); printf("    ******************************");
	SetPos(x, y++); printf("    ******************************");
	SetPos(x, y++); printf("    ***                        ***");
	SetPos(x, y++); printf("    ***                        ***");
	SetPos(x, y++); printf("    ******************************");
	SetPos(x, y++); printf("    ******************************");
	SetPos(x, y++); printf("    ***                        ***");
	SetPos(x, y++); printf("    ***                        ***");
	SetPos(x, y++); printf("    ***                        ***");
	SetPos(x, y++); printf("    ***                        ***");
	SetPos(x, y++); printf("    ***                        ***");
	SetPos(x, y++); printf("    ***                        ***");
	SetPos(x, y++); printf("    ***                        ***");
	SetPos(x, y++); printf("   ****                        ***");
	SetPos(x, y++); printf("  *****                        ***");
	SetPos(x, y++); printf(" ******                      *****");
	SetPos(x, y++); printf(" ****                       ******");
	SetPos(x, y++); printf("                             ****");
}

//��ӡ���˵�
void PrintMainMenu()
{
	note(45, 1);
	SetPos(0, 20);

	printf("***********************************************************************************************************************\n");
	printf("*                                                                                                                     *\n");
	printf("*                                                          ���˵�                                                     *\n");
	printf("*                                                                                                                     *\n");
	printf("*                                                        1.���и���                                                   *\n");
	printf("*                                                                                                                     *\n");
	printf("*                                                        2.�ҵĸ赥                                                   *\n");
	printf("*                                                                                                                     *\n");
	printf("*                                                        3.���ڲ���                                                   *\n");
	printf("*                                                                                                                     *\n");
	printf("*                                                        4.����ģʽ                                                   *\n");
	printf("*                                                                                                                     *\n");
	printf("*                                                        0.�˳�                                                       *\n");
	printf("*                                                                                                                     *\n");
	printf("***********************************************************************************************************************");
	printf("\n");
}

//��������
int AllMusic(void)
{
	system("cls");
	ListFlag = 0;
	note(45, 1);

	SetPos(0, 20);

	printf("***********************************************************************************************************************\n");

	for (int i = 1; i <= MusicNumber * 2 + 3; i++)
		printf("*                                                                                                                     *\n");

	printf("***********************************************************************************************************************\n");

	SetPos(58, 22);
	printf("���и���");
	SetPos(57, 24);
	int m = 24;

	for (int i = 1; i <= MusicNumber; i++)
	{
		printf("%d ", i);
		char temps[100] = { NULL };
		strncpy(temps, alls[i], strlen(alls[i]) - 4);
		printf("%s", temps);
		m += 2;
		SetPos(57, m);
	}
	log_record(INFO, "LINE:  %d   print all songs", __LINE__ - 2);

	printf("\n1ѡ�񲥷Ÿ��� 2�������˵�\n");

	int tempFlag = -1;
	do
	{
		printf("����������ָ�ע������ָ���Ƿ���Ч��\n");
		while (1 != scanf("%d", &tempFlag))
		{
			printf("���������ʽ��������������\n");
			log_record(ERROR_, "LINE:  %d   input format error", __LINE__ - 3);
			while (getchar() != '\n');                 //��ȡ���л��������������ʽ�������س�
		}
	} while (tempFlag < 1 || tempFlag > 2);

	if (1 == tempFlag)                                 //ѡ�񲥷Ÿ������Ӿ���id����
	{
		printf("���������id\n");
		scanf("%d", &IDInput);
		strcpy(BroName, alls[IDInput]);
		Play();
		log_record(INFO, "LINE:  %d   play music named  %s  ", __LINE__ - 1, BroName);
		log_record(INFO, "LINE:  %d   read lyric named  %s  ", __LINE__ - 2, BroName);
		return IDInput;
	}
	else return -1;
}

//�ҵĸ赥
int MyMusic(void)
{
	SetPos(58, 22);
	printf("�ҵĸ赥");
	SetPos(57, 24);
	printf("1ѡ��赥                 ");
	SetPos(57, 26);
	printf("2�����赥                   ");
	SetPos(57, 28);
	printf("3ɾ���赥          ");
	SetPos(57, 30);
	printf("0�������˵�");
	SetPos(57, 32);
	printf("          ");
	printf("\n\n\n");
	int temp = -1;
	do
	{
		printf("����������ָ�ע������ָ���Ƿ���Ч��\n");
		while (1 != scanf("%d", &temp))
		{
			printf("���������ʽ��������������\n");
			log_record(ERROR_, "input format error", __LINE__ - 3);
			while (getchar() != '\n');                 //��ȡ���л��������������ʽ�������س�
		}
	} while (temp < 0 || temp > 3);

	int m = 22;

	switch (temp)
	{
	case 1:
		system("cls");
		note(45, 1);
		SetPos(0, 20);

		printf("***********************************************************************************************************************\n");

		for (int i = 1; i <= ListNumber * 2 + 3; i++)
			printf("*                                                                                                                     *\n");

		printf("***********************************************************************************************************************\n");

		SetPos(57, 22);

		for (int i = 0; i <= ListNumber; i++)
		{
			if (!i)
			{
				SetPos(58, 22);
				printf("�赥����");  //
				SetPos(57, 24);
				m += 2;
				continue;
			}
			printf("%d ", i);
			char templ[100] = { NULL };
			strncpy(templ, listname[i], strlen(listname[i]) - 4);
			printf("%s", templ);

			m += 2;
			SetPos(57, m);

		}
		m += 2;
		SetPos(0, m);
		if (!ListNumber) printf("δ�����赥��\n");
		else
		{
			printf("������赥���\n");
			scanf("%d", &ListFlag);
			log_record(INFO, "LINE:  %d   open  %d  list sucessfully", __LINE__ - 1, ListFlag);
		}
		break;
	case 2:
		printf("������赥����\n");
		scanf("%s", listname[++ListNumber]);
		log_record(INFO, "LINE:  %d   create new list named %s", __LINE__ - 1, listname[ListNumber]);
		strcat(listname[ListNumber], ".txt");
		SaveList(ListNumber);
		log_record(INFO, "LINE:  %d   save list numbered %d  and  named %s sucessfully", __LINE__ - 1, ListNumber, listname[ListNumber]);
		printf("�����ɹ��������������˵�");
		Sleep(2000);
		return -1;
		break;
	case 3:
		system("cls");
		note(45, 1);
		SetPos(0, 20);

		printf("***********************************************************************************************************************\n");

		for (int i = 1; i <= ListNumber * 2 + 3; i++)
			printf("*                                                                                                                     *\n");

		printf("***********************************************************************************************************************\n");

		SetPos(57, 22);

		for (int i = 0; i <= ListNumber; i++)
		{
			if (!i)
			{
				SetPos(58, 22);
				printf("�赥����");  //
				SetPos(57, 24);
				m += 2;
				continue;
			}
			printf("%d ", i);
			char templ[100] = { NULL };
			strncpy(templ, listname[i], strlen(listname[i]) - 4);
			printf("%s", templ);

			m += 2;
			SetPos(57, m);

		}
		m += 2;
		SetPos(0, m);
		printf("�������ɾ���ĸ赥��ţ�\n");
		int deletelist;
		scanf("%d", &deletelist);

		if (!DeleteList(deletelist))
		{
			log_record(INFO, "LINE:  %d   list %d had been removed!", __LINE__ - 2, deletelist);

			printf("ɾ���赥�ɹ���\n");
			printf("�������ز˵�\n");
			Sleep(1000);
		}
		else
		{
			log_record(ERROR_, "LINE:  %d   list %d removed failed!", __LINE__ - 10, deletelist);

			printf("ɾ���赥ʧ�ܣ�\n");
			printf("�������ز˵�\n");
			Sleep(1000);
		}
		return -1;
		break;
	case 0:
		return -1;
	}

	if (!ListNumber) return -1;
MARK2:	PrintList(ListFlag);
	log_record(INFO, "LINE:  %d   print  %d  list named %s", __LINE__ - 1, ListFlag, listname[ListFlag]);

	printf("1ѡ�񲥷Ÿ��� 2�Ӹ赥ɾ������ 0�������˵�\n");
	int tempFlag = -1;
	do
	{
		printf("����������ָ�ע������ָ���Ƿ���Ч��\n");
		while (1 != scanf("%d", &tempFlag))
		{
			printf("���������ʽ��������������\n");
			log_record(ERROR_, "LINE:  %d   input format error", __LINE__ - 3);
			while (getchar() != '\n');                 //��ȡ���л��������������ʽ�������س�
		}
	} while (tempFlag < 0 || tempFlag > 2);


	if (1 == tempFlag)
	{
		if (!ListSize[ListFlag])
		{
			printf("�赥Ϊ�գ������������˵�");
			Sleep(1000);
			return -1;
		}

		do {
			printf("���������id��ע�����id�Ƿ���Ч��\n");
			while (1 != scanf("%d", &IDInput))
			{
				printf("�������id��������������\n");
				log_record(ERROR_, "LINE:  %d   input format error", __LINE__ - 3);
				while (getchar() != '\n');                 //��ȡ���л��������������ʽ�������س�
			}
		} while (IDInput < 1 || IDInput > ListSize[ListFlag]);              //�û�����Ϸ�

		strcpy(BroName, GetName(IDInput, ListFlag));
		Play();
		log_record(INFO, "LINE:  %d   play music named  %s  ", __LINE__ - 1, BroName);
		log_record(INFO, "LINE:  %d   read lyric named  %s  ", __LINE__ - 2, BroName);

		return IDInput;
	}

	else if (2 == tempFlag)
	{
		int MusicNum = -1;
		do {
			printf("�������ɾ���ĸ���id��ע�����id�Ƿ���Ч��\n");
			while (1 != scanf("%d", &MusicNum))
			{
				printf("�������id��������������\n");
				log_record(ERROR_, "LINE:  %d   input format error", __LINE__ - 3);
				while (getchar() != '\n');                 //��ȡ���л��������������ʽ�������س�
			}
		} while (MusicNum < 1 || MusicNum > ListSize[ListFlag]);                               // Ĭ���û�����Ϸ�
		deleteone(MusicNum, ListFlag);
		printf("ɾ���ɹ�\n");
		system("cls");
		SaveList(ListFlag);
		goto MARK2;
		return -1;

	}
	else return -1;

}

//��ʾ���Ž��湦��
void PrintDisPlayMenu(char* PreSong, char* NextSong)
{
	SetPos(0, 1), printf("����:");

	SetPos(5, 1), printf("1��ͣ\\����");

	SetPos(5, 3), printf("2��һ�ף�%s)", PreSong);

	SetPos(5, 5), printf("3��һ�ף�%s)", NextSong);

	SetPos(5, 7), printf("4ֹͣ");

	SetPos(5, 9), printf("5��ӵ��ҵĸ赥");

	SetPos(5, 11), printf("6��������");

	SetPos(5, 13), printf("7��С����");

	SetPos(5, 15), printf("8���5s");

	SetPos(5, 17), printf("9����5s");

	SetPos(5, 19), printf("0�������˵�");

	SetPos(5, 21), printf("����������ָ�ע������ָ���Ƿ���Ч��");

}

//���Ž���
void Display()
{

	int music_alltime = 0;
	char time_all[100];
	char time_cur[100];


	mciSendString("status mymusic length", time_all, 100, NULL);
	music_alltime = atoi(time_all);         //int��ʱ 6λ��
	music_alltime /= 1000;                  //������
	while (!kbhit())
	{
		mciSendString("status mymusic position", time_cur, 100, NULL);
		int music_timeing = atoi(time_cur);
		int music_timeing_ms = music_timeing;
		music_timeing /= 1000;
		//��һ�׸��������һ�׸����
		char PreSong[100] = { NULL }, NextSong[100] = { NULL };

		strncpy(PreSong, GetPrev(), strlen(GetPrev()) - 4);
		strncpy(NextSong, GetNext(), strlen(GetNext()) - 4);
		DoubleBuffSet(music_timeing_ms, music_alltime, music_timeing, PreSong, NextSong);

		Sleep(50);  // �ӳ�50���룬��CPU��Ϣһ��

		//��ϲ���ģʽ��ʱ��
		if (music_timeing == music_alltime - 2)
		{
			char CtrlCmd[CTRLCMD_SIZE];
			char pathindex[100] = "C://CloudMusic//";
			switch (ModeFlag)       //�ڵ�ǰ�赥����
			{
			case 1://˳�򲥷�
				strcpy(BroName, NextSong);
				mciSendString("close mymusic", NULL, 0, NULL);
				strcat(pathindex, BroName);
				sprintf(CtrlCmd, "open \"%s\" alias mymusic", pathindex);
				mciSendString(CtrlCmd, NULL, 0, NULL);
				mciSendString("setaudio mymusic volume to 500", NULL, 0, NULL);
				mciSendString("play mymusic", NULL, 0, NULL);
				mciSendString("status mymusic length", time_all, 100, NULL);

				if (IDInput == ListSize[ListFlag]) IDInput = 1;
				else IDInput++;

				Readlrc();
				break;
			case 2://����ѭ��
				mciSendString("seek mymusic to 0", NULL, 0, NULL);
				mciSendString("play mymusic", NULL, 0, NULL);
				break;
			case 3://�������
				IDInput = rand() % ListSize[ListFlag] + 1;
				strcpy(BroName, GetName(IDInput, ListFlag));
				mciSendString("close mymusic", NULL, 0, NULL);
				strcat(pathindex, BroName);
				sprintf(CtrlCmd, "open \"%s\" alias mymusic", pathindex);
				mciSendString(CtrlCmd, NULL, 0, NULL);
				mciSendString("setaudio mymusic volume to 500", NULL, 0, NULL);
				mciSendString("play mymusic", NULL, 0, NULL);
				Readlrc();
				mciSendString("status mymusic length", time_all, 100, NULL);

				break;
			}
		}

	}


	SetConsoleActiveScreenBuffer(handle_out);               //�л�ϵͳĬ�ϻ�����

}





char* Getprogress(int music_alltime, int music_timeing_ms) //all�ĵ�λ���룬ms��λ�Ǻ���
{
	char proline[101] = { NULL };
	int i = 0;
	for (; i < 100; i++)
	{
		if (music_timeing_ms >= music_alltime * 10 * i) proline[i] = '-';
		else break;
	}

	proline[i - 1] = '>';

	for (; i < 100; i++) proline[i] = '-';

	return proline;

}

char* Getvolumeline(int vol)
{
	char volline[13] = { NULL };
	int v = (vol + 20) / 100;
	int i = 0;

	for (; i < v; i++)  volline[i] = '-';
	for (; i < 10; i++) volline[i] = ' ';
	volline[10] = volline[11] = '|';

	volline[12] = '\0';

	return volline;

}

bool exist(const char* path)
{
	char pathindex[100] = { NULL };
	strcpy(pathindex, _PATH);

	FILE* fp;
	strcat(pathindex, path);
	if ((fp = fopen(pathindex, "r")) == NULL)
		return false;

	fclose(fp);
	return true;
}

void addone(const char* name, int ListFlag)
{
	LinkNode node = (LinkNode)malloc(sizeof(MusicNode));
	strcpy(node->name, name);
	node->next = NULL;

	if (head[ListFlag] == NULL)
	{
		node->ReId = 1;
		head[ListFlag] = node;
	}
	else
	{
		LinkNode temp = head[ListFlag];
		while (temp->next != NULL) temp = temp->next;

		node->ReId = temp->ReId + 1;
		temp->next = node;
	}

	ListSize[ListFlag] = node->ReId;

}

void initialalls()
{
	char tempindex[100] = { NULL };
	strcpy(tempindex, _PATH);
	strcat(tempindex, "*.mp3");

	const char* SreachAddr = tempindex;
	long Handle;
	struct _finddata_t FileInfo;
	Handle = _findfirst(SreachAddr, &FileInfo);
	if (-1 == Handle)
		return;

	addalls(FileInfo.name);
	while (!_findnext(Handle, &FileInfo)) addalls(FileInfo.name);

	_findclose(Handle);
}

char* GetName(int ReId, int ListFlag)
{
	if (!ListFlag) return alls[ReId];

	if (ReId == 1) return head[ListFlag]->name;

	LinkNode temp = head[ListFlag];

	while (temp->ReId < ReId) temp = temp->next;

	return temp->name;
}

void PrintList(int ListFlag)
{
	system("cls");

	note(45, 1);

	SetPos(0, 20);

	printf("***********************************************************************************************************************\n");

	for (int i = 1; i <= ListSize[ListFlag] * 2 + 3; i++)
		printf("*                                                                                                                     *\n");

	printf("***********************************************************************************************************************\n");

	SetPos(57, 22);
	int m = 22;

	LinkNode temp = head[ListFlag];



	for (int i = 0; i <= ListSize[ListFlag]; i++)
	{
		if (!i)
		{
			SetPos(58, 22);
			char templ[100] = { NULL };
			strncpy(templ, listname[ListFlag], strlen(listname[ListFlag]) - 4);
			printf("%s", templ);
			SetPos(57, 24);
			m += 2;
			continue;
		}
		char temps[100] = { NULL };
		strncpy(temps, temp->name, strlen(temp->name) - 4);
		printf("%d  %s", i, temps);
		m += 2;
		SetPos(57, m);

		temp = temp->next;
	}
	//printf("%s\n", listname[ListFlag]);

	//�赥Ϊ��

	puts("");
}

void SaveList(int ListFlag)
{
	char pathindex[1000] = { NULL };
	strcpy(pathindex, _PATH);

	strcat(pathindex, listname[ListFlag]);

	FILE* fp;
	if ((fp = fopen(pathindex, "w+")) == NULL)
	{
		printf("cannot open file!\n");
		return;
	}

	LinkNode temp = head[ListFlag];
	while (temp != NULL)
	{
		//if (fprintf(fp, "%s\n", temp->name) != strlen(temp->name)) printf("Save ERROR!\n"); ����ֵ = strlen(temp->name)+1��
		fprintf(fp, "%s\n", temp->name);
		temp = temp->next;
	}

	printf("���¸赥��ϣ�\n");

	fclose(fp);
}

void addlist(int n, const char* txtname)
{
	strcpy(listname[n], txtname);

	FILE* fp;

	char compath[100] = { NULL };
	strcpy(compath, _PATH);

	strcat(compath, txtname);
	if ((fp = fopen(compath, "r")) == NULL)
	{
		printf("cannot open file!\n");
		return;
	}

	printf("��ȡ%30s�ļ��ɹ�\n", txtname);
	char tempname[100] = { NULL };
	while (fgets(tempname, 100, fp) != NULL)
	{
		int len = strlen(tempname);
		tempname[len - 1] = '\0'; //ȥ�����з�
		if (exist(tempname)) addone(tempname, n);
	}

	fclose(fp);
}

void initiallist()
{
	char tempindex[100] = { NULL };
	strcpy(tempindex, _PATH);
	strcat(tempindex, "*.txt");

	const char* SreachAddr = tempindex;
	long Handle;
	struct _finddata_t FileInfo;
	Handle = _findfirst(SreachAddr, &FileInfo);
	if (-1 == Handle)
		return;

	addlist(++ListNumber, FileInfo.name); //FileInfo.nameΪ����׺�����·����
	while (!_findnext(Handle, &FileInfo)) addlist(++ListNumber, FileInfo.name);

	_findclose(Handle);
}

void deleteone(int n, int ListFlag)
{
	LinkNode temp = head[ListFlag];

	if (n <= 0)
	{
		printf("���������������ţ�\n");
		return;
	}
	if (!temp)
	{
		char templ[100] = { NULL };
		strncpy(templ, listname[ListFlag], strlen(listname[ListFlag]) - 4);
		printf("%sû�и�����\n", templ);
		return;
	}
	if (n == 1)
	{
		head[ListFlag] = head[ListFlag]->next;
		while (temp)
		{
			temp->ReId--;
			temp = temp->next;
		}

		ListSize[ListFlag]--;
		return;
	}
	while ((temp->next)->ReId != n && (temp->next)->next != NULL) temp = temp->next;

	if ((temp->next)->ReId < n)
	{
		char templ[100] = { NULL };
		strncpy(templ, listname[ListFlag], strlen(listname[ListFlag]) - 4);
		printf("%sû�иø�����\n", templ);
		return;
	}
	temp->next = (temp->next)->next;
	temp = temp->next;

	while (temp)
	{
		temp->ReId--;
		temp = temp->next;
	}

	ListSize[ListFlag]--;
}

char* GetPrev()
{
	if (!ListFlag)
	{
		if (IDInput == 1) return  alls[MusicNumber];
		else return alls[IDInput - 1];
	}
	LinkNode temp = head[ListFlag];

	if (IDInput == 1)  // �����˸赥����һ�׸�����
	{
		while (temp->next != NULL) temp = temp->next;
		return temp->name;
	}

	while (temp->ReId != IDInput - 1) temp = temp->next;

	return  temp->name;
}

char* GetNext()
{
	if (!ListFlag)
	{
		if (IDInput == MusicNumber) return  alls[1];
		else return alls[IDInput + 1];
	}

	if (IDInput == ListSize[ListFlag]) return head[ListFlag]->name;

	LinkNode temp = head[ListFlag];

	while (temp->ReId != IDInput + 1) temp = temp->next;

	return temp->name;

}

void Readlrc()
{
	LyricShowFlag = 0;

	char* tlyric[100] = { NULL };

	FILE* fp;
	char path[100] = "C://CloudMusic//";
	char pathtemp[100] = { NULL };

	strncpy(pathtemp, BroName, strlen(BroName) - 3);
	strcat(path, pathtemp);
	strcat(path, "lrc");

	if ((fp = fopen(path, "r")) == NULL)
	{
		log_record(WARNING, "LINE:  %d   lyric named  %s  read failed", __LINE__ - 2, BroName);
		return;
	}

	LyricShowFlag = 1;
	fseek(fp, 0, 2);
	int size = ftell(fp);
	char* str = (char*)calloc(1, size + 1);
	rewind(fp);
	fread(str, 1, size, fp);

	LinesNumber = 1;

	tlyric[LinesNumber] = strtok(str, "\r\n");
	while (tlyric[LinesNumber] != NULL)  tlyric[++LinesNumber] = strtok(NULL, "\r\n");
	LinesNumber--;

	fclose(fp);

	for (int i = 5; i <= LinesNumber; i++)
	{
		char s[100] = { NULL };
		int mins[2], secs[4];
		int min, sec;
		strncpy(s, tlyric[i] + 10, strlen(tlyric[i]) - 10);
		s[strlen(tlyric[i]) - 10] = '\0';
		mins[0] = tlyric[i][1] - '0';
		mins[1] = tlyric[i][2] - '0';
		secs[0] = tlyric[i][4] - '0';
		secs[1] = tlyric[i][5] - '0';
		secs[2] = tlyric[i][7] - '0';
		secs[3] = tlyric[i][8] - '0';
		min = mins[0] * 600000 + mins[1] * 60000;
		sec = secs[0] * 10000 + secs[1] * 1000 + secs[2] * 100 + secs[3] * 10;
		LRC[i - 4].time = min + sec;
		if (s)  strcpy(LRC[i - 4].lyric, s);
	}
}

int DeleteList(int n)// head listname ListSize ListNumber
{
	char deletepath[100] = { NULL };
	strcpy(deletepath, _PATH);
	strcat(deletepath, listname[n]);

	for (int i = n; i < ListNumber; i++)
	{
		head[i] = head[i + 1];
		strcpy(listname[i], listname[i + 1]);
		ListSize[i] = ListSize[i + 1];
	}

	head[ListNumber] = NULL;
	memset(listname[ListNumber], 0, sizeof listname[ListNumber]);
	ListSize[ListNumber] = 0;
	ListNumber--;

	if (remove(deletepath) == NULL) return 0;
	else return -1;

}





