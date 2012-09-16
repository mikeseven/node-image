var Image=new require('../lib/image').Image
  , util=require('util')
  , fs=require('fs');

var log=console.log;

function save(image, fif, filename, flags) {
  console.log("Saving image to " + filename + " format: " + fif.toString(16) + " flags: " + flags.toString(16));
  var buffer = image.saveToMemory(fif, filename, flags);
  fs.writeFileSync(filename, buffer);
}

log('FreeImage version: '+Image.getVersion());

var path=__dirname+'/mike_scooter.jpg';
var imageData = fs.readFileSync(path);
var image=Image.loadFromMemory(imageData);
log('Image '+path+': \n'+util.inspect(image));

image=image.convertTo32Bits();

save(image, Image.FIF_JPEG,"out_75.jpg",Image.JPEG_PROGRESSIVE | Image.JPEG_QUALITYGOOD);
save(image, Image.FIF_JPEG,"out_25.jpg",Image.JPEG_PROGRESSIVE | Image.JPEG_QUALITYAVERAGE);
save(image, Image.FIF_PNG,"out_9.png",Image.PNG_Z_BEST_COMPRESSION);
save(image, Image.FIF_PNG,"out_0.png",Image.PNG_Z_NO_COMPRESSION);
image.unload();
