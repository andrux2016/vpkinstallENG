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

#include <stdio.h>
#include <malloc.h>
#include <string.h>

#include <psp2/io/fcntl.h>
#include <psp2/kernel/processmgr.h>
#include <psp2/net/net.h>
#include <psp2/net/netctl.h>
#include <psp2/net/http.h>
#include <psp2/sysmodule.h>
#include "graphics.h"

#define printf psvDebugScreenPrintf

void netInit() {
	sceSysmoduleLoadModule(SCE_SYSMODULE_NET);

	SceNetInitParam netInitParam;
	int size = 10*1024*1024;
	netInitParam.memory = malloc(size);
	netInitParam.size = size;
	netInitParam.flags = 0;
	sceNetInit(&netInitParam);

	sceNetCtlInit();
}

void netTerm() {
	sceNetCtlTerm();
	sceNetTerm();
	sceSysmoduleUnloadModule(SCE_SYSMODULE_NET);
}

void httpInit() {
	sceSysmoduleLoadModule(SCE_SYSMODULE_HTTP);
	sceHttpInit(10*1024*1024);
}

void httpTerm() {
	sceHttpTerm();
	sceSysmoduleUnloadModule(SCE_SYSMODULE_HTTP);
}

void download(const char *src, const char *dst) {
	int ret;
	psvDebugScreenSetFgColor(COLOR_CYAN);
	printf("\n\n");
	printf("Downloading:\n\n");
	int tpl = sceHttpCreateTemplate("henkaku offline", 1, 1);
	if (tpl < 0) {
		printf("sceHttpCreateTemplate: 0x%x\n", tpl);
		return;
	}
	int conn = sceHttpCreateConnectionWithURL(tpl, src, 0);
	if (conn < 0) {
		printf("sceHttpCreateConnectionWithURL: 0x%x\n", conn);
		return;
	}
	int req = sceHttpCreateRequestWithURL(conn, SCE_HTTP_METHOD_GET, src, 0);
	if (req < 0) {
		printf("sceHttpCreateRequestWithURL: 0x%x\n", req);
		return;
	}
	ret = sceHttpSendRequest(req, NULL, 0);
	if (ret < 0) {
		printf("sceHttpSendRequest: 0x%x\n", ret);
		return;
	}
	unsigned char buf[16*1024] = {0};

	long long length = 0;
	ret = sceHttpGetResponseContentLength(req, &length);

	int fd = sceIoOpen(dst, SCE_O_WRONLY | SCE_O_CREAT , 0777);
	int total_read = 0;
	if (fd < 0) {
		printf("sceIoOpen: 0x%x\n", fd);
		return;
	}
	// draw progress bar background
	float lastpercent = 0;
	float percent = 0;
	int percentX = psvDebugScreenGetX() + 10;
	int percentY = psvDebugScreenGetY();
	psvDebugScreenSetXY(percentX,percentY);
	psvDebugScreenSetFgColor(COLOR_CYAN);
	printf("%d%%\n",((int)lastpercent*100));
	int barX = psvDebugScreenGetX();
	
	int fg_color = 0xFF333333;int barY = psvDebugScreenGetY();
	draw_rect(barX, barY, PROGRESS_BAR_WIDTH, PROGRESS_BAR_HEIGHT, fg_color);
	fg_color = 0xFFFFFF00;
	while (1) {
		int read = sceHttpReadData(req, buf, sizeof(buf));
		if (read < 0) {
			printf("sceHttpReadData error! 0x%x\n", read);
			return;
		}
		if (read == 0)
			break;
		ret = sceIoWrite(fd, buf, read);
		if (ret < 0 || ret != read) {
			printf("sceIoWrite error! 0x%x\n", ret);
			if (ret < 0)
				return;
			return;
		}
		total_read += read;
		if(percent < ((total_read+0.0)/length)){
			// delete lastpercent from screen
			psvDebugScreenSetXY(percentX,percentY);
			psvDebugScreenSetFgColor(COLOR_BLACK);
			printf("%d%%\n",((int)(lastpercent*100)));
			// draw bar
			psvDebugScreenSetFgColor(fg_color);
			lastpercent = percent;
			percent = (total_read+0.0)/length;
			draw_rect(barX + 1, barY + 1, (PROGRESS_BAR_WIDTH - 2) * percent, PROGRESS_BAR_HEIGHT - 2, fg_color);
			// draw percent
			psvDebugScreenSetXY(percentX,percentY);
			psvDebugScreenSetFgColor(COLOR_WHITE);
			printf("%d%%\n\n",((int)(percent*100)));
		}
	}
	printf("\n\n");

	ret = sceIoClose(fd);
	if (ret < 0)
		printf("sceIoClose: 0x%x\n", ret);
}
