#include <sys/types.h>         
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <pulse/simple.h>
#include <pulse/error.h>
#include <pulse/gccmacro.h>

#define BUFSIZE 1024
int main(int argc, char*argv[])
{
	int server_desc, client_desc, val;
	struct sockaddr_in server,client;
  int opt = 1;
  int addrlen = sizeof(client);
  ssize_t r;

  static const pa_sample_spec ss = {
        .format = PA_SAMPLE_S16LE,
        .rate = 8000,
        .channels = 1
    };
  pa_simple *s = NULL;
    int ret = 1;
    int error;
	server_desc = socket(AF_INET,SOCK_STREAM,0);
  printf("Server desc = %d\n",server_desc);
  if (!(s = pa_simple_new(NULL, argv[0], PA_STREAM_PLAYBACK, NULL, "playback", &ss, NULL, NULL, &error))) 
  {
        fprintf(stderr, __FILE__": pa_simple_new() failed: %s\n", pa_strerror(error));
        goto finish;
  }
  if (setsockopt(server_desc, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
  {
    perror("setsockopt");
    exit(EXIT_FAILURE);
  }

  server.sin_family=AF_INET;
	server.sin_addr.s_addr=INADDR_ANY;
	server.sin_port=htons(8000);
  memset(server.sin_zero, '\0', sizeof server.sin_zero);
  int bind_check = bind(server_desc, (struct sockaddr *)&server, sizeof(server));
  printf("Bind check = %d\n",bind_check );
  listen(server_desc, 5);
  int fd;
  fd = open("text2.txt", O_RDWR | O_CREAT);
  printf("%d\n",fd ); 
  client_desc = accept(server_desc, (struct sockaddr *)&client, (socklen_t*)&addrlen);
  while(1)
  {
      unsigned char message[BUFSIZE];
      uint8_t *buf;
      if ((r = read(client_desc, message, sizeof(buf))) <= 0) 
      {
            if (r == 0) /* EOF */
                break;
            fprintf(stderr, __FILE__": read() failed: %s\n", strerror(errno));
            goto finish;
      }
        buf = (uint8_t*)message;
        
  	  if (pa_simple_write(s, buf, (size_t) r, &error) < 0) 
      {
            fprintf(stderr, __FILE__": pa_simple_write() failed: %s\n", pa_strerror(error));
            goto finish;
       }
  }
  if (pa_simple_drain(s, &error) < 0) 
  {
        fprintf(stderr, __FILE__": pa_simple_drain() failed: %s\n", pa_strerror(error));
        goto finish;
  }
    ret = 0;

finish:

    if (s)
        pa_simple_free(s);

    return ret;
}
