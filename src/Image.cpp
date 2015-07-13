/*
** This file contains proprietary software owned by Motorola Mobility, Inc. **
** No rights, expressed or implied, whatsoever to this software are provided by Motorola Mobility, Inc. hereunder. **
**
** (c) Copyright 2011 Motorola Mobility, Inc.  All Rights Reserved.  **
*/

#include "string.h"

#include "Image.h"
#include <node_buffer.h>

#include <iostream>
using namespace std;

using namespace node;
using namespace v8;

namespace freeimage {

Persistent<FunctionTemplate> Image::constructor_template;

Image::Image(Handle<Object> wrapper) {}
Image::~Image() {
  Local<Value> internalField = handle_->GetInternalField(0);
  if (internalField->IsNull()) return;

  FIBITMAP *dib = static_cast<FIBITMAP*>(Local<External>::Cast(internalField)->Value());
  FreeImage_Unload(dib);
}

void Image::Initialize(Handle<Object> target) {
  NanScope();

  Local<FunctionTemplate> t = FunctionTemplate::New(Image::New);
  constructor_template = Persistent<FunctionTemplate>::New(t);

  constructor_template->InstanceTemplate()->SetInternalFieldCount(1);
  constructor_template->SetClassName(NanNew<String>("Image"));

  NODE_SET_PROTOTYPE_METHOD(constructor_template, "unload", unload);
  NODE_SET_PROTOTYPE_METHOD(constructor_template, "save", save);
  NODE_SET_PROTOTYPE_METHOD(constructor_template, "saveToMemory", saveToMemory);
  NODE_SET_PROTOTYPE_METHOD(constructor_template, "convertTo32Bits", convertTo32Bits);
  NODE_SET_PROTOTYPE_METHOD(constructor_template, "convertTo24Bits", convertTo24Bits);

  NODE_SET_PROTOTYPE_METHOD(constructor_template, "flipHorizontal", flipHorizontal);
  NODE_SET_PROTOTYPE_METHOD(constructor_template, "flipVertical", flipVertical);

  target->Set(NanNew<String>("Image"), constructor_template->GetFunction());
}

NAN_METHOD(Image::New) {
  NanScope();

  Image *fi = new Image(args.This());
  fi->Wrap(args.This());

  NanReturnThis();
}

Image *Image::New(FIBITMAP* dib) {
  NanScope();

  Local<Value> arg = Integer::NewFromUnsigned(0);
  Local<Object> obj = constructor_template->GetFunction()->NewInstance(1, &arg);

  Image *image = ObjectWrap::Unwrap<Image>(obj);

  int w,h,pitch;
  FREE_IMAGE_TYPE type = FreeImage_GetImageType(dib);

  obj->SetInternalField(0, External::New(dib));
  obj->Set(NanNew<String>("width"), NanNew<Integer>(w=FreeImage_GetWidth(dib)));
  obj->Set(NanNew<String>("height"), NanNew<Integer>(h=FreeImage_GetHeight(dib)));
  obj->Set(NanNew<String>("bpp"), NanNew<Integer>((int)FreeImage_GetBPP(dib)));
  obj->Set(NanNew<String>("pitch"), NanNew<Integer>(pitch=FreeImage_GetPitch(dib)));
  obj->Set(NanNew<String>("type"), NanNew<Integer>(type));
  obj->Set(NanNew<String>("redMask"), NanNew<Integer>((int)FreeImage_GetRedMask(dib)));
  obj->Set(NanNew<String>("greenMask"), NanNew<Integer>((int)FreeImage_GetGreenMask(dib)));
  obj->Set(NanNew<String>("blueMask"), NanNew<Integer>((int)FreeImage_GetBlueMask(dib)));

  BYTE *bits=FreeImage_GetBits(dib);
  node::Buffer *buf = node::Buffer::New((char*)bits,h*pitch);
  obj->Set(NanNew<String>("buffer"), buf->handle_);

  return image;
}

NAN_METHOD(Image::unload) {
  NanScope();

  Local<Value> internalField = args.This()->GetInternalField(0);
  if (!internalField->IsNull()) {
    FIBITMAP *dib = static_cast<FIBITMAP*>(Local<External>::Cast(internalField)->Value());
    FreeImage_Unload(dib);
    args.This()->SetInternalField(0, v8::Null());
  }

  NanReturnUndefined();
}

NAN_METHOD(Image::save) {
  NanScope();

  Local<External> wrap = Local<External>::Cast(args.This()->GetInternalField(0));
  FIBITMAP *dib=static_cast<FIBITMAP*>(wrap->Value());

  FREE_IMAGE_FORMAT fif=(FREE_IMAGE_FORMAT) args[0]->Uint32Value();

  String::Utf8Value str(args[1]->ToString());
  int flags=0;
  if(!args[2]->IsUndefined()) {
    flags=args[2]->Int32Value();
  }

  // cout<<"Saving image to "<<*str<<" format: "<<hex<<fif<<dec<<" flags: "<<hex<<flags<<dec<<endl;
  if(fif==FIF_JPEG && FreeImage_GetBPP(dib)!=24) {
    //FIBITMAP *old=dib;
    dib=FreeImage_ConvertTo24Bits(dib);
    //FreeImage_Unload(old);
  }
  FreeImage_Save(fif,dib,*str,flags);

  NanReturnUndefined();
}

NAN_METHOD(Image::saveToMemory) {
  NanScope();

  Local<External> wrap = Local<External>::Cast(args.This()->GetInternalField(0));
  FIBITMAP *dib=static_cast<FIBITMAP*>(wrap->Value());
  //cout<<"dib "<<hex<<dib<<dec<<endl;

  FREE_IMAGE_FORMAT fif=(FREE_IMAGE_FORMAT) args[0]->Uint32Value();

  int flags=0;
  if(!args[1]->IsUndefined()) {
    flags=args[1]->Int32Value();
  }

  if(fif==FIF_JPEG && FreeImage_GetBPP(dib)!=24) {
    // FIBITMAP *old=dib;
    dib=FreeImage_ConvertTo24Bits(dib);
    // FreeImage_Unload(old);
  }

  BYTE *mem_buffer = NULL;
  DWORD file_size;
  FIMEMORY *hmem = FreeImage_OpenMemory();

  FreeImage_SaveToMemory(fif,dib,hmem,flags);

  FreeImage_AcquireMemory(hmem, &mem_buffer, &file_size);
  Buffer *slowBuffer = Buffer::New(file_size);
  memcpy(Buffer::Data(slowBuffer), (char*) mem_buffer,(size_t) file_size);

  FreeImage_CloseMemory(hmem);

  Local<Object> globalObj = Context::GetCurrent()->Global();
  Local<Function> bufferConstructor = Local<Function>::Cast(globalObj->Get(String::New("Buffer")));
  Handle<Value> constructorArgs[3] = { slowBuffer->handle_, v8::Integer::New(file_size), v8::Integer::New(0) };
  Local<Object> actualBuffer = bufferConstructor->NewInstance(3, constructorArgs);

  NanReturnValue(actualBuffer);
}

NAN_METHOD(Image::convertTo32Bits) {
  NanScope();

  Local<External> wrap = Local<External>::Cast(args.This()->GetInternalField(0));
  FIBITMAP *dib=static_cast<FIBITMAP*>(wrap->Value());
  FIBITMAP *conv=FreeImage_ConvertTo32Bits(dib);

  NanReturnValue(NanObjectWrapHandle(Image::New(conv)));
}

NAN_METHOD(Image::convertTo24Bits) {
  NanScope();

  Local<External> wrap = Local<External>::Cast(args.This()->GetInternalField(0));
  FIBITMAP *dib=static_cast<FIBITMAP*>(wrap->Value());
  FIBITMAP *conv=FreeImage_ConvertTo24Bits(dib);

  NanReturnValue(NanObjectWrapHandle(Image::New(conv)));
}


NAN_METHOD(Image::flipHorizontal) {
  NanScope();

  Local<External> wrap = Local<External>::Cast(args.This()->GetInternalField(0));
  FIBITMAP *dib = static_cast<FIBITMAP *>(wrap->Value());
  BOOL flip = FreeImage_FlipHorizontal(dib);

  NanReturnValue(NanNew<Boolean>(flip));
}

NAN_METHOD(Image::flipVertical) {
  NanScope();

  Local<External> wrap = Local<External>::Cast(args.This()->GetInternalField(0));
  FIBITMAP *dib = static_cast<FIBITMAP *>(wrap->Value());
  BOOL flip = FreeImage_FlipVertical(dib);

  NanReturnValue(NanNew<Boolean>(flip));
}

}
