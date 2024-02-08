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

TaggIRInstance GenRes( char code[17])
{
    TaggIRInstance cfg;
    char* out = (char*)malloc(12);
    int width = 0;
    int height = 0;
    char* type = GenType(code);
    int model = atoi(type);
    uint8_t ESLType;

    switch (model)
    {
    case 1206:
        cfg.description = "(Continuum E2 HCS)";
        ESLType = 1;
        break;
    case 1207:
        cfg.description = "(Continuum E2 HCN)";
        // plBitDef = 4;
        ESLType = 1;
        break;


    case 1240:
        cfg.description = "(Continuum E4 HCS)";
        // plBitDef = 3;
        ESLType = 1;
        break;
    case 1241:
        cfg.description = "(Continuum E4 HCN)";
        // plBitDef = 0;
        ESLType = 1;
        break;
    case 1242:
        cfg.description = "(Continuum E4 HCN FZ)";
        // plBitDef = 0;
        ESLType = 1;
        break;
    case 1243:
        cfg.description = "(Continuum E4 HCW (18410-00))";
        ESLType = 1;
        break;

    case 1217:
    case 1265:
        cfg.description = "(Continuum E5 HCS)";
        // plBitDef = 2;
        ESLType = 1;
        break;
    case 1219:
        cfg.description = "(Continuum E5 HCN)";
        // plBitDef = 1;
        ESLType = 1;
        break;


    case 1291:
        cfg.description = "(FVL Promoline 3-16 (18619-00))";
        ESLType = 1;
        break;


    case 1510:
        cfg.description = "(SmartTag E5 M)";
        // plBitDef = 1;
        ESLType = 1;
        break;


    case 1300:
        cfg.description = "(DotMatrix DM3370 172x72)";
        width = 172;
        height = 72;
        ESLType = 2;
        break;
    case 1276:
        cfg.description = "(DotMatrix DM90 320x140) EXPERIMENTAL";
        width = 320;
        height = 140;
        ESLType = 2;
        break;
    case 1275:
        cfg.description = "(DotMatrix DM110 320x192 (13400-00)) EXPERIMENTAL";
        width = 320;
        height = 192;
        ESLType = 2;
        break;


    case 1317:
    case 1322:
        cfg.description = "(SmartTag HD S 152x152)";
        width = 152;
        height = 152;
        ESLType = 2;
        break;


    case 1318:
        cfg.description = "(SmartTag HD M 208x112)";
        width = 208;
        height = 112;
        ESLType = 2;
        break;
    case 1327:
        cfg.description = "(SmartTag HD M Red 208x112)";
        width = 208;
        height = 112;
        ESLType = 3;
        break;
    case 1324:
        cfg.description = "(SmartTag HD M FZ 208x112) EXPERIMENTAL";
        width = 208;
        height = 112;
        ESLType = 2;
        break;


    case 1315:
        cfg.description = "(SmartTag HD L 296x128)";
        width = 296;
        height = 128;
        ESLType = 2;
        break;

    case 1370: // 2021 revision of the SmartTag HD L Red 296x128 - black housing
    case 1328:
        cfg.description = "(SmartTag HD L Red 296x128)";
        width = 296;
        height = 128;
        ESLType = 3;
        break;
    case 1627:
    case 1628:
        cfg.description = "(SmartTag HD L Red 296x128) ";
        width = 296;
        height = 128;
        ESLType = 3;
        break;
    case 1344:
        cfg.description = "(SmartTag HD L Yellow 296x128) EXPERIMENTAL";
        width = 296;
        height = 128;
        ESLType = 3;
        break;


    case 1348:
        cfg.description = "(SmartTag HD T Red 264x176) EXPERIMENTAL";
        width = 264;
        height = 176;
        ESLType = 3;
        break;
    case 1349:
        cfg.description = "(SmartTag HD T Yellow 264x176) EXPERIMENTAL";
        width = 264;
        height = 176;
        ESLType = 3;
        break;


    case 1314:
        cfg.description = "(SmartTag HD110 400x300) EXPERIMENTAL";
        width = 400;
        height = 300;
        ESLType = 2;
        break;
    case 1336:
        cfg.description = "(SmartTag HD110 Red 400x300) EXPERIMENTAL";
        width = 400;
        height = 300;
        ESLType = 3;
        break;
    case 1339:
    case 1639:
        cfg.description = "(SmartTag HD S Red 152x152) ";
        width = 152;
        height = 152;
        ESLType = 3;
        break;

    case 1351:
        cfg.description = "(SmartTag HD150 648x480) EXPERIMENTAL";
        width = 648;
        height = 480;
        ESLType = 2;
        break;
    case 1371: // 2021 revision of the SmartTag HD150 Red 648x480 - black housing
    case 1353:
    case 1354:
        cfg.description = "(SmartTag HD150 Red 648x480)";
        width = 648;
        height = 480;
        ESLType = 3;
        break;


    case 1319:
        cfg.description = "(SmartTag HD200 800x480) EXPERIMENTAL";
        width = 800;
        height = 480;
        ESLType = 2;
        break;
    case 1340:
        cfg.description = "(SmartTag HD200 Red 800x480) EXPERIMENTAL";
        width = 800;
        height = 480;
        ESLType = 3;
        break;
    case 1346:
        cfg.description = "(SmartTag HD200 Yellow 800x480) EXPERIMENTAL";
        width = 800;
        height = 480;
        ESLType = 3;
        break;

    default:
        cfg.description = "(incompatible)";
        ESLType = 0;
        break;
    }

    snprintf(out,9, "%ix%i", width, height);
    cfg.res_string = out;
    cfg.width = width;
    cfg.height = height;
    cfg.mode = &ESLType;
    return cfg;
}