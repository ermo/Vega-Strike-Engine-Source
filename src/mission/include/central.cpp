/***************************************************************************
 *                           central.cpp  -  description
 *                           ----------------------------
 *                           begin                : January 18, 2002
 *                           copyright            : (C) 2002 by David Ranger
 *                           email                : sabarok@start.com.au
 **************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   any later version.                                                    *
 *                                                                         *
 **************************************************************************/

#include "central.h"
#ifdef _WIN32
#include <direct.h>
#else
#include <sys/dir.h>
#include <stdio.h>
#include <unistd.h>
#endif
glob_t *MISSIONS;
struct mission_data DATA;

// Primary initialization function. Sets everything up and takes care of the program
char *Start(int run_vegastrike) {
	InitGraphics();

	DATA.name = NewString("");
	DATA.author = NewString("");
	DATA.description = NewString("");
	DATA.briefing = NewString("");
	DATA.path = NewString("");

	ShowMain(1);
	gtk_main();
	return DATA.path;
}

void RunMission(void) {
	if (DATA.path[0] == '\0') { cout << "No mission selected\n"; return; }
	cout << "Starting " << MISSION_PROGRAM << " with mission " << DATA.path << endl;
#ifdef _WIN32
	char * execname = new char [strlen(DATA.path)+strlen (MISSION_PROGRAM)+200];
	char mypath[8000];
	getcwd (mypath,7999);
	sprintf (execname,"vegast~1 %s",DATA.path);
	system (execname);
	delete [] execname;
#else
	execlp("./vegastrike", "./vegastrike", DATA.path, NULL);
#endif
}
