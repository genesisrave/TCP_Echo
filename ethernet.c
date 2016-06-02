/**************************************************************************/
/*                                                                        */
/*                   VeriFone OS SQA Test Suite                           */
/*                   --------------------------                           */
/*                                                                        */
/*  Modification History:                                                 */
/*    Date        Programmer        Description                           */
/*    --------    ---------------   ---------------------------------     */
/*    01/23/2015  Ryan_P1            Sanity Test - Ethernet TCP Echo      */
/*                                                                        */
/*        Copyright 1996 by VeriFone, Inc.  All Rights Reserved.          */
/*        ------------------------------------------------------          */
/*     Accessing, using, copying, modifying  or  distributing  this       */
/*     source  code  is  expressly  not permitted without the prior       */
/*     written authorization of VeriFone,  Inc.   Engaging  in  any       */
/*     such   activities   without   such   authorization   is   an       */
/*     infringement of VeriFone's copyrights and a misappropriation       */
/*     of  VeriFone's trade secrets in this source code, which will       */
/*     subject  the  perpetrator  to  certain  civil  damages   and       */
/*     criminal penalties.                                                */
/**************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <svc.h>
#include <errno.h>
#include <svc_net.h>
#include <svc_net2.h>
#include "k2ats.h"

#define SOH		0x01
#define STX		0x02
#define ETX		0x03
#define EOT		0x04
#define ENQ		0x05
#define ACK		0x06
#define BEL		0x07
#define BS		0x08
#define HT		0x09
#define LF      0x0a
#define VT      0x0b
#define NP      0x0c
#define CR      0x0d
#define SO		0x0e
#define SI		0x0f
#define DLE		0x10
#define DC1		0x11
#define DC2		0x12
#define DC3		0x13
#define DC4		0x14
#define NAK		0x15
#define SYN		0x16
#define ETB		0x17
#define CAN		0x18
#define EM		0x19
#define SUB     0x1a
#define ESC     0x1b
#define FS      0x1c
#define GS      0x1d
#define RS      0x1e
#define US      0x1f
#define DOT     0x2e

#define CFG_TCP_ARP_RETRIES  -1
#define CFG_TCP_ARP_INTERVAL  1
#define CFG_TCP_ARP_TIMEOUT   1


void Fan(int x, int y);
//void LogtoPort (char* log_msg);
void log_data_hex (char* data, int size);
void Display_Cmd_Resp (char* AT_Cmd, char* OutDisplay);
void Write_To_Line(char* DisplayString, int x, int y);
void Add_Variable(char* varname, int value);
int Get_Variable(char* varname, int default_value);
int TCPIP_Init(int iDevHdl, long lEvent);
int TCPIP_SocketOpen(char *cpIP, short shPort);
int TCPIP_SocketSendRecv(char* SendMsg, int MsgSize);
int TCPIP_Deinit(void);
int TCPIP_SocketClose(void);


//extern int errno;

int Eth_Handle, Logport_Handle, eth_port;
int console_handle, ierr, xerr;
int debug_flag, my_tid, fail_cnt;
char msg[2*8192], filename[35], DispBuff[256];
struct Opn_Blk Com3ob;
char blade;

int g_iDevHdl = -1;
int g_iTcpHdl = -1;
int g_iSocHdl = -1;
int g_iSendCounter = 0;
int g_iRecvCounter = 0;
int g_iSendSuccessCtr = 0;
int g_iRecvSuccessCtr = 0;
int g_iConnectFailCtr = 0;


int main(int argc, char *argv[])  {
    int status, retval, cnt, temp_id, main_tid, port_num, host_port, x, loop_cnt, test_cnt;
    char term_type[10], eth_sts[4], buff2[128], output_buff[64], input_buff[64], SerNo[11], SendMsg[2*8192];
    char info[4], code;
    struct Opn_Blk ob;
    long event = 0;
	char chHostIpAddress[50]={'\0'};
	char szHostPortNum[10]={'\0'};
	short msg_size[8]= { 512, 1024, 2048, 3072, 4096, 5120, 6144, 8192};

    errno = 0;
    ierr = 0;
	    
    TLIB_AGENT();
	
    my_tid = get_task_id();
	debug_flag = Get_Variable("*DEBUG",0);
	loop_cnt = Get_Variable("LOOPCNT",100);   
	test_cnt = Get_Variable("TESTCNT",0);   
	eth_port = Get_Variable("ETH_PORT",1);
	fail_cnt = 0;

	console_handle =  open(DEV_CONSOLE, 0);
 	  
    SVC_INFO_EPROM(term_type);
   	SVC_INFO_SERLNO_EXT(SerNo,11);
    
    clrscr();
		
    sprintf(msg,"Run Ethernet Echo Test", filename);
    Write_To_Line(msg, 1, 1);

    memset(filename,0,sizeof(filename));
	status=0;
	status=get_env("TESTNAME",filename,sizeof(filename));  
    if(status<=0) {
        strcpy(filename,"NONE");
    }
    
    sprintf(msg,"Start Ethernet Echo Test");
    logger((unsigned char *)msg,strlen(msg));

    sprintf(msg,"OS: %s", term_type+1);
    logger((unsigned char *)msg,strlen(msg));
    Write_To_Line(msg, 1, 2);

    sprintf(msg,"SN: %s", SerNo);
    logger((unsigned char *)msg,strlen(msg));
    Write_To_Line(msg, 1, 3);

	for( test_cnt=0; test_cnt<loop_cnt; test_cnt++) {
	    
		//LogtoPort(" ");
		//LogtoPort("====================");
		sprintf(msg,"Loop Cnt: %d/%d", test_cnt+1, loop_cnt);
		logger((unsigned char *)msg,strlen(msg));
		Write_To_Line(msg, 1, 4);
		//LogtoPort("====================");
		
		if(eth_port==2) {
			errno=0;
			Eth_Handle = open(DEV_ETH2,0);
			xerr=errno;		
		} else {
			errno=0;
			Eth_Handle = open(DEV_ETH1,0);
			xerr=errno;		
		}
		if(Eth_Handle > 0 && xerr == 0) {
		    sprintf(msg,"PASS open(DEV_ETH%d) handle %d errno %d", eth_port, Eth_Handle, xerr);
		    logger((unsigned char *)msg,strlen(msg));
		} else {
		    sprintf(msg,"FAIL open(DEV_ETH%d) handle %d errno %d", eth_port, Eth_Handle, xerr);
		   logger((unsigned char *)msg,strlen(msg));   
		    fail_cnt++;     
		    goto END_TEST;
		}
				
		//check ETH sts
		do {
			event = read_event();
			errno=0;
			status = get_enet_status(Eth_Handle, eth_sts);
			xerr=errno;
			if(status<0) {
				sprintf(msg,"FAIL get_enet_status(DEV_ETH%d)=%d/%d", eth_port, status, xerr);
			 	logger((unsigned char *)msg,strlen(msg));
			 	fail_cnt++;
			}
			if(eth_sts[0] & 1) {
				sprintf(msg,"Ethernet Link UP");
			 	logger((unsigned char *)msg,strlen(msg));
			} else {
				sprintf(msg,"Ethernet Link LOW");
			 	logger((unsigned char *)msg,strlen(msg));
			}
		} while (!(eth_sts[0] & 1)  && !(event & EVT_KBD) );    
		
		if(!(eth_sts[0] & 1)) {
			sprintf(msg,"FAIL Ethernet Link LOW");
		 	logger((unsigned char *)msg,strlen(msg));
			fail_cnt++;	
		}
		
		//get Target IP Add
		memset(chHostIpAddress, '\0', sizeof(chHostIpAddress));
		memset(szHostPortNum, '\0', sizeof(szHostPortNum));
		get_env("HOSTIP",chHostIpAddress,sizeof(chHostIpAddress));
		get_env("PORTNUM",szHostPortNum,sizeof(szHostPortNum));
		host_port = atoi(szHostPortNum);
		
		//init TCP
		TCPIP_Init(Eth_Handle,EVT_NETWORK);
		
		status = TCPIP_SocketOpen(chHostIpAddress, host_port);
	
		if(g_iSocHdl>0 && !(status<0)) {
			for(cnt=0; cnt<8; cnt++) {
				//create message
				memset(SendMsg,0,sizeof(SendMsg));
				code=48;
				for(x=0; x<msg_size[cnt]; x++) {
					SendMsg[x]=code;
					code++;
					if(code>122) {
						code=48;	
					}
				}
				SendMsg[0]='*';
				if(cnt<7)
					SendMsg[msg_size[cnt]-1]='#';
				else 
					SendMsg[msg_size[cnt]-1]='$';	
				
				TCPIP_SocketSendRecv(SendMsg,msg_size[cnt]);
			}
			
			TCPIP_SocketClose();
		}
		
		TCPIP_Deinit();
	    
	CLOSE_ETH:		
	    sprintf(msg, "Closing Ethernet...");
	    logger((unsigned char *)msg,strlen(msg));
		xerr=0;
		errno=0;
		status = close(Eth_Handle);
		xerr=errno;
		
		if(status == 0 && xerr == 0) {
		    sprintf(msg,"PASS close(DEV_ETH%d) status %d errno %d", eth_port, status, xerr);
		    logger((unsigned char *)msg,strlen(msg));
		} else {
		    sprintf(msg,"FAIL close(DEV_ETH%d) status %d errno %d", eth_port, status, xerr);
		    logger((unsigned char *)msg,strlen(msg));
		    fail_cnt++;
		}		
		if(fail_cnt) {
	    	sprintf(msg,"FAIL COUNT=%d", fail_cnt);
	    	logger((unsigned char *)msg,strlen(msg));	
		}
		
		
	}
	
    //---------------------------------------------------------------------------------//

END_TEST:    
	
	if(fail_cnt) {
	    sprintf(msg,"FAIL TCP Echo Test");
	    logger((unsigned char *)msg,strlen(msg));
	} else {
	    sprintf(msg,"PASS TCP Echo Test");
	    logger((unsigned char *)msg,strlen(msg));
	}
    Write_To_Line(msg,1,7);
    
    //LogtoPort(" ");
    sprintf(msg,"End TCP Echo Test");
    logger((unsigned char *)msg,strlen(msg));
    Write_To_Line(msg,1,8);

    SVC_WAIT(1000);		
    
    load_test();
    
    return 0;


}


/*****************************************************************************
    Fn:    Creates and spins an activity fan at specified location

Inputs:    x      -  Cursor X location
           y      -  Cursor Y location

Return :   None
*****************************************************************************/
void Fan(int x, int y) {

    // Sequence blade in rotation
    switch(blade) {
        case '|' : blade='/';  break;
        case '/' : blade='-';  break;
        case '-' : blade='\\'; break;
        case '\\': blade='|';  break;
        default  : blade='-';  break;
    }
    write_at(&blade, 1, x, y);   
}

