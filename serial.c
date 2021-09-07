#include <termios.h>                                                         
#include <stdio.h>
#include <stdlib.h>	
#include <string.h>
#include <unistd.h>                                                          
#include <fcntl.h>                                                                                                               
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

#include <wiringPi.h>
#include <wiringSerial.h>

#include <errno.h>

#include <errno.h>
#include <fcntl.h> 
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <time.h>

#define BAUDRATE B115200                                                      
#define MODEMDEVICE "/dev/ttyUSB1"
char *portname = "/dev/ttyUSB0";
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
    int fd,flg1=1;                                                             
    char buf[255];  
    int t1=0,t2=0,tl1=0,tl2=0;
    float tf1,tf2;
    struct pollfd fds[1];
    int ret, res, cnt=0;
    FILE *fp;
    cnt=cnt;
    //fp=fp;
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
   
    
    if ((argc != 2))
    {
      printf("Użycie: %s /dev/tty* (Baud=B115200) \n",argv[0]);
      return 0;
    }
    printf("Odbior danych z portu %s i zapis ze znacznikiem czasu do pliku \n",argv[1]);
    
    //sscanf(argv[2],"%d",&var);
    /* open the device */
    fd = open(argv[1], O_RDWR | O_NOCTTY | O_NONBLOCK);
    //fd = open(portname, O_RDWR | O_NOCTTY | O_NONBLOCK);
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
    
    //set_interface_attribs (fd, var, 0);  	// set speed to 19200 bps, 8n1 (no parity)
    set_interface_attribs (fd, BAUDRATE, 0);  	// set speed to 19200 bps, 8n1 (no parity)
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
    char datefile[16];
    
    //sprintf(datefile,"%04d",tm.tm_mon+1+100*tm.tm_mday);
    //sprintf(datefile,"%02d.week",tm.tm_yday/7);
    //fp=fopen(datefile,"a");
    //fclose(fp);
    //return 0;

  for (;;)		// forever
    {
      ret = poll(fds, 1, 1000);			// wait for response

      if (ret > 0)
      { 
	/* An event on one of the fds has occurred. */
	//sprintf(datefile,"%04d",10+100*7);
	//sprintf(datefile,"%04d",tm.tm_mon+1+100*tm.tm_mday);
	sprintf(datefile,"%02d.week",tm.tm_yday/7);
	//fp=fopen(datefile,"a");
	fp=fopen("temper.csv","a");
	
	if (fds[0].revents & POLLHUP)
	{
	  //printf("Hangup\n");
	  fprintf(fp,"Hangup\n");
	} 
	
	if (fds[0].revents & POLLRDNORM)
	{
	  
	  res = read(fd,buf,255);
	  buf[res]=0;					// terminate buffer
	  sscanf(buf,"%d,%d,%d",&t1,&t2,&cnt); 		// convert string to number
	  if ( flg1 ){
	    tl1 = t1;
	    tl2 = t2;
	    flg1 = 0;
	  } 
	  //printf("raw=%s",buf);
	  
	  //if(t1 > -200 && t1 < 1000 && t2 > -200 && t2 < 1000 ){
	  if( abs(t1-tl1)<20 && abs(t2-tl2)<20 ){
	    
	    
	    //fputs(buf,fp);
	    t = time(NULL);
	    tm = *localtime(&t);
	    tf1=t1/10.0;
	    tf2=t2/10.0;
	    //printf("%d-%d-%d %d:%d:%d,%d\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec,var);
	    //fprintf(fp,"%d-%d-%d %d:%d:%d,%d\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec,var);
	    //fprintf(fp,"%4d%2d%2d%2d%2d,%d\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec,var);
    	    //fprintf(fp,"%02d%02d%02d%02d%02d%4d,%d\n",tm.tm_sec,tm.tm_min,tm.tm_hour,tm.tm_mday,tm.tm_mon + 1,tm.tm_year + 1900,t1);
	    //fprintf(fp,"%02d%02d%02d%02d%02d,%d\n",tm.tm_sec,tm.tm_min,tm.tm_hour,tm.tm_mday,tm.tm_mon+1,t1);
	    //fprintf(fp,"%02d%02d%02d%02d%02d,%d,%d\n",tm.tm_hour,tm.tm_min,tm.tm_sec,tm.tm_mday,tm.tm_mon+1,t1,t2); // python matplotlib
	    fprintf(fp,"%04d-%02d-%02d %02d:%02d:%02d,%.01f,%.01f\n",tm.tm_year+1900,tm.tm_mon+1,tm.tm_mday,tm.tm_hour,tm.tm_min,tm.tm_sec,tf1,tf2); // javascript


	    //printf("now:    %d-%d %d:%d:%d\n", 			  tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
	    //printf("int= %d \n",var);
	    //fprintf(fp,"%d\n",var);
	    tl1 = t1;
	    tl2 = t2;        	  
	  }
	  
	} else fprintf(fp,"No data\n");
	fclose(fp);
      }
    }
  }
