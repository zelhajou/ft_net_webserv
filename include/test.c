#include <unistd.h>
#include <stdio.h>

int main ()
{
	if (access("././../ss/ex00/Cat.hpp", F_OK) == 0)
		printf("File exists\n");
	else
		printf("File doesn't exist\n");
	return 0;
}