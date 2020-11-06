# Examples for TLS client and server

## Building

Make sure to that these variables in Makefile point to right location:
```
ZOWECOMMON:=~/zowe/zss/deps/zowe-common-c
ZSS:=~/zowe/zss
```

```
make all
```

## Usage:

Run https-server in background:
```
./https-server 12345&
```

Make GET request to `/hello` service:
```
./https-client localhost 12345 /hello 
```

## Notes

key.kdb is a test `gskkyman` database protected with password `password`. 
