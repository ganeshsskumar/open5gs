/*
 * Copyright (C) 2019-2025 by Sukchan Lee <acetcom@gmail.com>
 *
 * This file is part of Open5GS.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
#include <signal.h>
#include <unistd.h>
#include <sys/stat.h>
#include <pthread.h>
#include <time.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>

#include "ogs-app.h"
#include "version.h"

#ifdef ENABLE_LICENSE_CHECK

#include "License_Checker.h"

#define TOC_BUFFER_SIZE              30 
#define APP_NAME_BUFFER_SIZE         10

INT8 gsi8Appname[APP_NAME_BUFFER_SIZE] ="CORE";
INT8 gsi8TOC[TOC_BUFFER_SIZE] ="Fri 2026-02-6 11:00:40 UTC";

#endif

static void show_version(void)
{
    printf("Open5GS %s\n\n", OPEN5GS_VERSION);
}

static void show_help(const char *name)
{
    printf("Usage: %s [options]\n"
        "Options:\n"
       "   -c filename    : set configuration file\n"
       "   -l filename    : set logging file\n"
       "   -e level       : set global log-level (default:info)\n"
       "   -m domain      : set log-domain (e.g. mme:sgw:gtp)\n"
       "   -d             : print lots of debugging information\n"
       "   -t             : print tracing information for developer\n"
       "   -D             : start as a daemon\n"
       "   -v             : show version number and exit\n"
       "   -h             : show this message and exit\n"
       "   -k             : use <id> config section\n"
       "\n", name);
}

static int check_signal(int signum)
{
    switch (signum) {
    case SIGTERM:
    case SIGINT:
        ogs_info("%s received", 
                signum == SIGTERM ? "SIGTERM" : "SIGINT");

        return 1;
    case SIGHUP:
        ogs_info("SIGHUP received");
        ogs_log_cycle();

        break;
    case SIGUSR1:
        fprintf(stderr,
                "%*s%-30s contains %6lu bytes in %3lu blocks (ref %d) %p\n",
                0, "", "core",
                (unsigned long)talloc_total_size(__ogs_talloc_core),
                (unsigned long)talloc_total_blocks(__ogs_talloc_core),
                (int)talloc_reference_count(__ogs_talloc_core),
                __ogs_talloc_core);
        break;

    case SIGUSR2:
        talloc_report_full(__ogs_talloc_core, stderr);
        break;

    default:
        ogs_error("Signal-NUM[%d] received (%s)",
                signum, ogs_signal_description_get(signum));
        break;
            
    }
    return 0;
}

static void terminate(void)
{
    app_terminate();

    ogs_app_terminate();
}

#ifdef ENABLE_LICENSE_CHECK
static void dump_license_file_state(const char *tag)
{
    struct stat st;

    if (stat("/usr/local/License_LWS_CORE", &st) != 0) {
        ogs_error("[%s] stat failed on License_LWS_CORE: %s",
                   tag, strerror(errno));
        return;
    }

    ogs_info("[%s] License_LWS_CORE size=%ld mtime=%ld mode=%o",
              tag, (long)st.st_size, (long)st.st_mtime,
              st.st_mode & 0777);

    if (access("/usr/local/License_LWS_CORE", R_OK) != 0)
        ogs_error("[%s] not readable: %s", tag, strerror(errno));
    if (access("/usr/local/License_LWS_CORE", W_OK) != 0)
        ogs_error("[%s] not writable: %s", tag, strerror(errno));
}

static void *license_monitor_thread(void *arg)
{
    INT32 status;

    while (1) {

        sleep(3 * 60);   // Sleep for 24 hours
        setbuf(stdout, NULL);
        setbuf(stderr, NULL);
         /* Diagnostic: log exact filesystem state right before the check */
        if (access("/usr/local/License_LWS_CORE", F_OK) != 0) {
            ogs_error("License dir missing at recheck time: %s", strerror(errno));
        } else if (access("/usr/local/License_LWS_CORE", R_OK | W_OK) != 0) {
            ogs_error("License dir present but not R/W: %s", strerror(errno));
        } else {
            ogs_info("License dir present and R/W ok at recheck time");
        }
        dump_license_file_state("recheck");
        status = checkLicense(gsi8Appname, gsi8TOC);

        printf("Periodic License Check Status: %ld\n", status);

        if (status != LICENSE_VALID) {

            printf("License validation failed. Stopping Open5GS...\n");

            raise(SIGTERM);

            sleep(5);

            exit(EXIT_FAILURE);
        }
    }

    return NULL;
}
#endif

