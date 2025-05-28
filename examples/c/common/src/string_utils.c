/******************************************************************************
 *
 * @file
 * @copyright
 *                               --- WARNING ---
 *
 *     This work contains trade secrets of DataDirect Networks, Inc.  Any
 *     unauthorized use or disclosure of the work, or any part thereof, is
 *     strictly prohibited. Any use of this work without an express license
 *     or permission is in violation of applicable laws.
 *
 * @copyright DataDirect Networks, Inc. CONFIDENTIAL AND PROPRIETARY
 * @copyright DataDirect Networks Copyright, Inc. (c) 2021-2025. All rights reserved.
 *
 * @section DESCRIPTION
 *
 *   Name:       string_utils.c
 *   Project:    Red SDK examples common library
 *
 *   Description: Common string utilities
 *
 ******************************************************************************/
#include <stdlib.h>
#include <string.h>
#include "../include/string_utils.h"

void common_split_tensubten(const char *ten_subten, char **ten, char **subten)
{
    char  *dup;
    char  *str;
    char  *saveptr = NULL;
    char  *tenant_str;
    char  *first_tenant;
    size_t tenant_len;

    /* Handle empty input */
    if (!ten_subten || !*ten_subten)
    {
        *ten    = strdup("red");
        *subten = strdup("red");
        return;
    }

    /* Copy input string as strtok modifies it */
    tenant_str = strdup(ten_subten);
    if (!tenant_str)
    {
        *ten    = strdup("red");
        *subten = strdup("red");
        return;
    }

    /* Get first tenant from comma-separated list */
    first_tenant = strtok_r(tenant_str, ",", &saveptr);
    if (!first_tenant)
    {
        free(tenant_str);
        *ten    = strdup("red");
        *subten = strdup("red");
        return;
    }

    /* Trim whitespace */
    while (*first_tenant == ' ')
        first_tenant++;
    tenant_len = strlen(first_tenant);
    while (tenant_len > 0 && first_tenant[tenant_len - 1] == ' ')
        tenant_len--;
    first_tenant[tenant_len] = '\0';

    /* Split tenant/subtenant */
    dup = strdup(first_tenant);
    free(tenant_str);

    if (!dup)
    {
        *ten    = strdup("red");
        *subten = strdup("red");
        return;
    }

    str = strtok_r(dup, "/", &saveptr);
    if (str)
    {
        *ten = strdup(str);
        str  = strtok_r(NULL, "/", &saveptr);
        if (str)
        {
            *subten = strdup(str);
        }
        else
        {
            *subten = strdup("red");
        }
    }
    else
    {
        *ten    = strdup("red");
        *subten = strdup("red");
    }

    free(dup);
}
