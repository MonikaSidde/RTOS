#include <sys/types.h>         
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <stdio.h>
#include <arpa/inet.h> 
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <pulse/simple.h>
#include <pulse/error.h>
#include <pulse/gccmacro.h>

#define BUFSIZE 1024

int main(int argc, char*argv[])
{
	int client_fd, len_c, n, check;
	struct sockaddr_in server;
    uint8_t buf[BUFSIZE];
    int con_check;
    unsigned char *message;

    static const pa_sample_spec ss = {
        .format = PA_SAMPLE_S16LE,
        .rate = 8000,
        .channels = 1
    };
    pa_simple *s = NULL;
    int ret = 1;
    int error;
    client_fd = socket(AF_INET,SOCK_STREAM,0);
    printf("client desc = %d\n",client_fd );
    server.sin_family=AF_INET;
	server.sin_addr.s_addr=inet_addr("172.16.128.193");
	server.sin_port=htons(8000);
	memset(server.sin_zero, '\0', sizeof server.sin_zero);
	if (!(s = pa_simple_new(NULL, argv[0], PA_STREAM_RECORD, NULL, "record", &ss, NULL, NULL, &error))) 
    {
        fprintf(stderr, __FILE__": pa_simple_new() failed: %s\n", pa_strerror(error));
        goto finish;
    }

	con_check = connect(client_fd,(struct sockaddr *)&server, sizeof(server));
	printf("con_check = %d\n",con_check );
	int fd;
    fd = open("text3.txt", O_RDWR | O_CREAT);
    printf("%d\n",fd ); 
    while(1)
    {
    	if (pa_simple_read(s, buf, sizeof(buf), &error) < 0) 
        {
            fprintf(stderr, __FILE__": pa_simple_read() failed: %s\n", pa_strerror(error));
            goto finish;
        }
		write(client_fd,buf,sizeof(buf));
	}
	ret = 0;

finish:

    if (s)
        pa_simple_free(s);

    return ret;
}
