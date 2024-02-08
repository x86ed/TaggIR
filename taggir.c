#include "taggir.h"
#include "taggir_i.h"


#define TAG "TaggIR"

// Change this to BACKLIGHT_AUTO if you don't want the backlight to be continuously on.
#define BACKLIGHT_AUTO 1

/**
 * @brief      Callback for exiting the application.
 * @details    This function is called when user press back button.  We return VIEW_NONE to
 *            indicate that we want to exit the application.
 * @param      _context  The context - unused
 * @return     next view id
*/
static uint32_t TaggIR_navigation_exit_callback(void* _context) {
    UNUSED(_context);
    return VIEW_NONE;
}

/**
 * @brief      Callback for returning to submenu.
 * @details    This function is called when user press back button.  We return VIEW_NONE to
 *            indicate that we want to navigate to the submenu.
 * @param      _context  The context - unused
 * @return     next view id
*/
static uint32_t TaggIR_navigation_submenu_callback(void* _context) {
    UNUSED(_context);
    return TaggIRViewSubmenu;
}

/**
 * @brief      Callback for returning to configure screen.
 * @details    This function is called when user press back button.  We return VIEW_NONE to
 *            indicate that we want to navigate to the configure screen.
 * @param      _context  The context - unused
 * @return     next view id
*/
static uint32_t TaggIR_navigation_configure_callback(void* _context) {
    UNUSED(_context);
    return TaggIRViewConfigure;
}

/**
 * @brief      Handle submenu item selection.
 * @details    This function is called when user selects an item from the submenu.
 * @param      context  The context - TaggIRApp object.
 * @param      index     The TaggIRSubmenuIndex item that was clicked.
*/
static void TaggIR_submenu_callback(void* context, uint32_t index) {
    TaggIRApp* app = (TaggIRApp*)context;
    switch(index) {
    case TaggIRSubmenuIndexConfigure:
        view_dispatcher_switch_to_view(app->view_dispatcher, TaggIRViewConfigure);
        break;
    case TaggIRSubmenuIndexGame:
        view_dispatcher_switch_to_view(app->view_dispatcher, TaggIRViewGame);
        break;
    case TaggIRSubmenuIndexAbout:
        view_dispatcher_switch_to_view(app->view_dispatcher, TaggIRViewAbout);
        break;
    default:
        break;
    }
}

/**
 * config setting for the color of the display
*/
static const char* display_color_config_label = "Display Color";
static uint8_t display_color_values[] = {1, 2, 3, 4};
static char* display_color_names[] = {"Seg", "B&W", "3 Color", "RGB"};
static void TaggIR_display_color_change(VariableItem* item) {
    TaggIRApp* app = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, display_color_names[index]);
    TaggIRGameModel* model = view_get_model(app->view_game);
    model->display_color_index = index;
}

/**
 * Our 2nd sample setting is a text field.  When the user clicks OK on the configuration 
 * setting we use a text input screen to allow the user to enter a name.  This function is
 * called when the user clicks OK on the text input screen.
*/
static const char* barcode_config_label = "Code";
static const char* barcode_entry_text = "Enter ESL Barcode";
static const char* barcode_default_value = "G4591371776312423";
static void TaggIR_barcode_text_updated(void* context) {
    TaggIRApp* app = (TaggIRApp*)context;
    bool redraw = true;
    with_view_model(
        app->view_game,
        TaggIRGameModel * model,
        {
            furi_string_set(model->barcode_name, app->temp_buffer);
            variable_item_set_current_value_text(
                app->barcode_item, furi_string_get_cstr(model->barcode_name));

            char code[17];
            strcpy(code, furi_string_get_cstr(model->barcode_name));
            FURI_LOG_I("TaggIR","CODE: %s", code);

            char* plid = GenPLID(code);
            FURI_LOG_I("TaggIR","ID: %s", plid);
            variable_item_set_current_value_text(
                app->plid_item, plid);

            char* type = GenType(code);
            FURI_LOG_I("TaggIR","TYPE: %s", type);
            variable_item_set_current_value_text(
                app->type_item, type);

            TaggIRInstance res = GenRes(code);
            FURI_LOG_I("TaggIR","RES: %s", res.res_string);
            variable_item_set_current_value_text(
                app->res_item, res.res_string);

             char* img = "test.png";
            FURI_LOG_I("TaggIR","RES: %s", img);
            variable_item_set_current_value_text(
                app->img_item, img);
        },
        redraw);
    view_dispatcher_switch_to_view(app->view_dispatcher, TaggIRViewConfigure);
}

