/*vfs.c*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stddef.h>

//Variablen

char *pfadcpy1;
char *pfadcpy2;
char structure[11]=".structure";
char store[11]=".store";

//Öffentliche Strukturen
struct
{
char dateiname[100];
int64_t blockz;
int64_t blockc;
int blockbelegt; //0 = "Block noch frei", 1 = "Block belegt"
int64_t blocknummer;
int64_t dateigroesse;
} dateiinfo;



////////////////////////////////////////////////////////////////////////Funktion für die Interpretation der Rückgaben//////////////////////////////////////////////////////////////////////////
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
printf("Nicht genuegend freier Speicherplatz im VFS.\n");
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



///////////////////////////////////////////////////////////////////////////////Funktion 3.1 "create"/////////////////////////////////////////////////////////////////////////
int create(char *blocksize, char *blockcount)
{

int64_t bz;
int64_t bc;

bz = strtoull(blocksize,0,10); 
bc = strtoull(blockcount,0,10);

/*printf("%lld\n", (long long)bz);
printf("%lld\n", (long long)bc);*/

char *puffer;
puffer = malloc(bz*sizeof(char));


// Erstellung der Datei .store im Modus "w+" und Überprüfung ob diese schon vorhanden ist.
FILE *speich;
speich = fopen(pfadcpy2,"r");
if(speich != NULL)
{
return 3;
}
else
{
speich = fopen(pfadcpy2,"w+");
if(speich == NULL)
{

return 1;

}
}	
//Schreibe bc mal ein Objekt der größe bz Bytes in den Stream "speich".

int64_t u = 0;
while(u < bc)
{
fread(puffer, bz*sizeof(char), 1, speich);
fwrite(puffer, bz*sizeof(char), 1, speich);
u = u+1;
}
fclose(speich);


//Initialisierung der Struktur mit Blockgröße und Blockanzahl.
dateiinfo.blockz = bz;
dateiinfo.blockc = bc;
dateiinfo.blockbelegt = 0; //Alle unbelegt.

//Erstellung von Verwaltungsstrukturen

FILE *verwalt;
verwalt = fopen(pfadcpy1,"w+");
if(verwalt == NULL)
{
return 1;
}

//printf("Hier %lld\n", dateiinfo.blockc);
int64_t s = 0;
while(s < bc)
{
fwrite(&dateiinfo, sizeof(dateiinfo), 1, verwalt);
s = s+1;
}
fclose(verwalt);



return 0;
}


/////////////////////////////////////////////////////////////Funktion 3.2 "add"////////////////////////////////////////////////////////////////////////////////
int add(char *source, char *target)
{

FILE *temp;
FILE *verwalt;
FILE *speich;


verwalt = fopen(pfadcpy1, "r+");
if(verwalt == NULL)
{
return 2;
}
speich = fopen(pfadcpy2, "r+");
if (speich == NULL)
{
printf("offen");
return 2;
}


fread(&dateiinfo,sizeof(dateiinfo),1,verwalt);
int64_t bc = dateiinfo.blockc;
int64_t bz = dateiinfo.blockz;
rewind(verwalt);


//Öffnung der Quelle
temp = fopen(source,"r");
if(temp == NULL)
{
return 13; //Sollte die Datei nicht vorhanden sein
}


//Ist die Datei vielleicht schon im VFS?
char *o;
o = malloc(sizeof(dateiinfo));
int catch = 0;
while(fread(o,sizeof(dateiinfo),1,verwalt) == 1)
{
if(strcmp(dateiinfo.dateiname, target) == 0)
{
catch++;
}
}
if(catch != 0)
{
return 11; //Schon vorhanden
}
rewind(verwalt);



//Ansonsten muss die Größe der Datei ermittelt werden.
int64_t n = 1;
char *puffer;
puffer = malloc(bz*sizeof(char));
while(fread(puffer,bz*sizeof(char),1,temp) == 1)
{
n=n+1;
}

printf("Für die Eingelesene Datei werden %lld Blöcke benötigt \n", n);
free(puffer);

//Und wie viele Blöcke sind belegt???
int64_t zaehler = 0;
int64_t max = bc;
int64_t i = 0;
int64_t pos = 0;
while(fread(&dateiinfo,sizeof(dateiinfo),1,verwalt) == 1)
{
if(dateiinfo.blockbelegt == 0)
{
i = i+1;
zaehler = zaehler+1;

		if(zaehler == n)
		{
		max = zaehler;
		pos = i-max;
		break;
		}

}
if(dateiinfo.blockbelegt == 1)
{
i = i+1;
max = zaehler;
pos = i-max;
zaehler = 0;
//printf("Entgültige pos = %lld", pos);
}
if(i == bc && dateiinfo.blockbelegt == 0 )
{
max = zaehler;
pos = i - max;
}
}


//Ausgabe 2 Möglichkeiten. 1. Es existiert ein Maximum -> Nicht genügend Speicherplatz, 2. Es existiert ein Minimum -> Datei kann geschrieben werden



if(max < n)
{
return 12; //Rückgabe, wenn weniger zusammenhängende freie Plätze vorhanden als für die Datei notwendig.
}

printf("Mindestens %lld zusammenhängende leere Blöcke sind ab Block %lld noch unbelegt\n", max, pos);
printf("Daher kann die Datei aufgenommen werden.\n");

//Wenn genügend Plätze da sind, schreibe die Datei ab Block "pos" in den Store
rewind(verwalt);
rewind(temp);
fseek(speich, pos*bz, SEEK_SET);
fseek(verwalt,pos*sizeof(dateiinfo),SEEK_SET);

//dateiinfo wird mit den Daten der Datei beschrieben
dateiinfo.blockbelegt = 1;
strcpy(dateiinfo.dateiname, target);


char *p;
p = malloc(bz*sizeof(char));
int64_t m = 0;
while( m < n)
{
fread(p, bz*sizeof(char), 1, temp);
fwrite(p, bz*sizeof(char), 1, speich );
fwrite(&dateiinfo, sizeof(dateiinfo), 1, verwalt);
m = m+1;
}
free(p);

fclose(speich);
fclose(verwalt);
fclose(temp);

}





