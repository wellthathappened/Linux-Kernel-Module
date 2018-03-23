#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<fcntl.h>
#include<string.h>
#include<unistd.h>


int main(){
	int fd = open("/dev/charMode", O_RDWR); 
	int i = 0;
	int stringlen = 20;

	char stringtosend[stringlen+1];

	for(i = 0; i < stringlen; i++){
		stringtosend[i] = i+'a';
	}
	stringtosend[15] = '\0';

	if (fd < 0){
		perror("Failed to open the device...");
		return errno;
	}else{
		// writing
		char *labelPtr = stringtosend;

		printf("The sent message is: [%s]\n", stringtosend);
		int ret = write(fd, labelPtr, strlen(labelPtr));
		printf("RETURN VAL FROM DRIVER: %d\n", ret);


		// reading
		char buffer[1025];

		ret = read(fd, buffer, 15);
		printf("The received message is: [%s]\n", buffer);
		printf("RETURN VAL FROM DRIVER: %d\n", ret);
	}






	return 0;
}
