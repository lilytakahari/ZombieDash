## UCLA CS 32 Winter 2019 Project 3 - Zombie Dash

Play as Penelope Dolittle and maneuver through an abandoned building, annihilating zombies and saving helpless citizens along the way. This is my implementation of what was required in the spec; I cannot say whether it is free of bugs.

### How to Play

#### Installation
Download the appropriate skeleton `.zip` file. Switch out the `Actor.h, Actor.cpp, StudentWorld.h`, and `StudentWorld.cpp` files with the ones above. Compile and build the files in your IDE. You may also need to download other software depending on your operating system:

- **macOS:** You'll need to have XQuartz and freeGLUT installed to run the program.
    - Install XQuartz from xquartz.org:
        - Open the downloaded .dmg file, double-click on XQuartz.pkg, and follow the installation instructions.
        - Log out and log back in again.
        - To verify the installation, open a Terminal window and run the command `echo $DISPLAY`. That should produce one line of output that ends with `org.macosforge.xquartz:0`
    - Install freeGLUT:
        - Install the homebrew package manager from brew.sh
        - Open a Terminal window and run the command `brew install freeglut`.
Unzip the Mac sample executable zip file. In that zip file is a file named README.txt that tells you how to launch the program.
- **Linux:** Install OpenGL and freeGLUT.
    - Ubuntu: `sudo apt-get install freeglut3 freeglut3-dev`
    - Fedora: `sudo yum install freeglut freeglut-devel`

##### The Goal
Try to save all the living citizens on a level by guiding them toward the exit. Only then can Penelope use the exit and proceed to the next level. Kill zombies, pick up goodies, and save citizens to earn points.

##### The Keys
```
WASD or arrow keys         move around a level
space                      use flamethrower
tab                        deploy landmine at current location
enter                      use vaccine on self
f                          freezes the game
q                          quit
```
##### Other Notes
- Gas can goodies grant 5 flamethrower charges, landmine goodies grant 2 landmines to deploy, and vaccine goodies grant 1 vaccine to use.
- Zombies will vomit on humans in front of them. Zombie vomit causes humans to become infected, and after 500 ticks, the human becomes a zombie.
- Citizens will run away from zombies and will run toward Penelope, if a zombie or Penelope is close enough.
- All moving objects will block other moving objects, so keep that in mind when trying to get citizens to escape.

#### Point System
```
+50                        goodie is picked up by Penelope
+500                       citizen escapes
+1000                      dumb zombie dies
+2000                      smart zombie dies
-1000                      citizen becomes zombie or dies
```
