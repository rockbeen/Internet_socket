#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/un.h>
#include <stdio.h>
#include <locale.h>
#define ADDRESS "127.0.0.1" //адрес для связи

// логи /tmp/internet_sockets/internet_sockets_log.log

void main ()
{
    setlocale(LC_ALL, "Rus");
    int  s, len;

    //сообщение
    char* str[256];

    FILE *fp;
    struct sockaddr_in sa;

    //получаем сокет
    if ((s = socket (AF_INET, SOCK_STREAM, 0))<0)
    {
    perror ("Client: socket"); exit(1);
    }
    //заполняем sockaddr_in
    sa.sin_family = AF_INET;
    sa.sin_port=htons(9002);
    sa.sin_addr.s_addr = inet_addr(ADDRESS);

    //пытаемся связаться с сервером
    len =sizeof (sa);
    if ( connect ( s, &sa, len) < 0 ){
            perror ("Client: connect"); exit (1);
    }

    //читаем сообщения полученное от сервера
    fp = fdopen (s, "r");

    if(recv(s, str, sizeof(str), 0) < 0 )
    {
        perror ("Client: message"); exit (1);
    }

    //выводим информацию от сервера
    printf("%5s\n",str);

    //продолжаем диалог с сервером, пока не будет введенно 'esc'
    do{

        scanf("%5s",str);
        send (s, str,  sizeof(str), 0);

    } while(strcmp(str,"esc"));

    //завершаем работы
    close (s);
    exit (0);
}
