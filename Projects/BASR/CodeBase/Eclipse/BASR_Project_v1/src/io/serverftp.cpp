/*
 * ftpserver.cpp
 *
 *  Created on: Oct 8, 2019
 *      Author: zumair
 */

#define SERVERFTP_CPP_

#include "../io/serverftp.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

struct ftp_cmd FTP_CMD_LIST[FTP_CMD_COUNT] = {
    {"USER", USER},
    {"PASS", PASS},
    {"RETR", RETR},
    {"STOR", STOR},
    {"STOU", STOU},
    {"APPE", APPE},
    {"REST", REST},
    {"RNFR", RNFR},
    {"RNTO", RNTO},
    {"ABOR", ABOR},
    {"DELE", DELE},
    {"RMD" , RMD },
    {"MKD" , MKD },
    {"PWD" , PWD },
    {"CWD" , CWD },
    {"CDUP", CDUP},
    {"LIST", LIST},
    {"NLST", NLST},
    {"SITE", SITE},
    {"STAT", STAT},
    {"HELP", HELP},
    {"NOOP", NOOP},
    {"TYPE", TYPE},
    {"PASV", PASV},
    {"PORT", PORT},
    {"SYST", SYST},
    {"QUIT", QUIT},
    {"MDTM", MDTM},
	{"FEAT", FEAT},
    {"SIZE", SIZE}
};

enum FTP_CMD parse_cmd(char *buf, int len) {
    int i,j;
    for (i=0; i<FTP_CMD_COUNT; i++) {
        for (j=0; FTP_CMD_LIST[i].name[j] != '\0' && j < len; j++) {
            if (FTP_CMD_LIST[i].name[j] != buf[j] && FTP_CMD_LIST[i].name[j] != buf[j]-32) break;
        }
        if (FTP_CMD_LIST[i].name[j] == '\0')
            return FTP_CMD_LIST[i].cmd;
    }
    return INVALID;
}

void err(int type, const char *fmt, ...) {
    printf("\033[22;31m [ EE ] [ %s %d ]: ", type ? FTP_SESSION : FTP_DAEMON, getpid());
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
    printf("\e[0m\n");
}

void warn(int type, const char *fmt, ...) {
    printf("\033[22;33m [ WW ] [ %s %d ]: ", type ? FTP_SESSION : FTP_DAEMON, getpid());
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
    printf("\e[0m\n");
}

void info(int type, const char *fmt, ...) {
    printf("\033[22;32m [ II ] [ %s %d ]: ", type ? FTP_SESSION : FTP_DAEMON, getpid());
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
    printf("\e[0m\n");
}

struct sockaddr new_addr(uint32_t inaddr, unsigned short port) {
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(inaddr);
    addr.sin_port = htons(port);
    return *(struct sockaddr *)&addr;
}

int new_server(uint32_t inaddr, uint16_t port, int backlog) {
//    int ret = 0;
    int server;
    server = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr addr = new_addr(inaddr, port);
    if (bind(server, &addr, sizeof(addr)) < 0) {
        return -2;
    }
    if (listen(server, backlog) < 0) {
        return -3;
    }
    return server;
}

/**
 * new client
 * @return {int} status, -2 create socket error, -1 connect error
 */
int new_client(uint32_t srv_addr, unsigned short port) {
    int client = socket(AF_INET, SOCK_STREAM, 0);
    if (client < 0) return -2;
    struct sockaddr server = new_addr(srv_addr, port);
    int st = connect(client, &server, sizeof(server));
    if (st < 0) return -1;
    return client;
}

int send_str(int peer, const char* fmt, ...) {
    va_list args;
    char msgbuf[BUF_SIZE];
    va_start(args, fmt);
    vsnprintf(msgbuf, sizeof(msgbuf), fmt, args);
    va_end(args);
    return send(peer, msgbuf, strlen(msgbuf), 0);
}

/**
 * -1 error, 0 ok
 */
