/*
 * cdn_tester.cpp
 *
 *  Created on: 2015-9-28
 *      Author: Max.Chiu
 *      Email: Kingsleyyau@gmail.com
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

#include <string>
#include <map>
using namespace std;

#include <common/KLog.h>
#include <common/CommonFunc.h>

#include <LSPlayerImp.h>

#define VERSION_STRING "1.0.0"
bool Parse(int argc, char *argv[]);
void SignalFunc(int sign_no);

char url[1024] = {"rtmp://192.168.88.17:1936/aac/max"};
char flv[1024] = {"record/play.flv"};
char h264[1024] = {"record/play.h264"};
char aac[1024] = {"record/play.aac"};

int main(int argc, char *argv[]) {
	printf("############## cdn_tester ############## \n");
	printf("# Version : %s \n", VERSION_STRING);
	printf("# Build date : %s %s \n", __DATE__, __TIME__);
	srand(time(0));

	/* Ignore */
	struct sigaction sa;
	sa.sa_handler = SIG_IGN;
	sigemptyset(&sa.sa_mask);
	sigaction(SIGPIPE, &sa, 0);

	/* Handle */
	memset(&sa, 0, sizeof(sa));
	sa.sa_handler = SignalFunc;
	sa.sa_flags = SA_RESTART;
	sigemptyset(&sa.sa_mask);

//	sigaction(SIGHUP, &sa, 0);
	sigaction(SIGINT, &sa, 0); // Ctrl-C
	sigaction(SIGQUIT, &sa, 0);
	sigaction(SIGILL, &sa, 0);
	sigaction(SIGABRT, &sa, 0);
	sigaction(SIGFPE, &sa, 0);
	sigaction(SIGBUS, &sa, 0);
	sigaction(SIGSEGV, &sa, 0);
	sigaction(SIGSYS, &sa, 0);
	sigaction(SIGTERM, &sa, 0);
	sigaction(SIGXCPU, &sa, 0);
	sigaction(SIGXFSZ, &sa, 0);

	Parse(argc, argv);

	bool bFlag = true;

	MakeDir("./log");
	MakeDir("./record");

	KLog::SetLogEnable(true);
	KLog::SetLogDirectory("./log");

	LSPlayerImp player;

	while( bFlag ) {
		/* do nothing here */
		if( player.IsRuning() ) {
			sleep(5);

		} else {
			// 断线重连
			player.Stop();
			player.PlayUrl(url, flv, h264, aac);
		}
	}

	return EXIT_SUCCESS;
}

bool Parse(int argc, char *argv[]) {
	string key, value;
	for( int i = 1; (i + 1) < argc; i+=2 ) {
		key = argv[i];
		value = argv[i+1];

//		if( key.compare("-f") == 0 ) {
//			sConf = value;
//		}
	}

	return true;
}

void SignalFunc(int sign_no) {
	switch(sign_no) {
	default:{
		signal(sign_no, SIG_DFL);
		kill(getpid(), sign_no);
//		exit(EXIT_SUCCESS);
	}break;
	}
}