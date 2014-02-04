/*
 ** This file contains proprietary software owned by Motorola Mobility, Inc. **
 ** No rights, expressed or implied, whatsoever to this software are provided by Motorola Mobility, Inc. hereunder. **
 **
 ** (c) Copyright 2011 Motorola Mobility, Inc.  All Rights Reserved.  **
 */

#include "FreeImage.h"
#include "Image.h"

#include <node_buffer.h>

#include <iostream>
using namespace std;

using namespace node;
using namespace v8;

namespace freeimage {

Persistent<FunctionTemplate> FreeImage::constructor_template;

FreeImage::FreeImage(Handle<Object> wrapper) {
  FreeImage_Initialise(FALSE);
}

FreeImage::~FreeImage() {
  FreeImage_DeInitialise();
}

void FreeImage::Initialize(Handle<Object> target) {
  HandleScope scope;

  Local<FunctionTemplate> t = FunctionTemplate::New(New);
  constructor_template = Persistent<FunctionTemplate>::New(t);

  constructor_template->InstanceTemplate()->SetInternalFieldCount(1);
  constructor_template->SetClassName(JS_STR("FreeImage"));

  NODE_SET_PROTOTYPE_METHOD(constructor_template, "getVersion", getVersion);
  NODE_SET_PROTOTYPE_METHOD(constructor_template, "load", load);
  NODE_SET_PROTOTYPE_METHOD(constructor_template, "loadFromMemory", loadFromMemory);
  NODE_SET_PROTOTYPE_METHOD(constructor_template, "convertFromRawBits", convertFromRawBits);
  NODE_SET_PROTOTYPE_METHOD(constructor_template, "save", save);

  target->Set(JS_STR("FreeImage"), constructor_template->GetFunction());
}



void FreeImageErrorHandler(FREE_IMAGE_FORMAT fif, const char *message) {
  cout << "Error: " << message << endl;
}



JS_METHOD(FreeImage::New) {
  HandleScope scope;
  FreeImage *fi = new FreeImage(args.This());
  fi->Wrap(args.This());

  FreeImage_SetOutputMessage(FreeImageErrorHandler);

  return scope.Close(args.This());
}

JS_METHOD(FreeImage::getVersion) {
  HandleScope scope;
  return scope.Close(JS_STR(FreeImage_GetVersion()));
}

JS_METHOD(FreeImage::load) {
  HandleScope scope;

  String::Utf8Value filename(args[0]->ToString());

  FREE_IMAGE_FORMAT fif = FreeImage_GetFIFFromFilename(*filename);
  FIBITMAP *dib=NULL;

  if( fif != FIF_UNKNOWN )
    dib = FreeImage_Load(fif, *filename);

  // check that dib does not contains pixels
  if(!dib) return Undefined();
  if(!FreeImage_HasPixels(dib)) return Undefined();

  return scope.Close(Image::New(dib)->handle_);
}

JS_METHOD(FreeImage::loadFromMemory) {
  HandleScope scope;

  Local<Object> bufferObj    = args[0]->ToObject();
  BYTE*         bufferData   = (BYTE*) Buffer::Data(bufferObj);
  size_t        bufferLength = Buffer::Length(bufferObj);

  FIMEMORY *hmem = FreeImage_OpenMemory(bufferData, bufferLength);

  FREE_IMAGE_FORMAT fif = FreeImage_GetFileTypeFromMemory(hmem, 0);
  FIBITMAP *dib = FreeImage_LoadFromMemory(fif, hmem, 0);
  FreeImage_CloseMemory(hmem);

  // check that dib does not contains pixels
  if(!dib) return Undefined();
  if(!FreeImage_HasPixels(dib)) return Undefined();

  return scope.Close(Image::New(dib)->handle_);
}

JS_METHOD(FreeImage::convertFromRawBits) {
  HandleScope scope;

  Local<Object> bufferObj    = args[0]->ToObject();
  BYTE*         bufferData   = (BYTE*) Buffer::Data(bufferObj);

  uint32_t width  = args[1]->Int32Value();
  uint32_t height = args[2]->Int32Value();
  uint32_t pitch=width*4, bpp=32;
  uint32_t redMask=0xFF000000, greenMask=0x00FF0000, blueMask=0x0000FF00;
  BOOL topdown = FALSE;

  if(args.Length()>3) pitch=args[3]->Uint32Value();
  if(args.Length()>4) bpp=args[4]->Uint32Value();
  if(args.Length()>5) redMask=args[5]->Uint32Value();
  if(args.Length()>6) greenMask=args[6]->Uint32Value();
  if(args.Length()>7) blueMask=args[7]->Uint32Value();
  if(args.Length()>8) topdown=args[8]->BooleanValue();

// cout<<"convertFromRawBits: wxh: "<<width<<"x"<<height<<" bpp: "<<bpp<<" pitch: "<<pitch<<endl;

  FIBITMAP *dib =
    FreeImage_ConvertFromRawBits(bufferData, width, height, pitch, bpp,
                                 redMask, greenMask, blueMask, topdown);

  // check that dib does not contains pixels
  if(!dib) return Undefined();
  if(!FreeImage_HasPixels(dib)) return Undefined();

  return scope.Close(Image::New(dib)->handle_);
}

JS_METHOD(FreeImage::save) {
  HandleScope scope;
  String::Utf8Value filename(args[0]->ToString());
  cout<<"args length: "<<args.Length()<<endl;

  FREE_IMAGE_FORMAT format = FreeImage_GetFIFFromFilename(*filename);
  cout<<"saving format "<<format<<endl;

  Local<Object> obj=args[1]->ToObject();
  void *buffer = obj->GetIndexedPropertiesExternalArrayData();
  //cout<<"buffer uchar*: "<<(obj->GetIndexedPropertiesExternalArrayDataType()==kExternalUnsignedByteArray)<<endl;
  //cout<<"buffer size: "<<obj->GetIndexedPropertiesExternalArrayDataLength()<<endl;

  uint32_t width=args[2]->ToUint32()->Value();
  uint32_t height=args[3]->ToUint32()->Value();

  uint32_t pitch=width*4, bpp=32;
  uint32_t redMask=0xFF000000, greenMask=0x00FF0000, blueMask=0x0000FF00;

  if(args.Length()>4) pitch=args[4]->ToUint32()->Value();
  if(args.Length()>5) bpp=args[5]->ToUint32()->Value();
  if(args.Length()>6) redMask=args[6]->ToUint32()->Value();
  if(args.Length()>7) greenMask=args[7]->ToUint32()->Value();
  if(args.Length()>8) blueMask=args[8]->ToUint32()->Value();

  cout<<"wxh: "<<width<<"x"<<height<<" bpp: "<<bpp<<" pitch: "<<pitch<<endl;

  FIBITMAP *image = FreeImage_ConvertFromRawBits(
      (BYTE*)buffer,
      width, height, pitch, bpp,
      redMask, greenMask, blueMask);

  if(format==FIF_JPEG && bpp!=24) {
    FIBITMAP *old=image;
    image=FreeImage_ConvertTo24Bits(image);
    FreeImage_Unload(old);
  }
  return scope.Close(Boolean::New((FreeImage_Save(format, image, *filename) == TRUE) ? true : false));
}

}
