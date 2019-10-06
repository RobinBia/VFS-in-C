#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <stddef.h>

//Variablen
int defrag(void);

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
int64_t vanzahl;
} dateiinfo;

///////////////////////////////////////////////////////////////////////////////Funktion 3.1 "create"/////////////////////////////////////////////////////////////////////////
int create(char *blocksize, char *blockcount)
{

int64_t bz;
int64_t bc;

bz = strtoull(blocksize,0,10); 
bc = strtoull(blockcount,0,10);

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
char *puffer;
puffer = malloc(bz*sizeof(char));
int64_t u = 0;
memset(puffer,' ', bz*sizeof(char));
while(u < bc)
{
fread(puffer, bz*sizeof(char), 1, speich);
fwrite(puffer, bz*sizeof(char), 1, speich);
u = u+1;
}
free(puffer);
puffer = NULL;
fclose(speich);

//Initialisierung der Struktur mit Blockgröße und Blockanzahl.
dateiinfo.blockz = bz;
dateiinfo.blockc = bc;
dateiinfo.blockbelegt = 0; //Alle unbelegt.
dateiinfo.vanzahl = 0; //1 = 0 Dateien, 2 = 1 Datei, da beim Löschen 1Datei-1Datei = -1Dateien und somit ist die Dateianzahl immer dateiinfo.vanzahl-1

//Erstellung von Verwaltungsstrukturen

FILE *verwalt;
verwalt = fopen(pfadcpy1,"w+");
if(verwalt == NULL)
{
return 1;
}

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
free(o);
return 11; //Schon vorhanden
}
free(o);
rewind(verwalt);

//Die Dateigröße in Bytes sowie die Blockanzahl müssen ermittelt und gespeichert werden
int64_t groesse = 0;
char *g;
g = malloc(sizeof(char));
while(fread(g,sizeof(char),1,temp) == 1)
{
groesse = groesse+1;
}
free(g);
int64_t n;
n = (groesse+bz-1)/bz;
//printf("Die Eingelesene Datei ist %lld Bytes groß.\n", groesse);
//printf("Für die Eingelesene Datei werden %lld Blöcke benötigt \n", n);


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

//Ausgabe 2 Möglichkeiten. 1. Es existiert ein Maximum -> Nicht genügend Speicherplatz, 2. Es existiert ein Minimum -> Datei kann geschrieben werden.
if(max < n)
{
return 12; //Rückgabe, wenn weniger zusammenhängende freie Plätze vorhanden als für die Datei notwendig.
}

//printf("Mindestens %lld zusammenhängende leere Blöcke sind ab Block %lld noch unbelegt\n", max, pos);
//printf("Daher kann die Datei aufgenommen werden.\n");
//Hier müsste eine komplett neue structure geschrieben werden, sodass ein größerer Dateiname, sofern dieser erforderlich ist, hineinpasst.

//großer Aufwand kommt später
//großer Aufwand kommt später
//großer Aufwand kommt später
//großer Aufwand kommt später



//Wenn genügend Plätze da sind, schreibe die Datei ab Block "pos" in den Store
rewind(verwalt);
rewind(temp);
fseek(speich, pos*bz, SEEK_SET);
fseek(verwalt,pos*sizeof(dateiinfo),SEEK_SET);

//dateiinfo wird mit den Daten der Datei beschrieben
dateiinfo.blockbelegt = 1;
strcpy(dateiinfo.dateiname, target);
dateiinfo.dateigroesse = groesse;



char *p;
p = malloc(bz*sizeof(char));
int64_t m = 0;
int64_t blockpos = pos; //Zähler, um Dateiinfo zwischendurch mit der aktuellen Blocknummer zu initialisieren

	while( m < n)
	{
	memset(p,' ', bz*sizeof(char));
	fread(p, bz*sizeof(char), 1, temp);
	fwrite(p, bz*sizeof(char), 1, speich );
	dateiinfo.blocknummer = blockpos;
	fwrite(&dateiinfo, sizeof(dateiinfo), 1, verwalt);
	m = m+1;
	blockpos = blockpos+1;
	}

