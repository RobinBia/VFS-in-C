/*vfs.c*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stddef.h>

//Variablen

char *pfadcpy1;
char *pfadcpy2;

//Öffentliche Strukturen
struct det
{
char *dateiname;
int64_t dateigr;
int64_t *block;
} d;
struct
{
struct det *d;
int64_t blockz;
int64_t blockc;
int64_t anzspeich;
} dateiinfo;


//Funktion für die Interpretation der Rückgaben
void ret(int i)
{
switch(i)
{
case 0:
printf("Das Kommando wurde fehlerfrei ausgeführt.\n");
break;
case 1:
printf("Das Anlegen des VFS war nicht möglich.\n");
break;
case 2:
printf("Das VFS konnte nicht geöffnet werden.\n");
break;
case 3:
printf("Ein VFS mit dem gegebenen Namen existiert bereits.\n");
break;
case 11:
printf("Eine Datei mit diesem Dateinamen existiert bereits.\n");
break;
case 12:
printf("Nicht genugend freier Speicherplatz im VFS.\n");
break;
case 13:
printf("Die Quelldatei existiert nicht.\n");
break;
case 21:
printf("Die Datei existiert nicht im VFS.\n");
break;
case 30:
printf("Die Zieldatei im Host-Dateisystem konnte nicht erstellt werden.\n");
break;
}
}



//Funktion 3.1 "create"
int create(char *blocksize, char *blockcount)
{

int64_t bz;
int64_t bc;

bz = strtoull(blocksize,0,10); 
bc = strtoull(blockcount,0,10);

/*printf("%lld\n", (long long)bz);
printf("%lld\n", (long long)bc);*/

char *puffer;

puffer = malloc((bc*bz)*sizeof(char));

// Die char-Zeiger müssen umgewandelt werden

char structure[11]=".structure";
char store[11]=".store";
// Pfade zu Pfade+Dateinamen machen

strcat(pfadcpy2,store);	
strcat(pfadcpy1,structure);




// Erstellung der Datei .store im Modus a+
FILE *speich;
speich = fopen(pfadcpy2,"r");
if(speich != NULL)
{
return 3;
}
else
{
speich = fopen(pfadcpy2,"a+");
if(speich == NULL)
{
return 1;
}
}	


fread(puffer, bz*sizeof(char), bc, speich);

fwrite(puffer, bz*sizeof(char), bc, speich);

fclose(speich);


// Für die Verwaltungsdaten in der Datei .structure
printf("marker");



//Erstellung von Verwaltungsstrukturen

FILE *verwalt;
verwalt = fopen(pfadcpy1,"a+");
if(verwalt == NULL)
{
return 1;
}
//fseek(verwalt, 0, SEEK_END);
dateiinfo.blockz = bz;
dateiinfo.blockc = bc;
//printf("Hier %lld\n", dateiinfo.blockc);
fwrite(&dateiinfo, sizeof(dateiinfo), 1, verwalt);
fclose(verwalt);

/*/optio

verwalt = fopen(pfadcpy1,"a+");
while(fread(&dateiinfo, sizeof(dateiinfo),1, verwalt) ==1)
{
printf("Hier %lld\n", dateiinfo.blockc);
}
//optio end*/




return 0;
}


//Funktion 3.2 "add"
int add(char *source, char *target)
{

// Die char-Zeiger müssen umgewandelt werden

char structure[11]=".structure";
char store[11]=".store";
// Pfade zu Pfade+Dateinamen machen

strcat(pfadcpy2,store);	
strcat(pfadcpy1,structure);

FILE *temp;
FILE *verwalt;
FILE *speich;
/*temp = fopen(source,"r");
if(temp == NULL)
{
return 13;
}

*/

speich = fopen(pfadcpy2, "r");
if (speich == NULL)
{
return 2;
}
verwalt = fopen(pfadcpy1, "r");
if(verwalt == NULL)
{
return 2;
}


while(fread(&dateiinfo, sizeof(dateiinfo), 1, verwalt) == 1)
{

printf("%lld \n",dateiinfo.blockc);
printf("marker\n");
}






}





