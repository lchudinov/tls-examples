ZOWECOMMON:=~/zowe/zss/deps/zowe-common-c
ZSS:=~/zowe/zss

GSKDIR:=/usr/lpp/gskssl
GSKINC:=$(GSKDIR)/include
GSKLIB:=$(GSKDIR)/lib/GSKSSL.x
CC:=c89
CFLAGS:=-D_OPEN_THREADS=1 \
				-D_XOPEN_SOURCE=600 \
				-DAPF_AUTHORIZED=0 \
				-DHTTPSERVER_BPX_IMPERSONATION=1 \
				-DNOIBMHTTP=1 \
				-DUSE_ZOWE_TLS=1 \
				-Wc,dll,expo,langlvl\(extc99\),gonum,goff,hgpr,roconst,ASM,asmlib\('CEE.SCEEMAC','SYS1.MACLIB','SYS1.MODGEN'\) \
				-Wc,agg,exp,list\(\),so\(\),off,xref \
				-I $(ZOWECOMMON)/h \
				-I $(ZOWECOMMON)/jwt/jwt \
				-I $(ZOWECOMMON)/jwt/rscrypto \
				-I $(ZSS)/h \
				-Wl,ac=1,dll

COMMON_OBJS:=alloc.o \
						bpxskt.o \
						charsets.o \
						cmutils.o \
						client.o \
						collections.o \
						crossmemory.o \
						crypto.o \
						fdpoll.o \
						http.o \
						httpserver.o \
						httpclient.o \
						icsf.o \
						impersonation.o \
						json.o \
						jwt.o \
						le.o \
						logging.o \
						qsam.o \
						recovery.o \
						rs_icsfp11.o \
						rs_rsclibc.o \
						stcbase.o \
						scheduling.o \
						socketmgmt.o \
						timeutls.o \
						tls.o \
						utils.o \
						xml.o \
						xlate.o \
						zos.o \
						zosfile.o \
						zosaccounts.o \
						zvt.o

all:	https-client https-server

https-client: https-client.o $(COMMON_OBJS)
	_C89_LSYSLIB="CEE.SCEELKED:SYS1.CSSLIB:CSF.SCSFMOD0" \
	$(CC) $(CFLAGS) -o $@ $^ $(GSKLIB)

https-server: https-server.o $(COMMON_OBJS)
	_C89_LSYSLIB="CEE.SCEELKED:SYS1.CSSLIB:CSF.SCSFMOD0" \
	$(CC) $(CFLAGS) -o $@ $^  $(GSKLIB)

%.o:	$(ZOWECOMMON)/c/%.c
	$(CC) $(CFLAGS) -c $<

%.o:	$(ZOWECOMMON)/jwt/jwt/%.c
	$(CC) $(CFLAGS) -c $<

%.o:	$(ZOWECOMMON)/jwt/rscrypto/%.c
	$(CC) $(CFLAGS) -c $<

%.o:	$(ZSS)/c/zis/%.c
	$(CC) $(CFLAGS) -c $<

%.o:	%.c
	$(CC) $(CFLAGS) -c $<

clean:	
	rm -f *.o *.lst https-client https-server

.PHONY:	clean