static const char* plid_config_label = "PLID";
static const char* plid_default_value = "E7014563";

static const char* type_config_label = "Type";
static const char* type_default_value = "1242";

static const char* res_config_label = "Reso";
static const char* res_default_value = "296x128";

static const char* img_config_label = "Img";
static const char* img_default_value = "test.png";

/**
 * @brief      Callback when item in configuration screen is clicked.
 * @details    This function is called when user clicks OK on an item in the configuration screen.
 *            If the item clicked is our text field then we switch to the text input screen.
 * @param      context  The context - TaggIRApp object.
 * @param      index - The index of the item that was clicked.
*/
static void TaggIR_setting_item_clicked(void* context, uint32_t index) {
    TaggIRApp* app = (TaggIRApp*)context;
    index++; // The index starts at zero, but we want to start at 1.

    // Our configuration UI has the 2nd item as a text field.
    if(index == 1) {
        // Header to display on the text input screen.
        text_input_set_header_text(app->text_input, barcode_entry_text);
        text_input_set_validator(app->text_input, textInputValidator, app);

        // Copy the current name into the temporary buffer.
        bool redraw = false;
        with_view_model(
            app->view_game,
            TaggIRGameModel * model,
            {
                strncpy(
                    app->temp_buffer,
                    furi_string_get_cstr(model->barcode_name),
                    app->temp_buffer_size);
            },
            redraw);

        // Configure the text input.  When user enters text and clicks OK, TaggIR_setting_text_updated be called.
        bool clear_previous_text = false;
        text_input_set_result_callback(
            app->text_input,
            TaggIR_barcode_text_updated,
            app,
            app->temp_buffer,
            app->temp_buffer_size,
            clear_previous_text);

        // Pressing the BACK button will reload the configure screen.
        view_set_previous_callback(
            text_input_get_view(app->text_input), TaggIR_navigation_configure_callback);

        // Show text input dialog.
        view_dispatcher_switch_to_view(app->view_dispatcher, TaggIRViewTextInput);
    }
}

/**
 * @brief      Callback for drawing the game screen.
 * @details    This function is called when the screen needs to be redrawn, like when the model gets updated.
 * @param      canvas  The canvas to draw on.
 * @param      model   The model - MyModel object.
*/
static void TaggIR_view_game_draw_callback(Canvas* canvas, void* model) {
    TaggIRGameModel* my_model = (TaggIRGameModel*)model;
    canvas_draw_icon(canvas, my_model->x, 20, &I_glyph_1_14x40);
    canvas_draw_str(canvas, 1, 10, "LEFT/RIGHT to change x");
    FuriString* xstr = furi_string_alloc();
    furi_string_printf(xstr, "x: %u  OK=play tone", my_model->x);
    canvas_draw_str(canvas, 44, 24, furi_string_get_cstr(xstr));
    furi_string_printf(xstr, "random: %u", (uint8_t)(furi_hal_random_get() % 256));
    canvas_draw_str(canvas, 44, 36, furi_string_get_cstr(xstr));
    furi_string_printf(
        xstr,
        "team: %s (%u)",
        display_color_names[my_model->display_color_index],
        display_color_values[my_model->display_color_index]);
    canvas_draw_str(canvas, 44, 48, furi_string_get_cstr(xstr));
    furi_string_printf(xstr, "name: %s", furi_string_get_cstr(my_model->barcode_name));
    canvas_draw_str(canvas, 44, 60, furi_string_get_cstr(xstr));
    furi_string_free(xstr);
}

/**
 * @brief      Callback for timer elapsed.
 * @details    This function is called when the timer is elapsed.  We use this to queue a redraw event.
 * @param      context  The context - TaggIRApp object.
*/
static void TaggIR_view_game_timer_callback(void* context) {
    TaggIRApp* app = (TaggIRApp*)context;
    view_dispatcher_send_custom_event(app->view_dispatcher, TaggIREventIdRedrawScreen);
}

