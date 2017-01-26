/*************************************************************************
  > File Name: Client.c
  > Author: SongLee
 ************************************************************************/

#include<netinet/in.h>  // sockaddr_in 
#include<sys/types.h>  // socket 
#include<sys/socket.h>  // socket 
#include<stdio.h>    // printf 
#include<stdlib.h>    // exit 
#include<string.h>    // bzero 
#include <sys/stat.h>
#define SERVER_PORT 8000
#define BUFFER_SIZE 1024
#define FILE_NAME_MAX_SIZE 512

unsigned long get_file_size(const char *path)  
{  
    unsigned long filesize = -1;      
    struct stat statbuff;  
    if(stat(path, &statbuff) < 0){  
        return filesize;  
    }else{  
        filesize = statbuff.st_size;  
    }  
    return filesize;  
} 

int main()
{
    // ��������ʼ��һ���ͻ��˵�socket��ַ�ṹ
    struct sockaddr_in client_addr;
    bzero(&client_addr, sizeof(client_addr));
    client_addr.sin_family = AF_INET;
    client_addr.sin_addr.s_addr = htons(INADDR_ANY);
    client_addr.sin_port = htons(0);

    // ����socket�����ɹ�������socket������
    int client_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(client_socket_fd < 0)
    {
        perror("Create Socket Failed:");
        exit(1);
    }

    // �󶨿ͻ��˵�socket�Ϳͻ��˵�socket��ַ�ṹ �Ǳ���
    if(-1 == (bind(client_socket_fd, (struct sockaddr*)&client_addr, sizeof(client_addr))))
    {
        perror("Client Bind Failed:");
        exit(1);
    }

    // ����һ���������˵�socket��ַ�ṹ�����÷������Ǳߵ�IP��ַ���˿ڶ�����г�ʼ�������ں��������
    struct sockaddr_in server_addr;
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    if(inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr) == 0)
    {
        perror("Server IP Address Error:");
        exit(1);
    }
    server_addr.sin_port = htons(SERVER_PORT);
    socklen_t server_addr_length = sizeof(server_addr);

    // ��������������ӣ����ӳɹ���client_socket_fd�����˿ͻ��˺ͷ�������һ��socket����
    if(connect(client_socket_fd, (struct sockaddr*)&server_addr, server_addr_length) < 0)
    {
        perror("Can Not Connect To Server IP:");
        exit(0);
    }

    // �����ļ��� ���ŵ�������buffer�еȴ�����
    char file_name[FILE_NAME_MAX_SIZE+1];
    bzero(file_name, FILE_NAME_MAX_SIZE+1);
    printf("Please Input File Name Send to Server:\t");
    scanf("%s", file_name);

    char buffer[BUFFER_SIZE];
    bzero(buffer, BUFFER_SIZE);
    strncpy(buffer, file_name, strlen(file_name)>BUFFER_SIZE?BUFFER_SIZE:strlen(file_name));

    // �����������buffer�е�����,�ļ�����
    if(send(client_socket_fd, buffer, BUFFER_SIZE, 0) < 0)
    {
        perror("Send File Name Failed:");
        exit(1);
    }

    // ���ļ���׼��д��
    FILE *fp = fopen(file_name, "r");
    if(NULL == fp)
    {
        printf("File:\t%s Can Not Open To Write\n", file_name);
        exit(1);
    }

    // �ӷ������������ݵ�buffer��
    // ÿ����һ�����ݣ��㽫��д���ļ��У�ѭ��ֱ���ļ������겢д��Ϊֹ
    bzero(buffer, BUFFER_SIZE);
    int length = 0;
	int m_nFileSize=get_file_size(file_name);
	int m_nSendSize=0;;
	float m_fPersent;
	printf("m_nFileSize=%d bytes \n",m_nFileSize);
    while((length = fread(buffer, sizeof(char), BUFFER_SIZE, fp)) > 0)
    {
        if(send(client_socket_fd, buffer, length, 0) < 0)
        {
            printf("Send File:%s Failed./n", file_name);
            break;
        }
        bzero(buffer, BUFFER_SIZE);
        m_nSendSize+=length;
        m_fPersent=(float)(m_nSendSize*100/m_nFileSize);
		printf("\r\033 %.2f%% ",m_fPersent);
    }
    // ���ճɹ��󣬹ر��ļ����ر�socket
    printf("\nSend File:\t %s Successful!\n", file_name);
    close(fp);
    close(client_socket_fd);
    return 0;
}