////////////////////////////////////////////////////////////////////////////Funktion 3.3 "get"////////////////////////////////////////////////////////////////////////////////////

int get(char *source, char *output)
{

FILE *temp;
FILE *verwalt;
FILE *speich;


verwalt = fopen(pfadcpy1, "r+");
if(verwalt == NULL)
{
return 2;
}
speich = fopen(pfadcpy2, "r+");
if (speich == NULL)
{
printf("offen");
return 2;
}


//Initialisierung von Blockcount und Blocksize aus der Struktur
fread(&dateiinfo,sizeof(dateiinfo),1,verwalt);
int64_t bc = dateiinfo.blockc;
int64_t bz = dateiinfo.blockz;

//printf("Der erste Dateiname lautet %s\n",dateiinfo.dateiname);
rewind(verwalt);


////////Kopiervorgang von speich in temp Stream ;)
char *p;
p = malloc(bz*sizeof(char));
int64_t k = 0;
int hilf = 0;  //Schalter klappt um, wenn die Datei im VFS gefunden wurde - Sodass nur einmal eine neue Datei erstellt wird

while(fread(&dateiinfo, sizeof(dateiinfo), 1, verwalt) == 1)
{
if(strcmp(dateiinfo.dateiname, source) == 0)
{
printf("%s existiert im VFS, Es wird Block %lld gelesen und geschrieben\n", dateiinfo.dateiname, k);
fseek(speich,k*bz,SEEK_SET);
fread(p, bz*sizeof(char), 1, speich);

//Da die Verzweigung betreten wird, muss hier die gewünschte Kopie erstellt werden
if(hilf == 0)
{	
		temp = fopen(output, "w+");
		if(temp == NULL)
		{
		return 30;
		}
	hilf++;
}

fwrite(p, bz*sizeof(char), 1, temp);
}
k++;
}

//Wurde eine Datei gefunden?
if(hilf == 0)
{
return 21; //Datei existiert nicht im VFS
}


fclose(temp);
fclose(verwalt);
fclose(speich);


}





////////////////////////////////////////////////////////////////////////////Funktion 3.4 "del"//////////////////////////////////////////////////////////////////
int del(char *target)
{

FILE *verwalt;



verwalt = fopen(pfadcpy1, "r+");
if(verwalt == NULL)
{
return 2;
}


//Initialisierung von Blockcount und Blocksize aus der Struktur
fread(&dateiinfo,sizeof(dateiinfo),1,verwalt);
int64_t bc = dateiinfo.blockc;
int64_t bz = dateiinfo.blockz;
rewind(verwalt);


int64_t k = 0;
int schalter = 0;
while(fread(&dateiinfo,sizeof(dateiinfo), 1, verwalt) == 1)
{
if(strcmp(dateiinfo.dateiname, target) == 0)
{
dateiinfo.blockbelegt = 0;
strcpy(dateiinfo.dateiname, "");
fseek(verwalt,k*sizeof(dateiinfo),SEEK_SET);
fwrite(&dateiinfo, sizeof(dateiinfo), 1, verwalt);
schalter = 1;
}

k++;
}

if(schalter == 0)
{
return 21;
}


}

