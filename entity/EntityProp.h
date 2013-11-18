/*
 * Copyright 2008-2013 Nicolas Maingot
 *
 * This file is part of CSSMatch.
 *
 * CSSMatch is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * CSSMatch is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with CSSMatch; if not, see <http://www.gnu.org/licenses>.
 *
 * Additional permission under GNU GPL version 3 section 7
 *
 * If you modify CSSMatch, or any covered work, by linking or combining
 * it with "Source SDK" (or a modified version of that SDK), containing
 * parts covered by the terms of Source SDK licence, the licensors of 
 * CSSMatch grant you additional permission to convey the resulting work.
 */

#ifndef __ENTITY_PROP_H__
#define __ENTITY_PROP_H__

#include "../misc/common.h"

#include <string>
#include <fstream>

class SendTable;
struct edict_t;
class CBaseEntity;

#include "../exceptions/BaseException.h"

namespace cssmatch
{
    class EntityPropException : public BaseException
    {
    public:
        EntityPropException(const std::string & msg) : BaseException(msg)
        {}
    };

    /** Handle the (networked) entities game properties*/
    class EntityProp
    {
    private:
        bool initialized;

        /** The prop class */
        std::string theClass;

        /** The prop path (e.g. step1.step2.prop) */
        std::string path;

        /** The prop offset */
        int offset;

        /** Compute the prop offset
         * @param table The table which contains the prop
         * @param propPathLeft Path left until the prop
         */
        void getOffset(SendTable * table, std::istringstream & propPathLeft);

        /** Search the prop offset among the server props */
        void initialize();
    public:
        /** Initialize the handler
         * @param propClass The prop class
         * @param propPath The prop path - Step separator is "." (e.g. step1.step2.prop)
         */
        EntityProp(const std::string & propClass, const std::string & propPath);

        /** Retrieve a prop reference about a given entity
         * @param entity The VALID entity
         * @throws EntityPropException If the prop was not found
         */
        template<typename T>
        T & getProp(edict_t * entity) throw(EntityPropException);
    };

    template<typename T>
    T & EntityProp::getProp(edict_t * entity) throw(EntityPropException)
    {
        if (! initialized)
        {
            initialize();
            initialized = true;
        }

        T * prop = NULL;

        if (offset > 0)
        {
            CBaseEntity * baseEntity = getBaseEntity(entity);
            prop = reinterpret_cast<T *>(reinterpret_cast<char *>(baseEntity) + offset);
            if (prop == NULL)
                throw EntityPropException(std::string(
                                              "CSSMatch was unable to retrieve prop ") + theClass +
                                          "." + path + " for an entity");
        }
        else
            throw EntityPropException(std::string(
                                          "CSSMatch was unable to find the offset of prop ") +
                                      theClass + "." + path);

        return *prop;
    }
}

#endif // __ENTITY_PROP_H__
