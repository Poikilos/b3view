# Contributing
<https://github.com/poikilos/b3view>

## Regression Tests

### Manipulating mesh on failed load
- steps to reproduce
  - File, Open, choose a mesh file such as animal_bat.b3d
  - File, Open, choose a texture (purposely incorrect input)
- incorrect behaviors:
  - manipulating the loaded scene, such as calling remove()
  - SEGFAULT
- correct behaviors:
  - Do nothing to the current scene.
  - Show a message saying that the format is incorrect.


## What Qt Creator does
When building after clean, you can see the g++ commands in the "Compiler Output" bottom tab. See Debug and Release sections below.

### Debug
```
g++ -c -pipe -g -w -fPIC -DQT_QML_DEBUG -I../b3view -I. -I/usr/include/freetype2 -I/../lib64/qt5/mkspecs/linux-g++ -o tmp/main.o ../b3view/main.cpp
g++ -c -pipe -g -w -fPIC -DQT_QML_DEBUG -I../b3view -I. -I/usr/include/freetype2 -I/../lib64/qt5/mkspecs/linux-g++ -o tmp/Engine.o ../b3view/Engine.cpp
g++ -c -pipe -g -w -fPIC -DQT_QML_DEBUG -I../b3view -I. -I/usr/include/freetype2 -I/../lib64/qt5/mkspecs/linux-g++ -o tmp/EventHandler.o ../b3view/EventHandler.cpp
g++ -c -pipe -g -w -fPIC -DQT_QML_DEBUG -I../b3view -I. -I/usr/include/freetype2 -I/../lib64/qt5/mkspecs/linux-g++ -o tmp/UserInterface.o ../b3view/UserInterface.cpp
g++ -c -pipe -g -w -fPIC -DQT_QML_DEBUG -I../b3view -I. -I/usr/include/freetype2 -I/../lib64/qt5/mkspecs/linux-g++ -o tmp/Debug.o ../b3view/Debug.cpp
g++ -c -pipe -g -w -fPIC -DQT_QML_DEBUG -I../b3view -I. -I/usr/include/freetype2 -I/../lib64/qt5/mkspecs/linux-g++ -o tmp/View.o ../b3view/View.cpp
g++ -c -pipe -g -w -fPIC -DQT_QML_DEBUG -I../b3view -I. -I/usr/include/freetype2 -I/../lib64/qt5/mkspecs/linux-g++ -o tmp/CGUITTFont.o ../b3view/extlib/CGUITTFont.cpp
g++ -c -pipe -g -w -fPIC -DQT_QML_DEBUG -I../b3view -I. -I/usr/include/freetype2 -I/../lib64/qt5/mkspecs/linux-g++ -o tmp/Utility.o ../b3view/Utility.cpp
g++ -c -pipe -g -w -fPIC -DQT_QML_DEBUG -I../b3view -I. -I/usr/include/freetype2 -I/../lib64/qt5/mkspecs/linux-g++ -o tmp/settings.o ../b3view/settings.cpp
g++  -o build/b3view tmp/main.o tmp/Engine.o tmp/EventHandler.o tmp/UserInterface.o tmp/Debug.o tmp/View.o tmp/CGUITTFont.o tmp/Utility.o tmp/settings.o   -lIrrlicht -lX11 -lGL -lXxf86vm -lXcursor -lstdc++fs -lfreetype
```

### Release
```
g++ -c -pipe -O2 -w -fPIC  -I../b3view -I. -I/usr/include/freetype2 -I/../lib64/qt5/mkspecs/linux-g++ -o tmp/main.o ../b3view/main.cpp
g++ -c -pipe -O2 -w -fPIC  -I../b3view -I. -I/usr/include/freetype2 -I/../lib64/qt5/mkspecs/linux-g++ -o tmp/Engine.o ../b3view/Engine.cpp
g++ -c -pipe -O2 -w -fPIC  -I../b3view -I. -I/usr/include/freetype2 -I/../lib64/qt5/mkspecs/linux-g++ -o tmp/EventHandler.o ../b3view/EventHandler.cpp
g++ -c -pipe -O2 -w -fPIC  -I../b3view -I. -I/usr/include/freetype2 -I/../lib64/qt5/mkspecs/linux-g++ -o tmp/UserInterface.o ../b3view/UserInterface.cpp
g++ -c -pipe -O2 -w -fPIC  -I../b3view -I. -I/usr/include/freetype2 -I/../lib64/qt5/mkspecs/linux-g++ -o tmp/Debug.o ../b3view/Debug.cpp
g++ -c -pipe -O2 -w -fPIC  -I../b3view -I. -I/usr/include/freetype2 -I/../lib64/qt5/mkspecs/linux-g++ -o tmp/View.o ../b3view/View.cpp
g++ -c -pipe -O2 -w -fPIC  -I../b3view -I. -I/usr/include/freetype2 -I/../lib64/qt5/mkspecs/linux-g++ -o tmp/CGUITTFont.o ../b3view/extlib/CGUITTFont.cpp
g++ -c -pipe -O2 -w -fPIC  -I../b3view -I. -I/usr/include/freetype2 -I/../lib64/qt5/mkspecs/linux-g++ -o tmp/Utility.o ../b3view/Utility.cpp
g++ -c -pipe -O2 -w -fPIC  -I../b3view -I. -I/usr/include/freetype2 -I/../lib64/qt5/mkspecs/linux-g++ -o tmp/settings.o ../b3view/settings.cpp
g++ -Wl,-O1 -o build/b3view tmp/main.o tmp/Engine.o tmp/EventHandler.o tmp/UserInterface.o tmp/Debug.o tmp/View.o tmp/CGUITTFont.o tmp/Utility.o tmp/settings.o   -lIrrlicht -lX11 -lGL -lXxf86vm -lXcursor -lstdc++fs -lfreetype
```