void Write_To_Line(char* DisplayString, int x, int y) {
	
	write_at(DisplayString,strlen(DisplayString),x,y);
	clreol();
}


void Add_Variable(char* varname, int value) {
	
	char valuestring[10];
	
	if(value>=0) {
		memset(valuestring,0,sizeof(valuestring));
		sprintf(valuestring,"%d",value);
		put_env(varname,valuestring,strlen(valuestring));
	} else {
		put_env(varname," ",1); //remove variable
	}
	
}

int Get_Variable(char* varname, int default_value) {
	
	char value[10];
	int x, ret;
	
	memset(value,0,sizeof(value));
	errno=0;
	ret = get_env(varname,value,sizeof(value));	
	if(ret>0) 
		x = atoi(value);
	else 
		x = default_value;
		
	return x;
	
}

//void LogtoPort (char* log_msg)
//{
//	char temp[2*8192],sts[4], task_name[32];
//		
//	if(debug_flag) {
//		memset(temp,0,sizeof(temp));
//		strcpy(temp,"\n\r");
//		strcat(temp,"[BTPAN_TASK] ");
//		strncat(temp,log_msg,128);	
//		dbprintf(temp);
//	} else {
//		memset(temp,0,sizeof(temp));
//		strcpy(temp,"\n\r");
//		strcat(temp,"[BTPAN_TASK] ");
//		strcat(temp,log_msg);	
//		write(Logport_Handle,temp,strlen(temp));	
//		while(get_port_status(Logport_Handle,sts)>0);
//	}
//	SVC_WAIT(10);	
//	
//}