free(p);
fclose(speich);

//neue Dateianzahl nur im ersten Block (Nachtrag)
rewind(verwalt);
int64_t v = 0;
while(fread(&dateiinfo, sizeof(dateiinfo), 1, verwalt) == 1)
{
//printf("DIE URSPRÜNGLICHE DATEIANZAHL IST %lld\n", dateiinfo.vanzahl);
dateiinfo.vanzahl = dateiinfo.vanzahl+1;
//printf("DIE NEUE DATEIANZAHL IST %lld\n", dateiinfo.vanzahl);
fseek(verwalt,v*sizeof(dateiinfo),SEEK_SET);
fwrite(&dateiinfo, sizeof(dateiinfo), 1, verwalt);
v = v+1;
fseek(verwalt,v*sizeof(dateiinfo),SEEK_SET);
}
//

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
rewind(verwalt);

////////Kopiervorgang von speich in temp Stream ;)
int64_t bzaehl = 0;
char *p;
p = malloc(sizeof(char));
int64_t k = 0;
int hilf = 0;  //Schalter klappt um, wenn die Datei im VFS gefunden wurde - Sodass nur einmal eine neue Datei erstellt wird

while(fread(&dateiinfo, sizeof(dateiinfo), 1, verwalt) == 1)
{
	if(strcmp(dateiinfo.dateiname, source) == 0)
	{
	fseek(speich,k*bz,SEEK_SET);

	while(bzaehl < dateiinfo.dateigroesse)
		{
		//memset(p,' ', 1);
		fread(p, sizeof(char), 1, speich);

			    if(hilf == 0)
			    {
	    		    temp = fopen(output, "w+");
					if(temp == NULL)
					{
					return 30;
					}
				hilf++;
			    }

		
		fwrite(p, sizeof(char), 1, temp);
		bzaehl = bzaehl + 1;
		}
	fclose(temp);
	free(p);
	return 0;
	}
k++;
}

free(p);

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
FILE *speich;
verwalt = fopen(pfadcpy1, "r+");
if(verwalt == NULL)
{
return 2;
}
speich = fopen(pfadcpy2, "r+");
if(speich == NULL)
{
return 2;
}

//Initialisierung von Blockcount und Blocksize aus der Struktur
fread(&dateiinfo,sizeof(dateiinfo),1,verwalt);
int64_t bc = dateiinfo.blockc;
int64_t bz = dateiinfo.blockz;
rewind(verwalt);

char *p;//Puffer für Speicherdatei
p = malloc(bz*sizeof(char));
memset(p,' ',bz*sizeof(char));
int64_t k = 0;
int schalter = 0;
while(fread(&dateiinfo,sizeof(dateiinfo), 1, verwalt) == 1)
{
	if(strcmp(dateiinfo.dateiname, target) == 0)
	{
	dateiinfo.blockbelegt = 0;
	dateiinfo.dateigroesse = 0;
	dateiinfo.blocknummer = 0;
	strcpy(dateiinfo.dateiname, "");
	fseek(verwalt,k*sizeof(dateiinfo),SEEK_SET);
	fwrite(&dateiinfo, sizeof(dateiinfo), 1, verwalt);
	fseek(speich, k*bz*sizeof(char),SEEK_SET);
	fwrite(p, bz*sizeof(char), 1, speich);
	schalter = 1;
	}
k++;
}
free(p);

fclose(verwalt);
fclose(speich);
if(schalter == 1)
{
defrag();
}
if(schalter == 0)
{
return 21;
}

}//Funktion Ende

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
fclose(verwalt);
int64_t bytes = 0;
bytes = bz*k;
printf("%lld", bytes);

return 0;
}




/////////////////////////////////////////////////////////////////Funktion 3.6 "used"////////////////////////////////////////////////////////////////////////////////
int used()
{
FILE *verwalt;
verwalt = fopen(pfadcpy1, "r");
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
	if(dateiinfo.blockbelegt == 1)
	{
	k = k+1;
	}
}
fclose(verwalt);
int64_t bytes = 0;
bytes = bz*k;
printf("%lld", bytes);

return 0;
}


