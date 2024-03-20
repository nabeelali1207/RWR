/*
 * ftpserver.h
 *
 *  Created on: Oct 8, 2019
 *      Author: zumair
 */

#ifndef SERVERFTP_H_
#define SERVERFTP_H_

#include "../common/common.h"
#include "../io/systemstorage.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <shadow.h>
#include <pwd.h>
#include <signal.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <errno.h>
#include <crypt.h>

#define FTP_SESSION "SESSION"
#define FTP_DAEMON  "DAEMON "

#define LISTEN_ADDR INADDR_ANY

//#define W(s,d) "%d "s" \r\n",d
#define W(s,d) "%d %s \r\n",d,s
#define FTP_OK W("OK",RPL_OK)
#define FTP_RDY W("Welcome to PUP1P5 FTP", RPL_RDY)
#define FTP_QUIT W("Welcome back", RPL_QUIT)
#define FTP_NOFEATS W("No Features", RPL_NOFEATS)
#define FTP_HELP W("Help msg", RPL_OK)
#define FTP_NAMEOK W("User name okay, need password", RPL_NAMEOK)
#define FTP_ERR_NAME W("Cannot find user name. Ask for administrator.", 530)
#define FTP_NEED_LOGIN W("Need login. Login first.", 530)
#define FTP_LOGIN W("User logged in, proceed", RPL_LOGIN)
#define FTP_LOGIN_FAIL W("Password wrong! Please login again.", 530)
#define FTP_PWD W("%s", RPL_PWD)
#define FTP_SYST W("UNIX", RPL_SYST)
#define FTP_CTYPE W("data type changed to %c", RPL_OK)
#define FTP_ERR_DATATYPE W("error type change cmd, current data type is %c", RPL_ERR_UNKWNCMD)
#define FTP_PASV W("Enter passive mode (%d,%d,%d,%d,%d,%d)", RPL_PASVOK)
#define FTP_PORT W("PORT command success", RPL_OK)
#define FTP_ERR_PORT W("port command failed, parameter error", RPL_ERR_PARM)
#define FTP_ASCII W("Opening ASCII mode data connection for cmd %s", RPL_FILERDY)
#define FTP_BIN W("Opening Binary mode data connection for cmd %s", RPL_FILERDY)
#define FTP_REST W("restart at %d. use STORE or RETR to begin transfer", RPL_RESTOK)
#define FTP_ERR_PARAM W("cmd %s: wrong param", RPL_ERR_PARM)
#define FTP_TRSF_OK W("Transfer completed", RPL_TRSFOK)
#define FTP_ERROR W("FTP error: %s ", RPL_ERR_UNKWNCMD)
#define FTP_CDUP W("changd to parent directory success", RPL_CDUP)
#define FTP_CWD W("dir changed", RPL_CWD)
#define FTP_MDTM W("%s", RPL_FILEST)
#define FTP_SIZE W("%d", RPL_FILEST)
#define FTP_DELE W("Delete success", RPL_DELE)
#define FTP_MKDIR W("mkdir success", RPL_MKDIR)
#define FTP_RNFR W("RNFR success, waiting RNT", RPL_RNFR)
#define FTP_RNTO W("RNTO success", RPL_RNTO)
#define FTP_CMDNOIM W("command not implemented", RPL_CMDNOIM)

const uint32_t BUF_SIZE = 1024;
const uint32_t LISTEN_PORT = 2121;
const uint32_t MAX_CONNECTIONS = 2;
const uint32_t CMD_COUNT = 30;

#define RPL_RESTART 110
#define RPL_WAITRDY 120
#define RPL_DATACONN 125
#define RPL_FILERDY 150
#define RPL_OK 200
#define RPL_NOFEATS 211
#define RPL_FILEST 213
#define RPL_SYST 215
#define RPL_RDY 220
#define RPL_QUIT 221
#define RPL_TRSFOK 226
#define RPL_PASVOK 227
#define RPL_NAMEOK 331
#define RPL_CWD 250
#define RPL_CDUP 250
#define RPL_RNTO 250
#define RPL_DELE 250
#define RPL_PWD 257
#define RPL_MKDIR 257
#define RPL_LOGIN 230
#define RPL_RESTOK 350
#define RPL_RNFR 350
#define RPL_ERR_UNKWNCMD 500
#define RPL_ERR_PARM 501
#define RPL_ERR_IGN 202
#define RPL_ERR_NOTIMPL 502
#define RPL_BADSEQ 503
#define RPL_CMDNOIM 504

