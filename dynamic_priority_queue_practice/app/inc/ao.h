/*
 * ao.h
 *
 *  Created on: Nov 25, 2024
 *      Author: franco
 */

#ifndef INC_AO_H_
#define INC_AO_H_

/*
 * Estructura de objeto activo:
 * 1) callback para ejecutar al recibir un mensaje (se configura al inicio)
 * 2) queue para recibir mensajes (se configura al inicio y se crea una tarea con ese handle de queue)
 *
 * Estructura de mensaje que entra en la queue de RTOS:
 * 1) estructura de dato con parametros internos (cambia acorde al problema/contexto del momento)
 * 2) callback para ejecutar en la tarea que recibe el mensaje (posterior a la ejecucion del callback propio del objeto activo target)
 * 3) estructura de objeto activo target
 */
#endif /* INC_AO_H_ */