/**
 * @brief      Callback when the user starts the game screen.
 * @details    This function is called when the user enters the game screen.  We start a timer to
 *           redraw the screen periodically (so the random number is refreshed).
 * @param      context  The context - TaggIRApp object.
*/
static void TaggIR_view_game_enter_callback(void* context) {
    uint32_t period = furi_ms_to_ticks(200);
    TaggIRApp* app = (TaggIRApp*)context;
    furi_assert(app->timer == NULL);
    app->timer =
        furi_timer_alloc(TaggIR_view_game_timer_callback, FuriTimerTypePeriodic, context);
    furi_timer_start(app->timer, period);
}

/**
 * @brief      Callback when the user exits the game screen.
 * @details    This function is called when the user exits the game screen.  We stop the timer.
 * @param      context  The context - TaggIRApp object.
*/
static void TaggIR_view_game_exit_callback(void* context) {
    TaggIRApp* app = (TaggIRApp*)context;
    furi_timer_stop(app->timer);
    furi_timer_free(app->timer);
    app->timer = NULL;
}

/**
 * @brief      Callback for custom events.
 * @details    This function is called when a custom event is sent to the view dispatcher.
 * @param      event    The event id - TaggIREventId value.
 * @param      context  The context - TaggIRApp object.
*/
static bool TaggIR_view_game_custom_event_callback(uint32_t event, void* context) {
    TaggIRApp* app = (TaggIRApp*)context;
    switch(event) {
    case TaggIREventIdRedrawScreen:
        // Redraw screen by passing true to last parameter of with_view_model.
        {
            bool redraw = true;
            with_view_model(
                app->view_game, TaggIRGameModel * _model, { UNUSED(_model); }, redraw);
            return true;
        }
    case TaggIREventIdOkPressed:
        // Process the OK button.  We play a tone based on the x coordinate.
        if(furi_hal_speaker_acquire(500)) {
            float frequency;
            bool redraw = false;
            with_view_model(
                app->view_game,
                TaggIRGameModel * model,
                { frequency = model->x * 100 + 100; },
                redraw);
            furi_hal_speaker_start(frequency, 1.0);
            furi_delay_ms(100);
            furi_hal_speaker_stop();
            furi_hal_speaker_release();
        }
        return true;
    default:
        return false;
    }
}

/**
 * @brief      Callback for game screen input.
 * @details    This function is called when the user presses a button while on the game screen.
 * @param      event    The event - InputEvent object.
 * @param      context  The context - TaggIRApp object.
 * @return     true if the event was handled, false otherwise.
*/
static bool TaggIR_view_game_input_callback(InputEvent* event, void* context) {
    TaggIRApp* app = (TaggIRApp*)context;
    if(event->type == InputTypeShort) {
        if(event->key == InputKeyLeft) {
            // Left button clicked, reduce x coordinate.
            bool redraw = true;
            with_view_model(
                app->view_game,
                TaggIRGameModel * model,
                {
                    if(model->x > 0) {
                        model->x--;
                    }
                },
                redraw);
        } else if(event->key == InputKeyRight) {
            // Right button clicked, increase x coordinate.
            bool redraw = true;
            with_view_model(
                app->view_game,
                TaggIRGameModel * model,
                {
                    // Should we have some maximum value?
                    model->x++;
                },
                redraw);
        }
    } else if(event->type == InputTypePress) {
        if(event->key == InputKeyOk) {
            // We choose to send a custom event when user presses OK button.  TaggIR_custom_event_callback will
            // handle our TaggIREventIdOkPressed event.  We could have just put the code from
            // TaggIR_custom_event_callback here, it's a matter of preference.
            view_dispatcher_send_custom_event(app->view_dispatcher, TaggIREventIdOkPressed);
            return true;
        }
    }

    return false;
}

