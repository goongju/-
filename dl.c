#include <sys/types.h> 
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <getopt.h>
#include <grp.h>
#include <pwd.h>
#include <time.h>
int hflag = 0;
int cflag = 0;
int uflag = 0;
int aflag = 0;
int lflag = 0;
int iflag = 0; 
int Fflag = 0;
int Aflag = 0;
int oneflag = 0;
int nflag = 0;

static struct option options[] =
{
    {"help", 0, 0, 0},
    {0, 0, 0, 0}
};

char type(mode_t);
char* perm(mode_t);
void printStat(char*, char*, struct stat*);

void help() {
    printf("  %s, %-22s %-50s\n","-a", "--all", "do not ignore entries starting with .");
    printf("  %s  %-22s %-50s\n","-l", "", "use a long listing format");
    printf("  %s, %-22s %-50s\n","-i", "--inode", "print the index number of each file");
    printf("  %s  %-22s %-50s\n","-1", "", "list one file per line");
	printf("  %s  %-22s %-50s\n","-F", "--classify", " append indicator (one of */=>@|) to entries");
	printf("  %s  %-22s %-50s\n","-A", "--almost-all", "do not list implied . and ..");
	printf("  %s  %-22s %-50s\n","-n", "--numeric-uid-gid", " print  raw  entry names (don't treat e.g. control characters specially)");
	printf("  %s  %-22s %-50s\n","-c", "", " with -lt: sort by, and show, ctime (time of  last  modifica? tion  of  file  status information); with -l: show ctime and sort by name; otherwise: sort by ctime, newest first");
	printf("  %s  %-22s %-50s\n","-u", "", " with -lt: sort by, and show,  access  time;  with  -l:  show access  time  and  sort  by  name; otherwise: sort by access time, newest first");
    printf("  %s  %-22s %-50s\n","-h", "--human-readable", "with -l and/or -s, print human readable sizes (e.g., 1K 234M 2G)");
	exit(0);
}

int main(int argc, char** argv) {
    DIR* dp;
    char* dir;
    struct dirent* entry;
    struct stat st;
    char path[BUFSIZ + 1];
    if (argc == 1 || argv[argc - 1][0] == '-')
        dir = ".";      
    else
        dir = argv[argc - 1];
    if ((dp = opendir(dir)) == NULL) {
        perror(dir); 
        exit(1);
    }

    while (1) {
        int option_index = 0;
        int opt = getopt_long(argc, argv, "ailFA1ncuh", options, &option_index);
        if (opt == -1)
            break;
        switch (opt) {
        	case 0:
        		help();
        		break;
	        case 'a': 
	            aflag = 1;
	            break;
	        case 'i':
	            iflag = 1;
	            break;
	        case 'l':
	            lflag = 1;
	            break;
	        case 'F':
	        	Fflag =1;
	        	break;
	        case 'A':
	        	Aflag =1;
	        	break;
	        case '1':
	            oneflag = 1; 
	            break;
	        case 'n': 
	            nflag = 1; 
	            break;
	        case 'c': 
	            cflag = 1; 
	            break;
	        case 'u': 
	            uflag = 1; 
	            break;
			case 'h': 
	            hflag = 1; 
	            break;
	        
	        default:
	            printf("Try '%s --help' for more information.\n",argv[0]);
	            exit(1);
        }
    }
    char hidefile = '.';
    if (aflag)
        hidefile = ' ';
    while ((entry = readdir(dp)) != NULL) {   // file iterator in directory
        if (entry->d_name[0] != hidefile) {
            sprintf(path, "%s/%s", dir, entry->d_name); // 파일 경로명 만들기 
            if (lstat(path, &st) < 0) {   // 파일 상태 가져오기  
                perror(path);
                exit(1);
            }
            else 
                printStat(path, entry->d_name, &st);  // 상태 정보 출력
        }
    }
    if (!lflag && !oneflag)
        printf("\n");
    closedir(dp);

    return 0;
}


void printStat(char* pathname, char* file, struct stat* st) {
	if (iflag) //   inode print
        printf("%18ld ", st->st_ino);
    if (nflag){
    	printf("%c%s ", type(st->st_mode), perm(st->st_mode));
        printf("%3ld ", st->st_nlink);
        printf("%u %u ", st->st_uid,st->st_gid);
        printf("%9ld ", st->st_size);
        printf("%.12s ", ctime(&st->st_mtime) + 4);
	}
	
	if (lflag) {
        printf("%c%s ", type(st->st_mode), perm(st->st_mode));
        printf("%3ld ", st->st_nlink);
        printf("%s %s ", getpwuid(st->st_uid)->pw_name, getgrgid(st->st_gid)->gr_name);
        if (hflag && ((long double)(st->st_size)/1000.0>1.0)){
        		long double filesize=(long double)(st->st_size)/1000.0;
        		printf("%5.1LfK ",filesize);
		}
		else
			printf("%6ld ", st->st_size);
        if (cflag){
        	printf("%.12s ", ctime(&st->st_ctime) + 4);
		}
		else if(uflag){
			printf("%.12s ", ctime(&st->st_atime) + 4);
		}
		else{
			printf("%.12s ", ctime(&st->st_mtime) + 4);
		}
    }
    if (Fflag){
    	if(S_ISDIR(st->st_mode)) {
	        strcat(file, "/");
	        }
	    else if(S_ISLNK(st->st_mode)){
	        strcat(file, "@");
	    }               
	    else if(S_ISREG(st->st_mode)) {
            if (st->st_mode&S_IXUSR){
                strcat(file, "*");
                                }
        	else{
                strcat(file, "");
                }
        }
        else if(S_ISFIFO(st->st_mode)) {
            strcat(file, "|");   
        }
        else if(S_ISSOCK(st->st_mode)) {
            strcat(file, "=)");
        }
    }
	
    printf("%s ", file);
    if (oneflag || lflag|| nflag)
        printf("\n");
}

char type(mode_t mode) {
    if (S_ISREG(mode))
        return('-');
    if (S_ISDIR(mode))
        return('d');
    if (S_ISCHR(mode))
        return('c');
    if (S_ISBLK(mode))
        return('b');
    if (S_ISLNK(mode))
        return('l');
    if (S_ISFIFO(mode))
        return('p');
    if (S_ISSOCK(mode))
        return('s');
}

char* perm(mode_t mode) {
    int i;
    static char perms[10];

    strcpy(perms, "---------");

    for (i = 0; i < 3; i++) {
        if (mode & (S_IREAD >> i * 3))
            perms[i * 3] = 'r';
        if (mode & (S_IWRITE >> i * 3))
            perms[i * 3 + 1] = 'w';
        if (mode & (S_IEXEC >> i * 3))
            perms[i * 3 + 2] = 'x';
    }
    return(perms);
}