void log_data_hex (char* data, int size) {

    int x, y1, y2, cnt, loop, mod;
    char buff[10];

	mod =  size % 16;
	
	loop = size / 16;
	if(mod>0) {
		loop++;
	}
	
	for(cnt=1; cnt<(loop+1); cnt++) {
		y1 = 16*(cnt-1);
		y2 = (16*cnt)-1;
		
		if(y2 > (size-1) ) {
			y2 = y1 + mod - 1;
		}
			
		memset(msg,0,sizeof(msg));
		if(y1==y2) {
			sprintf(msg, "Data [%d]: ", y1);
		} else {
	    	sprintf(msg, "Data [%d-%d]: ", y1, y2);
	    }
	    
		for(x=y1; x<(y2+1); x++) {
			memset(buff,0,sizeof(buff));
			sprintf(buff,"%02x ",data[x]);
			strncat(msg,buff,strlen(buff));
		}
	    logger((unsigned char *)msg,strlen(msg));
	}

}


int TCPIP_Init(int iDevHdl, long lEvent)
{
    int arpRetries = CFG_TCP_ARP_RETRIES;
    unsigned char arpInterval = CFG_TCP_ARP_INTERVAL;
    unsigned char arpTimeout = CFG_TCP_ARP_TIMEOUT;
    char cpBuf[4];
    long lWaitEvt;
    int iTimerID;
    int iTcpHdl;
    int iRet;
    int iCtr;

    for (iCtr=0; iCtr<3; iCtr++)
    {
        memset(cpBuf, 0, sizeof(cpBuf));
        iRet = get_enet_status(iDevHdl, cpBuf);
        if ( !(cpBuf[0] & 1) ) {
			sprintf(msg,"FAIL Ethernet Link LOW");
		 	logger((unsigned char *)msg,strlen(msg));
		 	fail_cnt++;
            break;
        }
        SVC_WAIT(500);
    }

    iRet = net_addif(iDevHdl, get_task_id());
	sprintf(msg,"net_addif() returned %d, errno=%d", iRet, errno);
 	logger((unsigned char *)msg,strlen(msg));
    if (iRet < 0)
    {
		sprintf(msg,"FAIL net_addif()");
	 	logger((unsigned char *)msg,strlen(msg));
    	fail_cnt++;
        return iRet;
    }
    iTcpHdl = iRet;

    SVC_WAIT(500);
    iRet = AddInterface(iTcpHdl, LL_ETHERNET, lEvent);
	sprintf(msg,"AddInterface() returned %d, errno=%d", iRet, errno);
 	logger((unsigned char *)msg,strlen(msg));

    iRet = InterfaceSetOptions(iTcpHdl, DEV_OPTIONS_BOOT_ARP_RETRIES, &arpRetries, sizeof(arpRetries));
	sprintf(msg,"InterfaceSetOptions() returned %d, errno=%d", iRet, errno);
 	logger((unsigned char *)msg,strlen(msg));

    iRet = InterfaceSetOptions(iTcpHdl, DEV_OPTIONS_BOOT_ARP_INTVL, &arpInterval, sizeof(arpInterval));
	sprintf(msg,"InterfaceSetOptions() returned %d, errno=%d", iRet, errno);
 	logger((unsigned char *)msg,strlen(msg));

    iRet = InterfaceSetOptions(iTcpHdl, DEV_OPTIONS_BOOT_ARP_TIMEOUT, &arpTimeout, sizeof(arpTimeout));
	sprintf(msg,"InterfaceSetOptions() returned %d, errno=%d", iRet, errno);
 	logger((unsigned char *)msg,strlen(msg));

    iRet = UseDhcp(iTcpHdl);
	sprintf(msg,"UseDHCP() returned %d, errno=%d", iRet, errno);
 	logger((unsigned char *)msg,strlen(msg));

    iRet = OpenInterface(iTcpHdl, 0, 0);
	sprintf(msg,"OpenInterface() returned %d, errno=%d", iRet, errno);
 	logger((unsigned char *)msg,strlen(msg));

    g_iTcpHdl = iTcpHdl;

    iTimerID = set_timer(120000, EVT_TIMER);
    for ( ; ; )
    {
        lWaitEvt = wait_evt(lEvent | EVT_TIMER);
		sprintf(msg,"wait_evt() returned 0x%08x", lWaitEvt);
	 	logger((unsigned char *)msg,strlen(msg));

        if (lWaitEvt & lEvent)
        {
            iRet = GetDhcpEvent(iTcpHdl);
			sprintf(msg,"GetDhcpEvent(%d) returned %d, errno=%d", iTcpHdl, iRet, errno);
		 	logger((unsigned char *)msg,strlen(msg));
            if (iRet == 0)
                break;
        }

        if (lWaitEvt & EVT_TIMER)
        {
			sprintf(msg,"Timeout event");
		 	logger((unsigned char *)msg,strlen(msg));
		 	fail_cnt++;
            break;
        }
    }
    clr_timer(iTimerID);

    return iTcpHdl;
}

