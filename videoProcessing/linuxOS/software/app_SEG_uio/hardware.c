#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>
#include <cstring>
#include <stdlib.h>

#define SOBEL_FILTER "sobel_filter_top"
#define ERODE_FILTER "erode_filter_top"
#define GRAY_FILTER "gray_filter_top"


int fcount = 0;
char dev[20] ="";

void resetDev() {
	fcount = 0;
	strcpy(dev,"");
	printf("resetDev(): fcount: %i, dev: %s\n", fcount, dev);
}


int match(const char text[], const char pattern[]) {
  int c, d, e, text_length, pattern_length, position = -1;
 
  text_length    = strlen(text); 
  pattern_length = strlen(pattern);
 
  if (pattern_length > text_length) {
    return -1;
  }

  for (c = 0; c <= text_length - pattern_length; c++) {
    position = e = c;

    for (d = 0; d < pattern_length; d++) {
      if (pattern[d] == text[e]) {
        e++;
      }
      else {
        break;
      }
    }
    if (d == pattern_length) {
      return position;
    }
  }

  return -1;
}


int HAexists(const char *archDir, const char *haName, int level, const char *fil) {
    DIR *dir;
    struct dirent *entry;

    if (!(dir = opendir(archDir)))
        return -1;
    if (!(entry = readdir(dir)))
        return -1;


    do {
	 if (entry->d_type == DT_DIR) {
            char path[1024];
            int len = snprintf(path, sizeof(path)-1, "%s/%s", archDir, entry->d_name);
            path[len] = 0;
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) continue;
            HAexists(path, haName, level + 1,fil);
        }
        else
	{
            if (strcmp(entry->d_name, fil) == 0 )
		{
		  char pp[200]="";
		  strcat(pp, archDir);
		  strcat(pp, "/");
		  strcat(pp, entry->d_name);

		  FILE* fd = NULL;
		  char buff[100];
   		  memset(buff,0,sizeof(buff));
    		  fd = fopen(pp,"r");
  		  if(NULL == fd)
  		  {
  		      printf("fopen() %s Error!!!\n",pp);
  		  }

  		  //printf("File opened successfully through fopen() %s\n", pp);
		  fread(buff, sizeof(buff),1, fd);
  		  //printf("The bytes read are [%s]\n",buff);
		  fclose(fd);
			
           	  size_t ln = strlen(buff) - 1; if (buff[ln] == '\n') buff[ln] = '\0';
		  int mat = match(buff, haName);
		  if (mat==0 & strcmp(buff,haName)==0) {
			++fcount; 
		  	printf("HAExists(): HA Exists - Pattern [%s] found in file %s at location %i. fcount: %i\n", haName, pp, mat, fcount);

                  	FILE* fdi = NULL;
			char buffer[100];
			char udir[200]; 
			strcpy(udir,archDir);
			strcat(udir, "/uevent");
    			fdi = fopen(udir,"r");
	  		if(NULL == fdi)
  			{
  			    printf("fopen() %s Error!!!\n",udir);
			}

  			//printf("fopen() %s successfully!!!\n",archDir);

			char str[60];

			while( fgets (str, 60, fdi)!=NULL ) {
				char* token = strtok(str, "=");
				while (token) {
				    char tok[20] ;
				    strcpy(tok, token);
				   // printf("token: %s\n", tok);
				    token = strtok(NULL, " ");
				    if (match(tok, "DEVNAME") == 0) {
		 	    		printf("HAexists(): DEVNAME found: %s", token);
					strcpy(dev, token);
				    }
 			 	}
			 }
			close(fdi);
		  }
		 // 	else printf("Pattern [%s] is NOT found in file %s at location %i \n\n", haName, pp, mat);
		}
	}
    } while (entry = readdir(dir));
    closedir(dir);
    //printf("FCOUNT: %i\n", fcount);
    return fcount;
}

char * getDev(const char *archDir, const char *haName, int level, const char *fil) {
	strcpy(dev, "");
	int ret =  HAexists(archDir, haName, level, fil);
	if (ret >=1) printf("getDeviceDev(): Device dev found for %s: %s", haName, dev);
	return dev;
}


int mymain()
{
    fcount = 0;
    char pat[] = "gray_filter_top";
    int ret = HAexists("/sys/devices/soc0", pat, 0, "name");
    printf("Pattern [%s] found %i times\n", pat, ret);
    return ret;
}
