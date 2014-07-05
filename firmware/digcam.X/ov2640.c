/*
 * File:   ov2640.c
 * Author: Fernando Barbosa
 *
 * Created on May 10, 2014, 02:02 AM
 *
 * Description: camera isnstruction file.
 */

/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/

#include "lib/picdev/picDev.h"  // Connections of the picDev board
#include "lib/picdev/pinOut.h"  // Pin mapping of the picDev Board

#include "i2c.h"
#include "delay.h"
#include "main.h"
#include "ov2640.h"
#include "uart1.h"
#include "ov2640_regs.h"

#include <stdio.h>

/******************************************************************************/
/* OV2640 Functions                                                           */
/******************************************************************************/

// Private functions
int CamIsOn();
int CamIsReset();
int CamCheckPCLK();
int CamCheckHREFX();
int CamCheckVSYNC();
long CamCountLines();
long CamCountBytesPerLine();

/**
 * Initializes pin connections and turns on the Camera.
 */
void CamInit() {
    // Output
    _TRISB3 = 0; // PWDN
    _TRISB4 = 0; // RESET

    // Input
    _TRISD0 = 1; // PCLK
    _TRISF3 = 1; // HREFX
    _TRISD11 = 1; // VSYNC

    // Input
    _TRISB0 = 1;    // D7
    _TRISB14 = 1;   // D6
    _TRISB13 = 1;   // D5
    _TRISB12 = 1;   // D4
    _TRISB11 = 1;   // D3
    _TRISB10 = 1;   // D2
    _TRISB9 = 1;    // D1
    _TRISB8 = 1;    // D0
    _TRISB5 = 1;    // Y1 (not used)
    _TRISB2 = 1;    // Y0 (not used)

    // Clock output (XVCLK) 12 MHZ
    REFOCON = 0x9000;

    // Turn on camera
    CamTurnOn();
    CamReset();
}

/**
 * Software reset on the camera.
 */
void CamReset() {
    CamWrite(0xff, 0x01); // Select bank
    CamWrite(0x12, 0x80); // Reset

    Delayms(100);

    /* SQCIF mode = 1600 X 1200 */
//    CamWrite(0xff, 0x01); // Select bank
//    CamWrite(0x12, 0x10); // CIF mode
//    CamWrite(0x17, 0x11); // HREFST[10:3] (Default)
//    CamWrite(0x03, 0x06); // CIF default
//    CamWrite(0x18, 0x21); // HREFEND[10:3] (CIF default)
//    CamWrite(0x19, 0x00); // VSTRT[9:2] (CIF default)
//    CamWrite(0x1a, 0x18); // VEND[9:2] (Default)
//    CamWrite(0x32, 0x09); // Bit[5:3]: HREFEND[2:0]; Bit[2:0]: HREFST[2:0] (CIF default)
//    CamWrite(0x03, 0x0A); // Bit[3:2]: VEND[1:0]; Bit[1:0]: VSTRT[1:0] (CIF default)

    /* Window
     * HREFST = 0001 0001 001 = 137
     * HREFEND = 0010 0001 001 = 265
     * VSTRT = 0000 0000 10 = 2
     * VEND = 0001 1000 10 = 98
     * HREFEND - HREFST = 128
     * VEND - VSTRT = 96
     */

    /* SQCIF mode 128 x 96 */
//    CamWrite(0xff, 0x01); // Select bank
//    CamWrite(0x12, 0x10); // CIF mode
//    CamWrite(0x17, 0x22); // HREFST[10:3] (Default)
//    CamWrite(0x03, 0x06); // CIF default
//    CamWrite(0x18, 0x32); // HREFEND[10:3] (CIF default)
//    CamWrite(0x19, 0x00); // VSTRT[9:2] (CIF default)
//    CamWrite(0x1a, 0x4A); // VEND[9:2]
//    CamWrite(0x32, 0x09); // Bit[5:3]: HREFEND[2:0]; Bit[2:0]: HREFST[2:0] (CIF default)
//    CamWrite(0x03, 0x0A); // Bit[3:2]: VEND[1:0]; Bit[1:0]: VSTRT[1:0] (CIF default)

    /* Window
     * HREFST = 0010 0010 001 = 273
     * HREFEND = 0011 0010 001 = 401
     * VSTRT = 0000 0000 10 = 2
     * VEND = 0100 1010 10 = 298
     * HREFEND - HREFST = 400
     * VEND - VSTRT = 296
     */

    /* CIF mode */
    CamWrite(0xff, 0x01); // Select bank
    CamWrite(0x12, 0x10); // CIF mode
    CamWrite(0x17, 0x11); // HREFST[10:3] (Default)
    CamWrite(0x03, 0x06); // CIF default
    CamWrite(0x18, 0x43); // HREFEND[10:3] (CIF default)
    CamWrite(0x19, 0x00); // VSTRT[9:2] (CIF default)
    CamWrite(0x1a, 0x97); // VEND[9:2] (Default)
    CamWrite(0x32, 0x09); // Bit[5:3]: HREFEND[2:0]; Bit[2:0]: HREFST[2:0] (CIF default)
    CamWrite(0x03, 0x06); // Bit[3:2]: VEND[1:0]; Bit[1:0]: VSTRT[1:0] (CIF default)

    /* Window
     * HREFST = 0001 0001 001 = 137
     * HREFEND = 0100 0011 001 = 537
     * VSTRT = 0000 0000 10 = 2
     * VEND = 1001 0111 01 = 605
     * HREFEND - HREFST = 400
     * VEND - VSTRT = 603
     */

    CamWrite(0xff, 0x00); // Select bank
    CamWrite(0x2c, 0xff); // Configure PCLK/VSYNC/HREFX as output
    CamWrite(0x2e, 0xdf); // Configure PCLK/VSYNC/HREFX as output

    CamWrite(0xff, 0x01);
    CamWrite(0x11, 35); // divide XCLK by 36 (0,333 MHz)
}

