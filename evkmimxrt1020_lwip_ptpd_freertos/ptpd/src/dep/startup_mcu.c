/*-
 * Copyright (c) 2014-2015 Wojciech Owczarek,
 * Copyright (c) 2012-2013 George V. Neville-Neil,
 *                         Wojciech Owczarek
 * Copyright (c) 2011-2012 George V. Neville-Neil,
 *                         Steven Kreuzer, 
 *                         Martin Burnicki, 
 *                         Jan Breuer,
 *                         Gael Mace, 
 *                         Alexandre Van Kempen,
 *                         Inaqui Delgado,
 *                         Rick Ratzel,
 *                         National Instruments.
 * Copyright (c) 2009-2010 George V. Neville-Neil, 
 *                         Steven Kreuzer, 
 *                         Martin Burnicki, 
 *                         Jan Breuer,
 *                         Gael Mace, 
 *                         Alexandre Van Kempen
 *
 * Copyright (c) 2005-2008 Kendall Correll, Aidan Williams
 *
 * All Rights Reserved
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE AUTHORS ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @file   startup.c
 * @date   Wed Jun 23 09:33:27 2010
 * 
 * @brief  Code to handle daemon startup, including command line args
 * 
 * The function in this file are called when the daemon starts up
 * and include the getopt() command line argument parsing.
 */

#include "../ptpd.h"

/*
 * valgrind 3.5.0 currently reports no errors (last check: 20110512)
 * valgrind 3.4.1 lacks an adjtimex handler
 *
 * to run:   sudo valgrind --show-reachable=yes --leak-check=full --track-origins=yes -- ./ptpd2 -c ...
 */

/*
  to test daemon locking and startup sequence itself, try:

  function s()  { set -o pipefail ;  eval "$@" |  sed 's/^/\t/' ; echo $?;  }
  sudo killall ptpd2
  s ./ptpd2
  s sudo ./ptpd2
  s sudo ./ptpd2 -t -g
  s sudo ./ptpd2 -t -g -b eth0
  s sudo ./ptpd2 -t -g -b eth0
  ps -ef | grep ptpd2
*/

/*
 * Synchronous signal processing:
 * original idea: http://www.openbsd.org/cgi-bin/cvsweb/src/usr.sbin/ntpd/ntpd.c?rev=1.68;content-type=text%2Fplain
 */
volatile sig_atomic_t	 sigint_received  = 0;
volatile sig_atomic_t	 sigterm_received = 0;
volatile sig_atomic_t	 sighup_received  = 0;
volatile sig_atomic_t	 sigusr1_received = 0;
volatile sig_atomic_t	 sigusr2_received = 0;

/* Pointer to the current lock file */
FILE* G_lockFilePointer;

/*
 * Function to catch signals asynchronously.
 * Assuming that the daemon periodically calls checkSignals(), then all operations are safely done synchrously at a later opportunity.
 *
 * Please do NOT call any functions inside this handler - especially DBG() and its friends, or any glibc.
 */
void catchSignals(int sig)
{
	switch (sig) {
	case SIGINT:
		sigint_received = 1;
		break;
	case SIGTERM:
		sigterm_received = 1;
		break;
	case SIGHUP:
		sighup_received = 1;
		break;
	case SIGUSR1:
		sigusr1_received = 1;
		break;
	case SIGUSR2:
		sigusr2_received = 1;
		break;
	default:
		/*
		 * TODO: should all other signals be catched, and handled as SIGINT?
		 *
		 * Reason: currently, all other signals are just uncatched, and the OS kills us.
		 * The difference is that we could then close the open files properly.
		 */
		break;
	}
}

/*
 * exit the program cleanly
 */
void
do_signal_close(PtpClock * ptpClock)
{
    (void)ptpClock;

    return;
}

/** 
 * Signal handler for HUP which tells us to swap the log file
 * and reload configuration file if specified
 *
 * @param sig 
 */
void 
do_signal_sighup(RunTimeOpts * rtOpts, PtpClock * ptpClock)
{
    (void)rtOpts;
    (void)ptpClock;

    return;
}

void
restartSubsystems(RunTimeOpts *rtOpts, PtpClock *ptpClock)
{
    (void)rtOpts;
    (void)ptpClock;

    return;
}


/*
 * Synchronous signal processing:
 * This function should be called regularly from the main loop
 */
void
checkSignals(RunTimeOpts * rtOpts, PtpClock * ptpClock)
{
    (void)rtOpts;
    (void)ptpClock;

    return;
}

