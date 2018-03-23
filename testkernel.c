#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<fcntl.h>
#include<string.h>
#include<unistd.h>




// populates a buffer with x characters and tries to read y characters
int fillthenread(int x, int y){

	int fd = open("/dev/chardevdriver", O_RDWR); 
	int i = 0;
	int stringlen = x;
	int ret = 0;
	char stringtosend[stringlen+1];
	char buffer[1030];
	int charstoreq = y;
	char *labelPtr = stringtosend;
	
	

	// attempt to open the device
	if (fd < 0){
		perror("Failed to open the device...");
		return errno;
	}else{
		// writing
		
		if(x == 0){
			printf("x = 0, do not send any characters to driver\n");
		}else{
			// populate the string to send
			for(i = 0; i < stringlen; i++){
				stringtosend[i] = 'a';
			}
			stringtosend[i] = '\0';

			// send it
			printf("The sent message is: [%s]\n", stringtosend);
			int ret = write(fd, labelPtr, stringlen);
			printf("RETURN VAL FROM DRIVER: %d\n", ret);
		}

		// reading
	
		if(y != 0){

			ret = read(fd, buffer, charstoreq);
			printf("requesting %d characters from the driver\n", charstoreq);
			printf("The received message is: [%s]\n", buffer);
			printf("received %d characters from the driver\n", strlen(buffer));
			printf("RETURN VAL FROM DRIVER: %d\n", ret);
		}else{
			printf("y = 0, do not request characters from driver\n");
		}

		// close the device
		ret = close(fd);
	}
}

int main(){
	// empty the driver
	printf("emptying the driver\n");
	fillthenread(0, 1050);
	printf("\n");

	// fill the driver 
	//printf("filling the driver past limit\n");
	//readthenfill(1030, 0);
	//printf("\n");

	// partiall fill the driver
	printf("filling the driver to limit\n");
	fillthenread(1024, 0);
	printf("\n");

	// empty the driver
	printf("emptying the driver\n");
	fillthenread(0, 1050);
	printf("\n");
	return 0;
}
