// Debugging tools made by OGL and MMC
 
#ifndef _DEBUG_H
#define _DEBUG_H

#include <Arduino.h>

#undef DEBUG(x)
#define DEBUG(x) Serial.println(String(#x) + " = " + String(x))
/* Prints to serial the name and value of the given variable.
 * Example:
 *   [...]
 *   int a = 45;
 *   [...]
 *   DEBUG(a);
 *   [...]
 *   
 * >> Serial Monitor: a = 45
*/

#undef DEBUGLIN
#define DEBUGLIN Serial.println("Code executed line #" + String(__LINE__)) + " in file \"" + String(__FILE__) + "\"")
/* Prints to serial the line number and filename in which the macro is placed.
 * Example:
 *   [...]
 *   DEBUGLIN;
 *   [...]
 * 
 * >> Serial monitor: Code executed line #35 in file "debug.h"
*/

#undef DEBUGBRK
#define DEBUGBRK { Serial.println("Code reached line #" + String(__LINE__)) + " in file \"" + String(__FILE__) + "\" and stopped."); delay(10); exit(0) }
/*  Same as DEBUGLIN but stopping execution of code. Interruptions remain serviceable.
 *  Example:
 *    [...]
 *    DEBUGBRK;
 *    [...]
 *  
 *  >> Serial Monitor: Code reached line #45 in file "debug.h" and stopped.
 */

#undef DEBUGFCN
#define DEBUGFCN Serial.println("Code execution is in function \"" + String(__FUNCTION__)) + "\" in file \"" + String(__FILE__) + "\"")
/* Same as DEBUGLIN but printing function instead of line
 * Example:
 *   void func(){
 *     [...]
 *     DEBUGFCN;
 *     [...]
 *   }
 * >> Serial monitor: Code execution is in function "func" in file "debug.h"
*/
 
#endif // _DEBUG_H