#define FTP_MAX_FILE_SIZE 41943040

//const uint16_t RPL_RESTART = 110;
//const uint16_t RPL_WAITRDY = 120;
//const uint16_t RPL_DATACONN = 125;
//const uint16_t RPL_FILERDY = 150;
//const uint16_t RPL_OK = 200;
//const uint16_t RPL_NOFEATS = 211;
//const uint16_t RPL_FILEST = 213;
//const uint16_t RPL_SYST = 215;
//const uint16_t RPL_RDY = 220;
//const uint16_t RPL_QUIT = 221;
//const uint16_t RPL_TRSFOK = 226;
//const uint16_t RPL_PASVOK = 227;
//const uint16_t RPL_NAMEOK = 331;
//const uint16_t RPL_CWD = 250;
//const uint16_t RPL_CDUP = 250;
//const uint16_t RPL_RNTO = 250;
//const uint16_t RPL_DELE = 250;
//const uint16_t RPL_PWD = 257;
//const uint16_t RPL_MKDIR = 257;
//const uint16_t RPL_LOGIN = 230;
//const uint16_t RPL_RESTOK = 350;
//const uint16_t RPL_RNFR = 350;
//const uint16_t RPL_ERR_UNKWNCMD = 500;
//const uint16_t RPL_ERR_PARM = 501;
//const uint16_t RPL_ERR_IGN = 202;
//const uint16_t RPL_ERR_NOTIMPL = 502;
//const uint16_t RPL_BADSEQ = 503;
//const uint16_t RPL_CMDNOIM = 504;

enum DATA_TYPE {
    TYPE_ASCII,
    TYPE_EBDIC,
    TYPE_IMAGE,
    TYPE_LOCAL,
    TYPE_COUNT
};

enum TRSF_TYPE {
    TRSF_PASV,
    TRSF_PORT,
    TRSF_COUNT,
};

enum FTP_CMD {
    INVALID = -1,
    USER,
    PASS,
    RETR,
    STOR,
    STOU,
    APPE,
    REST,
    RNFR,
    RNTO,
    ABOR,
    DELE,
    RMD,
    MKD,
    PWD,
    CWD,
    CDUP,
    LIST,
    NLST,
    SITE,
    STAT,
    HELP,
    NOOP,
    TYPE,
    PASV,
    PORT,
    SYST,
    QUIT,
    MDTM,
	FEAT,
    SIZE,
    FTP_CMD_COUNT
};

struct ftp_cmd {
    char const *name;
    enum FTP_CMD cmd;
};

#ifdef SERVERFTP_CPP_
int server = -1, client = -1, running = 0;
pid_t forkpid = 1;
char buf[BUF_SIZE];
#else
extern int32_t server, client, running;
extern pid_t forkpid;
extern char buf[BUF_SIZE];
extern struct ftp_cmd FTP_CMD_LIST[CMD_COUNT];
#endif

enum FTP_CMD parse_cmd(char *buf, int len);
void handle_session(int client);
void ouch(int n);

// ZLOG FUNCTIONS
void err(int type, const char *fmt, ...);
void warn(int type, const char *fmt, ...);
void info(int type, const char *fmt, ...);

// UTILS FUNCTIONS
struct sockaddr new_addr(uint32_t inaddr, unsigned short port);
int new_server(uint32_t inaddr, uint16_t port, int backlog);
int new_client(uint32_t srv_addr, unsigned short port);
int send_str(int peer, const char* fmt, ...);
int send_file(int peer, FILE *f);
int send_path(int peer, char *file, uint32_t offset);
int recv_file(int peer, FILE *f);
int recv_path(int peer, char *file, uint32_t offset);
int parse_number(const char *buf, uint32_t *number);
int parse_addr_port(const char *buf, uint32_t *addr, uint16_t *port);
char * parse_path(const char *buf);
char * n2a(uint32_t addr);

#endif /* SERVERFTP_H_ */