//////////////////////////////////////////////////Funktion 3.7 "list"/////////////////////////////////////////////////////////////////////
int list()
{
char *prechar;
int b = 0; //boolischer Hilfswert

FILE *verwalt;
verwalt = fopen(pfadcpy1,"r");
if(verwalt == NULL)
{
return 2;
}

int schalter = 0;//Wenn prechar bereits einmal allokiert wurde. Dringend notwendig, da sonst Segfault.
int64_t k = 0; //für dritte if-Abfrage
while(fread(&dateiinfo, sizeof(dateiinfo), 1, verwalt) == 1)
{
	if(k != 0 )
	{
		if(strcmp(dateiinfo.dateiname, "") != 0 && schalter == 1)
		{
		
				if(strcmp(dateiinfo.dateiname, prechar) != 0) 
				{
				b = 0;
				free(prechar);
				printf("\n");
				}
		}
	}

	k = 1;
	
	if(b == 0 && (dateiinfo.dateigroesse != 0))
	{
	prechar = malloc(sizeof(dateiinfo.dateiname));
	strcpy(prechar, dateiinfo.dateiname);
	printf("%s",dateiinfo.dateiname);
	printf(",%lld",dateiinfo.dateigroesse);
	printf(",%lld",(((dateiinfo.dateigroesse)+(dateiinfo.blockz)-1)/dateiinfo.blockz));
	b = 1;
	schalter = 1;
	}

	if(strcmp(dateiinfo.dateiname,"") != 0)
	{
	printf(",%lld", dateiinfo.blocknummer);
	}
	

}
printf("\n");
free(prechar);
}

////////////////////////////////////////////////////////////////////Hilfsfunktion tausche() für defrag//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////Tausche die a unbelegten Blöcke durch die b belegten Blöcken von Block pos aus aus.///////////////////////////////////////
int tausche(int64_t a, int64_t b, int64_t pos)
{

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

char *p; //Puffer für die Speicherdatei
p = malloc(bz*sizeof(char));

fseek(verwalt,(pos+a)*sizeof(dateiinfo),SEEK_SET);
fseek(speich, (pos+a)*bz*sizeof(char),SEEK_SET);
int64_t s = 0;
int64_t t = 0;
while(s < b) //Ein Schleifendurchlauf pro belegtem Block, der verschoben werden soll
{
fread(&dateiinfo, sizeof(dateiinfo), 1, verwalt);
fseek(verwalt, (pos+s)*sizeof(dateiinfo),SEEK_SET);
dateiinfo.blocknummer = pos+s;
fwrite(&dateiinfo, sizeof(dateiinfo), 1, verwalt);
s = s+1;
fseek(verwalt, (pos+a+s)*sizeof(dateiinfo),SEEK_SET);

fread(p, bz*sizeof(char), 1, speich);
fseek(speich, ((pos+t)*bz)*sizeof(char),SEEK_SET);
fwrite(p, bz*sizeof(char), 1, speich);
t = t+1;
fseek(speich, ((pos+a+t)*bz)*sizeof(char),SEEK_SET);
}
//Jetzt müssen die virtuellen leeren Blöcke noch überschrieben werden
strcpy(dateiinfo.dateiname, "");
dateiinfo.blockbelegt = 0;
dateiinfo.blocknummer = 0;
dateiinfo.dateigroesse = 0;

fseek(verwalt,(pos+b)*sizeof(dateiinfo),SEEK_SET);
fseek(speich, (pos+b)*bz*sizeof(char),SEEK_SET);
int64_t l = 0;
memset(p,' ', bz*sizeof(char));
while(l < a)//So viele Schleifendurchläufe, wie es leere Blöcke gab.
{
fwrite(&dateiinfo, sizeof(dateiinfo), 1, verwalt);
fwrite(p, bz*sizeof(char), 1, speich);

l = l+1;
}

free(p);
fclose(verwalt);
fclose(speich);
}



