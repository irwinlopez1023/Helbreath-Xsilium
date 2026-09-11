// DebugCliente.h
//
// Depuracion del cliente: deja rastro cuando el cliente se cae, se congela
// o se cierra solo. Todo se escribe en la carpeta "Depuracion" junto al .exe:
//
//   Crash <fecha>.txt / .dmp      el cliente se cayo (excepcion no atrapada)
//   Congelado <fecha>.txt (.dmp)  el bucle principal dejo de responder > 3 s
//   Cierre inesperado <fecha>.txt la sesion anterior termino sin cerrar bien
//   UltimosEventos.txt            se reescribe cada 2 s con lo ultimo que paso
//   Excepciones.txt               excepciones graves aunque luego se atrapen
//
// La implementacion esta al final de classes/Wmain.cpp.

#pragma once

#include <windows.h>

#define DBGC_RECV   1   // mensaje recibido del servidor: a = id, b = tamano, c = tipo
#define DBGC_SEND   2   // comando enviado al servidor:   a = id, b = comando, c = valor 1
#define DBGC_MODO   3   // cambio de modo de juego:        a = modo nuevo, b = modo anterior
#define DBGC_TEXTO  4   // texto libre

void DebugCliente_Iniciar();                                  // al principio de WinMain
void DebugCliente_Latido();                                   // en cada vuelta del bucle principal
void DebugCliente_Evento(int iTipo, DWORD a, DWORD b, DWORD c);
void DebugCliente_Texto(const char * cTexto);
void DebugCliente_Cierre(const char * cMotivo);               // cierre normal del cliente
