/* SPDX-License-Identifier: MIT */
/**
 * @file main.cpp
 *
 * @author Lukasz Wiecaszek <lukasz.wiecaszek@gmail.com>
 */

/*===========================================================================*\
 * system header files
\*===========================================================================*/
#include <stdio.h>
#include <stdlib.h>

/*===========================================================================*\
 * project header files
\*===========================================================================*/
#include <version.h>
#include <cmdlineflags.h>
#include <cfgitems.h>

#include <flowgraph.hpp>

/*===========================================================================*\
 * 'using namespace' section
\*===========================================================================*/

/*===========================================================================*\
 * preprocessor #define constants and macros
\*===========================================================================*/

/*===========================================================================*\
 * local type definitions
\*===========================================================================*/
namespace
{

} // end of anonymous namespace

/*===========================================================================*\
 * global object definitions
\*===========================================================================*/

/*===========================================================================*\
 * local function declarations
\*===========================================================================*/

/*===========================================================================*\
 * local object definitions
\*===========================================================================*/

/*===========================================================================*\
 * inline function definitions
\*===========================================================================*/

/*===========================================================================*\
 * public function definitions
\*===========================================================================*/
static void usage(const char* progname);

static int display_version_information(const struct cmdlineflags_option* option);
CMDLINEFLAGS_DEFINE_LONG_OPTION(CMDLINEFLAGS_GLOBAL_MODULE, version, \
    CMDLINEFLAGS_NO_ARGUMENT, display_version_information, "Display version information");

static int display_help_message(const struct cmdlineflags_option* option);
CMDLINEFLAGS_DEFINE(CMDLINEFLAGS_GLOBAL_MODULE, h, help, \
    CMDLINEFLAGS_NO_ARGUMENT, display_help_message, "Display help message");

static int set_configuration_file(const struct cmdlineflags_option* option, const char* argument);
CMDLINEFLAGS_DEFINE(CMDLINEFLAGS_GLOBAL_MODULE, c, configuration_file, \
    CMDLINEFLAGS_REQUIRED_ARGUMENT, set_configuration_file, "Set configuration file");

static int set_dab_center_frequency(const struct cmdlineflags_option* option, const char* argument);
CMDLINEFLAGS_DEFINE(CMDLINEFLAGS_GLOBAL_MODULE, f, frequency, \
    CMDLINEFLAGS_REQUIRED_ARGUMENT, set_dab_center_frequency, "Set dab channel center frequency [Hz]");
CFGITEMS_DEFINE_U32(CFGITEMS_GLOBAL_MODULE, frequency, 0);

static int set_subchannel_start_addr(const struct cmdlineflags_option* option, const char* argument);
CMDLINEFLAGS_DEFINE(CMDLINEFLAGS_GLOBAL_MODULE, s, subchannel_start_addr, \
    CMDLINEFLAGS_REQUIRED_ARGUMENT, set_subchannel_start_addr, "Set subchannel start address [CU] - valid values [0 .. 863]");
CFGITEMS_DEFINE_U32(CFGITEMS_GLOBAL_MODULE, subchannel_start_addr, 0);

static int set_subchannel_size(const struct cmdlineflags_option* option, const char* argument);
CMDLINEFLAGS_DEFINE(CMDLINEFLAGS_GLOBAL_MODULE, z, subchannel_size, \
    CMDLINEFLAGS_REQUIRED_ARGUMENT, set_subchannel_size, "Set subchannel size (required only for equal error protection types) [CU] - valid values [0 .. 863]");
CFGITEMS_DEFINE_U32(CFGITEMS_GLOBAL_MODULE, subchannel_size, 0);

static int set_subchannel_protection_type(const struct cmdlineflags_option* option, const char* argument);
CMDLINEFLAGS_DEFINE(CMDLINEFLAGS_GLOBAL_MODULE, p, subchannel_protection_type, \
    CMDLINEFLAGS_REQUIRED_ARGUMENT, set_subchannel_protection_type, "Set subchannel protection type - valid values {uep, eep-a, eep-b}");
CFGITEMS_DEFINE_STRING(CFGITEMS_GLOBAL_MODULE, subchannel_protection_type, "undefined");

static int set_uep_table_index(const struct cmdlineflags_option* option, const char* argument);
CMDLINEFLAGS_DEFINE(CMDLINEFLAGS_GLOBAL_MODULE, i, table_index, \
    CMDLINEFLAGS_REQUIRED_ARGUMENT, set_uep_table_index, "Set unequal error protection table index - valid values [0 .. 63]");
CFGITEMS_DEFINE_U32(CFGITEMS_GLOBAL_MODULE, uep_table_index, 0);

static int set_eep_protection_level(const struct cmdlineflags_option* option, const char* argument);
CMDLINEFLAGS_DEFINE(CMDLINEFLAGS_GLOBAL_MODULE, l, protection_level, \
    CMDLINEFLAGS_REQUIRED_ARGUMENT, set_eep_protection_level, "Set equal error protection level - valid values [1 .. 4]");
CFGITEMS_DEFINE_U32(CFGITEMS_GLOBAL_MODULE, subchannel_protection_level, 0);

static int set_output_file(const struct cmdlineflags_option* option, const char* argument);
CMDLINEFLAGS_DEFINE(CMDLINEFLAGS_GLOBAL_MODULE, o, output_file, \
    CMDLINEFLAGS_REQUIRED_ARGUMENT, set_output_file, "Set output file");
CFGITEMS_DEFINE_STRING(CFGITEMS_GLOBAL_MODULE, output_file, "channel.out");

