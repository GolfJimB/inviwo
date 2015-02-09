/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 * Version 0.9
 *
 * Copyright (c) 2012-2015 Inviwo Foundation
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *********************************************************************************/

#ifndef IVW_INVIWOSETUPINFO_H
#define IVW_INVIWOSETUPINFO_H


#include <inviwo/core/common/inviwocoredefine.h>
#include <inviwo/core/io/serialization/ivwserializable.h>
#include <inviwo/core/processors/processorfactoryobject.h>
#include <string>
#include <vector>

namespace inviwo {

class InviwoModule;
class InviwoApplication;

struct IVW_CORE_API InviwoSetupInfo : public IvwSerializable {
    struct ModuleSetupInfo : public IvwSerializable {
        ModuleSetupInfo() : name_("") {}
        ModuleSetupInfo(const InviwoModule* module);
        virtual void serialize(IvwSerializer& s) const;
        virtual void deserialize(IvwDeserializer& d);
        std::string name_;
        std::vector<std::string> processors_;
    };

    InviwoSetupInfo(){};
    InviwoSetupInfo(const InviwoApplication* app);
    virtual void serialize(IvwSerializer& s) const;
    virtual void deserialize(IvwDeserializer& d);
    std::vector<ModuleSetupInfo> modules_;
    
    
    std::string getModuleForProcessor(const std::string& processor) const;
};

}  // namespace

#endif  // IVW_INVIWOSETUPINFO_H