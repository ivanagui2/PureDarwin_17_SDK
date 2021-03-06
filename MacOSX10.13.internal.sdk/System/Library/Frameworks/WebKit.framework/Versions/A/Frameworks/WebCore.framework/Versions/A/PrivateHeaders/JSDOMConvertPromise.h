/*
 * Copyright (C) 2017 Yusuke Suzuki <utatane.tea@gmail.com>.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

#pragma once

#include "IDLTypes.h"
#include "JSDOMConvertBase.h"
#include "JSDOMPromise.h"

namespace WebCore {

template<typename T> struct Converter<IDLPromise<T>> : DefaultConverter<IDLPromise<T>> {
    using ReturnType = JSC::JSPromise*;

    // https://heycam.github.io/webidl/#es-promise
    template<typename ExceptionThrower = DefaultExceptionThrower>
    static ReturnType convert(JSC::ExecState& state, JSC::JSValue value, ExceptionThrower&& exceptionThrower = ExceptionThrower())
    {
        JSC::VM& vm = state.vm();
        auto scope = DECLARE_THROW_SCOPE(vm);
        auto* globalObject = jsDynamicDowncast<JSDOMGlobalObject*>(vm, state.lexicalGlobalObject());
        if (!globalObject)
            return nullptr;

        // 1. Let resolve be the original value of %Promise%.resolve.
        // 2. Let promise be the result of calling resolve with %Promise% as the this value and V as the single argument value.
        auto* promise = JSC::JSPromise::resolve(*globalObject, value);
        if (scope.exception()) {
            exceptionThrower(state, scope);
            return nullptr;
        }
        ASSERT(promise);

        // 3. Return the IDL promise type value that is a reference to the same object as promise.
        return promise;
    }
};

template<typename T> struct JSConverter<IDLPromise<T>> {
    static constexpr bool needsState = false;
    static constexpr bool needsGlobalObject = false;

    static JSC::JSValue convert(JSC::JSPromise& promise)
    {
        // The result of converting an IDL promise type value to an ECMAScript value is the Promise value
        // that represents a reference to the same object that the IDL promise type represents.
        return &promise;
    }
};

} // namespace WebCore