/**
 * @brief      Allocate the TaggIR application.
 * @details    This function allocates the TaggIR application resources.
 * @return     TaggIRApp object.
*/
static TaggIRApp* taggir_app_alloc() {
    TaggIRApp* app = (TaggIRApp*)malloc(sizeof(TaggIRApp));

    Gui* gui = furi_record_open(RECORD_GUI);

    app->view_dispatcher = view_dispatcher_alloc();
    view_dispatcher_enable_queue(app->view_dispatcher);
    view_dispatcher_attach_to_gui(app->view_dispatcher, gui, ViewDispatcherTypeFullscreen);
    view_dispatcher_set_event_callback_context(app->view_dispatcher, app);

    app->submenu = submenu_alloc();
    submenu_add_item(
        app->submenu, "Config", TaggIRSubmenuIndexConfigure, TaggIR_submenu_callback, app);
    submenu_add_item(
        app->submenu, "Flash", TaggIRSubmenuIndexGame, TaggIR_submenu_callback, app);
    submenu_add_item(
        app->submenu, "About", TaggIRSubmenuIndexAbout, TaggIR_submenu_callback, app);
    view_set_previous_callback(submenu_get_view(app->submenu), TaggIR_navigation_exit_callback);
    view_dispatcher_add_view(
        app->view_dispatcher, TaggIRViewSubmenu, submenu_get_view(app->submenu));
    view_dispatcher_switch_to_view(app->view_dispatcher, TaggIRViewSubmenu);

    app->text_input = text_input_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher, TaggIRViewTextInput, text_input_get_view(app->text_input));
    app->temp_buffer_size = 32;
    app->temp_buffer = (char*)malloc(app->temp_buffer_size);

    app->variable_item_list_config = variable_item_list_alloc();
    variable_item_list_reset(app->variable_item_list_config);

    FuriString* barcode_name = furi_string_alloc();
    furi_string_set_str(barcode_name, barcode_default_value);
    app->barcode_item = variable_item_list_add(
        app->variable_item_list_config, barcode_config_label, 1, NULL, NULL);
    variable_item_set_current_value_text(
        app->barcode_item, furi_string_get_cstr(barcode_name));
    variable_item_list_set_enter_callback(
        app->variable_item_list_config, TaggIR_setting_item_clicked, app);

    VariableItem* item = variable_item_list_add(
        app->variable_item_list_config,
        display_color_config_label,
        COUNT_OF(display_color_values),
        TaggIR_display_color_change,
        app);
    uint8_t display_color_index = 0;
    variable_item_set_current_value_index(item, display_color_index);
    variable_item_set_current_value_text(item, display_color_names[display_color_index]);

    FuriString* plid_name = furi_string_alloc();
    furi_string_set_str(plid_name, plid_default_value);
    app->plid_item = variable_item_list_add(
        app->variable_item_list_config, plid_config_label, 1, NULL, NULL);
    variable_item_set_current_value_text(
        app->plid_item, furi_string_get_cstr(plid_name));
    variable_item_list_set_enter_callback(
        app->variable_item_list_config, TaggIR_setting_item_clicked, app);

    FuriString* type_name = furi_string_alloc();
    furi_string_set_str(type_name, type_default_value);
    app->type_item = variable_item_list_add(
        app->variable_item_list_config, type_config_label, 1, NULL, NULL);
    variable_item_set_current_value_text(
        app->type_item, furi_string_get_cstr(type_name));
    variable_item_list_set_enter_callback(
        app->variable_item_list_config, TaggIR_setting_item_clicked, app);
    
    FuriString* res_name = furi_string_alloc();
    furi_string_set_str(res_name, res_default_value);
    app->res_item = variable_item_list_add(
        app->variable_item_list_config, res_config_label, 1, NULL, NULL);
    variable_item_set_current_value_text(
        app->res_item, furi_string_get_cstr(res_name));
    variable_item_list_set_enter_callback(
        app->variable_item_list_config, TaggIR_setting_item_clicked, app);

    FuriString* img_name = furi_string_alloc();
    furi_string_set_str(img_name, img_default_value);
    app->img_item = variable_item_list_add(
        app->variable_item_list_config, img_config_label, 1, NULL, NULL);
    variable_item_set_current_value_text(
        app->img_item, furi_string_get_cstr(img_name));
    variable_item_list_set_enter_callback(
        app->variable_item_list_config, TaggIR_setting_item_clicked, app);

    view_set_previous_callback(
        variable_item_list_get_view(app->variable_item_list_config),
        TaggIR_navigation_submenu_callback);
    view_dispatcher_add_view(
        app->view_dispatcher,
        TaggIRViewConfigure,
        variable_item_list_get_view(app->variable_item_list_config));

    app->view_game = view_alloc();
    view_set_draw_callback(app->view_game, TaggIR_view_game_draw_callback);
    view_set_input_callback(app->view_game, TaggIR_view_game_input_callback);
    view_set_previous_callback(app->view_game, TaggIR_navigation_submenu_callback);
    view_set_enter_callback(app->view_game, TaggIR_view_game_enter_callback);
    view_set_exit_callback(app->view_game, TaggIR_view_game_exit_callback);
    view_set_context(app->view_game, app);
    view_set_custom_callback(app->view_game, TaggIR_view_game_custom_event_callback);
    view_allocate_model(app->view_game, ViewModelTypeLockFree, sizeof(TaggIRGameModel));
    TaggIRGameModel* model = view_get_model(app->view_game);
    model->display_color_index = display_color_index;
    model->barcode_name = barcode_name;
    model->x = 0;
    view_dispatcher_add_view(app->view_dispatcher, TaggIRViewGame, app->view_game);

    app->widget_about = widget_alloc();
    widget_add_text_scroll_element(
        app->widget_about,
        0,
        0,
        128,
        64,
        "TaggIR\n---\nA Flipper implementation of an E-INK ESL Updater.\n\n!!!WARNING!!!\nTHE AUTHORS OF THIS TOOL ARE NOT RESPONSIBLE FOR THE CONSEQUENCES IF YOU USE THIS ON DEVICES YOU \nDON'T OWN. PLEASE ONLY \nUSE THIS TOOL WITH DISPLAYS YOU OWN LEGALLY.\nTHE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.\nTo use it enter in the tags code, then select a properly sized image and fire it off.\nIt will take a couple of seconds to show up on the screen after the transmission completes.\n\nAuthor: @X86ed \n\nSpecial Thanks:\n@furrtek\n@david4599");
    view_set_previous_callback(
        widget_get_view(app->widget_about), TaggIR_navigation_submenu_callback);
    view_dispatcher_add_view(
        app->view_dispatcher, TaggIRViewAbout, widget_get_view(app->widget_about));

    app->notifications = furi_record_open(RECORD_NOTIFICATION);