int CamIsOn() {
    if (CAM_PWDN) {
        return 0;
    } else {
        return 1;
    }
}

int CamIsReset() {
    if (CAM_RESET) {
        return 0;
    } else {
        return 1;
    }
}

/**
 * Return camera status.
 * @return
 */
struct cam_status CamStatus() {
    struct cam_status tempStatus;

    tempStatus.POWER = CamIsOn();
    tempStatus.RESET = CamIsReset();
    tempStatus.PCLK = CamCheckPCLK();
    tempStatus.HREF = CamCheckHREFX();
    tempStatus.VSYNC = CamCheckVSYNC();

    tempStatus.WIDTH = CAM_WIDTH;
    tempStatus.HEIGHT = CAM_HEIGHT;

    if (tempStatus.PCLK && tempStatus.HREF && tempStatus.VSYNC) {
        tempStatus.NBYTELINE = CamCountBytesPerLine();
        tempStatus.NHSYNC = CamCountLines();
        tempStatus.NBYTETOTAL = (long)tempStatus.NBYTELINE * (long)tempStatus.NHSYNC;
    } else {
        tempStatus.NBYTELINE = 0;
        tempStatus.NHSYNC = 0;
        tempStatus.NBYTETOTAL = 0;
    }

    return tempStatus;
}

int CamCheckPCLK() {
    long i;
    unsigned char temp = CAM_PCLK;

    for (i = 0; i < 50000; i++) {
        if (temp != CAM_PCLK) {
            return 1;
        }
    }

    return 0;
}

int CamCheckVSYNC() {
    long i;
    unsigned char temp = CAM_VSYNC;

    for (i = 0; i < 50000000; i++) {
        if (temp != CAM_VSYNC) {
            return 1;
        }
    }

    return 0;
}

int CamCheckHREFX() {
    long i;
    unsigned char temp = CAM_HREFX;

    for (i = 0; i < 50000000; i++) {
        if (temp != CAM_HREFX) {
            return 1;
        }
    }

    return 0;
}