int TCPIP_SocketOpen(char *cpIP, short shPort)
{
    struct sockaddr_in socket_host;
    int iSocHdl;
    int iRet;

	sprintf(msg,"TCPIP_SocketOpen: IP=[%s], port=%d", cpIP, shPort);
 	logger((unsigned char *)msg,strlen(msg));

    iRet = socket (AF_INET, SOCK_STREAM, 0);
	sprintf(msg,"socket() returned %d, errno=%d", iRet, errno);
 	logger((unsigned char *)msg,strlen(msg));
    iSocHdl = iRet;

    iRet = 0;
    iRet = setsockopt( iSocHdl, IPPROTO_TCP, TCP_2MSLTIME, &iRet, sizeof( iRet ) ); // Set TCP 2MSL time to 0
	sprintf(msg,"setsockopt() returned %d, errno=%d", iRet, errno);
 	logger((unsigned char *)msg,strlen(msg));

    socket_host.sin_family = AF_INET;
    memset(socket_host.sin_zero, 0, 8);
    socket_host.sin_addr.s_addr = htonl(inet_addr(cpIP));
    socket_host.sin_port = htons(shPort);
    iRet = connect(iSocHdl,(struct sockaddr *)&socket_host,sizeof(struct sockaddr_in));
	sprintf(msg,"connect() returned %d, errno=%d", iRet, errno);
 	logger((unsigned char *)msg,strlen(msg));
    if(iRet < 0) {
		sprintf(msg,"FAIL Socket connect()");
	 	logger((unsigned char *)msg,strlen(msg));
	 	fail_cnt++;
        g_iConnectFailCtr++;
    } 
	sprintf(msg,"g_iConnectFailCtr [%d]", g_iConnectFailCtr);
 	logger((unsigned char *)msg,strlen(msg));
    g_iSocHdl = iSocHdl;

    return iRet;
}