int main(int argc, const char *const argv[])
{
    /**************************************************************************
     * Starting up process.
     *
     * Keep the order of starting-up
     */
setbuf(stdout, NULL);
setbuf(stderr, NULL);
    
#ifdef ENABLE_LICENSE_CHECK
   INT32 status = LICENSE_VOID;
    status = checkLicense(gsi8Appname,gsi8TOC);
    GetLicenseAPIVersionNumber();
    printf("License Check Status: %ld \n", status);

    if (status != LICENSE_VALID) {
        printf("License Expired...!, Closing the app\n");
        exit(1);
    }
    
    dump_license_file_state("startup");
     /* TEMP DIAGNOSTIC: call it again immediately, same thread */
    INT32 status2 = checkLicense(gsi8Appname, gsi8TOC);
    printf("License Check Status (2nd call, same thread, immediate): %ld \n", status2);

    
   # pthread_t tid;

  #  if (pthread_create(&tid, NULL,
  #          license_monitor_thread, NULL) == 0) {
  #      pthread_detach(tid);
  #  } else {
  #      printf("Failed to start license monitoring thread\n");
  #      exit(EXIT_FAILURE);
  #  }

#endif
    
    int rv, i, opt;
    ogs_getopt_t options;
    struct {
        char *config_file;
        char *config_section;
        char *log_file;
        char *log_level;
        char *domain_mask;

        bool enable_debug;
        bool enable_trace;
    } optarg;
    const char *argv_out[argc+1];

    memset(&optarg, 0, sizeof(optarg));

    ogs_getopt_init(&options, (char**)argv);
    while ((opt = ogs_getopt(&options, "vhDc:l:e:m:dtk:")) != -1) {
        switch (opt) {
        case 'v':
            show_version();
            return OGS_OK;
        case 'h':
            show_help(argv[0]);
            return OGS_OK;
        case 'D':
#if !defined(_WIN32)
        {
            pid_t pid;
            pid = fork();

            ogs_assert(pid >= 0);

            if (pid != 0)
            {
                /* Parent */
                return EXIT_SUCCESS;
            }
            /* Child */

            setsid();
            umask(027);
        }
#else
            printf("%s: Not Support in WINDOWS", argv[0]);
#endif
            break;
        case 'c':
            optarg.config_file = options.optarg;
            break;
        case 'l':
            optarg.log_file = options.optarg;
            break;
        case 'e':
            optarg.log_level = options.optarg;
            break;
        case 'm':
            optarg.domain_mask = options.optarg;
            break;
        case 'd':
            optarg.enable_debug = true;
            break;
        case 't':
            optarg.enable_trace = true;
            break;
        case 'k':
            optarg.config_section = options.optarg;
            break;
        case '?':
            fprintf(stderr, "%s: %s\n", argv[0], options.errmsg);
            show_help(argv[0]);
            return OGS_ERROR;
        default:
            fprintf(stderr, "%s: should not be reached\n", OGS_FUNC);
            return OGS_ERROR;
        }
    }

    if (optarg.enable_debug) optarg.log_level = (char*)"debug";
    if (optarg.enable_trace) optarg.log_level = (char*)"trace";

    i = 0;
    argv_out[i++] = argv[0];

    if (optarg.config_file) {
        argv_out[i++] = "-c";
        argv_out[i++] = optarg.config_file;
    }
    if (optarg.log_file) {
        argv_out[i++] = "-l";
        argv_out[i++] = optarg.log_file;
    }
    if (optarg.log_level) {
        argv_out[i++] = "-e";
        argv_out[i++] = optarg.log_level;
    }
    if (optarg.domain_mask) {
        argv_out[i++] = "-m";
        argv_out[i++] = optarg.domain_mask;
    }
    if (optarg.config_section) {
        argv_out[i++] = "-k";
        argv_out[i++] = optarg.config_section;
    }

    argv_out[i] = NULL;

    ogs_signal_init();
    ogs_setup_signal_thread();

    rv = ogs_app_initialize(OPEN5GS_VERSION, DEFAULT_CONFIG_FILENAME, argv_out);
    if (rv != OGS_OK) {
        if (rv == OGS_RETRY)
            return EXIT_SUCCESS;

        ogs_fatal("Open5GS initialization failed. Aborted");
        return OGS_ERROR;
    }
    rv = app_initialize(argv_out);
        if (rv != OGS_OK) {
            if (rv == OGS_RETRY)
                return EXIT_SUCCESS;

            ogs_fatal("Open5GS initialization failed. Aborted");
            return OGS_ERROR;
    }    

    atexit(terminate);
    ogs_signal_thread(check_signal);

    ogs_info("Open5GS daemon terminating...");

    return OGS_OK;
}

