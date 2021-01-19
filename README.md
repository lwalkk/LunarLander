#GraphicsA1

1. Modify World::draw() to calculate a transform for a zoomed view.
   [1 mark]

2. Modify Landscape::findClosestPoint() to compute the point on a segment
   that is closest to a given point. [1 mark]

3. Modify Lander::setupVAO() to set up the VAO that defines the lander
   segments. [1 mark]

4. Modify Lander::draw() to draw the lander in the correct position and
   orientation. [1 mark]

5. Modify Lander::addThrust() to change the lander velocity.  Use
   THRUST_ACCEL and the current lander orientation. [1 mark]

6. Modify World::draw() to output text that shows the same information as
   the original lunar lander game (search Google Images for this).  There
   are six pieces of information: score, time, fuel, altitude (above the
   closest ground directly below), horizontal speed (with an arrow), and
   vertical speed (with an arrow). [1 mark]

   You will need another function to calculate your altitude by finding
   which landscape segment the lander is above, and calculating the
   distance from the *bottom* of the lander to that segment. [2 marks]

   You will need to maintain the amount of fuel, and decrease it in
   proportion to thrust (differently for main thrust and rotational
   thrust). [1 mark]

7. Modify World::updateState() to check for landing or collision.
   Landing is successful if the vertical speed is less than 1 m/s and
   the horizontal speed is less than 0.5 m/s.  Perform some action so
   that the player knows whether the landing was successful, and award
   points.  [3 marks]

8. Optionally, implement an additional *graphics* feature.  Up to
   [2 bonus marks] can be awarded for additional features.  Simple
   features (like more output text or handling multiple rounds) will
   not get any marks.  A good graphics feature might be a flame that
   gradually extends below the lander and shimmers a bit.

DO NOT add any code that is specific to a particular operating system.
If you do so, you code might not compile on the TA's machine and will
not be marked.

DO NOT add new files, as the TA's marking environment will not see
them.