int TCPIP_SocketSendRecv(char* SendMsg, int MsgSize)
{
    char tempbuff[2048] = {0};
    int iRet, timeout, timer;
    char RcvMsg[2*8192] = {0};
    int bytes_in=0;
    int initialTime = 0;
    long event = 0;

 	//LogtoPort(" ");
	sprintf(msg," ");
 	logger((unsigned char *)msg,strlen(msg));
	sprintf(msg,"TCPIP_SocketSendRecv: Start");
 	logger((unsigned char *)msg,strlen(msg));
    SVC_WAIT(200);	
    
    
	sprintf(msg,"Sending %d Bytes...",  MsgSize);
 	logger((unsigned char *)msg,strlen(msg));
 	Write_To_Line(msg, 1, 5);
    read_event();
    errno=0;
    iRet = send(g_iSocHdl, SendMsg, MsgSize, 0);
	sprintf(msg,"send() returned %d, errno=%d", iRet, errno);
 	logger((unsigned char *)msg,strlen(msg));
 	if(iRet<0) {
		sprintf(msg,"FAIL Send Socket");
	 	logger((unsigned char *)msg,strlen(msg));
	 	fail_cnt++;
 	}
 	
	sprintf(msg,"Waiting for EVT_NETWORK....");
 	logger((unsigned char *)msg,strlen(msg)); 	
	Write_To_Line(msg, 1, 5);
	clr_timer(timer);
	event =0;
	timer = set_timer(180000, EVT_TIMER);
	do {
		event = wait_evt(EVT_NETWORK|EVT_TIMER);		
	} while ( !(event&EVT_NETWORK) && !(event&EVT_TIMER) );    	
	clr_timer(timer);
 	
	if(event & EVT_NETWORK) {
		sprintf(msg,"Received EVT_NETWORK....");
	 	logger((unsigned char *)msg,strlen(msg));
		sprintf(msg,"Receiving...");
	 	Write_To_Line(msg, 1, 5);
	 	
	 	memset(RcvMsg,0,sizeof(RcvMsg));
		timeout=0;
	 	initialTime = read_ticks();
	 	do {
	 		memset(tempbuff,0,sizeof(tempbuff));
	 		errno=0;
	 		iRet = recv(g_iSocHdl, tempbuff, sizeof(tempbuff), 0);
			sprintf(msg,"recv() returned %d, errno=%d", iRet, errno);
		 	logger((unsigned char *)msg,strlen(msg));
		 	
		 	if(iRet>0) {
				memcpy(RcvMsg+bytes_in,tempbuff,iRet);
				bytes_in =  bytes_in + iRet;
				if( (strstr(RcvMsg,"#")!=NULL) || (strstr(RcvMsg,"$")!=NULL) ) {
					break;	    
				}
			}
	 		timeout++;
	 		SVC_WAIT(10);  	
	 	} while(timeout<60000);
		sprintf(msg, "Lapsed Read Time: %ld", read_ticks() - initialTime);
		logger((unsigned char *)msg,strlen(msg));
		sprintf(msg, "Read size %d", bytes_in);
		logger((unsigned char *)msg,strlen(msg)); 	
	
		//memset(msg,0,sizeof(msg));
		//LogtoPort("Sent Message:");
		//memset(msg,0,sizeof(msg));
		//sprintf(msg, "%s", SendMsg);
		//logger((unsigned char *)msg,strlen(msg));
		//memset(msg,0,sizeof(msg));
		//LogtoPort("Rcvd Message:");
		//memset(msg,0,sizeof(msg));
		//sprintf(msg, "%s", RcvMsg);
		//logger((unsigned char *)msg,strlen(msg));
		//memset(msg,0,sizeof(msg));
	
	 	
		//check content
		if( strncmp(SendMsg,RcvMsg,MsgSize)==0 ) {
			sprintf(msg,"PASS Received Message matches.");
			logger((unsigned char *)msg,strlen(msg));
		}else {
			sprintf(msg,"FAIL Received Message does not match.");
			logger((unsigned char *)msg,strlen(msg));
			fail_cnt++;
		} 							
		
		//check size
		if( bytes_in == MsgSize ) {
			sprintf(msg,"PASS Message Size Correct");
			logger((unsigned char *)msg,strlen(msg));
		} else {
			sprintf(msg,"FAIL Message Size Incorrect. Exp: %d, Act: %d", MsgSize, bytes_in);
			logger((unsigned char *)msg,strlen(msg));
			fail_cnt++;
		}			
	}
	
	if(event & EVT_TIMER) {
		sprintf(msg,"FAIL Read Timeout... ");
		logger((unsigned char *)msg,strlen(msg));
	 	Write_To_Line(msg, 1, 5);
		fail_cnt++;
	}	

    return 0;
}


