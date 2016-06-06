VRXSDK = C:\VerixVAps\VRXSDK
TESTROOT = C:\GitHub\OSSQA
TLIBOBJS = -I C:\GitHub\OSSQA\tlib\Test.o -I C:\GitHub\OSSQA\tlib\TestRegistry.o -I C:\GitHub\OSSQA\tlib\tlib.o -I C:\GitHub\OSSQA\tlib\clibpi.o
SDSIncludes = $(VRXSDK)\include
EOSIncludes = D:\SDK\EOS_SDK\include
EOSLibs = D:\SDK\EOS_SDK\lib\svc_net.o
TLIB_INCLUDE = $(TESTROOT)\include
TLIB_INC_2 = $(TESTROOT)\tlib
NO_TLIB=

#Includes = -I$(SDSIncludes) -I$(EOSIncludes)
COptions =  -s 655350 -h 655350 -vv -g -p

all : ethernet.out

clean :
	-del *.o *.out *.p7s *.bak *.axf *.err

ethernet.out : ethernet.c 
	$(VRXSDK)\bin\vrxcc $(COptions) -I $(TLIB_INCLUDE) -I $(TLIB_INC_2) -I $(EOSIncludes) -e ethernet.err ethernet.c  $(TLIBOBJS) $(EOSLibs)

clean :
	del *.o *.out *.p7s *.err *.bak