////////////////////////////////////////////////////////////////////////////////////Funktion 3.8 "defrag"//////////////////////////////////////////////////////////////////////////////////////
int defrag()
{

FILE *verwalt;

verwalt = fopen(pfadcpy1, "r+");
if(verwalt == NULL)
{
return 2;
}

fread(&dateiinfo,sizeof(dateiinfo),1,verwalt);
int64_t bc = dateiinfo.blockc;
int64_t bz = dateiinfo.blockz;
int64_t va = dateiinfo.vanzahl;
char *prechar;
prechar = malloc(sizeof(dateiinfo.dateiname));
//printf("Die aktuelle Dateianzahl ist %d\n",dateiinfo.vanzahl);
rewind(verwalt);

int schalter = 0;
int64_t a = 0; //Anzahl freier Speicherblöcke vor einer Datei
int64_t b = 0; //Anzahl belegter Speicherblöcke durch die Datei
int64_t n = 0;
int64_t pos = 0;

while(fread(&dateiinfo, sizeof(dateiinfo), 1, verwalt) == 1)
{
	if(schalter == 1)
	{	
		if(n == 1)
		{
			if(strcmp(dateiinfo.dateiname, prechar) != 0) 
			{
			n = 0;
			fclose(verwalt);
			tausche(a,b,pos);
			verwalt = fopen(pfadcpy1,"r+");
			pos = pos+b;
			a = 0;
			b = 0;
			fseek(verwalt, pos*sizeof(dateiinfo), SEEK_SET);
			continue;
			}
		}
	}
	
		
	if(n == 0 && (dateiinfo.blockbelegt == 1))
	{
	prechar = realloc(prechar,sizeof(dateiinfo.dateiname)); 
	strcpy(prechar, dateiinfo.dateiname);
	n = 1;
	schalter = 1; //Dauerschalter
	}

	if(strcmp(dateiinfo.dateiname,"") != 0)
	{
	b = b+1;
	}
	if(strcmp(dateiinfo.dateiname,"") == 0)
	{
	a = a+1;
	}

}//Schleife zu
tausche(a,b,pos);
free(prechar);

}//Funktion zu


//argv[0] == "vfs", argv[1] == "archiv",argv[2] == "operation",argv[3,4] == "parameter",

int main(int argc, char *argv[])
{

if(argc < 3)
{
return 66;
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
					return 66;
			   	}
			h = create(argv[3], argv[4]);
			free(pfadcpy1);
			free(pfadcpy2);	
			return h;
			}

//add-Abfrage
			if(strcmp(argv[2], "add") == 0)
			{
				if(argc < 5)
				{	
					return 66;
			   	}
			h = add(argv[3], argv[4]);
			free(pfadcpy1);
			free(pfadcpy2);
			return h;
			}

//del-Abfrage
			if(strcmp(argv[2], "del") == 0)
			{
				if(argc < 4)
				{
					return 66;
			   	}
			h = del(argv[3]);
			free(pfadcpy1);
			free(pfadcpy2);
			return h;
			}



//defrag-Abfrage
			if(strcmp(argv[2], "defrag") == 0)
			{

				h = defrag();
				free(pfadcpy1);
				free(pfadcpy2);
				return h;
			}

//free-Abfrage
			if(strcmp(argv[2], "free") == 0)
			{

				h = freee();
				free(pfadcpy1);
				free(pfadcpy2);
				return h;
			}

//get-Abfrage
			if(strcmp(argv[2], "get") == 0)
			{ 
				if(argc < 5)
				{
					return 66;
			   	}
			h = get(argv[3], argv[4]);
			free(pfadcpy1);
			free(pfadcpy2);
			return h;
			}
//list-Abfrage
			if(strcmp(argv[2], "list") == 0)
			{

				h = list();
				free(pfadcpy1);
				free(pfadcpy2);
				return h;
			}
//used-Abfrage
			if(strcmp(argv[2], "used") == 0)
			{

				h = used();
				free(pfadcpy1);
				free(pfadcpy2);
				return h;

			}


//Abfrage für jede andere Eingabe
			else
			{
				free(pfadcpy1);
				free(pfadcpy2);
				return 66;

			}
pfadcpy1 = NULL;
pfadcpy2 = NULL;
return EXIT_SUCCESS;
}
