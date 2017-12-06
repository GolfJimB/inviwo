/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2012-2017 Inviwo Foundation
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

#ifndef IVW_UTILITIES_H
#define IVW_UTILITIES_H

#include <inviwo/core/common/inviwocoredefine.h>
#include <inviwo/core/util/exception.h>
#include <inviwo/core/util/stdextensions.h>
#include <inviwo/core/common/inviwoapplication.h>
#include <inviwo/core/util/settings/systemsettings.h>
#include <string>


namespace inviwo {

class ProcessorNetwork;

class Property;
class ProcessorWidget;

namespace util {

IVW_CORE_API void saveNetwork(ProcessorNetwork* network, std::string filename);


IVW_CORE_API void saveAllCanvases(ProcessorNetwork* network, std::string dir,
                                  std::string name = "UPN", std::string ext = ".png");

IVW_CORE_API bool isValidIdentifierCharacter(char c, const std::string& extra = "");

IVW_CORE_API void validateIdentifier(const std::string& identifier, const std::string& type,
                                     ExceptionContext context, const std::string& extra = "");

/**
 * Utility to augment an identifier with a number to make it unique. Will add an increasing number
 * to the end of the given identifier until the isUnique test returns true.
 * Example for a processor identifier:
 *     auto uniqueIdentifier = util::findUniqueIdentifier(
 *         startIdentifier,
 *         [&](const std::string& id) { 
 *             return processorNetwork->getProcessorByIdentifier(id) == nullptr; },
 *         ""
 *     );
 */
IVW_CORE_API std::string findUniqueIdentifier(const std::string& identifier,
                                              std::function<bool(const std::string&)> isUnique,
                                              const std::string& sep = " ");

IVW_CORE_API std::string cleanIdentifier(const std::string& identifier,
                                         const std::string& extra = "");

/** 
 * \brief Removes inviwo-module from module library file name.
 * Turns "/path/to/inviwo-module-yourmodule.dll" into "yourmodule".
 * Returns filename without extension if inviwo-module was not found.
 *
 * @param std::string filePath Path to module file
 * @return IVW_CORE_API std::string Module name
 */
IVW_CORE_API std::string stripModuleFileNameDecoration(std::string filePath);

IVW_CORE_API std::string stripIdentifier(std::string identifier);

namespace detail {

struct IVW_CORE_API Shower {
    void operator()(Property& p);
    void operator()(ProcessorWidget& p);
};

struct IVW_CORE_API Hideer {
    void operator()(Property& p);
    void operator()(ProcessorWidget& p);
};



// https://stackoverflow.com/a/22882504/600633
struct IsCallableTest {
    template <typename F, typename... A>
    static decltype(std::declval<F>()(std::declval<A>()...), std::true_type()) f(int);

    template <typename F, typename... A>
    static std::false_type f(...);
};

template <typename F, typename... A>
struct IsCallable : decltype(IsCallableTest::f<F, A...>(0)) {};

template <typename F, typename... A>
struct IsCallable<F(A...)> : IsCallable<F, A...> {};

template <typename... A, typename F>
constexpr IsCallable<F, A...> is_callable_with(F&&) {
    return IsCallable<F(A...)>{};
}

template <typename Callback, typename IT>
void foreach_helper(std::false_type, IT a, IT b, Callback callback, size_t = 0) {
    std::for_each(a, b, callback);
}

template <typename Callback, typename IT>
void foreach_helper(std::true_type, IT a, IT b, Callback callback, size_t start = 0) {
    std::for_each(a, b, [&](auto v) { callback(v, start++); });
}


} // detail namespace

template <typename... Args>
void show(Args&&... args) {
    util::for_each_argument(detail::Shower{}, std::forward<Args>(args)...);
}

template <typename... Args>
void hide(Args&&... args) {
    util::for_each_argument(detail::Hideer{}, std::forward<Args>(args)...);
}



/**
 * Use multiple threads to iterate over all elements in an iterable data structure (such as
 * std::vector). If the Inviwo pool size is zero it will be executed directly in the same thread as
 * the caller.
 * The function will return once all jobs as has been created and queued.
 *
 * @param iterable the data structure to iterate over
 * @param the callback to call for each element, can be either `[](auto &a){}` or `[](auto &a,
 * size_t id){}` where `a` is an data item from the iterable data structure and `id` is the index in
 * the data structure
 * @param jobs optional parameter specifying how many jobs to create, if jobs==0 (default) it will
 * create pool size * 4 jobs
 * @return a vector of futures, one for each job created. 
 */
template <typename Iterable, typename Callback>
std::vector<std::future<void>> forEachParallelAsync(const Iterable& iterable, Callback callback,
                                                    size_t jobs = 0) {
    using T = typename Iterable::value_type;
    auto settings = InviwoApplication::getPtr()->getSettingsByType<SystemSettings>();
    auto poolSize = settings->poolSize_.get();
    auto includeIndex = typename std::conditional<detail::is_callable_with<T, size_t>(callback),
                                                  std::true_type, std::false_type>::type();
    if (poolSize == 0) {
        detail::foreach_helper(includeIndex, iterable.begin(), iterable.end(), callback);
        return {};
    }

    if (jobs == 0) {  // If jobs is zero, set to 4 times the pool size
        jobs = 4 * poolSize;
    }

    auto s = iterable.size();
    std::vector<std::future<void>> futures;
    for (size_t job = 0; job < jobs; ++job) {
        auto start = (s * job) / jobs;
        auto end = (s * (job + 1)) / jobs;
        futures.push_back(dispatchPool([&callback, &iterable, start, end, &includeIndex]() {
            detail::foreach_helper(includeIndex, iterable.begin() + start,
                                   iterable.begin() + static_cast<size_t>(end), callback, start);
        }));
    }
    return futures;
}


/**
* Use multiple threads to iterate over all elements in an iterable data structure (such as
* std::vector). If the Inviwo pool size is zero it will be executed directly in the same thread as
* the caller.
* The function will return once all jobs as has finished processing.
*
* @param iterable the data structure to iterate over
* @param the callback to call for each element, can be either `[](auto &a){}` or `[](auto &a,
* size_t id){}` where `a` is an data item from the iterable data structure and `id` is the index in
* the data structure
* @param jobs optional parameter specifying how many jobs to create, if jobs==0 (default) it will
* create pool size * 4 jobs
*/
template <typename Iterable, typename Callback>
void forEachParallel(const Iterable& iterable, Callback callback, size_t jobs = 0) {
    auto futures = forEachParallelAsync(iterable, callback, jobs);

    for (const auto& e : futures) {
        e.wait();
    }
}



} // util namespace

} // inviwo namespace

#endif  // IVW_UTILITIES_H
