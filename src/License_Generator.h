/****************************************************************************
 * Copyright   : All rights reserved. This document/code contains information
 * that is proprietary to Lekha Wireless Solutions Pvt. Ltd. No part of this
 * document/code may be reproduced or used in whole or part in any form or
 * by any means- graphic, electronic or mechanical without the written
 * permission of Lekha Wireless Solutions Pvt. Ltd.
 * Company     : Lekha Wireless Solutions Pvt. Ltd.
 * Author      : Apporva
 * File Name   : License_installer.h
 * Description : License Generator API
 * Comments    : None
 ****************************************************************************/
#ifndef _LICENSEGENERATOR_H_
#define _LICENSEGENERATOR_H_

/* STANDARD INCLUDES ********************************************************/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <errno.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>
#include <termios.h>

/* PROJECT INCLUDES *********************************************************/

/* MACRO  AND TYPE DEFINITIONS **********************************************/

#if !defined _DATATYPES_H_
#define UINT8	unsigned char
#define	UINT16	unsigned short int
#define UINT32	unsigned long int
#define INT8	char
#define	INT16	signed short int
#define INT32	signed long int
#define FLOAT	float
#endif

#define SIZE               	 	100
#define DEBUG_PRINTS         		0
#define ZERO                 		0
#define ONE                     	1
#define TWO                     	2
#define TEN 				10
#define TWELVE				12
#define ONE_TWENTY_SEVEN		127
#define EIGHT				8
#define MAX_NUM_OF_APPLICATIONS 	2
#define MIN_NUM_OF_APPLICATIONS 	1
#define MIN_ALLOWED_DAYS        	1
#define MIN_ALLOWED_RUN_COUNT   	1
#define MIN_VERSION_NUMBER		1
#define GENERATOR_PASSKEY       	"lwsLicense@!23"
#define SUCCESS 			0
#define FAILURE                 	-1
#define MAX_LENGTH              	100
#define COMPANY_NAME			"Lekha Wireless Solutions Pvt. Ltd."
#define LICENSE_APPLICATION             "License for Application "
#define ISSUE_DATE_AND_TIME             "Issue Date & Time:"
#define ALLOWED_COUNT                   "App Run count:"
#define EXPIRY_DATE_AND_TIME		"Expiry Date & Time:"
#define UNIQUE_ID			"Unique ID:"
#define VERSION_NUMBER			"Version no.:"
#define LICENSE_LWS_L1_APP		"License_LWS_L1_APP"
#define LICENSE_LWS_L2_L3_APP		"License_LWS_L2_L3_APP"
#define GENERATOR_VERSION_NUMBER	1
#define NEW_LINE_CHARACTER       '\n' 
#define NULL_CHARACTER		 '\0'

/* PRIVATE FUNCTION DECLARATIONS *********************************************/

/******************************************************************************
 * Function             : GetCurrentTime
 * Description          : API Fetches current time and date and set tm mytime
 *                        structure and convert the structure member value 
 *                        into string
 * Input parameter      : stMyCurrentTime : structure contain current time. 
 *                        gsi8Buf  : buffer to store the current time as string
 * Output parameters    : None
 * I/P & O/P parameter  : None
 * Return value         : None
 ******************************************************************************/
void GetCurrentTime(struct tm *stMyCurrentTime, INT8 *gsi8Buf);

/******************************************************************************
 * Function             : CreateLicenseFile
 * Description          : API Creates license file
 * Input parameter      : si8LicFilename : License file name, 
 *                        i32AppNo       : Application name, 
 *                        si8DeviceID    : Device ID,
 *                        i32AllowCnt    : Allowed count, 
 *			  i32AllowDays   : Allowed days,
 *                        i32VersionNo   : Version no.
 * Output parameters    : None
 * I/P & O/P parameter  : None
 * Return value         : None
 ******************************************************************************/
INT32 CreateLicenseFile(INT32 i32AppNo,INT8 *si8DeviceID,
		INT32 i32AllowCnt,INT32 i32AllowDays, INT32 i32VersionNo);

/******************************************************************************
 * Function             : EncryptData
 * Description          : API encrypts data to be written to file.
 * Input parameter      : psi8Buf : String to be encrypted.
 * Output parameters    : None
 * I/P & O/P parameter  : None
 * Return value         : None
 ******************************************************************************/
void EncryptData(INT8 *psi8Buf);

/******************************************************************************
 * Function             : CalculateExpiryDate
 * Description          : API calculates the expiry date for license
 * Input parameter      : i32days : Number of days.
 * Output parameters    : None
 * I/P & O/P parameter  : None
 * Return value         : None
 ******************************************************************************/
void CalculateExpiryDate(INT32 i32days);

/******************************************************************************
 * Function             : GetPassword
 * Description          : Gets Password from the console
 * Input parameter      : gsi8Password : buffer to store entered Password
 * Output parameters    : None
 * I/P & O/P parameter  : None
 * Return value         : None
 ******************************************************************************/
 void GetPassword(INT8 gsi8Password[]);
 
/******************************************************************************
 * Function             : GetDeviceID
 * Description          : API gets device id
 * Input parameter      : si8DevID : Device ID
 * Output parameters    : None
 * I/P & O/P parameter  : None
 * Return value         : None
 ******************************************************************************/
void GetDeviceID(INT8 *si8DevID);

/******************************************************************************
 * Function             : DisplayApplications
 * Description          : API to Display application.
 * Input parameter      : None
 * Output parameters    : None
 * I/P & O/P parameter  : None
 * Return value         : None
 ******************************************************************************/
void DisplayApplications();

/******************************************************************************
 * Function             : FetchLicenseParameters
 * Description          : Fetches the license parameters from the userspace.
 * Input parameter      : pi32AppNo        : Application choice{L1_APP/L2_L3_APP}
 *                        psi8DeviceID     : Device ID
 *                        pi32AllowedCount : Number of allowed count
 *                        pi32AllowedDays  : Number of allowed days
 *                        pi32VersionNo    : version number
 * Output parameters    : None
 * I/P & O/P parameter  : None
 * Return value         : SUCCESS
 ******************************************************************************/
INT32 FetchLicenseParameters(INT32 *pi32AppNo,INT8 *psi8DeviceID,
	INT32 *pi32AllowedCount,INT32 *pi32AllowedDays, INT32 *pi32VersionNo);
	
#endif   
/********************************END _LICENSEGENERATOR_H_**********************/
