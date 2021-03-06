
/***************************************************************************
 Name     :     T30.H
 Comment  :     Main include file for T30 driver.
                        All common structire defns etc.
 Functions:     (see Prototypes just below)

 Revision Log
 Date     Name  Description
 -------- ----- ---------------------------------------------------------
***************************************************************************/

#include "timeouts.h"


/**---------------------- #define of sizes of things ---------------------

        Frames can be at most 2.55 secs (sent) or 3.45 secs (recvd) long, or
        2.55 * 300/8 = 96 bytes and 132 bytes long respectively

        For clumps of received frames, we collect them in a GlobalAlloced
        space for passing to teh WhatNext callback function. We arbitrarily
        decide to allow at most 10 frames and at most 500 bytes at a time.
        (500 bytes = 500/(300/8) = 13.33 secs, so that should be quite enough)

---------------------- #define of sizes of things ---------------------**/

//#define ECM_TOTALFRAMESIZE    (ECM_FRAME_SIZE + 4)


// Used for calling ModemSync(). The shorter one during calls,
// the longer one only on hangup and stuff
#define RESYNC_TIMEOUT1         1100
#define RESYNC_TIMEOUT2         500


// This are way too long. We have built-in delays anyway, so don't need
// the full time, and the recvrs have to recv from other fax machines
// so take it easy here
// #define PHASEC_PAUSE                 100     // how long to pause before TCF/phase C
#define RECV_PHASEC_PAUSE               55      // used by RecvSil() before TCF/phase C
// ModemRecvSilence() always waits a bit longer than the amount requested
// so to get it right we ask for the minimum required, i.e. 55ms
// However ModemSendSilence() is pretty accurate, so ask for teh exact
// nominal amount, i.e. 75ms


// How long to try for AT+FRM before timing out and trying
// for an AT+FRH instead. Must be 100% *sure* to get page if there
// is one, because there's no recovery after missing that. But must
// also be sure to get the 2nd CTC if there is one.
// Other critical situations:- MCF is missed, and sender resends MPS
// We miss this because we're in FRM, but we mustn't miss the 3rd one!
// Ditto with DCS-TCF-missedCFR. Can't get 2nd, but we'll get 3rd
// #define PHASEC_TIMEOUT               2800L           // 2.8s
// increase this to 3200. Limit is 3550, but we need 200ms or so to restart
// receiving FRH=3
//#define PHASEC_TIMEOUT   3300L   // 3.3s
// increase this to 5 seconds
// it appears fax macines accept this delay before entering phase C
#define PHASEC_TIMEOUT   5000L   


// How long to try for AT+FRM in TCF before timing out and trying
// +FRH. Mustn't miss TCF, but if we do, must catch next DCS
// increase this to 3200. Limit is 3550, but we need 200ms or so to restart
// receiving FRH=3
#define TCF_TIMEOUT             3300L           // 3.3s


// don't need this one. Besides don't want to send it at startup when
// we're already transmitting HDLC. This can cause problems, if we already
// have a delay (example after TCF, sending CFR. Delay is now 115, it will
// become 180 or so).
// #define LOWSPEED_PAUSE               60              // how long to pause before HDLC
// reduce this too
#define RECV_LOWSPEED_PAUSE     55              // used by RecvSil() before HDLC
// ModemRecvSilence() always waits a bit longer than the amount requested
// so to get it right we ask for the minimum required, i.e. 55ms
// However ModemSendSilence() is pretty accurate, so ask for teh exact
// nominal amount, i.e. 75ms

// How long of silence to look for before sending V.21. (IFAX/MDDI only)
// Use LOWSPEED_PAUSE defined above.

// How long of silence to look for before sending PhaseC. (IFAX/MDDI only)
// use 40ms or more (want to be pretty sure we got silenec)
// ---> we use PHASEC_PAUSE which is OK

// How long to look for silence before sending high-speed.
// If this fails we go ahead and send anyway, so we don't want
// it too long. But we want to try pretty hard to get silence,
// because otherwise the other side will miss my training.
// 3secs is about how long a NSF-DIS takes. If we hit one
// we want to wait until it's done, not timeout??
#define LONG_RECVSILENCE_TIMEOUT        3000 // how long to wait for silence before send HDLC or PIX

// when sending DIS, DCS or DTC we may collide with DCS, DIS or DIS coming
// from the other side. This can be really long (preamble+2NSFs+CSI+DIS > 5secs)
// so wait for upto 6 secs (preamble + 150+ bytes)
#define REALLY_LONG_RECVSILENCE_TIMEOUT 6000 // how long to wait for silence before send DIS/DCS/DTC



#define ECHOPROTECT(ifr, mode)  { pTG->EchoProtect.ifrLastSent=ifr; pTG->EchoProtect.modePrevRecv=mode; pTG->EchoProtect.fGotWrongMode=0; }

extern USHORT TCFLen[];

#define GetResponse(pTG, ifr)        GetCmdResp(pTG, FALSE, ifr)
#define GetCommand(pTG, ifr)         GetCmdResp(pTG, TRUE, ifr)


/****************** begin prototypes from hdlc.c *****************/
USHORT ModemRecvBuf(PThrdGlbl pTG, LPBUFFER far* lplpbf, ULONG ulTimeout);
/***************** end of prototypes from hdlc.c *****************/

/****************** begin prototypes from t30.c *****************/
IFR GetCmdResp(PThrdGlbl pTG, BOOL fCommand, USHORT ifrResp);
/***************** end of prototypes from t30.c *****************/



/****************** begin prototypes from t30main.c *****************/
USHORT T30MainBody(PThrdGlbl pTG, BOOL fCaller);
/***************** end of prototypes from t30main.c *****************/