//Funktion 3.3 "get"
int get(char *source, char *output)
{

}


//Funktion 3.4 "del"
int del(char *target)
{
}

//Funktion 3.5 "free"
int freee()
{
}




//Funktion 3.6 "used"
int used()
{
}


//Funktion 3.7 "list"
int list()
{
}


//Funktion 3.8 "defrag"
int defrag()
{
}




//argv[0] == "vfs", argv[1] == "archiv",argv[2] == "operation",argv[3,4] == "parameter",

int main(int argc, char *argv[])
{
//Initialisierung der Dateipfade von .structure und .store
int i = strlen(argv[1]);
i = i+11;
printf("marker %d \n",i);
pfadcpy1 = malloc(i*sizeof(char));
strcpy(pfadcpy1, argv[1]);
pfadcpy2 = malloc(i*sizeof(char));
strcpy(pfadcpy2, argv[1]);


int *epsilon;
epsilon = &argc;

int h; // Hilfsvariable um Übergeben der Rückgaben an die Ausgabefunktion ret()
int zeichen;


	if(argc < 3)
	{
	printf("Bitte überprüfen Sie ihre Eingabe.");
	return EXIT_FAILURE;
	}

	else
	{

	
//create-Abfrage
			if(strcmp(argv[2], "create") == 0)
			{ 
				if(argc < 5)
				{
					printf("Sie haben eine falsche Eingabe gemacht./n");
					return EXIT_FAILURE;
			   	}
			h = create(argv[3], argv[4]);
			ret(h);
			return EXIT_SUCCESS;
			}

//add-Abfrage
			if(strcmp(argv[2], "add") == 0)
			{
				if(argc < 5)
				{
					printf("Sie haben eine falsche Eingabe gemacht./n");
					return EXIT_FAILURE;
			   	}
			h = add(argv[3], argv[4]);
			ret(h);
			return EXIT_SUCCESS;
			}

//del-Abfrage
			if(strcmp(argv[2], "del") == 0)
			{
				if(argc < 4)
				{
					printf("Sie haben kein zu löschendes Ziel angegeben/n");
					return EXIT_FAILURE;
			   	}
			h = del(argv[3]);
			ret(h);
			return EXIT_SUCCESS;
			}



//defrag-Abfrage
			if(strcmp(argv[2], "defrag") == 0)
			{
				if(argc != 3)
				{
					printf("Sie haben eine falsche Eingabe gemacht./n");
					return EXIT_FAILURE;
			   	}

				h = defrag();
				ret(h);
				return EXIT_SUCCESS;
			}

//free-Abfrage
			if(strcmp(argv[2], "free") == 0)
			{
				if(argc != 3)
				{
					printf("Sie haben eine falsche Eingabe gemacht./n");
					return EXIT_FAILURE;
			   	}
				h = freee();
				ret(h);
				return EXIT_SUCCESS;
			}

//get-Abfrage
			if(strcmp(argv[2], "get") == 0)
			{ 
				if(argc < 5)
				{
					printf("Sie haben eine falsche Eingabe gemacht./n");
					return EXIT_FAILURE;
			   	}
			h = get(argv[3], argv[4]);
			ret(h);
			return EXIT_SUCCESS;
			}
//list-Abfrage
			if(strcmp(argv[2], "list") == 0)
			{
				if(argc != 3)
				{
					printf("Sie haben eine falsche Eingabe gemacht./n");
					return EXIT_FAILURE;
			   	}
				h = list();
				ret(h);
				return EXIT_SUCCESS;
			}
//used-Abfrage
			if(strcmp(argv[2], "used") == 0)
			{
				if(argc != 3)
				{
					printf("Sie haben eine falsche Eingabe gemacht./n");
					return EXIT_FAILURE;
			   	}
				h = used();
				ret(h);
				return EXIT_SUCCESS;

			}


//Abfrage für jede andere Eingabe
			else
			{
				printf("Sie haben eine unzulässige Operation angegeben. /n");
				
				return EXIT_FAILURE;

			}
		}
return EXIT_SUCCESS;
	
}
