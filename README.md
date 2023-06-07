# Illumination models in C++ and OpenGL

## Description
This project explores the implementation of the following illumination models: Phong, Blinn-Phong, and Cook-Torrance, using C++ and OpenGL. It showcases several interesting features and techniques.

<img src="https://github.com/valibojici/licenta/assets/68808448/9b8c226c-4f85-42fa-8b9f-de508ba68905" width=30%>
<img src="https://github.com/valibojici/licenta/assets/68808448/83981675-8399-4e31-af7e-04b1e4e89af2" width=60%>


## Cool stuff I learned to do
### 🌐 Generating a sphere from an icosahedron

The project includes a method for generating an sphere from an [icosahedron](https://en.wikipedia.org/wiki/Icosahedron). It subdivides each triangle into four smaller triangles. This approach ensures better vertex distribution compared to a UV sphere. This approach ensures better vertex distribution compared to a UV sphere (where more vertices are at poles). 

<img src="https://github.com/valibojici/licenta/assets/68808448/4fdc3e8b-b71a-4bf7-89d2-75048431df39" width=50%>

Based on: *OpenGL programming guide: the official guide to learning OpenGL, versions 3.0 and 3.1 (p. 118)*

```c++
for (int k = 0; k < subdivisions; ++k) {
  // vector to hold the newly created vertices
  std::vector<unsigned int> newIndices;
  // iterate through each triangle (3 indices at a time)
  for (unsigned int i = 0; i < indices.size(); i += 3) {
    // get the indices of this triangle
    unsigned int i1 = indices[i];
    unsigned int i2 = indices[i + 1];
    unsigned int i3 = indices[i + 2];
    // get the indices of the (newly) created middle vertices
    unsigned int t1 = getMiddle(i1, i2);
    unsigned int t2 = getMiddle(i2, i3);
    unsigned int t3 = getMiddle(i3, i1);

    // replace triangle i1 i2 i3 with 4 triangles
    /* 
           i3
           /\
       t3 /__\t2
         /_\/_\
       i1  t1  i2
    */
    std::vector<unsigned int> newTriangles{
      i1, t1, t3,
      t1, t2, t3,
      t1, i2, t2,
      t3, t2, i3
    };
    // insert the all the indices for these 4 triangles
    newIndices.insert(newIndices.end(), newTriangles.begin(), newTriangles.end());
  }
  // replace the original indices after this subdivision
  indices = newIndices;
  }
```

### 🗒️ Template Shaders

A very basic templating system has been developed, resembling Blade syntax, to simplify the creation and customization of shaders. This is done by some simple string manipulation.

1. `@include "path" `

If a file contains this, then the content of that file is pasted there, replacing the command

2. `@extends "path"` 
If a file contains this at the start then it uses the file at 'path' as a template. For example file_1 has:
```c++
int x = 1;
@has "important_section" 
x += 1;
```
and file_2 has:
```c++
@extends "path_to_file_1"
// this line wont be included <----
@section "important_section" 
x = 10;
@endsection
```
Then after processing file_2 the final result is:
```c++
int x = 1;
x = 10;
x += 1;
```
### 📆 EventManager
I implemented an EventManager class based on the observer pattern (the class has a list of objects which are interested to process events). 

Internally this class uses a `unordered_map` for textures: key=*path to texture* and value=*weak pointer to texture object*.

#### Smart pointers

`std::weak_ptr` is used so if the texture is not needed anymore then it's deleted (the EventManager class does not keep the object alive)
```c++
// store weak pointers for each texture name, check if they are null and load texture
std::unordered_map<std::string, std::weak_ptr<Texture> > m_textures;
```
When a texture is needed, first get the pointer, then check if it points to null (without taking ownership):
```c++
// get the stored texture (can be null)
std::weak_ptr<Texture> storedTexture = m_textures[path];

// try to access the texture with a shared pointer
std::shared_ptr<Texture> texture = storedTexture.lock();
// if it is null then texture is not loaded
if (texture == nullptr) {
    // load texture
    texture = std::make_shared<Texture>(path, type, flipY);
    m_textures[path] = texture; // store weak_ptr to loaded texture
}

return texture;
```


### 🎨 ImGui Integration 
[ImGui](https://github.com/ocornut/imgui), a popular GUI library, is used providing an interface for tweaking parameters and interacting with the application in real-time.

<img src="https://github.com/valibojici/licenta/assets/68808448/c1faa7ba-23dc-49dd-ae13-3a59bd1bd7c5" width=20%>


### ✏️ Post-processing
Some [post-processing](https://en.wikipedia.org/wiki/Image_editing) is done on the texture (image) which contains the scene. 
#### Edge Detection with Sobel Operator
[Sobel operator](https://en.wikipedia.org/wiki/Sobel_operator) is used for toon shading to draw the outlines. The scene is rendered to a texture using normals instead of colors then the sobel filter is used to detect edges.

<img src="https://github.com/valibojici/licenta/assets/68808448/5d182a18-0483-46f6-a83d-3edd9b2244f8" width=33%>
<img src="https://github.com/valibojici/licenta/assets/68808448/1a04e5cb-c0bc-4f87-a5ab-c7e8ffd40dd8" width=33%>
<img src="https://github.com/valibojici/licenta/assets/68808448/5ca65fdb-eeb3-42a9-b43e-e0856daa070b" width=33%>

#### Gamma correction
[Gamma correction](https://learnopengl.com/Advanced-Lighting/Gamma-Correction) is used so the lighting calculations are done in linear space and resulting colors are displayed correctly.

#### HDR- High Dynamic Range
[Tone mapping](https://en.wikipedia.org/wiki/High-dynamic-range_rendering) is used to compresses the wide range of RGB values into a narrower range (between 0 and 1) that can be displayed properly on regular monitors, while preserving important details and visual appearance.

### 🎥 FPS Camera
The user can fly around the scene using a [camera](https://ogldev.org/www/tutorial13/tutorial13.html) controlled by keyboard and mouse. The camera calculates the view matrix manually from the basis vectors.
