#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<unistd.h>

int8_t write_buf[1024];
int8_t read_buf[1024];

int main()
{
	int fd;
	char option;
	
	printf("Demo of character device driver\n");

	fd = open("/dev/my_device", O_RDWR);
	if(fd < 0)
	{
		printf("cannot open device file...\n");
		return 0;
	}
	while(1)
	{
		printf("*****Enter options*******\n");
		printf("	1. Write	\n");
		printf("	2. Read		\n");
		printf("	3. Exit		\n");
		scanf(" %c", &option);
		printf(" your option = %c\n", option);

		switch(option)
		{
			case '1':
				printf("Enter string to write into driver:\n");
				scanf(" %[^\t\n]s", write_buf);
				write(fd, write_buf, strlen(write_buf) + 1);
				printf("Data written...\n");
				break;
			case '2':
				printf("Data is being read...\n");
				read(fd, read_buf, 1024);
				printf("Data read...\n");
				printf("Data = %s\n", read_buf);
				break;
			case '3':
				close(fd);
				return 0;
				break;
			default:
				printf("Enter valid option...\n");
				break;
		}
	}
	close(fd);
}
