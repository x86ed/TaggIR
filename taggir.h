#pragma once
#include <gui/gui.h>
#include <gui/view.h>
#include <gui/view_dispatcher.h>
#include <gui/modules/submenu.h>
#include <gui/modules/text_input.h>
#include <gui/modules/widget.h>
#include <gui/modules/variable_item_list.h>
#include <dialogs/dialogs.h>
#include <notification/notification.h>
#include <toolbox/stream/file_stream.h>

// Our application menu has 3 items.  You can add more items if you want.
typedef enum {
    TaggIRSubmenuIndexConfigure,
    TaggIRSubmenuIndexGame,
    TaggIRSubmenuIndexAbout,
} TaggIRSubmenuIndex;

// Each view is a screen we show the user.
typedef enum {
    TaggIRViewSubmenu, // The menu when the app starts
    TaggIRViewTextInput, // Input for configuring text settings
    TaggIRViewConfigure, // The configuration screen
    TaggIRViewGame, // The main screen (delete)
    TaggIRViewAbout, // The about screen with directions, link to social channel, etc.
    TaggIRViewSelectFile //select image file to load
} TaggIRView;

typedef enum {
    TaggIREventIdRedrawScreen = 0, // Custom event to redraw the screen
    TaggIREventIdOkPressed = 42, // Custom event to process OK button getting pressed down
} TaggIREventId;

typedef struct {
    ViewDispatcher* view_dispatcher; // Switches between our views
    NotificationApp* notifications; // Used for controlling the backlight
    Submenu* submenu; // The application menu
    TextInput* text_input; // The text input screen
    VariableItemList* variable_item_list_config; // The configuration screen
    View* view_game; // The main screen
    Widget* widget_about; // The about screen
    Stream* stream;
    Storage* storage;

    char* esl_barcode;
    char* plid;
    uint32_t* width;
    uint32_t* height;
    VariableItem* barcode_item; // The name setting item (so we can update the text)
    VariableItem* plid_item;
    VariableItem* type_item;
    VariableItem* res_item;
    VariableItem* img_item;
    VariableItem* desc_item;
    char* temp_buffer; // Temporary buffer for text input
    uint32_t temp_buffer_size; // Size of temporary buffer

    FuriTimer* timer; // Timer for redrawing the screen
} TaggIRApp;

typedef struct {
    char* res_string;
    char* description;
    int width;
    int height;
    uint8_t* mode;
} TaggIRInstance;

typedef struct {
    uint32_t display_color_index; // The team color setting index
    FuriString* barcode_name; // The name setting
    uint8_t x; // The x coordinate
} TaggIRGameModel;