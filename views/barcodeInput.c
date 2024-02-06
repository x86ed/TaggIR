#include "barcodeInput.h"

typedef enum {
    BasicScenesGreetingInputSceneSaveEvent,
} BasicScenesGreetingInputEvent;

bool textInputValidator(const char* text, FuriString* error, void* context) {
    UNUSED(context);
    if(strlen(text) != 17) {
        furi_string_set(
            error, "Barcode\n is wrong\nlength.");
        return false;
    }
    char *code = NULL;
    code = (char *)text;
    int serval = SerialValidator(code);
    if (serval == 1){
        furi_string_set(
            error, "Barcode\nisn't valid.");
        return false;
    }
    return true;
}

static void basic_scenes_text_input_callback(void* context) {
    TaggIRApp* app = (TaggIRApp*)context;
   view_dispatcher_switch_to_view(app->view_dispatcher, TaggIRViewConfigure);
}

void taggir_barcode_input_scene_on_enter(void* context) {
    TaggIRApp* app = (TaggIRApp*)context;
    bool clear_text = true;
    text_input_reset(app->text_input);
    text_input_set_header_text(app->text_input, "Enter an ESL Code");
    text_input_set_validator(app->text_input, textInputValidator, app);
    text_input_set_result_callback(
        app->text_input,
        basic_scenes_text_input_callback,
        app,
        app->temp_buffer,
        app->temp_buffer_size,
        clear_text);
    view_dispatcher_switch_to_view(app->view_dispatcher, TaggIRViewTextInput);
}

bool taggir_barcode_input_scene_on_event(void* context, SceneManagerEvent event) {
    TaggIRApp* app = (TaggIRApp*)context;
    bool consumed = false;
    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == BasicScenesGreetingInputSceneSaveEvent) {
            view_dispatcher_switch_to_view(app->view_dispatcher, TaggIRViewConfigure);
            consumed = true;
        }
    }
    return consumed;
}

void taggir_barcode_input_scene_on_exit(void* context) {
    UNUSED(context);
}