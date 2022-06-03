#include <stdio.h>
#include <sys/types.h>
#include <sys/param.h>
#include <time.h>
#include <syslog.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/un.h>
#include <locale.h>
#define ADDRESS "127.0.0.1" //адрес для связи

//логи /tmp/internet_sockets/internet_sockets_log.log

//запись в логи
void logging(FILE *log, char* str[])
{
    time_t now;
    time(&now);
    fprintf(log,"%s Сообщение:\n %10s\n",ctime(&now),str);
}

//сервер
void server ()
{
    int d, d1, len, ca_len;
    FILE *fp;
    FILE *log;
    struct sockaddr_in sa, ca;

    //создаем папку и internet_sockets_log.log в ней
    system("mkdir -p /tmp/internet_sockets");
    char path[] = "/tmp/internet_sockets/internet_sockets_log.log";
    log = fopen(path, "a");

    char* message[256];//сообщение
    strcpy(message,"Server: init");

    logging(log, message);//запись в логи

    //получаем сокет:
    if((d = socket (AF_INET, SOCK_STREAM, 0)) < 0)
    {
     perror ("Client: socket"); exit (1);
    }

    //заполняем sockaddr_in
    sa.sin_family = AF_INET;
    sa.sin_port=htons(9002);
    sa.sin_addr.s_addr =  inet_addr(ADDRESS);

    unlink (ADDRESS);
    len = sizeof (sa);
    if ( bind ( d, &sa, len) < 0 ) {
            perror ("Server: bind"); exit (1);
    }

    //слушаем запросы
    if ( listen ( d, 5) < 0 ) {
            perror ("Server: listen"); exit (1);
    }

    //связываемся с клиентом через неименованный сокет
    if (( d1 = accept ( d, &ca, &ca_len)) < 0 ) {
            perror ("Server: accept"); exit (1);
    }

    //пишем клиенту
    strcpy(message,"Сервер подключен, для выхода ввелите 'esc'");
    send (d1, message,sizeof(message), 0);

    logging(log, message);
    char* str[256];

    while(1)
    {
        if(recv(d1, str, sizeof(str), 0) < 0 )
        {
           perror ("Server: Message"); exit (1);
        }
        logging(log, str);

        if(!strcmp(str,"esc")) {
            break;
        }
    }

    //закрываем все
    close (d1);
    close (log);
    remove (ADDRESS);

    exit (0);
}

int init_daemon(void)
{
    int pid;
    int i;

    signal(SIGTTOU,SIG_IGN);
    signal(SIGTTIN,SIG_IGN);
    signal(SIGTSTP,SIG_IGN);
    signal(SIGHUP,SIG_IGN);

    pid = fork();
    // Завершаем родительский процесс, делая дочерний процесс фоновым процессом
    if(pid > 0) {
            exit(0);
    }
    else if(pid < 0) {
            return -1;
    }


    setsid();

    // Создаем новый дочерний процесс снова
    pid=fork();
    if( pid > 0) {
            exit(0);
    }
    else if( pid< 0) {
            return -1;
    }

    // Закрываем все файловые дескрипторы, унаследованные от родительского процесса, которые больше не нужны
    for(i=0;i< NOFILE;close(i++));

    chdir("/");
    umask(0);
    signal(SIGCHLD,SIG_IGN);

    //запускаем сервер для общения с клиентом
    server();

    return 0;
}


int main()
{
    setlocale(LC_ALL, "Rus");
    init_daemon();
}

