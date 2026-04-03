# HTTP Image request

This sample shows how to implement an asynchronous *HTTP* request to a *REST API* to download image files and apply them to scene objects at runtime.

Two requests are performed to retrieve sample image data:

<ol>
-**eu.httpbin.org/image/png** - to download a *PNG* image
-**eu.httpbin.org/image/jpeg** - to download a *JPEG* image
</ol>
Only *PNG* and *JPEG* formats are supported for runtime loading into *Image* Class instance from raw data.

The **![github.com/yhirose/cpp-httplib](https://github.com/yhirose/cpp-httplib)** library is used to perform the *HTTP* requests.

Once an image is retrieved, it is loaded from raw byte data using the *Image::load()* method. If successful, the image is assigned to the albedo texture slot of the target material using *Material::setTextureImage()*. The texture is applied at runtime to the specified surface of an object in the scene. If loading fails, the downloaded data is written to a file for further inspection.

This sample showcases a practical approach to fetching external media assets, validating them, and using them in your scenes or application logic.