int main(int argc, char* argv[])
{
    int status;

    status = cfgitems_init(NULL);
    if (status != CFGITEMS_SUCCESS) {
        fprintf(stderr, "cfgitems_init() failed\n");
        exit(EXIT_FAILURE);
    }

    cmdlineflags_parse(argc, argv);

    do {
        int status;
        uint32_t frequency;
        uint32_t subchannel_start_addr;
        uint32_t subchannel_size;
        const char* subchannel_protection_type;
        uint32_t uep_table_index;
        uint32_t subchannel_protection_level;
        const char* output_file;

        status = cfgitems_get_u32(NULL, "frequency", &frequency);
        if (status != CFGITEMS_SUCCESS)
            break;

        status = cfgitems_get_u32(NULL, "subchannel_start_addr", &subchannel_start_addr);
        if (status != CFGITEMS_SUCCESS)
            break;

        status = cfgitems_get_u32(NULL, "subchannel_size", &subchannel_size);
        if (status != CFGITEMS_SUCCESS)
            break;

        status = cfgitems_get_string(NULL, "subchannel_protection_type", &subchannel_protection_type);
        if (status != CFGITEMS_SUCCESS)
            break;

        status = cfgitems_get_u32(NULL, "uep_table_index", &uep_table_index);
        if (status != CFGITEMS_SUCCESS)
            break;

        status = cfgitems_get_u32(NULL, "subchannel_protection_level", &subchannel_protection_level);
        if (status != CFGITEMS_SUCCESS)
            break;

        status = cfgitems_get_string(NULL, "output_file", &output_file);
        if (status != CFGITEMS_SUCCESS)
            break;

        dab::flowgraph flowgraph(output_file);

        if (!strcmp(subchannel_protection_type, "uep")) {
            flowgraph.start_msc_uep(frequency, subchannel_start_addr, uep_table_index);
        }
        else
        if (!strcmp(subchannel_protection_type, "eep-a")) {
            flowgraph.start_msc_eep_a(
                frequency, subchannel_start_addr, subchannel_size, subchannel_protection_level);
        }
        else
        if (!strcmp(subchannel_protection_type, "eep-b")) {
            flowgraph.start_msc_eep_b(
                frequency, subchannel_start_addr, subchannel_size, subchannel_protection_level);
        }
        else {
            break;
        }

        fprintf(stdout, "press any key to terminate\n");
        (void)std::getchar();

    } while (0);

    return 0;
}

/*===========================================================================*\
 * protected function definitions
\*===========================================================================*/

/*===========================================================================*\
 * private function definitions
\*===========================================================================*/

/*===========================================================================*\
 * local function definitions
\*===========================================================================*/
static void usage(const char* progname)
{
    char help_message[1024];

    int status = cmdlineflags_get_help_msg(help_message, sizeof(help_message), false);
    if (status < 0)
        help_message[0] = '\0';

    fprintf(stdout, "usage: %s [--version] [-h|--help]\n%s",
        progname, help_message);
}

static int display_version_information(const struct cmdlineflags_option* option)
{
    fprintf(stdout, "rtl-sdr-dab version %d.%d.%d\n",
        RTL_SDR_DAB_VERSION_MAJOR, RTL_SDR_DAB_VERSION_MINOR, RTL_SDR_DAB_VERSION_MICRO);
    exit(EXIT_SUCCESS);
}

static int display_help_message(const struct cmdlineflags_option* option)
{
    usage("rtl-sdr-dab");
    exit(EXIT_SUCCESS);
}

static int set_configuration_file(const struct cmdlineflags_option* option, const char* argument)
{
    return cfgitems_parse(argument);
}

static int set_dab_center_frequency(const struct cmdlineflags_option* option, const char* argument)
{
    uint32_t frequency;

    return (cfgitems_to_u32(argument, &frequency) == CFGITEMS_SUCCESS) ?
        cfgitems_set_u32(NULL, "frequency", frequency) : CFGITEMS_FAILURE;
}

static int set_subchannel_start_addr(const struct cmdlineflags_option* option, const char* argument)
{
    uint32_t subchannel_start_addr;

    return (cfgitems_to_u32(argument, &subchannel_start_addr) == CFGITEMS_SUCCESS) ?
        cfgitems_set_u32(NULL, "subchannel_start_addr", subchannel_start_addr) : CFGITEMS_FAILURE;
}

static int set_subchannel_size(const struct cmdlineflags_option* option, const char* argument)
{
    uint32_t subchannel_size;

    return (cfgitems_to_u32(argument, &subchannel_size) == CFGITEMS_SUCCESS) ?
        cfgitems_set_u32(NULL, "subchannel_size", subchannel_size) : CFGITEMS_FAILURE;
}

static int set_subchannel_protection_type(const struct cmdlineflags_option* option, const char* argument)
{
    return cfgitems_set_string(NULL, "subchannel_protection_type", argument);
}

static int set_uep_table_index(const struct cmdlineflags_option* option, const char* argument)
{
    uint32_t uep_table_index;

    return (cfgitems_to_u32(argument, &uep_table_index) == CFGITEMS_SUCCESS) ?
        cfgitems_set_u32(NULL, "uep_table_index", uep_table_index) : CFGITEMS_FAILURE;
}

static int set_eep_protection_level(const struct cmdlineflags_option* option, const char* argument)
{
    uint32_t subchannel_protection_level;

    return (cfgitems_to_u32(argument, &subchannel_protection_level) == CFGITEMS_SUCCESS) ?
        cfgitems_set_u32(NULL, "subchannel_protection_level", subchannel_protection_level) : CFGITEMS_FAILURE;
}

static int set_output_file(const struct cmdlineflags_option* option, const char* argument)
{
    return cfgitems_set_string(NULL, "output_file", argument);
}
