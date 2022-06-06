#include <termios.h>                                                         
#include <stdio.h>
#include <stdlib.h>	
#include <string.h>
#include <unistd.h>                                                          
#include <sys/types.h> 
#include <stdint.h>
#include <fcntl.h>
#include <sys/signal.h>
#include <sys/types.h>
#include <termios.h>
#include <time.h>
#include <stdbool.h>
#include <stropts.h>
#include <poll.h>	
#include <errno.h>	

#include "ic.h"

#define BAUDRATE B115200                                                      
#define MODEMDEVICE "/dev/ttyUSB1"
char *portname = "/dev/ttyXX";
//#define MODEMDEVICE "/dev/ttyACM2"

/*
Punkty na które musisz zwrócić uwagę to
1) w Linuksie urządzenie szeregowe to plik
2) funkcja poll () czeka na dane w linii szeregowej i zwraca z fds [0] .revents -> POLLRDNORM
3) res = read (fd, buf, 255); odczytuje do 255 znaków z bufora szeregowego
4) sscanf (buf, "% d \ n", & zmienna); skanuje łańcuch znaków buf i konwertuje dane na liczbę całkowitą i umieszcza je w zmiennej.
5) Funkcje set_blocking () i set_interface_attribs () zostały zebrane z sieci.

Funkcja fopen() otwiera plik, którego nazwa podana jest w pierwszym argumencie. Drugim jest łańcuch znaków zwierający litery oznaczające sposób otwarcia pliku:
"r" - otwiera plik do czytania
"r+" - otwiera plik do czytania i nadpisywania (aktualizacja)
"w" - otwiera plik do nadpisywania (zamazuje starą treść)
"w+" - otwiera plik do nadpisywania i czytania
"a" - otwiera plik do dopisywania (jeśli plik nie istnieje, to jest tworzony)
"a+" - otwiera plik do dopisywania i odczytu (jeśli plik nie istnieje, to jest tworzony)
"t" - otwiera plik w trybie tekstowym
"b" - otwiera plik w trybie binarnym
Litery można ze sobą łączyć, np. "rwb" albo "wt".
*/

int set_interface_attribs (int fd, int speed, int parity)	{
    struct termios tty;
    memset (&tty, 0, sizeof tty);
    if (tcgetattr (fd, &tty) != 0)
    {
      printf ("error %d from tcgetattr", errno);
      return -1;
    }
      cfsetospeed (&tty, speed);
      cfsetispeed (&tty, speed);
      //printf("speed=%d \n",cfgetospeed(&tty));
      tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;     // 8-bit chars
      // disable IGNBRK for mismatched speed tests; otherwise receive break
      // as \000 chars
      tty.c_iflag &= ~IGNBRK;         // disable break processing
      tty.c_lflag = 0;                // no signaling chars, no echo,
                                      // no canonical processing
      tty.c_oflag = 0;                // no remapping, no delays
      tty.c_cc[VMIN]  = 0;            // read doesn't block
      tty.c_cc[VTIME] = 5;            // 0.5 seconds read timeout

      tty.c_iflag &= ~(IXON | IXOFF | IXANY); // shut off xon/xoff ctrl

      tty.c_cflag |= (CLOCAL | CREAD);// ignore modem controls,
                                        // enable reading
      tty.c_cflag &= ~(PARENB | PARODD);      // shut off parity
      tty.c_cflag |= parity;
      tty.c_cflag &= ~CSTOPB;
      tty.c_cflag &= ~CRTSCTS;

    if (tcsetattr (fd, TCSANOW, &tty) != 0)
    {
      return -1;
    }
    return 0;
	}

void set_blocking (int fd, int should_block)	{
      struct termios tty;
      memset (&tty, 0, sizeof tty);
      if (tcgetattr (fd, &tty) != 0)
      {
        printf("error %d from tggetattr", errno);
        return;
      }
      tty.c_cc[VMIN]  = should_block ? 1 : 0;
      tty.c_cc[VTIME] = 5;            // 0.5 seconds read timeout
      if (tcsetattr (fd, TCSANOW, &tty) != 0)
    printf("error %d setting term attributes", errno);
	}
	
