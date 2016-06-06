#VRXSDK = C:\VerixVAps\VRXSDK
#TESTROOT = C:\GitHub\OSSQA
#TLIBOBJS = -I C:\GitHub\OSSQA\tlib\Test.o -I C:\GitHub\OSSQA\tlib\TestRegistry.o -I C:\GitHub\OSSQA\tlib\tlib.o
SDSIncludes = $(VRXSDK)\include
EOSIncludes = D:\SDK\EOS_SDK\include
EOSLibs = D:\SDK\EOS_SDK\lib\svc_net.o
TLIB_INCLUDE = $(TESTROOT)\include
TLIB_INC_2 = $(TESTROOT)\tlib
NO_TLIB=

#Includes = -I$(SDSIncludes) -I$(EOSIncludes)
COptions =  -s 655350 -h 655350 -vv -g -p

all : ethernet.out #ethernet.p7s

clean :
	-del *.o *.out *.p7s *.bak *.axf *.err

#ethernet.p7s : ethernet.out
#	$(NEWFST)\FileSignature.exe -C "C:\TestSuite10\Common\SponsorCert.crt" -F "%cd%\ethernet.out" -CM
#	rename ethernet.out.p7s ethernet.p7s

ethernet.out : ethernet.c 
	$(VRXSDK)\bin\vrxcc $(COptions) -I $(TLIB_INCLUDE) -I $(TLIB_INC_2) -I $(EOSIncludes) -e ethernet.err ethernet.c $(TESTRO) $(TLIBO) $(TESTO) $(EOSLibs) 
	$(VRXSDK)\bin\vrxhdr ethernet.out -l TLIB.LIB=I:/TLIB.LIB -l net.lib=N:15/net.lib
clean :
	del *.o *.out *.p7s *.err *.bak
