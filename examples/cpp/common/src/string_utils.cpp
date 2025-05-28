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
 *   Name:       string_utils.cpp
 *   Project:    Red SDK examples common library
 *
 *   Description: Common string utilities
 *
 ******************************************************************************/
#include <vector>
#include <string>
#include <cstring>

#include "../include/string_utils.hpp"

void common::split_tensubten(char const *ten_subten, char **ten, char **subten)
{
    /*
     * Need to handle the RED_TENANT that has comma separated tenants.
     * For this, use the first defined tenant.
     */
    std::string              tenant_env(ten_subten);
    std::string              component;
    size_t                   pos;
    std::vector<std::string> tenant_vec;

    while ((pos = tenant_env.find_first_of(",")) != std::string::npos)
    {
        component = tenant_env.substr(0, pos);
        tenant_vec.push_back(component);

        tenant_env.erase(0, pos + 1);
    }

    if (tenant_env.length() > 0)
        tenant_vec.push_back(tenant_env);

    /*
     * If no tenants are defined, default to "red/red"
     */
    if (tenant_vec.size() == 0)
    {
        *ten    = strdup("red");
        *subten = strdup("red");
        return;
    }

    /*
     * Split out the tenant and subtenant from the
     * program parameters
     */
    char *dup = strdup(tenant_vec[0].c_str());
    char *str;
    char *rem = NULL;
    str       = strtok_r(dup, "/", &rem);
    if (str != NULL)
    {
        *ten = strdup(str);
        str  = strtok_r(NULL, "/", &rem);
        if (str != NULL)
            *subten = strdup(str);
        else
            *subten = strdup("red");
    }
    else
    {
        *ten    = strdup("red");
        *subten = strdup("red");
    }

    free(dup);
}
