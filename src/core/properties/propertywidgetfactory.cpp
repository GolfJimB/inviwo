/**********************************************************************
 * Copyright (C) 2012-2013 Scientific Visualization Group - Link�ping University
 * All Rights Reserved.
 * 
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * No part of this software may be reproduced or transmitted in any
 * form or by any means including photocopying or recording without
 * written permission of the copyright owner.
 *
 * Primary author : Timo Ropinski
 *
 **********************************************************************/

#include <inviwo/core/common/inviwoapplication.h>
#include <inviwo/core/common/inviwomodule.h>
#include <inviwo/core/properties/propertywidgetfactory.h>
#include <inviwo/core/properties/propertysemantics.h>

namespace inviwo {

PropertyWidgetFactory::PropertyWidgetFactory() {}

PropertyWidgetFactory::~PropertyWidgetFactory() {}


void PropertyWidgetFactory::registerObject(PropertyWidgetFactoryObject* propertyWidget) {
    std::string className = propertyWidget->getClassName();
    PropertySemantics sematics = propertyWidget->getSematics();

    std::pair<WidgetMap::const_iterator, WidgetMap::const_iterator> sameKeys;
    sameKeys = widgetMap_.equal_range(className);

    for(WidgetMap::const_iterator it = sameKeys.first; it != sameKeys.second; ++it) {
        if(sematics == it->second->getSematics()) {
            LogWarn("Adding a PropertyWidget for a Property (" << className
                    << ") and senamics (" << sematics << ") that is already registed");
        }
    }
    widgetMap_.insert(std::make_pair(className, propertyWidget));
}

PropertyWidget* PropertyWidgetFactory::create(Property* property) {
    PropertySemantics sematics = property->getSemantics();
    std::pair<WidgetMap::const_iterator, WidgetMap::const_iterator> sameKeys;
    sameKeys = widgetMap_.equal_range(property->getClassName());

    for(WidgetMap::const_iterator it = sameKeys.first; it != sameKeys.second; ++it) {
        if(sematics == it->second->getSematics()) {
            return it->second->create(property);
        }
    }
    for(WidgetMap::const_iterator it = sameKeys.first; it != sameKeys.second; ++it) {
        if(PropertySemantics::Default == it->second->getSematics()) {
            LogWarn("Requested property widget semantics ("<< sematics <<") for property ("
                   <<property->getClassName()<<") does not exist, returning Deafult senamic");
            return it->second->create(property);
        }
    }
    LogWarn("Can not find a property widget for property: " << property->getClassName() << "("<< sematics <<")");
    return 0;
}

IvwSerializable* PropertyWidgetFactory::create(std::string className) const {
    // Widgets are not serializable...
    return 0;
}

bool PropertyWidgetFactory::isValidType(std::string className) const {
    WidgetMap::const_iterator it = widgetMap_.find(className);
    if(it != widgetMap_.end())
        return true;
    else
        return false;
}

std::vector<PropertySemantics> PropertyWidgetFactory::getSupportedSemanicsForProperty(Property* property) {
    std::pair<WidgetMap::const_iterator, WidgetMap::const_iterator> sameKeys;
    sameKeys = widgetMap_.equal_range(property->getClassName());

    std::vector<PropertySemantics> semantics;
    for(WidgetMap::const_iterator it = sameKeys.first; it != sameKeys.second; ++it) {
        semantics.push_back(it->second->getSematics());
    }
    return semantics;
}

} // namespace