int send_file(int peer, FILE *f) {
    char filebuf[1025];
    int n= 0;
    int ret = 0;

    while ((n=(fread(filebuf, 1, sizeof(filebuf), f))) > 0) {
        int st = send(peer, filebuf, n, 0);
        if (st < 0) {
            err(1, "send file error, errno = %d, %s", errno, strerror(errno));
            ret = -1;
            break;
        } else {
            filebuf[n] = 0;
            info(1, "%d bytes sent", st);
        }
    }
    return ret;
}

/**
 *  -1 error opening file, -2 send file error, -3 close file error
 */
int send_path(int peer, char *file, uint32_t offset) {
    FILE *f = fopen(file, "rb");

    if(strstr(file, ".tle") != NULL) {
    	return -1;
    }

    if (f) {
        fseek(f, offset, SEEK_SET);
        int st = send_file(peer, f);
        if (st < 0) {
            return -2;
        }
    } else {
        return -1;
    }
    int ret = fclose(f);
    return ret == 0 ? 0 : -3;
}

int recv_file(int peer, FILE *f) {
	char filebuf[BUF_SIZE ];
    int n;
    unsigned long long sz = 0;
    unsigned long long fsz = 0;
    while (((n=recv(peer, filebuf, BUF_SIZE, 0)) > 0)) {
    	sz += n;
        fwrite(filebuf, 1, n , f);
        fsz =ftell(f);


//        printf("%llu\t%llu\n", sz, fsz);

        if(fsz > FTP_MAX_FILE_SIZE)
        {
        	break;
        }
    }
    return n;
}

/**
 * recv file by file path
 * @param {int} peer, peer socket
 * @param {char *} file path
 * @param {int} offset
 * @return {int} status,
 *              -1 means recv_file error,
 *              -2 means file open failure,
 *              EOF means close file error
 *
 */
int recv_path(int peer, char *file, uint32_t offset) {
    FILE *f = fopen(file, "wb");
    if (!f) return -2;
    fseek(f, offset, SEEK_SET);
    int st = recv_file(peer, f);
    int cl = fclose(f);
    return st < 0 ? st : cl;
}

int parse_number(const char *buf, uint32_t *number) {
    int f = -1;
    uint32_t i;
    char tmp[BUF_SIZE] = {0};
    int ret = -1;
    for (i=0; buf[i]!=0 && i<BUF_SIZE; i++) {
        if (!isdigit(buf[i])) {
            if (f >= 0) {
                memcpy(tmp, &buf[f], i-f);
                tmp[i-f] = 0;
                *number = atoi(tmp);
                ret = 0;
                f = -1;
                break;
            }
        } else {
            if (f < 0) {
                f = i;
            }
        }
    }
    return ret;
}

int parse_addr_port(const char *buf, uint32_t *addr, uint16_t *port) {
    uint32_t i;
    *addr = *port = 0;
    int f = -1;
    char tmp[BUF_SIZE] = {0};
    int cnt = 0;
    int portcnt = 0;
    for(i=0; buf[i]!=0 && i<BUF_SIZE; i++) {
        if(!isdigit(buf[i])) {
            if (f >= 0) {
                memcpy(tmp, &buf[f], i-f);
                tmp[i-f] = 0;
                if (cnt < 4) {
                    *addr = (*addr << 8) + (0xff & atoi(tmp));
                    cnt++;
                } else if (portcnt < 2) {
                    *port = (*port << 8) + (0xff & atoi(tmp));
                    portcnt++;
                } else {
                    break;
                }
                f = -1;
            }
        } else {
            if (f < 0) {
                f = i;
            }
        }
    }
    return cnt == 4 && portcnt == 2;
}

char * parse_path(const char *buf) {
    char * path = (char *)malloc(BUF_SIZE);
    uint32_t i, j;
    for (i=0; buf[i]!=' ' && i < BUF_SIZE; i++);
    if (i == BUF_SIZE) return NULL;
    i++;
    for (j=i; buf[j]!='\r' && buf[j]!= '\n' && j < BUF_SIZE; j++);
    memcpy(path, &buf[i], j-i);
    path[j-i] = 0;
    printf("%s\n", path);
    return path;
}

char * n2a(uint32_t addr) {
    uint32_t t = htonl(addr);
    return inet_ntoa(*(struct in_addr *)&t);
}

