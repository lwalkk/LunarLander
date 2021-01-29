// world.cpp


#include "world.h"
#include "lander.h"
#include "ll.h"
#include "gpuProgram.h"
#include "strokefont.h"

#include <sstream>
#include <iomanip>

//const float textAspect = 0.7;	// text width-to-height ratio (you can use this for more realistic text on the screen)


void World::updateState( float elapsedTime )

{
  // See if any keys are pressed for thrust

  if (glfwGetKey( window, GLFW_KEY_RIGHT ) == GLFW_PRESS) // right arrow
    lander->rotateCW( elapsedTime );

  if (glfwGetKey( window, GLFW_KEY_LEFT ) == GLFW_PRESS) // left arrow
    lander->rotateCCW( elapsedTime );

  if (glfwGetKey( window, GLFW_KEY_DOWN ) == GLFW_PRESS) // down arrow
    lander->addThrust( elapsedTime );

  if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) { // Z key
      zoomView = true;
  }

  if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_RELEASE) { // Z key
      zoomView = false;
  }



  // Update the position and velocity

  lander->updatePose( elapsedTime );

  // See if the lander has touched the terrain

  vec3 closestTerrainPoint = landscape->findClosestPoint( lander->centrePosition() );
  float closestDistance = ( closestTerrainPoint - lander->centrePosition() ).length();

  // Find if the view should be zoomed

#if 1

  zoomView = (closestDistance < ZOOM_RADIUS);

#else
  // debug
  zoomView = true;

#endif

  

  currentTime += elapsedTime;


  // Check for landing or collision and let the user know
  //
  // Landing is successful if the vertical speed is less than 1 m/s and
  // the horizontal speed is less than 0.5 m/s.
  //
  // SHOULD ALSO CHECK THAT LANDING SURFACE IS HORIZONAL, BUT THIS IS
  // NOT REQUIRED IN THE ASSIGNMENT.
  //
  // SHOULD ALSO CHECK THAT THE LANDER IS VERTICAL, BUT THIS IS NOT
  // REQUIRED IN THE ASSIGNMENT.

  if (altitude < 0)
  {
      float velX = (lander->velocity).x;
      float velY = (lander->velocity).y;

      if (velX < 0.5 || velY < 1)
      {
          lander->StopMovement();
          Sleep(5000);
          world->resetLander();
          lander->StartMovement();
          // say success or some shit
      }
      else
      {
          lander->StopMovement();
          Sleep(5000);
          world->resetLander();
          lander->StartMovement();
          // say failure
          // subtract from score?
      }
          
     
  }

  
}



void World::draw()

{
  mat4 worldToViewTransform;
  if (!zoomView) {

    // Find the world-to-view transform that transforms the world
    // to the [-1,1]x[-1,1] viewing coordinate system, with the
    // left edge of the landscape at the left edge of the screen, and
    // the bottom of the landscape BOTTOM_SPACE above the bottom edge
    // of the screen (BOTTOM_SPACE is in viewing coordinates).

    float s = 2.0 / (landscape->maxX() - landscape->minX());

    worldToViewTransform
      = translate( -1, -1 + BOTTOM_SPACE, 0 )
      * scale( s, s, 1 )
      * translate( -landscape->minX(), -landscape->minY(), 0 );

  } else {

  

    // Find the world-to-view transform that is centred on the lander
    // and is 2*ZOOM_RADIUS wide (in world coordinates).

      float s = 2.0 / (2 * ZOOM_RADIUS);

      worldToViewTransform
          = translate(0, 0, 0)
          * scale(s, s, 1)
          * translate(-1 * lander->centrePosition());
  }

  // Draw the landscape and lander, passing in the worldToViewTransform
  // so that they can append their own transforms before passing the
  // complete transform to the vertex shader.

  landscape->draw( worldToViewTransform );
  lander->draw(worldToViewTransform);

  altitude = landscape->findClosestPointf(lander->centrePosition()) - 0.5 * lander->height;

  std::cout << altitude << std::endl;

  // Draw the heads-up display (i.e. all text).

  /*
  * TODO:
  * Score 
  * Time -- DONE
  * Fuel -- DONE
  * Altitude -- DONE
  * Horizontal Speed
  * Vertical Speed
  */

  stringstream ss;

  drawStrokeString( "LUNAR LANDER", -0.2, 0.85, 0.06, glGetUniformLocation( myGPUProgram->id(), "MVP") );

  ss.setf( ios::fixed, ios::floatfield );
  ss.precision(1);

  ss << "SPEED " << lander->speed() << " m/s\n";
  ss << "FUEL " << lander->fuel << "\n";
  ss << "TIME " << currentTime << "\n";
  
  drawStrokeString( ss.str(), -0.95, 0.75, 0.04, glGetUniformLocation( myGPUProgram->id(), "MVP") );

  // YOUR CODE HERE (modify the above code, too)


}

void World::setupArrowVAO()

{
    // create an arrow from line segments

    GLfloat arrowVerts[] = {
      0,    1,    0,    -1,
      0,    1,    0.5, 0.25,
      0,    1,   -0.5, 0.25,
      0.5, 0.25, -0.5, 0.25
    };

    numArrowVerts = sizeof(arrowVerts) / sizeof(GLfloat);

    // ---- Create a VAO for this object ----

    glGenVertexArrays(1, &arrowVAO);
    glBindVertexArray(arrowVAO);

    // Store the vertices

    GLuint VBO;

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, numArrowVerts * sizeof(GLfloat), arrowVerts, GL_STATIC_DRAW);

    // define the position attribute

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);

    // Done

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