/////////////////////////////////////////////////////////////////////////////////////////Funktion 3.5 "free"//////////////////////////////////////////////////////////////////////////////////////////
int freee()
{
FILE *verwalt;
verwalt = fopen(pfadcpy1, "r+");
if(verwalt == NULL)
{
return 2;
}
fread(&dateiinfo, sizeof(dateiinfo), 1, verwalt);
int64_t bz = dateiinfo.blockz;
rewind(verwalt);

//Zähle k jedes mal hoch, wenn ein Block unbelegt ist
int64_t k = 0;
while(fread(&dateiinfo, sizeof(dateiinfo), 1, verwalt) == 1)
{
	if(dateiinfo.blockbelegt == 0)
	{
	k = k+1;
	}
}

int64_t bytes = 0;

bytes = bz*k;

printf("Ihr VFS hat %lld freie Bytes\n", bytes);
fclose(verwalt);

}




//Funktion 3.6 "used"
int used()
{
}


//////////////////////////////////////////////////Funktion 3.7 "list"/////////////////////////////////////////////////////////////////////
int list()
{
/*
char *prechar;
int b = 0; //boolischer Hilfswert



while(fread(&dateiinfo, sizeof(dateiinfo), 1, verwalt) == 1)
{


if((strcmp(dateiinfo.dateiname, prechar) != 0)  && (strcmp(dateiinfo.dateiname, "") != 0) && (dateiinfo.blocknummer) != 0)
{
b = 0;
free(prechar);
}


if(b == 0)
{
prechar = malloc(sizeof(dateiinfo.dateiname));
prechar = dateiinfo.dateiname;
printf("%s,",dateiinfo.dateiname);
printf("%lld,",dateiinfo.dateigroesse);
b = 1;
}

if(strcmp(dateiinfo.dateiname,"") != 0)
{
printf("%lld,", dateiinfo.blocknummer);
}


}
free(prechar);
*/

}


//Funktion 3.8 "defrag"
int defrag()
{
}




//argv[0] == "vfs", argv[1] == "archiv",argv[2] == "operation",argv[3,4] == "parameter",

int main(int argc, char *argv[])
{

if(argc < 3)
{
printf("Ihre Eingabe enthält keine Funktion\n");
return EXIT_FAILURE;
}


//Initialisierung der Dateipfade von .structure und .store
int i = strlen(argv[1]);
i = i+11;
pfadcpy1 = malloc(i*sizeof(char));
strcpy(pfadcpy1, argv[1]);
pfadcpy2 = malloc(i*sizeof(char));
strcpy(pfadcpy2, argv[1]);
//Anhängen der ".store", ".structure"-Dateiendungen und somit redefinierung der Pfade als Dateinamen
strcat(pfadcpy2,store);	
strcat(pfadcpy1,structure);
int h; // Hilfsvariable um Übergeben der Rückgaben an die Ausgabefunktion ret()
int zeichen;


	
//create-Abfrage
			if(strcmp(argv[2], "create") == 0)
			{ 
				if(argc < 5)
				{
					printf("Sie haben keine oder zu wenige Parameter eingegeben.\n");
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
					printf("Sie haben keine oder zu wenige Parameter eingegeben.\n");
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
					printf("Sie haben kein zu löschendes Ziel angegeben.\n");
					return EXIT_FAILURE;
			   	}
			h = del(argv[3]);
			ret(h);
			return EXIT_SUCCESS;
			}



//defrag-Abfrage
			if(strcmp(argv[2], "defrag") == 0)
			{

				h = defrag();
				ret(h);
				return EXIT_SUCCESS;
			}

//free-Abfrage
			if(strcmp(argv[2], "free") == 0)
			{

				h = freee();
				ret(h);
				return EXIT_SUCCESS;
			}

//get-Abfrage
			if(strcmp(argv[2], "get") == 0)
			{ 
				if(argc < 5)
				{
					printf("Sie haben keine oder zu wenige Parameter eingegeben.\n");
					return EXIT_FAILURE;
			   	}
			h = get(argv[3], argv[4]);
			ret(h);
			return EXIT_SUCCESS;
			}
//list-Abfrage
			if(strcmp(argv[2], "list") == 0)
			{

				h = list();
				ret(h);
				return EXIT_SUCCESS;
			}
//used-Abfrage
			if(strcmp(argv[2], "used") == 0)
			{

				h = used();
				ret(h);
				return EXIT_SUCCESS;

			}


//Abfrage für jede andere Eingabe
			else
			{
				printf("Sie haben eine unzulässige Operation eingegeben.\n");
				
				return EXIT_FAILURE;

			}
		
return EXIT_SUCCESS;
	
}
