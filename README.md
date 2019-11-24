# MechEngine

The included files are a template for the Mech Engine. Below you'll
find an FAQ explaining just what the Mech Engine is and how to use it. 
## FAQ

### 1) Q: What is the 'Mech Engine?'

A: The Mech Engine is a general purpose game engine for creating RTS-styled 
games. Games such as Starcraft, Warcraft, and Command and Conquer should come 
to mind. However, it doesn't stop at just building bases and controlling armies. 
This engine will be able to support a wide range of genres such as hack'n'slash,
tower-defense, MOBAs, etc... If the camera is top-down and there's clicking
then this engine can do it!

### 2) Q: How was the Mech Engine made?

A: This engine is written in C++ with the DirectX API. DirectX 9 to be specific
but that may change in the future. Don't count on it for now though.

### 3) Q: Is it free and/or open source?

A: Absolutely! You may use the engine and make changes as you see fit. Just be
sure to tell everyone where you got it from. I'd really appreciate it considering
I'm just a single programmer trying to build up his career from scratch.

### 4) Q: I downloaded the engine but I'm not sure how to use it...

A: First off, make sure you understand how to program with C++! Second, learn the basics 
of DirectX. The very beginning of this engine I put together with the help of a book
called 'Beginning Game Programming Fourth Edition' by Jonathan S. Harbour. So long as
you understand C++, this book is fantastic for getting your feet wet with DirectX and
learning this engine. As the engine develops, I will put together resources on using it.
For now, hold tight and don't be afraid to mess around with it!

### 5) Q: I'm having trouble getting the engine to start!

A: Before the engine will work you need to make a few changes. In regards to Microsoft
Visual Studio, right click on the project name in the solution explorer. Click 'properties.'
Under Configuration Properties then General you must change Character Set to Multi-Byte Character
Set. 

Next, go to VC++ Directories under the Configuration Properties. Change the Include Directories
and Library Directories to where your installation of DirectX is. Mine is located here:
C:\Program Files (x86)\Microsoft DirectX SDK (June 2010). Give it the location of your include
and library folders respectively. 

Finally, under Linker then Input you need to edit Additional Dependencies. Put in the following:

* dxguid.lib
* d3d9.lib
* d3dx9.lib
* dinput8.lib
* XInput.lib
* dsound.lib
* dxerr.lib

Hit apply and you should be all set! Run the program and if you see a box with nothing but a blue
screen then you know it compiled successfully.
