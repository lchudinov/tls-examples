
/*
  This program and the accompanying materials are
  made available under the terms of the Eclipse Public License v2.0 which accompanies
  this distribution, and is available at https://www.eclipse.org/legal/epl-v20.html

  SPDX-License-Identifier: EPL-2.0

  Copyright Contributors to the Zowe Project.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "alloc.h"
#include "logging.h"
#include "xlate.h"
#include "httpclient.h"
#include "tls.h"

void usage(char *name) {
  printf("usage: %s <server-ip> <port> <path>\n", name);
}

int main(int argc, char *argv[]) {
  int status = 0;

  TlsSettings tlsSettings = {
    .keyring = "key.kdb",
    .stash = "key.sth",
    .label = "client"
  };
  TlsEnvironment *tlsEnv = NULL;
  HttpClientSettings clientSettings;
  HttpClientContext *httpClientContext = NULL;
  HttpClientSession *session = NULL;
  LoggingContext *loggingContext = NULL;

  loggingContext = makeLoggingContext();
  logConfigureStandardDestinations(loggingContext);
  logConfigureComponent(loggingContext, LOG_COMP_HTTPCLIENT, "HTTP Client", LOG_DEST_PRINTF_STDOUT, ZOWE_LOG_DEBUG2);

  char *path = NULL;
  char buffer[2048];

  do {
    if (argc != 4) {
      usage(argv[0]);
      break;
    }

    clientSettings.host = strdup(argv[1]);
    clientSettings.port = atoi(argv[2]);
    clientSettings.recvTimeoutSeconds = 10;
    path = strdup(argv[3]);

    status = tlsInit(&tlsEnv, &tlsSettings);
    if (status) {
      printf ("failed to init tls environment, rc=%d (%s)\n", status, tlsStrError(status));
      exit(1);
    }
    status = httpClientContextInitSecure(&clientSettings, loggingContext, tlsEnv, &httpClientContext);

    if (status) {
      printf("error in httpcb ctx init: %d\n", status);
      break;
    }
    printf("successfully initialized http client\n");

    status = httpClientSessionInit(httpClientContext, &session);
    if (status) {
      printf("error initing session: %d\n", status);
      break;
    }

    printf("successfully inited session\n");

    status = httpClientSessionStageRequest(httpClientContext, session, "GET", path, NULL, NULL, NULL, 0);
    if (status) {
      printf("error staging request: %d\n", status);
      break;
    }
    printf("successfully staged request\n");

    status = httpClientSessionSend(httpClientContext, session);
    if (status) {
      printf("error sending request: %d\n", status);
      break;
    }
    printf("successfully sent request\n");

    /* need to call receive native in a loop*/
    int quit = 0;
    while (!quit) {
      status = httpClientSessionReceiveNative(httpClientContext, session, 1024);
      if (status != 0) {
        printf("error receiving request: %d\n", status);
        break;
      }
      if (session->response != NULL) {
        printf("received valid response!\n");
        quit = 1;
        break;
      }
      printf("looping\n");
    }
    if (quit) {
      char *responseEbcdic = safeMalloc(session->response->contentLength + 1, "response");
      memcpy(responseEbcdic, session->response->body, session->response->contentLength);
      responseEbcdic[session->response->contentLength] = '\0';
      const char *trTable = getTranslationTable("iso88591_to_ibm1047");
      for (int i = 0; i < session->response->contentLength; i++) {
        responseEbcdic[i] = trTable[responseEbcdic[i]];
      }
      printf("successfully received request(EBCDIC): %s\n", responseEbcdic);
    } else
      printf("error\n");

  } while (0);

  printf("test ending with status: %d\n", status);

  return status;
}

/*
  This program and the accompanying materials are
  made available under the terms of the Eclipse Public License v2.0 which accompanies
  this distribution, and is available at https://www.eclipse.org/legal/epl-v20.html

  SPDX-License-Identifier: EPL-2.0

  Copyright Contributors to the Zowe Project.
*/