#ifdef RUNTIME_DEBUG
/* These functions are useful to temporarily enable Debug around parts of code, similar to bash's "set -x" */
void enable_runtime_debug(void )
{
	extern RunTimeOpts rtOpts;
	
	rtOpts.debug_level = max(LOG_DEBUGV, rtOpts.debug_level);
}

void disable_runtime_debug(void )
{
	extern RunTimeOpts rtOpts;
	
	rtOpts.debug_level = LOG_INFO;
}
#endif

int
writeLockFile(RunTimeOpts * rtOpts)
{
    (void)rtOpts;

    return 1;
}

void 
ptpdShutdown(PtpClock * ptpClock)
{

	extern RunTimeOpts rtOpts;
	
	/* 
         * go into DISABLED state so the FSM can call any PTP-specific shutdown actions,
	 * such as canceling unicast transmission
         */
	toState(PTP_DISABLED, &rtOpts, ptpClock);

	netShutdown(&ptpClock->netPath);
	free(ptpClock->foreign);

	/* free management and signaling messages, they can have dynamic memory allocated */
	if(ptpClock->msgTmpHeader.messageType == MANAGEMENT)
		freeManagementTLV(&ptpClock->msgTmp.manage);
	freeManagementTLV(&ptpClock->outgoingManageTmp);
	if(ptpClock->msgTmpHeader.messageType == SIGNALING)
		freeSignalingTLV(&ptpClock->msgTmp.signaling);
	freeSignalingTLV(&ptpClock->outgoingSignalingTmp);

#ifdef PTPD_SNMP
	snmpShutdown();
#endif /* PTPD_SNMP */

#ifdef HAVE_SYS_TIMEX_H
#ifndef PTPD_STATISTICS
	/* Not running statistics code - write observed drift to driftfile if enabled, inform user */
	if(ptpClock->slaveOnly && !ptpClock->servo.runningMaxOutput)
		saveDrift(ptpClock, &rtOpts, FALSE);
#else
	ptpClock->oFilterMS.shutdown(&ptpClock->oFilterMS);
	ptpClock->oFilterSM.shutdown(&ptpClock->oFilterSM);
        freeDoubleMovingStatFilter(&ptpClock->filterMS);
        freeDoubleMovingStatFilter(&ptpClock->filterSM);

	/* We are running statistics code - save drift on exit only if we're not monitoring servo stability */
	if(!rtOpts.servoStabilityDetection && !ptpClock->servo.runningMaxOutput)
		saveDrift(ptpClock, &rtOpts, FALSE);
#endif /* PTPD_STATISTICS */
#endif /* HAVE_SYS_TIMEX_H */

	if (rtOpts.currentConfig != NULL)
		//dictionary_del(rtOpts.currentConfig);
	if(rtOpts.cliConfig != NULL)
		//dictionary_del(rtOpts.cliConfig);

	timerShutdown(ptpClock->timers);

	free(ptpClock);
	ptpClock = NULL;

	extern PtpClock* G_ptpClock;
	G_ptpClock = NULL;
}

