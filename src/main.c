/*
	VPKMirror
	Copyright (C) 2016, SMOKE

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "main.h"
#include "init.h"
#include "net.h"
#include "sqlite3.h"
#include <psp2/ctrl.h>

#define APP_DB "ur0:/shell/db/app.db"

int sceAppMgrGetAppParam(char *param);

void sql_simple_exec(sqlite3 *db, const char *sql) {
	char *error = NULL;
	int ret = 0;
	ret = sqlite3_exec(db, sql, NULL, NULL, &error);
	if (error) {
		printf("Failed to execute %s: %s\n", sql, error);
		sqlite3_free(error);
		goto fail;
	}
	return;
fail:
	sqlite3_close(db);
}

void do_uri_mod(void) {
	int ret;

	sqlite3 *db;
	ret = sqlite3_open(APP_DB, &db);
	if (ret) {
		printf("Failed to open DB: %s\n", sqlite3_errmsg(db));
	}

	sql_simple_exec(db, "DELETE FROM tbl_uri WHERE titleId='VPKINSTAL'");
	sql_simple_exec(db, "INSERT INTO tbl_uri VALUES ('VPKINSTAL', '1', 'vdi', NULL)");

	sqlite3_close(db);
	db = NULL;

	return;
}

int main() {
	psvDebugScreenInit();
	initSceAppUtil();
psvDebugScreenSetFgColor(COLOR_CYAN);
	printf("VpkDirectInstallerMod By Draqen, Jarki26 and Befeeter\n\n");
	printf("Thanks SMOKE, haxxey  and HaiHakkuIku for VPKMirror\n\n");
	printf("------------------------------------------------------------\n\n");

	/* grab app param from our custom uri
	   full app param looks like:
	   type=LAUNCH_APP_BY_URI&uri=vpk:install?test
	*/
	char AppParam[1024];
	sceAppMgrGetAppParam(AppParam);

	// checks if argument is present, if not it does the uri mod and opens the website
	int arg_len = strlen(AppParam);
	if (arg_len == 0) {
		do_uri_mod();
		psvDebugScreenSetFgColor(COLOR_YELLOW);
		printf("VpkDirectInstallerMod installation ok...\n\n");
		printf("No need to start the application manually\n");
		printf("To DOWNLOAD and INSTALL\n\n");
		printf("Exiting in 10 seconds...\n");
		sceKernelDelayThread(10 * 1000 * 1000); // 5 seconds
		sceKernelExitProcess(0);
	}


	// argument recieved, init everything else
	netInit();
	httpInit();

	

	// get the part of the argument that we need
	char *file_name_download;
	file_name_download = strchr(AppParam, '?')+1;

	char *file_name;
	file_name = strchr(AppParam, '/')+1;


	while(strchr(file_name, '/') != NULL ){//Botched? way to get last one of /
		file_name = strchr(file_name, '/')+1;
	}

	// get the file type
	char *file_type;
	file_type = strchr(file_name, '.')+1;

	while(strchr(file_type, '.') != NULL ){//Botched? way to get last one of .
		file_type = strchr(file_type, '.')+1;
	}

	// create url based off the file name
	char *file_url = malloc(512 * sizeof(char));
	snprintf(file_url, 512, "http://%s", file_name_download);

	SceCtrlData pad;

	if(strcmp(file_type, "vpk")==0){
		psvDebugScreenSetFgColor(COLOR_YELLOW);
		psvDebugScreenPrintf("The file %s Is going to DOWNLOAD and INSTALL\n\n", file_name);
		psvDebugScreenSetFgColor(COLOR_YELLOW);
		psvDebugScreenPrintf("For DOWNLOAD and INSTALL %s press START\n\n", file_name);
		psvDebugScreenSetFgColor(COLOR_RED);
		printf("VPK files can BRICK your console,\n");
		printf("Vpks may be harmfull only use vpks you trust!!!\n\n");
		printf("CANCEL the download and CLOSE the application press O\n\n");		
	}

	if(strcmp(file_type, "iso")==0){
		psvDebugScreenSetFgColor(COLOR_YELLOW);
		psvDebugScreenPrintf("The file %s Is going to DOWNLOAD and INSTALL\n\n", file_name);
		psvDebugScreenSetFgColor(COLOR_YELLOW);
			psvDebugScreenPrintf("For DOWNLOAD and INSTALL %s press START\n\n", file_name);
		psvDebugScreenSetFgColor(COLOR_RED);
		printf("CANCEL the download and CLOSE the application press O\n\n");			
	}
	if(strcmp(file_type, "cso")==0){
			psvDebugScreenSetFgColor(COLOR_YELLOW);
		psvDebugScreenPrintf("The file %s Is going to DOWNLOAD and INSTALL\n\n", file_name);
		psvDebugScreenSetFgColor(COLOR_YELLOW);
			psvDebugScreenPrintf("For DOWNLOAD and INSTALL %s press START\n\n", file_name);
		psvDebugScreenSetFgColor(COLOR_RED);
		printf("CANCEL the download and CLOSE the application press O\n\n");			
	}

	

	while (1) {
		sceCtrlPeekBufferPositive(0, &pad, 1);
		if (pad.buttons == SCE_CTRL_START) {
			// download file
		psvDebugScreenSetFgColor(COLOR_YELLOW);
		printf("\n%s\n\n", file_name);



			char *file_path = malloc(512 * sizeof(char));

			if(strcmp(file_type, "vpk")==0){
				snprintf(file_path, 512, "ux0:/pspemu/ISO/%s", file_name);
			}
			if(strcmp(file_type, "iso")==0){
				snprintf(file_path, 512, "ux0:/pspemu/ISO/%s", file_name);


				SceUID fd;	
				fd = sceIoOpen("ux0:/pspemu/log.txt", SCE_O_WRONLY|SCE_O_CREAT, 0777);

				char* data=0;
				int fdSize=0; 

				fdSize = sceIoLseek(fd, 0, SCE_SEEK_END); 

				data = (char*) malloc(fdSize);	
				sceIoRead(fd, data, fdSize);
				

				size_t len_file_path = strlen(file_path);
				
				char *new_entry = malloc(fdSize + len_file_path + 2 + 1 );
				strcat(new_entry, data);
				strcat(new_entry, file_path);
				strcat(new_entry, " 1");


				sceIoWrite(fd, new_entry, fdSize); 

				sceIoClose(fd);

				
			}
			if(strcmp(file_type, "cso")==0){
				snprintf(file_path, 512, "ux0:/pspemu/ISO/%s", file_name);	
			}
			
			download(file_url, file_path);

			if(strcmp(file_type, "vpk")==0){
				// install vpk
				printf("Installing PsVita package %s...\n\n", file_name);
				installPackage(file_path);
				
				// cleanup
				sceIoRemove(file_path);
			}
			if(strcmp(file_type, "iso")==0){
				// install iso
			psvDebugScreenSetFgColor(COLOR_GREEN);
			printf("Ready\n\n");
			printf("\nExiting in 5 seconds...\n");
			sceKernelDelayThread(5 * 1000 * 1000);
			httpTerm();
			netTerm();
			finishSceAppUtil();
			sceKernelExitProcess(0);
			}
			if(strcmp(file_type, "cso")==0){
				// install iso
			psvDebugScreenSetFgColor(COLOR_GREEN);
			printf("Ready\n\n");
			printf("Press X to close the APPLICATION");			printf("\nExiting in 5 seconds...\n");
			sceKernelDelayThread(5 * 1000 * 1000);
			httpTerm();
			netTerm();
			finishSceAppUtil();
			sceKernelExitProcess(0);
				
			}
			
	
		}
		if (pad.buttons == SCE_CTRL_CIRCLE) {
			printf("\nExiting in 5 seconds...\n");
			sceKernelDelayThread(5 * 1000 * 1000);
			httpTerm();
			netTerm();
			finishSceAppUtil();
			sceKernelExitProcess(0);
		
while (1) {
		sceCtrlPeekBufferPositive(0, &pad, 1);
		if (pad.buttons == SCE_CTRL_CROSS) {
			
			httpTerm();
			netTerm();
			finishSceAppUtil();

			sceKernelExitProcess(0);
		}}
		
	}

}}
