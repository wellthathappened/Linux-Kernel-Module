#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<fcntl.h>
#include<string.h>
#include<unistd.h>

int send(char * string, int fdwo) {
	int ret = 0;

		printf("The sent message is: [%s]\n", string);

		ret = write(fdwo, string, strlen(string));

		printf("RETURN VAL FROM DRIVER: %d\n", ret);

		return ret;

}

// tries to read y characters
int reado(int y, int fdro){
	int i = 0;
	int ret = 0;
	char buffer[1030];
	int charstoreq = y;
	
		ret = read(fdro, buffer, charstoreq);

		printf("requesting %d characters from the driver\n", charstoreq);
		printf("The received message is: [%s]\n", buffer);
		printf("received %d characters from the driver\n", strlen(buffer));
		printf("RETURN VAL FROM DRIVER: %d\n", ret);

		return ret;
}


//
int testcase1(){
	// empty the rodriver
	//reado(1050);
	
	sleep(1);
	
	// fill the wodriver
	//fill(1050, 'a');
	
	sleep(1);
	
	// empty the rodriver
	//reado(1050);

}

int testcase2(){

	int fdro = open("/dev/rochardevdriver", O_RDONLY); 
	int fdwo = open("/dev/wochardevdriver", O_WRONLY); 
	int i = 1;

	// attempt to open the ro device
	if (fdro < 0) {
		perror("Failed to open the ro device...");
		exit(-1);
	}
	if (fdwo < 0) {
		perror("Failed to open the wo device...");
		exit(-1);
	}

	send("UCF is a school in Florida.", fdwo);
	printf("Read how many?: ");
	scanf("%d",&i);
	reado(i,fdro);

	send("Orlando's UCF is a college.", fdwo);
	printf("Read how many?: ");
	scanf("%d",&i);
	reado(i,fdro);
	
	send("This sentence is stupid", fdwo);
	printf("Read how many?: ");
	scanf("%d",&i);
	reado(i,fdro);
	
	send("UCWha", fdwo);
	printf("Read how many?: ");
	scanf("%d",&i);
	reado(i,fdro);


	send("UbCbF", fdwo);
	printf("Read how many?: ");
	scanf("%d",&i);
	reado(i,fdro);

	close(fdro);
	close(fdwo);
}

int main(){
	// empty the rodriver
	//testcase1();
	testcase2();
		
	return 0;		
}