#ifdef BACKLIGHT_ON
    notification_message(app->notifications, &sequence_display_backlight_enforce_on);
#endif

    return app;
}

/**
 * @brief      Free the TaggIR application.
 * @details    This function frees the TaggIR application resources.
 * @param      app  The TaggIR application object.
*/
static void taggir_app_free(TaggIRApp* app) {
#ifdef BACKLIGHT_ON
    notification_message(app->notifications, &sequence_display_backlight_enforce_auto);
#endif
    furi_record_close(RECORD_NOTIFICATION);

    view_dispatcher_remove_view(app->view_dispatcher, TaggIRViewTextInput);
    text_input_free(app->text_input);
    free(app->temp_buffer);
    view_dispatcher_remove_view(app->view_dispatcher, TaggIRViewAbout);
    widget_free(app->widget_about);
    view_dispatcher_remove_view(app->view_dispatcher, TaggIRViewGame);
    view_free(app->view_game);
    view_dispatcher_remove_view(app->view_dispatcher, TaggIRViewConfigure);
    variable_item_list_free(app->variable_item_list_config);
    view_dispatcher_remove_view(app->view_dispatcher, TaggIRViewSubmenu);
    submenu_free(app->submenu);
    view_dispatcher_free(app->view_dispatcher);
    furi_record_close(RECORD_GUI);

    free(app);
}

/**
 * @brief      Main function for TaggIR application.
 * @details    This function is the entry point for the TaggIR application.  It should be defined in
 *           application.fam as the entry_point setting.
 * @param      _p  Input parameter - unused
 * @return     0 - Success
*/
int32_t taggir_app(void* p) {
    UNUSED(p);

    TaggIRApp* app = taggir_app_alloc();
    view_dispatcher_run(app->view_dispatcher);

    taggir_app_free(app);
    return 0;
}