/*vfs.c*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

//Variablen
char pfad[50];

//Funktion für die Interpretation der Rückgaben
void ret(int i)
{
switch(i)
{
case 0:
printf("Das Kommando wurde fehlerfrei ausgeführt.");
break;
case 1:
printf("Das Anlegen des VFS war nicht möglich.");
break;
case 2:
printf("Das VFS konnte nicht geöffnet werden.");
break;
case 3:
printf("Ein VFS mit dem gegebenen Namen existiert bereits.");
break;
case 11:
printf("Eine Datei mit diesem Dateinamen existiert bereits.");
break;
case 12:
printf("Nicht genugend freier Speicherplatz im VFS.");
break;
case 13:
printf("Die Quelldatei existiert nicht.");
break;
case 21:
printf("Die Datei existiert nicht im VFS");
break;
case 30:
printf("Die Zieldatei im Host-Dateisystem konnte nicht erstellt werden.");
break;
}
}

//Umwandlung für create
int64_t charTo64bitNum(char a[])
 {
  int64_t n = 0;
  memcpy(&n, a, 8);
  return n;
}

 

//Funktion 3.1 "create"
int create(char *blocksize, char *blockcount)
{
int64_t bz = charTo64bitNum(blocksize);
int64_t bc = charTo64bitNum(blocksize);
// Die char-Zeiger müssen umgewandelt werden
char pfadcpy1[50];
strcpy(pfadcpy1, pfad);
char pfadcpy2[50];
strcpy(pfadcpy2, pfad);
char structure[11]=".structure";
char store[11]=".store";
FILE *datei;

		strcat(pfadcpy1,structure);
		strcat(pfadcpy2,store);


		datei = fopen(pfadcpy1,"a+");
		if(datei == NULL)
		{
			return 1;
		}
		
		else
		{
		//existiert bereits, mit welchem inhalt?
		}

		datei = fopen(pfadcpy2,"a+");
	printf("Marker");
	








}


//Funktion 3.2 "add"
int add(char *source, char *target)
{

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


strcpy(pfad, argv[1]);


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
