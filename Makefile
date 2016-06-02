SDSIncludes = $(VRXSDK)\include
EOSIncludes = D:\SDK\EOS_SDK\include
EOSLibs = D:\SDK\EOS_SDK\lib\svc_net.o
TLIB_INCLUDE = $(TESTROOT)\include
TLIB_INC_2 = $(TESTROOT)\tlib
NO_TLIB=

#Includes = -I$(SDSIncludes) -I$(EOSIncludes)
COptions =  -s 655350 -h 655350 -vv -g -p

all : ethernet.out ethernet.p7s 

clean :
	-del *.o *.out *.p7s *.bak *.axf *.err


ethernet.p7s : ethernet.out
	$(VRXSDK)\bin\vrxhdr ethernet.out -l TLIB.LIB=I:/TLIB.LIB -l net.lib=N:15/net.lib
	FileSignature.exe -C "D:\TestSuite10\Common\SponsorCert.crt" -F "%cd%\ethernet.out" -CM
	rename ethernet.out.p7s ethernet.p7s

ethernet.out : ethernet.c 
	$(VRXSDK)\bin\vrxcc $(COptions) -I $(TLIB_INCLUDE) -I $(TLIB_INC_2) -I $(EOSIncludes) -e ethernet.err ethernet.c  $(TLIBOBJS) $(EOSLibs)

clean :
	del *.o *.out *.p7s *.err *.bak

