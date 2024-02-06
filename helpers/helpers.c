#include "helpers.h"

int SerialValidator(char code[17])
{

    // Single char buffer
    char subchar[2];
    subchar[0] = code[1];
    subchar[1] = '\0';
    int four = atoi(subchar);
    if (four != 4)
    {
        FURI_LOG_W("TaggIR","2nd character isn't 4.\n");
        return 1;
    }

    // Extract substring from 3rd and 4th characters
    char substring[3];
    substring[0] = code[2];
    substring[1] = code[3];
    substring[2] = '\0';

    // Convert substring to integers
    int mfg = atoi(substring);

    // Check if the sum is less than 64
    if (mfg > 63)
    {
        FURI_LOG_W("TaggIR","Invalid manufacturing unit\n");
        return 1;
    }

    substring[0] = code[5];
    substring[1] = code[6];

    int week = atoi(substring);

    if (week > 53)
    {
        FURI_LOG_W("TaggIR","Invalid week.\n");
        return 1;
    }

    char subCereal[6];
    subCereal[0] = code[7];
    subCereal[1] = code[8];
    subCereal[2] = code[9];
    subCereal[3] = code[10];
    subCereal[4] = code[11];
    subCereal[5] = '\0';

    int cereal = atoi(subCereal);

    if (cereal > 65535)
    {
        FURI_LOG_W("TaggIR","bad serial number.\n");
        return 1;
    }

    char *digits = code;
    int checksum = 0;

    // Calculate checksum by adding ASCII values of all 16 characters
    for (int i = 0; i < 16; i++)
    {
        checksum += (int)digits[i];
    }

    // Apply modulo 10 to get the final checksum
    checksum %= 10;

    subchar[0] = code[16];
    int realcheck = atoi(subchar);
    if (checksum != realcheck)
    {
        FURI_LOG_W("TaggIR","Bad Checksum.\n");
        return 1;
    }

    return 0;
}

char* GenPLID( char code[17])
{
    // Allocate memory for the output string (assuming doubling the length)
    char* out = (char*)malloc(9);

    char subCereal[6];
    subCereal[0] = code[7];
    subCereal[1] = code[8];
    subCereal[2] = code[9];
    subCereal[3] = code[10];
    subCereal[4] = code[11];
    subCereal[5] = '\0';

    int cereal = atoi(subCereal);

    char mfgChar[6];
    mfgChar[0] = code[2];
    mfgChar[1] = code[3];
    mfgChar[2] = code[4];
    mfgChar[3] = code[5];
    mfgChar[4] = code[6];
    mfgChar[5] = '\0';

    int mfgInt = atoi(mfgChar);

    snprintf(out,9, "%X%X", mfgInt, cereal);
    FURI_LOG_I("TaggIR","PLID: %s", out);
    return out;
}

char* GenType( char code[17])
{
    char* out = (char*)malloc(5);
    out[0] = code[12];
    out[1] = code[13];
    out[2] = code[14];
    out[3] = code[15];
    out[4] = '\0';
    return out;
}

char* GenRes( char code[17])
{
    char* out = (char*)malloc(12);
    int width = 0;
    int height = 0;
    char* type = GenType(code);

    if (strcmp(type,"1370")==0)
    {
        width= 296;
        height= 128;
    }

    snprintf(out,9, "%ix%i", width, height);
    return out;
}
// 296x128 = 37888 which is divisible by 8 (nice)