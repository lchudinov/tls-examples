
/*
  This program and the accompanying materials are
  made available under the terms of the Eclipse Public License v2.0 which accompanies
  this distribution, and is available at https://www.eclipse.org/legal/epl-v20.html

  SPDX-License-Identifier: EPL-2.0

  Copyright Contributors to the Zowe Project.
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <errno.h>
#include <pthread.h>
#include <signal.h>

#include "zowetypes.h"
#include "alloc.h"
#include "utils.h"
#include "zos.h"
#include "bpxnet.h"
#include "collections.h"
#include "socketmgmt.h"
#include "le.h"
#include "logging.h"
#include "scheduling.h"
#include "json.h"
#include "httpserver.h"
#include "tls.h"

static int serveHello(HttpService *service, HttpResponse *response) {
  jsonPrinter *out = respondWithJsonPrinter(response);
  setResponseStatus(response, 200, "OK");
  setDefaultJSONRESTHeaders(response);
  writeHeader(response);
  jsonStart(out);
  jsonAddString(out, "message", "hello");
  jsonEnd(out);
  finishResponse(response);
  return 0;
}

static void installHelloService(HttpServer *server) {
  HttpService *httpService = makeGeneratedService("hello", "/hello");
  httpService->authType = SERVICE_AUTH_NONE;
  httpService->serviceFunction = serveHello;
  httpService->runInSubtask = FALSE;
  httpService->doImpersonation = FALSE;
  registerHttpService(server, httpService);
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    printf ("Usage: %s port\n", argv[0]);
    exit(1);
  }
  STCBase *base = (STCBase *)safeMalloc31(sizeof(STCBase), "stcbase");
  memset(base, 0, sizeof(STCBase));
  stcBaseInit(base);
  int port = atoi(argv[1]);
  char *address = "0.0.0.0";
  TlsSettings tlsSettings = {0};
  tlsSettings.keyring = "key.kdb";
  tlsSettings.stash = "key.sth";
  tlsSettings.label = "server";
  InetAddr *inetAddress = getAddressByName(address);
  TlsEnvironment *env = NULL;
  int rc = tlsInit(&env, &tlsSettings);
  if (rc != 0) {
    printf("failed to init tls environment rc = %d (%s)\n", rc, tlsStrError(rc));
    exit(1);
  }
  int returnCode = 0;
  int reasonCode = 0;
  HttpServer *server = makeSecureHttpServer(base, inetAddress, port, env, TRUE, &returnCode, &reasonCode);
  if (server) {
    installHelloService(server);
    printf ("about to listen on %s:%d\n", address, port);
    mainHttpLoop(server);
  }
}


/*
  This program and the accompanying materials are
  made available under the terms of the Eclipse Public License v2.0 which accompanies
  this distribution, and is available at https://www.eclipse.org/legal/epl-v20.html

  SPDX-License-Identifier: EPL-2.0

  Copyright Contributors to the Zowe Project.
*/