/**
 * Return Lume (Y) bytes.
 * @param r0 Row number.
 */
void CamReadPixelRow(int r0, unsigned int buffer[CAM_WIDTH]) {
    int r;
    
    // Vsync
    while (!CAM_VSYNC);
    while (CAM_VSYNC);

    // get to line r0
    for (r = 0; r < r0; r++) {
        // wait for Hsync
        while (!CAM_HREFX);
        while (CAM_HREFX);
    }

    // wait for HREF high
    while (!CAM_HREFX);

    for (r = 0; r < CAM_WIDTH; r++) {
        while (CAM_PCLK);
        while (!CAM_PCLK);

        // read first px (Y) when pclk low
        buffer[r] = PORTB;

        // discard Y/U bit
        while (CAM_PCLK);
        while (!CAM_PCLK);
    }
    
}

///**
// * Return Lume (Y) bytes.
// * @param r0 Row number.
// */
//void CamReadPixelRow(int r0, unsigned int buffer[CAM_WIDTH]) {
//    int r;
//    r0 = r0 + 98;
//
//    // Vsync
//    while (!CAM_VSYNC);
//    while (CAM_VSYNC);
//
//    // get to line r0
//    for (r = 0; r < r0; r++) {
//        // wait for Hsync
//        while (!CAM_HREFX);
//        while (CAM_HREFX);
//    }
//
//    // wait for HREF high
//    while (!CAM_HREFX);
//
//    for (r = 0; r < CAM_WIDTH; r++) {
//        while (CAM_PCLK);
//        while (!CAM_PCLK);
//
//        // read first px (Y) when pclk low
//        buffer[r] = PORTB;
//
//        // discard Y/U bit
//        while (CAM_PCLK);
//        while (!CAM_PCLK);
//    }
//}

/**
 * Turn on camera.
 */
void CamTurnOn() {
    CAM_PWDN = 0;
    CAM_RESET = 1;
}

/**
 * Turn off camera.
 */
void CamTurnOff() {
    CAM_PWDN = 1;
}

/**
 * Write on camera register  through the I2C protocol.
 * @param reg_addr
 * @param data
 */
void CamWrite(unsigned char reg_addr, unsigned char data) {
    unsigned char error = 0;
    error = I2CWriteByte(CAM_ADDR, reg_addr, data);
    return;
}

/**
 * Write on a list of registers through the I2C protocol.
 * @param camlist
 */
void CamWriteArray(const struct cam_reg camlist[]) {
    unsigned char reg_addr = 0;
    unsigned char reg_val = 0;

    const struct cam_reg *next = camlist;

    while ((reg_addr != 0xFF) | (reg_val != 0xFF)) {
        reg_addr = next->reg;
        reg_val = next->val;
        CamWrite(reg_addr, reg_val);
        next++;
    }
}

/**
 * Read camera register through the I2C protocol.
 * @param reg_addr  register address
 * @return data
 */
unsigned char CamRead(unsigned char reg_addr) {
    unsigned char error = 0;
    unsigned char data;

    error = I2CReadByte(CAM_ADDR, reg_addr, &data);
    return data;
}

long CamCountBytesPerLine() {
    long w = 0;

    // wait for initial pulse
    while (!CAM_VSYNC);
    while (CAM_VSYNC);

    while (CAM_HREFX);
    while (!CAM_HREFX);

    for (w = 0;CAM_HREFX; w++) {
        while (CAM_PCLK);
        while (!CAM_PCLK);
    }

    return w;
}

long CamCountLines() {
    long h = 0;

    // wait for initial pulse
    // WARNING: VSYNC oposite to datasheet (only if not using default config)
    while (!CAM_VSYNC);
    while (CAM_VSYNC);

    // while valid data
    while (!CAM_VSYNC) {
        // start counting horizontal syncs!
        while (!CAM_VSYNC && !CAM_HREFX);
        h++;
        while (CAM_HREFX);
    }
    h--;

    return h;
}