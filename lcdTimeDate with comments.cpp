/*
 *	C++ Quellcode fuer LCD-Anzeige (1602A)
 *	zur Ausgabe der Uhrzeit mit Zeitzone, des Wochentages & des Datums.
 *	Laufzeitumgebung: Linux auf Raspberry Pi 3 Model 8 V1.2
 *	Format:		hh:mm:ss zone
 *			   Tag dd mon yyyy
 */


#include <stdio.h>
#include <time.h>
#include <lcd.h>
#include <wiringPi.h>



int disp;															//	Variable fuer Anzeige



PI_THREAD(refreshDisp)												//	Thread fuer Displayaktualisierung
{
	time_t tnow;													//	Datentyp d. Zeit (time.h)
	char tme[17];													//	Puffer Zeit
	char dte[17];

	lcdClear(disp);
	while (TRUE)
	{
		time(&tnow);												//	Zeitabfrage
																	//	Formatierung
		strftime(tme, 17, " %H:%M:%S  %Z  ", localtime(&tnow));		//	Puffer schreiben
		strftime(dte, 17, "%a %e %b %G ", localtime(&tnow));
		for (int i = 0; i < 16; i++)
		{
			lcdPosition(disp, i, 0);								//	Cursor-Position setzen
			lcdPutchar(disp, tme[i]);
			lcdPosition(disp, i, 1);
			lcdPutchar(disp, dte[i]);
		}
	}
}






int initDisplay()
{
	disp = lcdInit(													//	Initialisierung LCD-Anzeige 1602A
		2,															//		Zeilen
		16,															//		Spalten
		4,															//		Bandbreite Daten (bit)
		2,															//		rs-pin (register select)
		3,															//		strb-pin (enable)
		6, 5, 4, 1,													//		4-bit Daten  (D0, D1, D2, D3)
		0, 0, 0, 0													//		8-bit Daten +(D4, D5, D6, D7)
	);

	printf("display: %d\n", disp);

	if (disp == -1)													//	Initialisierung fehlerfrei?
	{
		printf("lcdInit 1 failed\n");
		return -1;
	}

	lcdClear(disp);													//	LCD-Anzeige loeschen
	delay(100);

	return 0;
}



int main(int argc, char** args)
{
	printf("application: lcdTimeDate\n");

	if (wiringPiSetup() == -1)										//	Setup der wiringPi Bibliothek
	{
		printf("setup wiringPi failed\n");
		return -1;
	}


	initDisplay();													//	Anzeige initialisieren

	piThreadCreate(refreshDisp);									//	Aktual. der Anzeige starten

	while (TRUE)													//	Laufzeit der Threads sichern
	{
		delay(10000);
	}

	return 0;														//	(Hauptprogramm beenden)
}