PtpClock *
ptpdStartup(int argc, char **argv, Integer16 * ret, RunTimeOpts * rtOpts)
{
	PtpClock * ptpClock;
	int i = 0;

    (void)argc;
    (void)argv;

	/** 
	 * If a required setting, such as interface name, or a setting
	 * requiring a range check is to be set via getopts_long,
	 * the respective currentConfig dictionary entry should be set,
	 * instead of just setting the rtOpts field.
	 *
	 * Config parameter evaluation priority order:
	 * 	1. Any dictionary keys set in the getopt_long loop
	 * 	2. CLI long section:key type options
	 * 	3. Config file (parsed last), merged with 2. and 3 - will be overwritten by CLI options
	 * 	4. Defaults and any rtOpts fields set in the getopt_long loop
	**/

	/**
	 * Load defaults. Any options set here and further inside loadCommandLineOptions()
	 * by setting rtOpts fields, will be considered the defaults
	 * for config file and section:key long options.
	 */
	//loadDefaultSettings(rtOpts);

	/* Check network before going into background */
	if(!testInterface(rtOpts->primaryIfaceName, rtOpts)) {
	    ERROR("Error: Cannot use %s interface\n",rtOpts->primaryIfaceName);
	    *ret = 1;
	    goto configcheck;
	}
	if(rtOpts->backupIfaceEnabled && !testInterface(rtOpts->backupIfaceName, rtOpts)) {
	    ERROR("Error: Cannot use %s interface as backup\n",rtOpts->backupIfaceName);
	    *ret = 1;
	    goto configcheck;
	}

	*ret = 0;


configcheck:
	/*
	 * We've been told to check config only - clean exit before checking locks
	 */
	if(rtOpts->checkConfigOnly) {
	    if(*ret != 0) {
		printf("Configuration has errors\n");
		*ret = 1;
		}
	    else
		printf("Configuration OK\n");
	    return 0;
	}

	/* Previous errors - exit */
	if(*ret !=0)
		return 0;

	/* First lock check, just to be user-friendly to the operator */
	if(!rtOpts->ignore_daemon_lock) {
		if(!writeLockFile(rtOpts)){
			/* check and create Lock */
			ERROR("Error: file lock failed (use -L or global:ignore_lock to ignore lock file)\n");
			*ret = 3;
			return 0;
		}
		/* check for potential conflicts when automatic lock files are used */
		if(!checkOtherLocks(rtOpts)) {
			*ret = 3;
			return 0;
		}
	}

	/* Manage log files: stats, log, status and quality file */
	//restartLogging(rtOpts);

	/* Allocate memory after we're done with other checks but before going into daemon */
	ptpClock = (PtpClock *) malloc(sizeof(PtpClock));
	if (!ptpClock) {
		PERROR("Error: Failed to allocate memory for protocol engine data");
		*ret = 2;
		return 0;
	} else {
		DBG("allocated %d bytes for protocol engine data\n", 
		    (int)sizeof(PtpClock));
		memset(ptpClock, 0, sizeof(PtpClock));

		ptpClock->foreign = (ForeignMasterRecord *)
				malloc(rtOpts->max_foreign_records * sizeof(ForeignMasterRecord));
		if (!ptpClock->foreign) {
			PERROR("failed to allocate memory for foreign "
			       "master data");
			*ret = 2;
			free(ptpClock);
			return 0;
		} else {
			DBG("allocated %d bytes for foreign master data\n", 
			    (int)(rtOpts->max_foreign_records * 
				  sizeof(ForeignMasterRecord)));
		}
	}

	/* Init to 0 net buffer */
	//memset(ptpClock->msgIbuf, 0, PACKET_SIZE);
	//memset(ptpClock->msgObuf, 0, PACKET_SIZE);

	/* Init user_description */
	memset(ptpClock->user_description, 0, sizeof(ptpClock->user_description));
	memcpy(ptpClock->user_description, &USER_DESCRIPTION, sizeof(USER_DESCRIPTION));
	
	/* Init outgoing management message */
	ptpClock->outgoingManageTmp.tlv = NULL;
	

	/*  DAEMON */
#ifdef PTPD_NO_DAEMON
	if(!rtOpts->nonDaemon){
		rtOpts->nonDaemon=TRUE;
	}
#endif

//	if(!rtOpts->nonDaemon){
//		/*
//		 * fork to daemon - nochdir non-zero to preserve the working directory:
//		 * allows relative paths to be used for log files, config files etc.
//		 * Always redirect stdout/err to /dev/null
//		 */
//		if (daemon(1,0) == -1) {
//			PERROR("Failed to start as daemon");
//			*ret = 3;
//			return 0;
//		}
//		INFO("  Info:    Now running as a daemon\n");
//		/*
//		 * Wait for the parent process to terminate, but not forever.
//		 * On some systems this happened after we tried re-acquiring
//		 * the lock, so the lock would fail. Hence, we wait.
//		 */
//		for (i = 0; i < 1000000; i++) {
//			/* Once we've been reaped by init, parent PID will be 1 */
//			if(getppid() == 1)
//				break;
//			usleep(1);
//		}
//	}
//
//	/* Second lock check, to replace the contents with our own new PID and re-acquire the advisory lock */
//	if(!rtOpts->nonDaemon && !rtOpts->ignore_daemon_lock){
//		/* check and create Lock */
//		if(!writeLockFile(rtOpts)){
//			ERROR("Error: file lock failed (use -L or global:ignore_lock to ignore lock file)\n");
//			*ret = 3;
//			return 0;
//		}
//	}

#if defined(linux) && defined(HAVE_SCHED_H)
	/* Try binding to a single CPU core if configured to do so */

	if(rtOpts->cpuNumber > -1) {

		cpu_set_t mask;
    		CPU_ZERO(&mask);
    		CPU_SET(rtOpts->cpuNumber,&mask);
    		if(sched_setaffinity(0, sizeof(mask), &mask) < 0) {
		    PERROR("Could not bind to CPU core %d", rtOpts->cpuNumber);
		} else {
		    INFO("Successfully bound "PTPD_PROGNAME" to CPU core %d\n", rtOpts->cpuNumber);
		}
	}
#endif /* linux && HAVE_SCHED_H */

#ifdef HAVE_SYS_CPUSET_H

	/* Try binding to a single CPU core if configured to do so */

	if(rtOpts->cpuNumber > -1) {
		cpuset_t mask;
    		CPU_ZERO(&mask);
    		CPU_SET(rtOpts->cpuNumber,&mask);
    		if(cpuset_setaffinity(CPU_LEVEL_WHICH, CPU_WHICH_PID,
				      -1, sizeof(mask), &mask) < 0) {
			PERROR("Could not bind to CPU core %d",
			       rtOpts->cpuNumber);
		} else {
			INFO("Successfully bound "PTPD_PROGNAME" to CPU core %d\n",
			     rtOpts->cpuNumber);
		}
	}
#endif /* HAVE_SYS_CPUSET_H */

	/* set up timers */
	if(!timerSetup(ptpClock->timers)) {
		PERROR("failed to set up event timers");
		*ret = 2;
		free(ptpClock);
		return 0;
	}
    
	/* use new synchronous signal handlers */
//	signal(SIGINT,  catchSignals);
//	signal(SIGTERM, catchSignals);
//	signal(SIGHUP,  catchSignals);
//
//	signal(SIGUSR1, catchSignals);
//	signal(SIGUSR2, catchSignals);

#if defined PTPD_SNMP
	/* Start SNMP subsystem */
	if (rtOpts->snmp_enabled)
		snmpInit(rtOpts, ptpClock);
#endif



//	NOTICE(USER_DESCRIPTION" started successfully on %s using \"%s\" preset\n",
//			    rtOpts->ifaceName,
//			    (getPtpPreset(rtOpts->selectedPreset,rtOpts)).presetName);
	ptpClock->resetStatisticsLog = TRUE;

#ifdef PTPD_STATISTICS

	outlierFilterSetup(&ptpClock->oFilterMS);
	outlierFilterSetup(&ptpClock->oFilterSM);

	ptpClock->oFilterMS.init(&ptpClock->oFilterMS,&rtOpts->oFilterMSConfig, "delayMS");
	ptpClock->oFilterSM.init(&ptpClock->oFilterSM,&rtOpts->oFilterSMConfig, "delaySM");


	if(rtOpts->filterMSOpts.enabled) {
		ptpClock->filterMS = createDoubleMovingStatFilter(&rtOpts->filterMSOpts,"delayMS");
	}

	if(rtOpts->filterSMOpts.enabled) {
		ptpClock->filterSM = createDoubleMovingStatFilter(&rtOpts->filterSMOpts, "delaySM");
	}

#endif

#ifdef PTPD_PCAP
		ptpClock->netPath.pcapEventSock = -1;
		ptpClock->netPath.pcapGeneralSock = -1;
#endif /* PTPD_PCAP */

		//ptpClock->netPath.generalSock = -1;
		//ptpClock->netPath.eventSock = -1;
 
	*ret = 0;
	return ptpClock;
	
fail:
	dictionary_del(rtOpts->candidateConfig);
	return 0;
}

