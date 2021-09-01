#ifndef MAX3421E_REGS_H
#define MAX3421E_REGS_H 1

// host mode (R27 HOST == 1)
// R = read
// RC = read/clear        (write 1 to clear)
// RSC = read/set/clear
// LS = initiates host operation
#define RCVFIFO  1  // b7        b6        b5        b4       b3        b2        b1        b0          |  RSC
#define SNDFIFO  2  // b7        b6        b5        b4       b3        b2        b1        b0          |  RSC
#define SUDFIFO  4  // b7        b6        b5        b4       b3        b2        b1        b0          |  RSC
#define RCVBC    6  // 0         BC6       BC5       BC4      BC3       BC2       BC1       BC0         |  RSC
#define SNDBC    7  // 0         BC6       BC5       BC4      BC3       BC2       BC1       BC0         |  RSC
#define USBIRQ   13 // 0         VBUSIRQ   NOVBUSIRQ 0        0         0         0         OSCOKIRQ    |  RC
#define USBIEN   14 // 0         VBUSIE    NOVBUSIE  0        0         0         0         OSCOKIE     |  RSC
#define USBCTL   15 // 0         0         CHIPRES   PWRDOWN  0         0         0         0           |  RSC
#define CPUCTL   16 // PULSEWID1 PULSEWID0 0         0        0         0         0         IE          |  RSC
#define PINCTL   17 // EP3INAK   EP2INAK   EP0INAK   FDUPSPI  INTLEVEL  POSINT    GPXB      GPXA        |  RSC
#define REVISION 18 // 0         0         0         1        0         0         1         1           |  R
#define IOPINS1  20 // GPIN3     GPIN2     GPIN1     GPIN0    GPOUT3    GPOUT2    GPOUT1    GPOUT0      |  RSC
#define IOPINS2  21 // GPIN7     GPIN6     GPIN5     GPIN4    GPOUT7    GPOUT6    GPOUT5    GPOUT4      |  RSC
#define GPINIRQ  22 // GPINIRQ7  GPINIRQ6  GPINIRQ5  GPINIRQ4 GPINIRQ3  GPINIRQ2  GPINIRQ1  GPINIRQ0    |  RC
#define GPINIEN  23 // GPINIEN7  GPINIEN6  GPINIEN5  GPINIEN4 GPINIEN3  GPINIEN2  GPINIEN1  GPINIEN0    |  RSC
#define GPINPOL  24 // GPINPOL7  GPINPOL6  GPINPOL5  GPINPOL4 GPINPOL3  GPINPOL2  GPINPOL1  GPINPOL0    |  RSC
#define HIRQ     25 // HXFRDNIRQ FRAMEIRQ  CONNIRQ   SUSDNIRQ SNDBAVIRQ RCVDAVIRQ RSMREQIRQ BUSEVENTIRQ |  RC
#define HIEN     26 // HXFRDNIE  FRAMEIE   CONNIE    SUSDNIE  SNDBAVIE  RCVDAVIE  RSMREQIE  BUSEVENTIE  |  RSC
#define MODE     27 // DPPULLDN  DMPULLDN  DELAYISO  SEPIRQ   SOFKAENAB HUBPRE    SPEED     HOST        |  RSC
#define PERADDR  28 // 0         b6        b5        b4       b3        b2        b1        b0          |  RSC
#define HCTL     29 // SNDTOG1   SNDTOG0   RCVTOG1   RCVTOG0  SIGRSM    BUSSAMPLE FRMRST    BUSRST      |  LS
#define HXFR     30 // HS        ISO       OUTNIN    SETUP    EP3       EP2       EP1       EP0         |  LS
#define HRSL     31 // JSTATUS   KSTATUS   SNDTOGRD  RCVTOGRD HRSLT3    HRSLT2    HRSLT1    HRSLT0      |  R

#define BUSEVENTIE 0
#define BUSEVENTIRQ 0
#define BUSRST 0
#define BUSSAMPLE 2
#define CHIPRES 5
#define CONNIE 5
#define CONNIRQ 5
#define DELAYISO 5
#define DMPULLDN 6
#define DPPULLDN 7
#define EP0 0
#define EP0INAK 5
#define EP1 1
#define EP2 2
#define EP2INAK 6
#define EP3 3
#define EP3INAK 7
#define FDUPSPI 4
#define FRAMEIE 6
#define FRAMEIRQ 6
#define FRMRST 1
#define GPIN0 4
#define GPIN1 5
#define GPIN2 6
#define GPIN3 7
#define GPIN4 4
#define GPIN5 5
#define GPIN6 6
#define GPIN7 7
#define GPINIEN0 0
#define GPINIEN1 1
#define GPINIEN2 2
#define GPINIEN3 3
#define GPINIEN4 4
#define GPINIEN5 5
#define GPINIEN6 6
#define GPINIEN7 7
#define GPINIRQ0 0
#define GPINIRQ1 1
#define GPINIRQ2 2
#define GPINIRQ3 3
#define GPINIRQ4 4
#define GPINIRQ5 5
#define GPINIRQ6 6
#define GPINIRQ7 7
#define GPINPOL0 0
#define GPINPOL1 1
#define GPINPOL2 2
#define GPINPOL3 3
#define GPINPOL4 4
#define GPINPOL5 5
#define GPINPOL6 6
#define GPINPOL7 7
#define GPOUT0 0
#define GPOUT1 1
#define GPOUT2 2
#define GPOUT3 3
#define GPOUT4 0
#define GPOUT5 1
#define GPOUT6 2
#define GPOUT7 3
#define GPXA 0
#define GPXB 1
#define HOST 0
#define HRSLT0 0
#define HRSLT1 1
#define HRSLT2 2
#define HRSLT3 3
#define HS 7
#define HUBPRE 2
#define HXFRDNIE 7
#define HXFRDNIRQ 7
#define IE 0
#define INTLEVEL 3
#define ISO 6
#define JSTATUS 7
#define KSTATUS 6
#define NOVBUSIE 5
#define NOVBUSIRQ 5
#define OSCOKIE 0
#define OSCOKIRQ 0
#define OUTNIN 5
#define POSINT 2
#define PULSEWID0 6
#define PULSEWID1 7
#define PWRDOWN 4
#define RCVDAVIE 2
#define RCVDAVIRQ 2
#define RCVTOG0 4
#define RCVTOG1 5
#define RCVTOGRD 4
#define RSMREQIE 1
#define RSMREQIRQ 1
#define SEPIRQ 4
#define SETUP 4
#define SIGRSM 3
#define SNDBAVIE 3
#define SNDBAVIRQ 3
#define SNDTOG0 6
#define SNDTOG1 7
#define SNDTOGRD 5
#define SOFKAENAB 3
#define SPEED 1
#define SUSDNIE 4
#define SUSDNIRQ 4
#define VBUSIE 6
#define VBUSIRQ 6

#endif /* MAX3421E_REGS_H */