int main(int argc, char *argv[])
  { 
    int fd;                                                             
    char buf[255],fbuf[300 +1];  
    char *pbuf = buf;
    struct pollfd fds[1];
    int ret, res=0, cnt=0, sum=0;
    FILE *fp;
    cnt=cnt;
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    long db[6],time=164594798;
    float dbf[6];

    char myhostname[256 +1];
	
		srandom(getpid());
		//ic_debug(0); /* maximum output */
		//ic_influx_database("localhost", 8086, "testdb");
		//	ic_influx_userpw("nigel", "secret");

		/* Intitalise */

        /* get the local machine hostname */
		//if( gethostname(myhostname, sizeof(myhostname)) == -1) {		perror("gethostname() failed");	}
		//snprintf(fbuf, 300, "host=%s", myhostname);
		//snprintf(fbuf, 300, "zzz=rpi");
		//ic_tags(fbuf);
		//ic_tags("zzz=rpi");


    if ((argc != 2))
    {
      printf("Uzycie: %s /dev/ttyX \n",argv[0]);
      return 0;
    }
    //printf("Port %s plik %s \n",argv[1],argv[2]
    //sscanf(argv[2],"%d",&var);
    		
    /* open the device */
    fd = open(argv[1], O_RDWR | O_NOCTTY);
    //fd = open(argv[1], O_RDWR | O_NOCTTY | O_NONBLOCK);
    //fd = open(portname, O_RDWR | O_NOCTTY | O_SYNC);
    //printf("fd = %d \n",fd);
    //printf("sp = %d \n",var);
    
    /*if (fd == 0)    {
      perror(MODEMDEVICE);
      //printf("Failed to open MODEMDEVICE \" %s \"\n",MODEMDEVICE);
      exit(-1); 
    }*/
    
    if (fd < 0)    {
      printf ("Error %d opening %s: %s \n", errno, portname, strerror (errno));
      return 0;
    }
    
    set_interface_attribs (fd, BAUDRATE, 0);  	// set speed to BAUDRATE  bps, 8n1 (no parity)
    set_blocking (fd, 0);                	// set no blocking

    /* Open STREAMS device. */
    fds[0].fd = fd;
    fds[0].events = POLLRDNORM;
    //fp=fopen("temp.txt","at");
    //write (fd, "hello!\n", 7);        // send 7 character greeting
    //usleep ((7 + 25) * 100);          // sleep enough to transmit the 7 plus
    //				    	// receive 25:  approx 100 uS per char transmit
    //char buf1 [100];
    //int n = read (fd, buf1, sizeof buf1);  // read up to 100 characters if ready to read

		//printf("debug_0 \n");
		
		for (;;)		// forever
    {
      ret = poll(fds, 1, 1000);			// wait for response
      if (ret > 0)   { 
				/* An event on one of the fds has occurred. */
				if (fds[0].revents & POLLHUP)		{
					printf("Hangup\n");
				}
				
				if (fds[0].revents & POLLRDNORM)
				{
					//res = read(fd,buf,sizeof buf);
						do{
						res = read(fd,pbuf,32);
					  pbuf += res;
					  printf("res=%d ",res); 	// gdy odebrano prawidlowa ilosc znakow
					} while (res);
					*pbuf = 0;				// terminate buffer
				  printf("num=%d %s \n",pbuf-buf,buf); 	// gdy odebrano prawidlowa ilosc znakow
					pbuf = buf;
					//printf("res=%d %s",res,buf); 	// gdy odebrano prawidlowa ilosc znakow
					
					if(0){ 				
//					if(res == 40){ 

						sscanf(buf,"%d,%d,%d,%d,%d,%d",&db[0],&db[1],&db[2],&db[3],&db[4],&db[5]);
						// convert string to number
						// printf(" v1=%d v2=%d v3=%d v4=%d v5=%d v6=%d \n",db[0],db[1],db[2],db[3],db[4],db[5]);
						// printf("raw=%d",cnt++);
						// printf(" v1=%.1f v2=%.1f v3=%.1f v4=%.1f v5=%.1f v6=%.1f \n",db[0],db[1],db[2],db[3],db[4],db[5]);
						// for(int i=0;i<6;i++) dbf[i] = 1.00*db[i];
						dbf[0] = 1.0*db[0]/1000;
						dbf[1] = 1.0*db[1]/1000;
						dbf[2] = 1.0*db[2]/100000;
						dbf[3] = 1.0*db[3]/1000;
						dbf[4] = 1.0*db[4]/100000;
						dbf[5] = 1.0*db[5]/1000;

						//     printf(" v1=%.3f v2=%.3f v2=%.3f v2=%.3f v2=%.3f v2=%.3f \n",dbf[0],dbf[1],dbf[2],dbf[3],dbf[4],dbf[5]);
						ic_measure("lion");
						// ic_long("user",    1);
						ic_double("time",time++) ;
						ic_double("vbus",dbf[0]) ;
						ic_double("cur" ,dbf[1]) ;
						ic_double("cah" ,dbf[2]) ;
						ic_double("pow" ,dbf[3]) ;
						ic_double("cwh" ,dbf[4]) ;
						ic_double("rwo" ,dbf[5]) ;

						ic_measureend();
						ic_push();

						//    fp=fopen(argv[2],"a");
						//    t = time(NULL);
						//    tm = *localtime(&t);
						
					}
				}
      }
    }
  }

// DROP SERIES FROM lion WHERE machine='rpi'
// select count(*) from (select * from lion)
// curl -i -XPOST "http://localhost:8086/write?db=testdb" --data-binary 'weather,location=us-midwest temperature=82 1465839830100400200'
// curl -G 'http://localhost:8086/query?db=testdb' --data-urlencode 'q=SELECT * FROM lion where time>now()-10m'