/**
 * handle a newly accepted ftp session
 *
 */
void handle_session(int client) {
    send_str(client, FTP_RDY);
    uint32_t i, n;
    int retry;
    char cwd[BUF_SIZE] = {0}, cmdbuf[BUF_SIZE + 15] = {0}; // +15 to remove warning
    enum DATA_TYPE datatype = TYPE_IMAGE;
    srand(time(0));
    uint32_t pasv_port = 0;
    enum TRSF_TYPE trsf_type = TRSF_PORT;
    int pasv_server = -1;
    struct sockaddr_in svr_addr;
    int svr_addr_len = sizeof(svr_addr);
    getsockname(client, (struct sockaddr*)&svr_addr, (unsigned int*)&svr_addr_len);
//    uint32_t svr_host_addr = ntohl(svr_addr.sin_addr.s_addr);
    uint32_t port_address = 0;
    uint16_t port_port = 0;
    int data_client = -1;
    struct sockaddr_in data_client_addr;
    int data_client_len = sizeof(data_client_addr);
    uint32_t restdata = 0;
    char rnfr[BUF_SIZE];
    char *p = NULL;            // tmp file path
    struct stat file_stat;      // file stat for time and size
    struct tm mdtime;

    /* Updated Nov 8 2016 by @Holmes1st */
    /* Variables for login process */
    char *input_id;      // inputed user name
    char *input_pw;      // inputed password data
    char *input_hash;    // hash generated with input id & pw
    char shadow_salt[BUF_SIZE];   // parsed from shadow->pwdp

    char msgbuf[1024];
    struct spwd *shadow = 0; // shadow data // Check shadow.h
//    struct passwd *passwd_data; // passwd data for uid, gid  //check pwd.h
    int count;
    int flag=0;          // 0: not logined  1: id check  2: logined

    //int stringLength = 0;

    FILE *p1;


    while ((n=recv(client, buf, BUF_SIZE, MSG_PEEK)) > 0) {
        if (!running) break;
        buf[n] = '\0';
        //info(1, "recved %d bytes: %s", n, buf);
        for (i=0; i<n; i++) {
            if (buf[i] == '\n') break;
        }
        if (buf[i] != '\n') {
            err(1, "no line break found");
            break;
        }
        n = recv(client, buf, i+1, 0);
        buf[n] = '\0';
        enum FTP_CMD cmd = parse_cmd(buf, n);
        if (cmd < 0) {
            buf[n-2] = 0;
            err(1, "unknown cmd: %s", buf);
            continue;
        }
        info(1, "cmd: %s, %d", FTP_CMD_LIST[cmd].name, cmd);
        if (((flag == 0) && (cmd != USER) && (cmd != QUIT)) || ((flag == 1) && (cmd != PASS) && (cmd != QUIT))){
            send_str(client, FTP_NEED_LOGIN);
        }
        else {
            switch(cmd) {
                case NOOP:
                {
                    send_str(client, FTP_OK);
                }break;
                case QUIT:
                {
                    send_str(client, FTP_QUIT);
                    running = 0;
                    flag = 0;
                }break;
                case HELP:
                {
                    send_str(client, FTP_HELP);
                }break;
                case FEAT:
                {
                    send_str(client, FTP_NOFEATS);
                }break;
                case USER:
                {
                    input_id = buf;

                    // cut id
                    while(*(input_id-1) != ' ') input_id++;
                    // remove <CRLF>
                    for (i = 0; i<strlen(input_id);i++){
                        if (input_id[i] == '\r' || input_id[i] == '\n')
                            input_id[i] = 0;
                    }

                    // get shadow data from /etc/shadow
                    shadow = getspnam(input_id);
                    if (shadow){
                        send_str(client, FTP_NAMEOK);
                        info(1,"user %s trying to login...",shadow->sp_namp);
                        flag = 1;
                    }
                    else {
                        send_str(client, FTP_ERR_NAME);
                        info(1, "%s login failed.", input_id);
                    }
                    // send_str(client, FTP_NAMEOK);
                }break;
                case PASS:
                {
                    input_pw = buf;

                    // cut pw
                    while (*(input_pw-1) != ' ') input_pw++;
                    // remove <CRLF>
                    for (i=0;i<strlen(input_pw);i++)
                        if (input_pw[i] == '\r' || input_pw[i] == '\n')
                            input_pw[i] = 0;

                    // get salt from shadow data
                    count = 0;
                    for(i = 0; i < 3;){
                        if (shadow->sp_pwdp[count++] == '$')
                            i++;
                    }
                    strncpy(shadow_salt, shadow->sp_pwdp, count);
                    input_hash = crypt(input_pw, shadow_salt);

                    if (strcmp(input_hash, shadow->sp_pwdp) == 0){
                        send_str(client, FTP_LOGIN);
                        info(1, "user %s logged in.", shadow->sp_namp);

                        // get data from /etc/passwd for uid and gid
//                        passwd_data = getpwnam(shadow->sp_namp);
//                        setgid(passwd_data->pw_gid);
//                        setuid(passwd_data->pw_uid);
                        flag = 2;
                    }
                    else {
                        send_str(client, FTP_LOGIN_FAIL);
                        info(1, "user %s:%s logged in fail.", shadow->sp_namp,input_pw);
                        flag = 0;
                    }
                }break;
                case PWD:
                {
                    getcwd(cwd, sizeof(cwd));
                    sprintf(msgbuf,"257 ");

                    if(strstr(cwd, "/home/dtm") == NULL) {
						strcpy(cwd, "");
						strcpy(cwd, "/home/dtm");
                    }
                   // stringLength =
                    sprintf(msgbuf + strlen(msgbuf), "%s", cwd);
                    sprintf(msgbuf + strlen(msgbuf), "\r\n");
                    send(client, msgbuf, strlen(msgbuf), 0);
                    info(1, "pwd: %s", cwd);
                   //send_str(client, FTP_PWD, cwd);
                }break;
                case SYST:
                {
                    send_str(client, FTP_SYST);
                  //  strcpy(buf, "TYPE I\r\n");
                    //buf[5] = 'I';
                   // send_str(client, FTP_CTYPE, buf[5]);

                }break;
                case TYPE:
                {
                    if (buf[5] == 'A') {
                        datatype = TYPE_ASCII;
                        send_str(client, FTP_CTYPE, buf[5]);
                    } else if (buf[5] == 'I') {
                        datatype = TYPE_IMAGE;
                        send_str(client, FTP_CTYPE, buf[5]);
                    } else {
                        send_str(client, FTP_ERR_DATATYPE, datatype == TYPE_ASCII ? 'A' : 'I');
                    }
                }break;
                case PASV:
                {
                    retry = 100;
                    while (retry--) { // in case of create server error, port used
                        pasv_port = (rand() % 64512 + 1024);
                        trsf_type = TRSF_PASV;
                        pasv_server = new_server(INADDR_ANY, pasv_port, 1);
                        if (pasv_server >= 0) break;
                    }
                    if (pasv_server < 0) {
                        err(1, "can not create pasv port for passive mode");
                        // send err msg here
                    } else {
                        info(1, "PASV server created, port : %hu", pasv_port);
                        uint32_t t = svr_addr.sin_addr.s_addr;
                        send_str(client, FTP_PASV, t&0xff, (t>>8)&0xff, (t>>16)&0xff, (t>>24)&0xff, pasv_port>>8, pasv_port & 0xff);
                    }
                }break;
                case PORT:
                {
                    trsf_type = TRSF_PORT;
                    int _st = parse_addr_port(buf, &port_address, &port_port);
                    if (!_st) {
                        err(1, "port cmd error parsing addr and port");
                        send_str(client, FTP_ERR_PORT);
                    } else {
                        info(1, "address is %s, port is %ld", n2a(port_address), port_port);
                        send_str(client, FTP_PORT);
                    }
                }break;
                case LIST:
                {
                    if (trsf_type == TRSF_PASV) {
                        if (pasv_port > 1024 && pasv_port <= 65535 && pasv_server >= 0) {
                            send_str(client, FTP_ASCII, "LIST");
                            data_client = accept(pasv_server, (struct sockaddr *)&data_client_addr, (unsigned int*)&data_client_len);
                            if (data_client < 0) {
                                err(1, "LIST, accept data client socket error");
                            }
                        } else {
                            err(1, "LIST, no pasv server created");
                            break;
                        }
                    } else if (trsf_type == TRSF_PORT) {
                        if (port_address == 0 || port_port == 0) {
                            err(1, "LIST, in PORT mode, address and port not set before");
                            break;
                        }
                        send_str(client, FTP_ASCII, "LIST");
                        info(1, "LIST, in PORT mode, try connecting %s %lu", n2a(port_address), port_port);
                        data_client = new_client(port_address, port_port);
                        if (data_client < 0) {
                            err(1, "port mode connect client data sock error");
                            break;
                        } else {
                            info(1, "LIST, in PORT mode, %s %lu connected", n2a(port_address), port_port);
                        }
                    } else {
                        err(1, "LIST: transfer type no specified");
                    }
                    if (data_client >= 0) {
                        getcwd(cwd, sizeof(cwd));
                       /* if(strcmp(cwd, "/") == 0)
                        {
                        	strncat(cwd, &append, 1);
                        }*/

                        if(strstr(cwd, "/home/dtm") == NULL) {
    						strcpy(cwd, "");
    						strcpy(cwd, "/home/dtm");
                        }

                        sprintf(cmdbuf, "ls -l %s 2>&1", cwd);
                        p1 = popen(cmdbuf, "r");
                        if(p1 == NULL)
                        {
                        	printf("Server pipe not opened for List\n");
                        }
                        send_file(data_client, p1);
                        send_str(client, FTP_TRSF_OK);
                        pclose(p1);
                        info(1, "LIST , data client closed, status %d", close(data_client));
                        data_client = -1;
                    } else {
                        err(1, "LIST , no data client created");
                    }
                    if (pasv_server >= 0) {
                        info(1, "LIST, closing passive server ... %d", close(pasv_server));
                        pasv_server = -1;
                    }
                }break;
                case REST:
                {
                    if (parse_number(buf, &restdata) == 0) {
                        send_str(client, FTP_REST, restdata);
                    } else {
                        err(1, "REST, command error, wrong param");
                        send_str(client, FTP_ERR_PARAM, "REST");
                    }
                }break;
                case RETR:
                {
                    if (trsf_type == TRSF_PASV) {
                        if (pasv_port > 1024 && pasv_port <= 65535 && pasv_server >= 0) {
                            if (datatype == TYPE_ASCII) {
                                send_str(client, FTP_ASCII, "RETR");
                            } else {
                                send_str(client, FTP_BIN, "RETR");
                            }
                            data_client = accept(pasv_server, (struct sockaddr *)&data_client_addr, (unsigned int* )&data_client_len);
                            if (data_client < 0) {
                                err(1, "accept data client error");
                                break;
                            }
                        } else {
                            err(1, "RETR, pasv server not ready ");
                        }
                    } else if (trsf_type == TRSF_PORT) {
                        if (port_address == 0 || port_port == 0) {
                            err(1, "RETR, in PORT mode, address and port not set before");
                            break;
                        }
                        if (datatype == TYPE_ASCII) {
                            send_str(client, FTP_ASCII, "RETR");
                        } else {
                            send_str(client, FTP_BIN, "RETR");
                        }
                        info(1, "RETR , PORT mode, try connecting %s %lu", n2a(port_address), port_port);
                        data_client = new_client(port_address, port_port);
                        if (data_client < 0) {
                            err(1, "RETR: connect client error ");
                        }
                    } else {
                        err(1, "RETR: transfer type no specified");
                        break;
                    }
                    p = parse_path(buf);
                    if (!p) {
                        err(1, "RETR, wrong param");
                        send_str(1, FTP_ERR_PARAM, "RETR");
                        break;
                    } else {
                        int st = send_path(data_client, p, restdata);
                        if (st >= 0) {
                            send_str(client, FTP_TRSF_OK);
                            restdata = 0;

                        /* added controlling permission error */
                        } else {
                            send_str(client, FTP_ERROR, st == -1 ? (access(p, F_OK)==0? "access denied. Check Permission" : "file not exist") : "Unknown error");
                        }
                    }
                    if (data_client >= 0) {
                        info(1, "RETR, closing data client ... %d", close(data_client));
                        data_client = -1;
                    }
                    if (pasv_server >= 0) {
                        info(1, "RETR, closing passive server ... %d", close(pasv_server));
                        pasv_server = -1;
                    }
                }break;
                case STOR:
                {
                    if (trsf_type == TRSF_PASV) {
                        if (pasv_port > 1024 && pasv_port <= 65535 && pasv_server >= 0) {
                            if (datatype == TYPE_ASCII) {
                                send_str(client, FTP_ASCII, "STOR");
                            } else {
                                send_str(client, FTP_BIN, "STOR");
                            }
                            data_client = accept(pasv_server, (struct sockaddr *)&data_client_addr, (unsigned int*)&data_client_len);
                            if (data_client < 0) {
                                err(1, "STOR, accept data client error");
                                break;
                            }
                        } else {
                            err(1, "STOR, pasv server not ready ");
                        }
                    } else if (trsf_type == TRSF_PORT) {
                        if (port_address == 0 || port_port == 0) {
                            err(1, "STOR, PORT mode, address and port not set before");
                            break;
                        }
                        if (datatype == TYPE_ASCII) {
                            send_str(client, FTP_ASCII, "STOR");
                        } else {
                            send_str(client, FTP_BIN, "STOR");
                        }
                        info(1, "STOR, PORT mode, try connecting %s %lu", n2a(port_address), port_port);
                        data_client = new_client(port_address, port_port);
                        if (data_client < 0) {
                            err(1, "STOR: connect client error ");
                        }
                    } else {
                        err(1, "STOR: transfer type no specified");
                        break;
                    }
                    p = parse_path(buf);
                    if (!p) {
                        err(1, "STOR, wrong param");
                        send_str(1, FTP_ERR_PARAM, "RETR");
                        break;
                    } else {
                    	if(strstr(p, ".tle") != NULL) {
                    		formatTlPartition();

                            strcpy(p, "");
                            strcpy(p, "library.tle");
                    	}
                    	if(strstr(p, ".key") != NULL) {
							strcpy(p, "");
							strcpy(p, "key.key");
						}
                        int st = recv_path(data_client, p, restdata);
                        if (st >= 0) {
                            send_str(client, FTP_TRSF_OK);
                            restdata = 0;

                            copyActualTlToEmmc();	// copy actual tl from ramfs to emmc
                            copyDummyTlFromEmmc();	// copy dummy tl from emmc to ramfs
                        /* added controlling permission error */
                        } else {
                            copyDummyTlFromEmmc();	// copy dummy tl from emmc to ramfs
                            send_str(client, FTP_ERROR, access(p, W_OK)!=0 ? "access denied. Check permission" : "Unknown error");
                        }
                    }
                    if (data_client >= 0) {
                        info(1, "STOR, closing data client ... %d", close(data_client));
                        data_client = -1;
                    }
                    if (pasv_server >= 0) {
                        info(1, "STOR, closing passive server ... %d", close(pasv_server));
                        pasv_server = -1;
                    }
                }break;
                case CDUP:
                {
                    if (!chdir("..")) {
                        send_str(client, FTP_CDUP);
                    } else {
                        send_str(client, FTP_ERROR, "change to parent dir failed");
                    }
                }break;
                case CWD:
                {
                    p = parse_path(buf);
                    if (!p) {
                        err(1, "CWD, wrong param");
                        send_str(1, FTP_ERR_PARAM, "CWD");
                        break;
                    }

                    if(strstr(p, "/home/dtm") == NULL) {
						strcpy(p, "");
						strcpy(p, "/home/dtm");
                    }
                    info(1, "chdir \"%s\"", p);
                    if (!(chdir(p))) {
                        send_str(client, FTP_CWD);
                    } else {
                        err(1, "errno = %d, errstr is %s", errno, strerror(errno));
                        send_str(client, FTP_ERROR, "change dir failed");
                    }
                }break;
                case MDTM:
                case SIZE:
                {
                    p = parse_path(buf);
                    if (!p) {
                        if (cmd == MDTM) {
                            err(1, "MDTM, wrong param");
                            send_str(client, FTP_ERR_PARAM, "MDTM");
                        } else {
                            err(1, "SIZE, wrong param");
                            send_str(client, FTP_ERR_PARAM, "SIZE");
                        }
                        break;
                    }
                    if (stat(p, &file_stat) == 0) {
                        if (cmd == MDTM) {
                            char _buf[BUF_SIZE];
                            gmtime_r(&(file_stat.st_mtime), &mdtime);
                            strftime(_buf, sizeof(_buf), "%Y%m%d%H%M%S", &mdtime);
                            send_str(client, FTP_MDTM, _buf);
                        } else {
                            send_str(client, FTP_SIZE, file_stat.st_size);
                        }
                    }
                }break;
                case DELE:
                {
                    p = parse_path(buf);
                    if (!p) {
                        err(1, "DELE, param error");
                        send_str(client, FTP_ERR_PARAM, "DELE");
                    } else {
                        if (remove(p) == 0) {
                            send_str(client, FTP_DELE);
                        } else {
                            send_str(client, FTP_ERROR, "delete failed, file not exist ?");
                        }
                    }
                }break;
                case RMD:
                {
                    p = parse_path(buf);
                    if (!p) {
                        err(1, "RMD, param error");
                        send_str(client, FTP_ERR_PARAM, "RMD");
                    } else {
                        if (rmdir(p) == 0) {
                            send_str(client, FTP_DELE);
                        } else {
                            send_str(client, FTP_ERROR, "rmdir failed, dir not exist ?");
                        }
                    }
                }break;
                case MKD:
                {
                    p = parse_path(buf);
                    if (!p) {
                        err(1, "MKD, param error");
                        send_str(client, FTP_ERR_PARAM, "MKD");
                    } else {
                        if (mkdir(p, 0777) == 0) {
                            send_str(client, FTP_MKDIR);
                        } else {
                            send_str(client, FTP_ERROR, "mkdir failed, dir already exist ?");
                        }
                    }
                }break;
                case RNFR:
                {
                    p = parse_path(buf);
                    if (!p) {
                        err(1, "RNFR param error");
                        send_str(client, FTP_ERR_PARAM, "RNFR");
                    } else {
                        strcpy(rnfr, p);
                        send_str(client, FTP_RNFR);
                    }
                }break;
                case RNTO:
                {
                    p = parse_path(buf);
                    if (!p) {
                        err(1, "RNTO param error");
                        send_str(client, FTP_ERR_PARAM, "RNTO");
                    } else {
                        if (rename(rnfr, p) == 0) {
                            send_str(client, FTP_RNTO);
                        } else {
                            send_str(client, FTP_ERROR, "rnto error, please check param");
                        }
                    }
                }break;
                default:
                {
                    send_str(client, FTP_CMDNOIM);
                }break;
            }
        }
        if (p) {
            free(p);
            p = NULL;
        }
        if (!running) break;
    }
    info(1, "exiting session ...");
    int st = close(client);
    info(1, "clent closed , status %d", st);
    client = -1;
    if (data_client > 0) {
        info(1, "data client closed, status %d", close(data_client));
    }
    if (pasv_server > 0) {
        info(1, "pasv server closed, status %d", close(pasv_server));
    }
}

/**
 * handle Ctrl-C
 *
 * @param {int} socket fd
 *
 * the parent and all its children will recieve kill signal and execute this
 */
void ouch(int n) {
    running = 0;
    puts("");
    if (forkpid > 0) {      // ftpd
        if (server >= 0) {
            int st = close(server);
            info(0, "shutdown ftp ... %d", st);
        }
    } else {                // session
        if (client >= 0) {
            int st = close(client);
            info(1, "shutdown ftp session... %d", st);
        }
    }
    exit(0);
}