void
ntpSetup (RunTimeOpts *rtOpts, PtpClock *ptpClock)
{
	TimingService *ts = &ptpClock->ntpControl.timingService;



	if (rtOpts->ntpOptions.enableEngine) {
	    timingDomain.services[1] = ts;
	    strncpy(ts->id, "NTP0", TIMINGSERVICE_MAX_DESC);
	    ts->dataSet.priority1 = 0;
	    ts->dataSet.type = TIMINGSERVICE_NTP;
	    ts->config = &rtOpts->ntpOptions;
	    ts->controller = &ptpClock->ntpControl;
	    /* for now, NTP is considered always active, so will never go idle */
	    ts->timeout = 60;
	    ts->updateInterval = rtOpts->ntpOptions.checkInterval;
	    timingDomain.serviceCount = 2;
	} else {
	    timingDomain.serviceCount = 1;
	    timingDomain.services[1] = NULL;
	    if(timingDomain.best == ts || timingDomain.current == ts || timingDomain.preferred == ts) {
		timingDomain.best = timingDomain.current = timingDomain.preferred = NULL;
	    }
	}
}

static void ptpd_thread(void *pvParaMeter)
{
    Boolean bslaveOnly;
    bool *ptp_master = (bool *)pvParaMeter;

	bslaveOnly = (*ptp_master) ? FALSE : TRUE;


    ptpd_task(bslaveOnly);
}


void ptpd_init(bool bMaster)
{
    TaskHandle_t xCreatedTask;

    xTaskCreate(ptpd_thread, "PTPd", DEFAULT_THREAD_STACKSIZE, &bMaster, DEFAULT_THREAD_PRIO + 1, &xCreatedTask);

}




