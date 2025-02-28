/*-
 * Copyright (c) 2012-2013 Wojciech Owczarek,
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
 * @file   ptpd.c
 * @date   Wed Jun 23 10:13:38 2010
 *
 * @brief  The main() function for the PTP daemon
 *
 * This file contains very little code, as should be obvious,
 * and only serves to tie together the rest of the daemon.
 * All of the default options are set here, but command line
 * arguments and configuration file is processed in the 
 * ptpdStartup() routine called
 * below.
 */

#include "ptpd.h"

RunTimeOpts rtOpts;			/* statically allocated run-time
					 * configuration data */

Boolean startupInProgress;

/*
 * Global variable with the main PTP port. This is used to show the current state in DBG()/message()
 * without having to pass the pointer everytime.
 *
 * if ptpd is extended to handle multiple ports (eg, to instantiate a Boundary Clock),
 * then DBG()/message() needs a per-port pointer argument
 */
PtpClock *G_ptpClock = NULL;

TimingDomain timingDomain;


int
ptpd_task(Boolean bslaveOnly)
{
	PtpClock *ptpClock;
	Integer16 ret;
	TimingService *ts;

	startupInProgress = TRUE;

	memset(&timingDomain, 0, sizeof(timingDomain));
	timingDomainSetup(&timingDomain);

	timingDomain.electionLeft = 10;

	/* Wipe the memory first to avoid unconsistent behaviour - no need to set Boolean to FALSE, int to 0 etc. */
	memset(&rtOpts, 0, sizeof(RunTimeOpts));

	rtOpts.logAnnounceInterval = DEFAULT_ANNOUNCE_INTERVAL;
	rtOpts.logSyncInterval = DEFAULT_SYNC_INTERVAL;
	rtOpts.logMinPdelayReqInterval = DEFAULT_PDELAYREQ_INTERVAL;
	rtOpts.clockQuality.clockAccuracy = DEFAULT_CLOCK_ACCURACY;
	rtOpts.clockQuality.clockClass = DEFAULT_CLOCK_CLASS;
	rtOpts.clockQuality.offsetScaledLogVariance = DEFAULT_CLOCK_VARIANCE;
	rtOpts.priority1 = DEFAULT_PRIORITY1;
	rtOpts.priority2 = DEFAULT_PRIORITY2;
	rtOpts.domainNumber = DEFAULT_DOMAIN_NUMBER;
	rtOpts.portNumber = NUMBER_PORTS;

	rtOpts.anyDomain = FALSE;

	rtOpts.transport = UDP_IPV4;

	/* timePropertiesDS */
	rtOpts.timeProperties.currentUtcOffsetValid = DEFAULT_UTC_VALID;
	rtOpts.timeProperties.currentUtcOffset = DEFAULT_UTC_OFFSET;
	rtOpts.timeProperties.timeSource = INTERNAL_OSCILLATOR;
	rtOpts.timeProperties.timeTraceable = FALSE;
	rtOpts.timeProperties.frequencyTraceable = FALSE;
	rtOpts.timeProperties.ptpTimescale = TRUE;

	rtOpts.ipMode = IPMODE_MULTICAST;
	rtOpts.dot2AS = FALSE;

	rtOpts.unicastNegotiation = FALSE;
	rtOpts.unicastNegotiationListening = FALSE;
	rtOpts.disableBMCA = FALSE;
	rtOpts.unicastGrantDuration = 300;

	rtOpts.noAdjust = NO_ADJUST;  // false
	rtOpts.logStatistics = TRUE;
	rtOpts.statisticsTimestamp = TIMESTAMP_DATETIME;

	rtOpts.periodicUpdates = FALSE; /* periodically log a status update */

	/* Deep display of all packets seen by the daemon */
	rtOpts.displayPackets = FALSE;

	rtOpts.s = DEFAULT_DELAY_S;
	rtOpts.inboundLatency.nanoseconds = DEFAULT_INBOUND_LATENCY;
	rtOpts.outboundLatency.nanoseconds = DEFAULT_OUTBOUND_LATENCY;
	rtOpts.max_foreign_records = DEFAULT_MAX_FOREIGN_RECORDS;
	rtOpts.nonDaemon = FALSE;

	/*
	 * defaults for new options
	 */
	rtOpts.ignore_delayreq_interval_master = FALSE;
	rtOpts.do_IGMP_refresh = TRUE;
	rtOpts.useSysLog       = FALSE;
	rtOpts.announceReceiptTimeout  = DEFAULT_ANNOUNCE_RECEIPT_TIMEOUT;
#ifdef RUNTIME_DEBUG
	rtOpts.debug_level = LOG_INFO;			/* by default debug messages as disabled, but INFO messages and below are printed */
#endif
	rtOpts.ttl = 64;
	rtOpts.delayMechanism   = DEFAULT_DELAY_MECHANISM;
	rtOpts.noResetClock     = DEFAULT_NO_RESET_CLOCK;
	rtOpts.stepOnce	 = FALSE;
	rtOpts.stepForce	 = FALSE;
#ifdef HAVE_LINUX_RTC_H
	rtOpts.setRtc		 = FALSE;
#endif /* HAVE_LINUX_RTC_H */

	rtOpts.clearCounters = FALSE;
	rtOpts.statisticsLogInterval = 0;

	rtOpts.initial_delayreq = DEFAULT_DELAYREQ_INTERVAL;
	rtOpts.logMinDelayReqInterval = DEFAULT_DELAYREQ_INTERVAL;
	rtOpts.autoDelayReqInterval = TRUE;
	rtOpts.masterRefreshInterval = 60;

	/* maximum.alues for unicast negotiation */
    rtOpts.logMaxPdelayReqInterval = 5;
	rtOpts.logMaxDelayReqInterval = 5;
	rtOpts.logMaxSyncInterval = 5;
	rtOpts.logMaxAnnounceInterval = 5;



	rtOpts.drift_recovery_method = DRIFT_KERNEL;
	rtOpts.autoLockFile = FALSE;
	rtOpts.snmp_enabled = FALSE;
	/* This will only be used if the "none" preset is configured */
	rtOpts.slaveOnly = bslaveOnly;
	/* Otherwise default to slave only via the preset */
	//rtOpts.selectedPreset = PTP_PRESET_SLAVEONLY;
	rtOpts.pidAsClockId = FALSE;

	/* highest possible */
	rtOpts.logLevel = LOG_ALL;

	/* ADJ_FREQ_MAX by default */
	rtOpts.servoMaxPpb = ADJ_FREQ_MAX;
	/* kP and kI are scaled to 10000 and are gains now - values same as originally */
	rtOpts.servoKP = 0.5;
	rtOpts.servoKI = 0.25;

	rtOpts.servoDtMethod = DT_CONSTANT;
	/* when measuring dT, use a maximum of 5 sync intervals (would correspond to avg 20% discard rate) */
	rtOpts.servoMaxdT = 5.0;

	/* disabled by default */
	rtOpts.announceTimeoutGracePeriod = 0;
	rtOpts.alwaysRespectUtcOffset=FALSE;
	rtOpts.preferUtcValid=FALSE;
	rtOpts.requireUtcValid=FALSE;

	/* Try 46 for expedited forwarding */
	rtOpts.dscpValue = 0;

#if (defined(linux) && defined(HAVE_SCHED_H)) || defined(HAVE_SYS_CPUSET_H)
	rtOpts.cpuNumber = -1;
#endif /* (linux && HAVE_SCHED_H) || HAVE_SYS_CPUSET_H*/

#ifdef PTPD_STATISTICS

	rtOpts.oFilterMSConfig.enabled = FALSE;
	rtOpts.oFilterMSConfig.discard = TRUE;
	rtOpts.oFilterMSConfig.autoTune = TRUE;
	rtOpts.oFilterMSConfig.stepDelay = FALSE;
	rtOpts.oFilterMSConfig.alwaysFilter = FALSE;
	rtOpts.oFilterMSConfig.stepThreshold = 1000000;
	rtOpts.oFilterMSConfig.stepLevel = 500000;
	rtOpts.oFilterMSConfig.capacity = 20;
	rtOpts.oFilterMSConfig.threshold = 1.0;
	rtOpts.oFilterMSConfig.weight = 1;
	rtOpts.oFilterMSConfig.minPercent = 20;
	rtOpts.oFilterMSConfig.maxPercent = 95;
	rtOpts.oFilterMSConfig.thresholdStep = 0.1;
	rtOpts.oFilterMSConfig.minThreshold = 0.1;
	rtOpts.oFilterMSConfig.maxThreshold = 5.0;
	rtOpts.oFilterMSConfig.delayCredit = 200;
	rtOpts.oFilterMSConfig.creditIncrement = 10;
	rtOpts.oFilterMSConfig.maxDelay = 1500;

	rtOpts.oFilterSMConfig.enabled = FALSE;
	rtOpts.oFilterSMConfig.discard = TRUE;
	rtOpts.oFilterSMConfig.autoTune = TRUE;
	rtOpts.oFilterSMConfig.stepDelay = FALSE;
	rtOpts.oFilterSMConfig.alwaysFilter = FALSE;
	rtOpts.oFilterSMConfig.stepThreshold = 1000000;
	rtOpts.oFilterSMConfig.stepLevel = 500000;
	rtOpts.oFilterSMConfig.capacity = 20;
	rtOpts.oFilterSMConfig.threshold = 1.0;
	rtOpts.oFilterSMConfig.weight = 1;
	rtOpts.oFilterSMConfig.minPercent = 20;
	rtOpts.oFilterSMConfig.maxPercent = 95;
	rtOpts.oFilterSMConfig.thresholdStep = 0.1;
	rtOpts.oFilterSMConfig.minThreshold = 0.1;
	rtOpts.oFilterSMConfig.maxThreshold = 5.0;
	rtOpts.oFilterSMConfig.delayCredit = 200;
	rtOpts.oFilterSMConfig.creditIncrement = 10;
	rtOpts.oFilterSMConfig.maxDelay = 1500;

	rtOpts.filterMSOpts.enabled = FALSE;
	rtOpts.filterMSOpts.filterType = FILTER_MIN;
	rtOpts.filterMSOpts.windowSize = 4;
	rtOpts.filterMSOpts.windowType = WINDOW_SLIDING;

	rtOpts.filterSMOpts.enabled = FALSE;
	rtOpts.filterSMOpts.filterType = FILTER_MIN;
	rtOpts.filterSMOpts.windowSize = 4;
	rtOpts.filterSMOpts.windowType = WINDOW_SLIDING;

	/* How often refresh statistics (seconds) */
	rtOpts.statsUpdateInterval = 30;
	/* Servo stability detection settings follow */
	rtOpts.servoStabilityDetection = FALSE;
	/* Stability threshold (ppb) - observed drift std dev value considered stable */
	rtOpts.servoStabilityThreshold = 10;
	/* How many consecutive statsUpdateInterval periods of observed drift std dev within threshold  means stable servo */
	rtOpts.servoStabilityPeriod = 1;
	/* How many minutes without servo stabilisation means servo has not stabilised */
	rtOpts.servoStabilityTimeout = 10;
	/* How long to wait for one-way delay prefiltering */
	rtOpts.calibrationDelay = 0;
	/* if set to TRUE and maxDelay is defined, only check against threshold if servo is stable */
	rtOpts.maxDelayStableOnly = FALSE;
	/* if set to non-zero, reset slave if more than this amount of consecutive delay measurements was above maxDelay */
	rtOpts.maxDelayMaxRejected = 0;
#endif

	/* status file options */
	rtOpts.statusFileUpdateInterval = 1;

	/* panic mode options */
	rtOpts.enablePanicMode = FALSE;
	rtOpts.panicModeDuration = 2;
	rtOpts.panicModeExitThreshold = 0;

	/* full network reset after 5 times in listening */
	rtOpts.maxListen = 5;

	rtOpts.panicModeReleaseClock = FALSE;
	rtOpts.ntpOptions.enableEngine = FALSE;
	rtOpts.ntpOptions.enableControl = FALSE;
	rtOpts.ntpOptions.enableFailover = FALSE;
	rtOpts.ntpOptions.failoverTimeout = 120;
	rtOpts.ntpOptions.checkInterval = 15;
	rtOpts.ntpOptions.keyId = 0;
	strncpy(rtOpts.ntpOptions.hostAddress,"localhost",MAXHOSTNAMELEN); 	/* not configurable, but could be */
	rtOpts.preferNTP = FALSE;

	rtOpts.leapSecondPausePeriod = 5;
	/* by default, announce the leap second 12 hours before the event:
	 * Clause 9.4 paragraph 5 */
	rtOpts.leapSecondNoticePeriod = 43200;
	rtOpts.leapSecondHandling = LEAP_ACCEPT;
	rtOpts.leapSecondSmearPeriod = 86400;

/* timing domain */
	rtOpts.idleTimeout = 120; /* idle timeout */
	rtOpts.electionDelay = 15; /* anti-flapping delay */

/* Log file settings */

	rtOpts.statisticsLog.logID = "statistics";
	rtOpts.statisticsLog.openMode = "a+";
	rtOpts.statisticsLog.logFP = NULL;
	rtOpts.statisticsLog.truncateOnReopen = FALSE;
	rtOpts.statisticsLog.unlinkOnClose = FALSE;
	rtOpts.statisticsLog.maxSize = 0;

	rtOpts.recordLog.logID = "record";
	rtOpts.recordLog.openMode = "a+";
	rtOpts.recordLog.logFP = NULL;
	rtOpts.recordLog.truncateOnReopen = FALSE;
	rtOpts.recordLog.unlinkOnClose = FALSE;
	rtOpts.recordLog.maxSize = 0;

	rtOpts.eventLog.logID = "log";
	rtOpts.eventLog.openMode = "a+";
	rtOpts.eventLog.logFP = NULL;
	rtOpts.eventLog.truncateOnReopen = FALSE;
	rtOpts.eventLog.unlinkOnClose = FALSE;
	rtOpts.eventLog.maxSize = 0;

	rtOpts.statusLog.logID = "status";
	rtOpts.statusLog.openMode = "w";
	rtOpts.statusLog.logFP = NULL;
	rtOpts.statusLog.truncateOnReopen = FALSE;
	rtOpts.statusLog.unlinkOnClose = TRUE;

/* Management message support settings */
	rtOpts.managementEnabled = TRUE;
	rtOpts.managementSetEnable = FALSE;

/* IP ACL settings */

	rtOpts.timingAclEnabled = FALSE;
	rtOpts.managementAclEnabled = FALSE;
	rtOpts.timingAclOrder = ACL_DENY_PERMIT;
	rtOpts.managementAclOrder = ACL_DENY_PERMIT;

	// by default we don't check Sync message sequence continuity
	rtOpts.syncSequenceChecking = FALSE;
	rtOpts.clockUpdateTimeout = 0;

	char eth0[] = "eth0";
	memcpy(rtOpts.primaryIfaceName, eth0, sizeof(eth0));
	rtOpts.backupIfaceEnabled = FALSE;
	rtOpts.delayMechanism = DEFAULT_DELAY_MECHANISM;

	/* Initialize run time options with command line arguments */
	if (!(ptpClock = ptpdStartup(0, NULL, &ret, &rtOpts))) {
		if (ret != 0 && !rtOpts.checkConfigOnly)
			ERROR(USER_DESCRIPTION" startup failed\n");
		return ret;
	}

	timingDomain.electionDelay = rtOpts.electionDelay;

	/* configure PTP TimeService */

	timingDomain.services[0] = &ptpClock->timingService;
	ts = timingDomain.services[0];
	strncpy(ts->id, "PTP0", TIMINGSERVICE_MAX_DESC);
	ts->dataSet.priority1 = rtOpts.preferNTP;
	ts->dataSet.type = TIMINGSERVICE_PTP;
	ts->config = &rtOpts;
	ts->controller = ptpClock;
	ts->timeout = rtOpts.idleTimeout;
	ts->updateInterval = 1;
	ts->holdTime = rtOpts.ntpOptions.failoverTimeout;
	timingDomain.serviceCount = 1;

	if (rtOpts.ntpOptions.enableEngine) {
		ntpSetup(&rtOpts, ptpClock);
	} else {
	    timingDomain.serviceCount = 1;
	    timingDomain.services[1] = NULL;
	}

	timingDomain.init(&timingDomain);
	timingDomain.updateInterval = 1;

	startupInProgress = FALSE;

	/* global variable for message(), please see comment on top of this file */
	G_ptpClock = ptpClock;

	/* do the protocol engine */
	protocol(&rtOpts, ptpClock);
	/* forever loop.. */

	/* this also calls ptpd shutdown */
	timingDomain.shut(&timingDomain);

	NOTIFY("Self shutdown\n");

	return 1;
}
