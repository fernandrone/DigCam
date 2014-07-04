/*
 * File:   ov2640.h
 * Author: Fernando Barbosa
 *
 * Created on May 10, 2014, 02:02 AM
 *
 * Description: defines all commands known to the pic. Each command is define by
 * two hexadecimal characters, for a total of 256 commands.
 */

#ifndef OV2640_H
#define	OV2640_H


/******************************************************************************/
/* Commands #define Macros                                                    */
/******************************************************************************/

// Camera Pin Map
#define CAM_SCL     _RD10
#define CAM_SDA     _RD9
#define CAM_PCLK    _RD0
#define CAM_HREFX   _RF3
#define CAM_XVCLK   _RB15       // _RB0
#define CAM_VSYNC   _RD11
#define CAM_PWDN    _RB3     // must be 1 to activate CAM_era
#define CAM_RESET   _RB4
#define CAM_D7      _RB0        //_RB15
#define CAM_D6      _RB14
#define CAM_D5      _RB13
#define CAM_D4      _RB12
#define CAM_D3      _RB11
#define CAM_D2      _RB10
#define CAM_D1      _RB9
#define CAM_D0      _RB8
#define CAM_Y1      _RB5
#define CAM_Y0      _RB2

// Slave operation
#define CAM_MHSYNC      CAM_RESET
#define CAM_MVSYNC      CAM_PWDN

// Camera definitions
#define CAM_DATA        (unsigned char)(CAM_D7 << 7 | CAM_D6 << 6 | CAM_D5 << 5 | CAM_D4 << 4 | CAM_D3 << 3 | CAM_D2 << 2 | CAM_D1 << 1 | CAM_D0 << 0)
#define CAM_WIDTH       320
#define CAM_HEIGHT      160

// Slave address
#define CAM_ADDR        0x30

// Camera registers
#define CAM_REG_COM7    0x12    // Common Control 7

/******************************************************************************/
/* OV2640 Functions                                                           */
/******************************************************************************/

struct cam_reg {
    unsigned char reg;
    unsigned char val;
};

struct cam_status {
    int POWER;
    int RESET;
    int PCLK;
    int HREF;
    int VSYNC;

    long WIDTH;
    long HEIGHT;

    long NBYTELINE;
    long NHSYNC;

    long NBYTETOTAL;
};

struct cam_status CamStatus();

void CamInit();
void CamReset();
void CamTakePic();
void CamTurnOn();
void CamTurnOff();

void CamGetPixelRow(int r0);
unsigned int CamReadPixel(int r0, int c0);
unsigned char CamRead(unsigned char reg_addr);
void CamWrite(unsigned char reg_addr, unsigned char data);
void CamWriteArray(const struct cam_reg camlist[]);

#endif