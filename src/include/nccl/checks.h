/*************************************************************************
 * Copyright (c) 2019-2022, NVIDIA CORPORATION. All rights reserved.
 ************************************************************************/

#ifndef NCCL_CHECKS_H_
#define NCCL_CHECKS_H_

#include "debug.h"

#include <errno.h>
// Check system calls
#define SYSCHECK(statement, name) do { \
  int retval; \
  SYSCHECKSYNC((statement), name, retval); \
  if (retval == -1) { \
    WARN("Call to " name " failed: %s", strerror(errno)); \
    return ncclSystemError; \
  } \
} while (false)

#define SYSCHECKSYNC(statement, name, retval) do { \
  retval = (statement); \
  if (retval == -1 && (errno == EINTR || errno == EWOULDBLOCK || errno == EAGAIN)) { \
    INFO(NCCL_ALL,"Call to " name " returned %s, retrying", strerror(errno)); \
  } else { \
    break; \
  } \
} while(true)

#define SYSCHECKGOTO(statement, name, RES, label) do { \
  int retval; \
  SYSCHECKSYNC((statement), name, retval); \
  if (retval == -1) { \
    WARN("Call to " name " failed: %s", strerror(errno)); \
    RES = ncclSystemError; \
    goto label; \
  } \
} while (0)

// Pthread calls don't set errno and never return EINTR.
#define PTHREADCHECK(statement, name) do { \
  int retval = (statement); \
  if (retval != 0) { \
    WARN("Call to " name " failed: %s", strerror(retval)); \
    return ncclSystemError; \
  } \
} while (0)

#define PTHREADCHECKGOTO(statement, name, RES, label) do { \
  int retval = (statement); \
  if (retval != 0) { \
    WARN("Call to " name " failed: %s", strerror(retval)); \
    RES = ncclSystemError; \
    goto label; \
  } \
} while (0)

#define NEQCHECK(statement, value) do { \
  if ((statement) != value) { \
    /* Print the back trace*/ \
    INFO(NCCL_ALL,"%s:%d -> %d (%s)", __FILE__, __LINE__, ncclSystemError, strerror(errno)); \
    return ncclSystemError; \
  } \
} while (0);

#define NEQCHECKGOTO(statement, value, RES, label) do { \
  if ((statement) != value) { \
    /* Print the back trace*/ \
    RES = ncclSystemError; \
    INFO(NCCL_ALL,"%s:%d -> %d (%s)", __FILE__, __LINE__, RES, strerror(errno)); \
    goto label; \
  } \
} while (0);

#define EQCHECK(statement, value) do { \
  if ((statement) == value) { \
    /* Print the back trace*/ \
    INFO(NCCL_ALL,"%s:%d -> %d (%s)", __FILE__, __LINE__, ncclSystemError, strerror(errno)); \
    return ncclSystemError; \
  } \
} while (0);

#define EQCHECKGOTO(statement, value, RES, label) do { \
  if ((statement) == value) { \
    /* Print the back trace*/ \
    RES = ncclSystemError; \
    INFO(NCCL_ALL,"%s:%d -> %d (%s)", __FILE__, __LINE__, RES, strerror(errno)); \
    goto label; \
  } \
} while (0);

// Propagate errors up
#define NCCLCHECK(call) do { \
  ncclResult_t RES = call; \
  if (RES != ncclSuccess && RES != ncclInProgress) { \
    /* Print the back trace*/ \
    /* if (ncclDebugNoWarn == 0) */ INFO(NCCL_ALL,"%s:%d -> %d", __FILE__, __LINE__, RES);    \
    return RES; \
  } \
} while (0);

#define NCCLCHECKGOTO(call, RES, label) do { \
  RES = call; \
  if (RES != ncclSuccess && RES != ncclInProgress) { \
    /* Print the back trace*/ \
    /* if (ncclDebugNoWarn == 0) */ INFO(NCCL_ALL,"%s:%d -> %d", __FILE__, __LINE__, RES);    \
    goto label; \
  } \
} while (0);

#define NCCLCHECKTHREAD(a, args) do { \
  if (((args)->ret = (a)) != ncclSuccess && (args)->ret != ncclInProgress) { \
    INFO(NCCL_INIT,"%s:%d -> %d [Async thread]", __FILE__, __LINE__, (args)->ret); \
    return args; \
  } \
} while(0)

#endif
