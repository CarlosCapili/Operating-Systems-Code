#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <limits.h>

void fileAttr(char const filename[]);
void traverseDir(char* name);

int main(){
	int choice = 0;

	printf("Press 1 for file and 2 for directory\n");
	scanf("%d", &choice);

	switch(choice){
		case 1:
			ssize_t read;
			char* buffer = 0;
			size_t buf_size = 0;

			printf("Enter name of file: \n");
			read = getline(&buffer,&buf_size,stdin);

			if(read <= 0){
				printf("Failed to get file name\n");
				exit(1);
			}

			if(buffer[read-1] == '\n'){
				buffer[read-1] = 0;
			}

			int s = open(buffer, O_RDONLY);
			if(s==-1){
				printf("File does not exist\n");
				exit(1);
			}
			else{
				fileAttr(buffer);
			}
			free(buffer);
			break;

		case 2:
			char path[100];
			printf("Enter path:\n");
			scanf("%s", path);
			traverseDir(path);
			break;

		default:
			printf("Invalid input\n");
			break;

	}
	

	return 0;
}

void fileAttr(char const filename[]){

	struct stat file;
	struct group *groupName;
	struct passwd *userName;
	char mtime[100];

	if(stat(filename, &file)==-1){
		printf("Error Occurred\n");
	}

	//check if it is a symbolic link
	if(lstat(filename, &file)!=0){

		groupName = getgrgid(file.st_gid);
		userName = getpwuid(file.st_uid);

		//format numerical value of st_mtime to string representation  
		time_t t = file.st_mtime;
		struct tm lt;
		localtime_r(&t, &lt);
		strftime(mtime, sizeof mtime, "%b %d %T", &lt);

		//Print file permissions
		printf("\n");
		//User Permission
		printf((file.st_mode & S_IRUSR)? "r":"-");
	  	printf((file.st_mode & S_IWUSR)? "w":"-");
	  	printf((file.st_mode & S_IXUSR)? "x":"-");
	  	//Group Permission
	  	printf((file.st_mode & S_IRGRP)? "r":"-");
	  	printf((file.st_mode & S_IWGRP)? "w":"-");
	  	printf((file.st_mode & S_IXGRP)? "x":"-");
	  	//Other Permission
	  	printf((file.st_mode & S_IROTH)? "r":"-");
	  	printf((file.st_mode & S_IWOTH)? "w":"-");
	  	printf((file.st_mode & S_IXOTH)? "x":"-");
	  	printf("\n");

	  	//Access file information
		printf("Number of Links: %u\n", (unsigned int)file.st_nlink);
		printf("Owner's Name: %s\n", groupName->gr_name);
		printf("Group Name: %s\n", userName->pw_name);
		printf("Size in Bytes: %ld\n", file.st_size);
		printf("Size in Blocks: %ld\n", file.st_blocks);
		printf("Last Mod Time: %s\n", mtime);
		printf("Name: %s\n", filename);

	}

}

void traverseDir(char* name){

	DIR *dir;
	//struct dirent *dp;
	//struct stat file;

	dir = opendir(name);

	if(!dir){
		perror(name);
		//return(1);
	}

	while(1){
		struct dirent *dp;
		//stat(dp->d_name, &file);
		
		//read entries from directory
		dp=readdir(dir);
		if(!dp){
			//if no more entries break out of loop
			break;
		}

		//print name of current directory and file
		printf("%s/%s\n", name, dp->d_name);

		if(dp->d_type & DT_DIR){
			printf("LOL");
			if (strcmp (dp->d_name, "..") != 0 && strcmp (dp->d_name, ".") != 0) {

				int path_length;
                char path[PATH_MAX];
 
              path_length = snprintf(path, PATH_MAX,"%s/%s", name, dp->d_name);
				//strcat(name,"/");
				//strcat(name,dp->d_name);
				//printf("%s\n", path);
				traverseDir(path);
				//printf("%s %s\n","DIR",dp->d_name);
			}
		}

	}

	closedir(dir);
}
