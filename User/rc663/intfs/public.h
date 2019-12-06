/*****************************************************************************
* 						全局变量及公共函数
*
*
*******************************************************************************/

/*RC663 Includes--------------------------------------------------------------*/
/* Configuration Headers */
/* Controls build behavior of components */
#include <ph_NxpBuild.h>
/* Status code definitions */
#include <ph_Status.h>
/* Reader Library Headers */
/* Generic ISO14443-3A Component of
 * Reader Library Framework */
 // xwh
#include <phpalI14443p3a.h>
#include <phpalI14443p3b.h>
/* Generic ISO14443-4 Component of
 * Reader Library Framework */
#include <phpalI14443p4.h>
/* Generic MIFARE(R) Application Component
 * of Reader Library Framework */
#include <phalMfc.h>
/* Generic KeyStore Component of
 * Reader Library Framework
 * We need the KeyStore for the
 * authentication with secured areas. */
#include <phKeyStore.h>
/* Drivers Interface*/
#include <RegCtl_SpiHw.h>
