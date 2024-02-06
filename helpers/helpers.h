#pragma once

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <furi.h>

int SerialValidator(char code[17]);

char* GenPLID(char code[17]);

char* GenType( char code[17]);

char* GenRes(char code[17]);