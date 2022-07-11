/*

2021/07/31   14:07





*/


#include "MusicPlayer8.0.h"
#include <stdio.h>
#include <io.h>
#include <Windows.h>
#include <string.h>
#include <stdlib.h>
#include <string>
#include <mmsystem.h>
#include <time.h>

#pragma comment(lib,"winmm.lib")
COORD size = { 120,60 };



int main()
{
	system("color 0B");
	SetConsoleScreenBufferSize(handle_out, size);

	printf("��������������ڵ��ļ���Ŀ¼��    (�ļ�·���ָ��������//�� ����C://CloudMusic//�� \n");
	scanf("%s", _PATH);
	//strcpy(_PATH, "C://CloudMusic//");


	initialalls();                                         //��ȡ���⣬ɨ������mp3�ļ�
	ListSize[0] = MusicNumber;
	log_record(INFO, "LINE:  %d   path:  initial all songs in the form of .mp3 suffix", __LINE__ - 2, _PATH);

	printf("���ڳ�ʼ��������͸赥\n");
	initiallist();                                         //��ʼ���赥��������������Ϣ
	log_record(INFO, "LINE:  %d   path:  initial all lists in the form of .txt suffix", __LINE__ - 1, _PATH);

	DoubleBuffInitial();                                   //��ʼ��˫����

	for (int i = 1; i <= ListNumber; i++) SaveList(i);     //���������ĸ赥�ļ�


	log_record(INFO, "LINE:  %d   initial and update songs stored locally and listfiles", __LINE__ - 2);


	srand((unsigned)time(NULL));                           //�������������

	int iFlag = 1;                                         //�˳�����ı�־
	int iSignal = -1;                                      //�û������˵������ָ����
	while (iFlag)
	{
		system("cls");
		PrintMainMenu();

		do
		{
			printf("����������ָ�ע������ָ���Ƿ���Ч��\n");
			while (1 != scanf("%d", &iSignal))
			{
				printf("���룬�����ʽ��������������\n");
				log_record(ERROR_, "LINE:  %d   input format error", __LINE__ - 3);
				while (getchar() != '\n');                 //��ȡ���л��������������ʽ�������س�
			}
		} while (iSignal < 0 || iSignal > 4);

		switch (iSignal)
		{
		case 1:                         //���и���
			BackFlag = AllMusic();
			log_record(INFO, "LINE:  %d   move to all songs menu", __LINE__ - 1);
			break;
		case 2:                         //�ҵĸ赥 
			BackFlag = MyMusic();
			log_record(INFO, "LINE:  %d   move to my playlist menu", __LINE__ - 1);
			break;
		case 3:                         //���ڲ���
			if (Mci_Flag == 1)
			{
				BackFlag = 1;
				goto MARK1;
			}
			else {
				printf("��ǰδ���Ÿ����������������˵�\n");
				Sleep(3000);
			}
			BackFlag = -1;
			break;
		case 4:                         //����ģʽ
			note(45, 1);
			SetPos(0, 20);
			printf("***********************************************************************************************************************\n");
			printf("*                                                                                                                     *\n");
			printf("*                                                          ����ģʽ                                                   *\n");
			printf("*                                                                                                                     *\n");
			printf("*                                                        1.˳�򲥷�                                                   *\n");
			printf("*                                                                                                                     *\n");
			printf("*                                                        2.����ѭ��                                                   *\n");
			printf("*                                                                                                                     *\n");
			printf("*                                                        3.�������                                                   *\n");
			printf("*                                                                                                                     *\n");
			printf("*                                                        ��ǰ����ģʽΪ:  %s                                    *\n", Mode[ModeFlag]);
			printf("*                                                                                                                     *\n");
			printf("*                                                                                                                     *\n");
			printf("*                                                                                                                     *\n");
			printf("***********************************************************************************************************************");
			printf("\n");

			do
			{
				printf("����������ĵ�ģʽ��ע������ָ���Ƿ���Ч��\n");
				while (1 != scanf("%d", &ModeFlag))
				{
					printf("���룬�����ʽ��������������\n");
					log_record(ERROR_, "LINE:  %d   input format error", __LINE__ - 3);
					while (getchar() != '\n');         //��ȡ���л��������������ʽ�������س�
				}
			} while (ModeFlag < 0 || ModeFlag > 3);
			//����ModeFlag֮���൱�ڸ����˲���ģʽ

			break;
		case 0:                         //�û�ѡ���˳�����
			iFlag = -1;
			log_record(INFO, "LINE:  %d   exit mp3 program sucessfully", __LINE__ - 1);
			exit(0);
			break;
		default:                        //�����������
			system("cls");
			printf("�����������!��������\n");
			break;
		}

		if (BackFlag == -1) log_record(INFO, "LINE:  %d   return to mainmenu", 54);

		while (BackFlag != -1)
		{

		MARK1:Display();                                   //���벥�Ž���

			system("cls");
			char PreSong[100] = { NULL }, NextSong[100] = { NULL };
			strncpy(PreSong, GetPrev(), strlen(GetPrev()) - 4);
			strncpy(NextSong, GetNext(), strlen(GetNext()) - 4);
			PrintDisPlayMenu(PreSong, NextSong);
			SetPos(5, 23);
			printf("��������\n");

			SetPos(5, 24);

			int fFlag;

			do
			{
				while (1 != scanf("%d", &fFlag))
				{
					printf("���������ʽ��������������\n");
					log_record(ERROR_, "LINE:  %d   input format error", __LINE__ - 3);
					while (getchar() != '\n');             //��ȡ���л��������������ʽ�������س�
				}
			} while (fFlag < 0 || fFlag > 9);

			switch (fFlag)
			{
			case 0:
				BackFlag = -1;
				break;
			case 1:
				MusicPauseBroadcast();                     //������ͣ
				break;
			case 2:
				BackFlag = IDInput;
				strcpy(BroName, GetPrev());

				if (IDInput != 1) --IDInput;
				else IDInput = ListSize[ListFlag];         //��һ�׵���һ�������һ��

				Play();
				Readlrc();
				log_record(INFO, "LINE:  %d   play music named  %s  ", __LINE__ - 2, BroName);
				log_record(INFO, "LINE:  %d   read lyric named  %s  ", __LINE__ - 2, BroName);
				Display();
				break;
			case 3:
				BackFlag = IDInput;
				strcpy(BroName, GetNext());

				if (IDInput != ListSize[ListFlag]) ++IDInput;
				else IDInput = 1;                          //���һ�׵���һ���ǵ�һ��

				Play();
				Readlrc();
				log_record(INFO, "LINE:  %d   play music named  %s  ", __LINE__ - 2, BroName);
				log_record(INFO, "LINE:  %d   read lyric named  %s  ", __LINE__ - 2, BroName);
				Display();
				break;
			case 4:                                        //ֹͣ����
				MusicStop();
				MusicClose();
				log_record(INFO, "LINE:  %d   music close", __LINE__ - 2);
				break;
			case 5:                                        //��ӵ��ҵĸ赥
				for (int i = 1; i <= ListNumber; i++)      //��ʾ��ǰ���и赥
				{
					char templ[100] = { NULL };
					strncpy(templ, listname[i], strlen(listname[i]) - 4);
					printf("%d  %s\n", i, templ);
				}

				printf("�������ӵĸ赥���\n");

				int templist;
				scanf("%d", &templist);
				while (templist == ListFlag)
				{
					printf("�����Ѵ��ڣ�\n");
					break;
				}

				addone(GetName(IDInput, ListFlag), templist);
				SaveList(templist);                        //��Ӻ����̸��¸赥�ļ�
				printf("�������ز��Ž���\n");
				Sleep(1000);
				log_record(INFO, "LINE:  %d   save list numbered %d  and  named %s sucessfully", __LINE__ - 2, templist, listname[templist]);
				break;
			case 6:                                        //��������
				MusicSetVolumeUp();
				log_record(INFO, "LINE:  %d   music volume up", __LINE__ - 1);

				break;
			case 7:                                        //��С����
				MusicSetVolumeDown();
				log_record(INFO, "LINE:  %d   music volume down", __LINE__ - 1);

				break;
			case 8:                                        //���5s
				MusicFastForward();
				log_record(INFO, "LINE:  %d   music fastforward by 5 seconds", __LINE__ - 1);
				break;
			case 9:                                        //����5s
				MusicFastBackward();
				log_record(INFO, "LINE:  %d   music fastbackward by 5 seconds", __LINE__ - 1);
				break;
			}

		}
	}

	return 0;
}
