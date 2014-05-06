/*
** This file contains proprietary software owned by Motorola Mobility, Inc. **
** No rights, expressed or implied, whatsoever to this software are provided by Motorola Mobility, Inc. hereunder. **
**
** (c) Copyright 2011 Motorola Mobility, Inc.  All Rights Reserved.  **
*/


#ifndef COMMON_H_
#define COMMON_H_

#include <node.h>
#include "nan.h"

#define JS_STR(...) NanSymbol(__VA_ARGS__)
#define JS_INT(val) v8::Integer::New(val)
#define JS_FLOAT(val) v8::Number::New(val)
#define JS_BOOL(val) v8::Boolean::New(val)
#define JS_EXCEPTION(reason) v8::ThrowException(v8::Exception::Error(JS_STR(reason)))
#define JS_RETHROW(tc) v8::Local<v8::Value>::New(tc.Exception());

#define REQ_ARGS(N)                                                     \
    if (args.Length() < (N))                                            \
        return NanThrowException(Exception::TypeError(                  \
        JS_STR("Expected " #N "arguments")));

#define REQ_STR_ARG(I, VAR)                                             \
    if (args.Length() <= (I) || !args[I]->IsString())                   \
        return NanThrowException(Exception::TypeError(                  \
        JS_STR("Argument " #I " must be a string"))); \
        String::Utf8Value VAR(args[I]->ToString());

#define REQ_EXT_ARG(I, VAR)                                             \
    if (args.Length() <= (I) || !args[I]->IsExternal())                 \
        return NanThrowException(Exception::TypeError(                  \
        JS_STR("Argument " #I " invalid")));                            \
        Local<External> VAR = Local<External>::Cast(args[I]);

#define REQ_FUN_ARG(I, VAR)                                             \
    if (args.Length() <= (I) || !args[I]->IsFunction())                 \
        return NanThrowException(Exception::TypeError(                  \
        JS_STR("Argument " #I " must be a function")));                 \
        Local<Function> VAR = Local<Function>::Cast(args[I]);

#define REQ_ERROR_THROW(error) if (ret == error) return JS_EXCEPTION(#error);


#endif /* COMMON_H_ */
