#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<fcntl.h>
#include<string.h>
#include<unistd.h>



// populates a buffer with x characters of value c
int fill(int x, char c){

	int fdwo;
	int i = 0;
	int stringlen = x;
	int ret = 0;
	char stringtosend[stringlen+1];
	char buffer[1030];
	char *labelPtr = stringtosend;
	

	fdwo = open("/dev/wochardevdriver", O_WRONLY); 
	
	// attempt to open the wo device
	if (fdwo < 0){
		perror("Failed to open the wo device...");
		return errno;
	}else{
		if(x == 0){
			printf("x = 0, do not send any characters to driver\n");
		}else{
			// populate the string to send
			for(i = 0; i < stringlen; i++){
				stringtosend[i] = c;
			}
			stringtosend[i] = '\0';

			// send it
			printf("The sent message is: [%s]\n", stringtosend);
			int ret = write(fdwo, labelPtr, stringlen);
			printf("RETURN VAL FROM DRIVER: %d\n", ret);
		}
	
	
		// close the wo device
		ret = close(fdwo);
	}
}

// tries to read y characters
int reado(int y){

	int fdro;
	int i = 0;
	int ret = 0;
	char buffer[1030];
	int charstoreq = y;
	
	
	fdro = open("/dev/rochardevdriver", O_RDONLY); 
	
	// attempt to open the ro device
	if (fdro < 0){
		perror("Failed to open the ro device...");
		return errno;
	}else{
	
		if(y != 0){

			ret = read(fdro, buffer, charstoreq);
			printf("requesting %d characters from the driver\n", charstoreq);
			printf("The received message is: [%s]\n", buffer);
			printf("received %d characters from the driver\n", strlen(buffer));
			printf("RETURN VAL FROM DRIVER: %d\n", ret);
		}else{
			printf("y = 0, do not request characters from driver\n");
		}
	
		// close the ro device
		ret = close(fdro);
	}
}


//
int testcase1(){
	// empty the rodriver
	reado(1050);
	
	sleep(1);
	
	// fill the wodriver
	//fill(1050, 'a');
	
	sleep(1);
	
	// empty the rodriver
	reado(1050);

}

int testcase2(){
	fill(7, 'a');
	reado(4);
	
	fill(5, 'b');
	reado(7);
	
	fill(1, 'c');
	reado(1);
}

int main(){
	// empty the rodriver
	testcase1();
	testcase2();
		
	return 0;		
}
