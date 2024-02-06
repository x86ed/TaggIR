#pragma once

#include <gui/scene_manager.h>
#include "../taggir.h"
#include "../helpers/helpers.h"


void taggir_barcode_input_scene_on_enter(void* context);
bool taggir_barcode_input_scene_on_event(void* context, SceneManagerEvent event);
void taggir_barcode_input_scene_on_exit(void* context);
bool textInputValidator(const char* text, FuriString* error, void* context);