int TCPIP_SocketClose(void)
{
    int iRet;

	sprintf(msg,"TCPIP_SocketClose: Start");
 	logger((unsigned char *)msg,strlen(msg));

    if (g_iSocHdl == -1)
        return -1;

    iRet = socketclose(g_iSocHdl);
	sprintf(msg,"socketclose() returned %d, errno=%d", iRet, errno);
 	logger((unsigned char *)msg,strlen(msg));
 	if(iRet<0) {
		sprintf(msg,"FAIL Socket close()");
	 	logger((unsigned char *)msg,strlen(msg));
	 	fail_cnt++;
 	}

    g_iSocHdl = -1;

    return 0;
}


int TCPIP_Deinit(void)
{
    int iRet;

	sprintf(msg,"TCPIP_Deinit: Start");
 	logger((unsigned char *)msg,strlen(msg));

    if (g_iTcpHdl == -1)
        return -1;

    iRet = CloseInterface(g_iTcpHdl);
	sprintf(msg,"CloseInterface() returned %d, errno=%d", iRet, errno);
 	logger((unsigned char *)msg,strlen(msg));

    iRet = net_delif(g_iTcpHdl);
	sprintf(msg,"net_delif() returned %d, errno=%d", iRet, errno);
 	logger((unsigned char *)msg,strlen(msg));

    g_iTcpHdl = -1;

    return 0;
}


