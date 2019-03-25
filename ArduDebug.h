/*
 * Debug.h
 *
 *  Created on: 19 sept. 2016
 *      Author: oscar
 *	Last Update: 8 dec. 2017
 */

#ifndef ARDU_DEBUG_H_
#define ARDU_DEBUG_H_



/* DEBUG(x) shows by serial the name and value of the given variable.
 * Example:
 *   1. int x = 45;
 *   2. DEBUG(x)
 *   Serial Monitor: "x = 45"
*/
#define DEBUG(x) Serial.println(String(#x) + " = " + String(x));

/*
 *  BRK shows by serial the line and the filename where BRK is written.
 *  Example:
 *    1. if(true)
 *    2.   BRK
 *    3. else
 *    4.   BRK
 *    Serial Monitor: "Code reached line #2 File: Example.ino"
 */
#define BRK Serial.println("Reached Line #" + String(__LINE__)) + " File:" + String(__FILE__);




#endif /* ARDU_DEBUG_H_ */
