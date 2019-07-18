// myls.c ... my very own "ls" implementation

#include <sys/types.h>
#include <sys/stat.h>

#include <dirent.h>
#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <grp.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef __linux__
# include <bsd/string.h>
#endif
#include <sysexits.h>
#include <unistd.h>

#define MAXDIRNAME 256
#define MAXFNAME 256
#define MAXNAME 24

char *rwxmode (mode_t, char *);
char *username (uid_t, char *);
char *groupname (gid_t, char *);

int main (int argc, char *argv[])
{
	// string buffers for various names
	char uname[MAXNAME+1]; // UNCOMMENT this line
	char gname[MAXNAME+1]; // UNCOMMENT this line
	char mode[MAXNAME+1]; // UNCOMMENT this line

	// collect the directory name, with "." as default
	char dirname[MAXDIRNAME] = ".";
	if (argc >= 2)
		strlcpy (dirname, argv[1], MAXDIRNAME);

	// check that the name really is a directory
	struct stat info;
	if (stat (dirname, &info) < 0)
		err (EX_OSERR, "%s", dirname);

	if (! S_ISDIR (info.st_mode)) {
		errno = ENOTDIR;
		err (EX_DATAERR, "%s", dirname);
	}

    // open the directory to start reading
    DIR *df; // UNCOMMENT this line
    // ... TODO ...
	// read directory entries
	struct dirent *entry[100]; // UNCOMMENT this line
	
	int count_entry = 0; 
    // readthough all the entry in the directory 
    while((entry[count_entry] = readdir(df))!= NULL){
        if (entry[count_entry]->d_name[0] == '.'){
            // filter the first charater is .
            continue ;
        }
        // add one for each recorded file 
        count_entry ++;        
    }
	
	struct stat sb;
    
    for (int i = 0; i< count_entry; i++){
        // get the file name with directory 
        char fileLo[512];
        sprintf(fileLo, "%s/%s", dirname, entry[i]->d_name);


        if (lstat(fileLo, &sb) == -1) {
            // try to get the information of this file
            perror("stat");
            exit(EXIT_FAILURE);
        }

        char fileName[512];
        // give it the basic value of file name;
        strcpy(fileName, entry[i]->d_name);
        
    
        printf("%s  %-8.8s %-8.8s %8lld  %s\n",
			rwxmode(sb.st_mode, mode),
			username(sb.st_uid, uname),
			groupname(sb.st_gid, gname),
			(long long)sb.st_size,
			fileName); 
    
    }
    
    // finish up
    closedir(df); // UNCOMMENT this line

	return EXIT_SUCCESS;
}

// convert octal mode to -rwxrwxrwx string
char *rwxmode (mode_t mode, char *str)
{
    // ... TODO ...
	 //clear the input string 
    strcpy(str,"");
    // use the input mode to generate the string represent the file mode.
    
    // determine the file type
    switch (mode& S_IFMT){
    case S_IFREG:strcat(str,"-");break;
    case S_IFLNK:strcat(str,"l");break;
    case S_IFDIR:strcat(str,"d");break;
    // unknown file type for this iteration
    default: strcat(str,"?"); break;
    }

    // determine owner premission 
    strcat(str,((mode&S_IRUSR)?"r":"-"));
    strcat(str,((mode&S_IWUSR)?"w":"-"));
    strcat(str,((mode&S_IXUSR)?"x":"-"));
    
    // determine group premission 
    strcat(str,((mode&S_IRGRP)?"r":"-"));
    strcat(str,((mode&S_IWGRP)?"w":"-"));
    strcat(str,((mode&S_IXGRP)?"x":"-"));
    
    // determine others premission 
    strcat(str,((mode&S_IROTH)?"r":"-"));
    strcat(str,((mode&S_IWOTH)?"w":"-"));
    strcat(str,((mode&S_IXOTH)?"x":"-"));
    
    
    return str;
}

// convert user id to user name
char *username (uid_t uid, char *name)
{
	struct passwd *uinfo = getpwuid (uid);
	if (uinfo != NULL)
		snprintf (name, MAXNAME, "%s", uinfo->pw_name);
	else
		snprintf (name, MAXNAME, "%d?", (int) uid);
	return name;
}

// convert group id to group name
char *groupname (gid_t gid, char *name)
{
	struct group *ginfo = getgrgid (gid);
	if (ginfo != NULL)
		snprintf (name, MAXNAME, "%s", ginfo->gr_name);
	else
		snprintf (name, MAXNAME, "%d?", (int) gid);
	return name;
}