void Display_Cmd_Resp (char* AT_Cmd, char* OutDisplay) {
	
	int index_rcv, index_disp, size;
	char buff[5];
	
    //create display message
    memset(OutDisplay, 0, sizeof(OutDisplay));    
    size=strlen(AT_Cmd);
    
    for(index_rcv=0, index_disp=0; index_rcv<size; index_rcv++) {
	    if(AT_Cmd[index_rcv]==SOH) {
	    	strcat(OutDisplay,"<SOH>");
	    	index_disp=index_disp+5;
	    } else if(AT_Cmd[index_rcv]==STX) {
	    	strcat(OutDisplay,"<STX>");
	    	index_disp=index_disp+5;
	    } else if(AT_Cmd[index_rcv]==ETX) {
	    	strcat(OutDisplay,"<ETX>");
	    	index_disp=index_disp+5;
	    } else if(AT_Cmd[index_rcv]==EOT) {
	    	strcat(OutDisplay,"<EOT>");
	    	index_disp=index_disp+5;
	    } else if(AT_Cmd[index_rcv]==ENQ) {
	    	strcat(OutDisplay,"<ENQ>");
	    	index_disp=index_disp+5;
	    } else if(AT_Cmd[index_rcv]==ACK) {
	    	strcat(OutDisplay,"<ACK>");
	    	index_disp=index_disp+5;
	    } else if(AT_Cmd[index_rcv]==BEL) {
	    	strcat(OutDisplay,"<BEL>");
	    	index_disp=index_disp+5;
	    } else if(AT_Cmd[index_rcv]==BS) {
	    	strcat(OutDisplay,"<BD>");
	    	index_disp=index_disp+4;
	    } else if(AT_Cmd[index_rcv]==HT) {
	    	strcat(OutDisplay,"<HT>");
	    	index_disp=index_disp+4;
	    } else if(AT_Cmd[index_rcv]==LF) {
	    	strcat(OutDisplay,"<LF>");
	    	index_disp=index_disp+4;
	    } else if(AT_Cmd[index_rcv]==VT) {
	    	strcat(OutDisplay,"<VT>");
	    	index_disp=index_disp+4;
	    } else if(AT_Cmd[index_rcv]==NP) {
	    	strcat(OutDisplay,"<NP>");
	    	index_disp=index_disp+4;
	    } else if(AT_Cmd[index_rcv]==CR) {
	    	strcat(OutDisplay,"<CR>");
	    	index_disp=index_disp+4;
	    } else if(AT_Cmd[index_rcv]==SO) {
	    	strcat(OutDisplay,"<SO>");
	    	index_disp=index_disp+4;
	    } else if(AT_Cmd[index_rcv]==SI) {
	    	strcat(OutDisplay,"<SI>");
	    	index_disp=index_disp+4;
	    } else if(AT_Cmd[index_rcv]==DLE) {
	    	strcat(OutDisplay,"<DLE>");
	    	index_disp=index_disp+5;
	    } else if(AT_Cmd[index_rcv]==DC1) {
	    	strcat(OutDisplay,"<DC1>");
	    	index_disp=index_disp+5;
	    } else if(AT_Cmd[index_rcv]==DC2) {
	    	strcat(OutDisplay,"<DC2>");
	    	index_disp=index_disp+5;
	    } else if(AT_Cmd[index_rcv]==DC3) {
	    	strcat(OutDisplay,"<DC3>");
	    	index_disp=index_disp+5;
	    } else if(AT_Cmd[index_rcv]==DC4) {
	    	strcat(OutDisplay,"<DC4>");
	    	index_disp=index_disp+5;
	    } else if(AT_Cmd[index_rcv]==NAK) {
	    	strcat(OutDisplay,"<NAK>");
	    	index_disp=index_disp+5;
	    } else if(AT_Cmd[index_rcv]==SYN) {
	    	strcat(OutDisplay,"<SYN>");
	    	index_disp=index_disp+5;
	    } else if(AT_Cmd[index_rcv]==ETB) {
	    	strcat(OutDisplay,"<ETB>");
	    	index_disp=index_disp+5;
	    } else if(AT_Cmd[index_rcv]==CAN) {
	    	strcat(OutDisplay,"<CAN>");
	    	index_disp=index_disp+5;
	    } else if(AT_Cmd[index_rcv]==EM) {
	    	strcat(OutDisplay,"<EM>");
	    	index_disp=index_disp+4;
	    } else if(AT_Cmd[index_rcv]==SUB) {
	    	strcat(OutDisplay,"<SUB>");
	    	index_disp=index_disp+5;
	    } else if(AT_Cmd[index_rcv]==ESC) {
	    	strcat(OutDisplay,"<ESC>");
	    	index_disp=index_disp+5;
	    } else if(AT_Cmd[index_rcv]==FS) {
	    	strcat(OutDisplay,"<FS>");
	    	index_disp=index_disp+4;
	    } else if(AT_Cmd[index_rcv]==GS) {
	    	strcat(OutDisplay,"<GS>");
	    	index_disp=index_disp+4;
	    } else if(AT_Cmd[index_rcv]==RS) {
	    	strcat(OutDisplay,"<RS>");
	    	index_disp=index_disp+4;
	    } else if(AT_Cmd[index_rcv]==US) {
	    	strcat(OutDisplay,"<US>");
	    	index_disp=index_disp+4;
	    } else if(AT_Cmd[index_rcv]==DOT) {
	    	strcat(OutDisplay,"<DOT>");
	    	index_disp=index_disp+5;
	    } else {
	    	memset(buff, 0, sizeof(buff));
	    	sprintf(buff,"%c",AT_Cmd[index_rcv]);
	    	strcat(OutDisplay,buff);
	    	index_disp++;
	    }
    }	
	
}


