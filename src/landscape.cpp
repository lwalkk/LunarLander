// landscape.cpp


#include "headers.h"
#include "landscape.h"
#include "gpuProgram.h"
#include "ll.h"


// Set up the landscape by creating a VAO and rewriting the landscape
// vertices so that the x values fit in [ 0, LANDSCAPE_WIDTH ].


void Landscape::setupVAO()

{
  // ---- Rewrite the landscape vertices into world coordinates ----

  // Find the bounding box of the landscape

  vec3 min = vec3( landscapeVerts[0], landscapeVerts[1], 0 );
  vec3 max = vec3( landscapeVerts[0], landscapeVerts[1], 0 );

  numVerts = 0;

  for (int i=0; landscapeVerts[i] != -1; i+=2) {

    vec3 v( landscapeVerts[i], landscapeVerts[i+1], 0 );

    if (v.x < min.x) min.x = v.x;
    if (v.x > max.x) max.x = v.x;
    if (v.y < min.y) min.y = v.y;
    if (v.y > max.y) max.y = v.y;

    numVerts++;
  }

  // Translate the landscape so that its lower-left corner is at (0,0)
  // and its width is LANDSCAPE_WIDTH and y increases upward.
  // 
  // Note that y increases downward in the model, so the y axis must
  // be inverted.

  float s = LANDSCAPE_WIDTH / (max.x - min.x);

  mat4 modelToWorldTransform = scale( s, -s, 1 ) * translate( -min.x, -max.y, 0 );

  // Rewrite the model vertices so that they are in the world
  // coordinate system.
  //
  // Also, fix the landscape so that it doesn't have more than one
  // segment and any x position.  Segments that go backward in x
  // should be pushed a bit forward to prevent this.  This makes it
  // much easier to detect a lander/landscape collision.

  float prevX = 0;

  for (int i=0; landscapeVerts[i] != -1; i+=2) {

    vec4 newV = modelToWorldTransform * vec4( landscapeVerts[i], landscapeVerts[i+1], 0, 1 );

    landscapeVerts[i]   = newV.x / newV.w;
    landscapeVerts[i+1] = newV.y / newV.w;

    // prevent the landscape from going backward
    
    if (landscapeVerts[i] < prevX)
      landscapeVerts[i] = prevX;

    prevX = landscapeVerts[i];
  }

  // ---- Create a VAO for this object ----

  glGenVertexArrays( 1, &VAO );
  GLCall(glBindVertexArray( VAO ));

  // Store the vertices

  GLuint VBO;
  glGenBuffers( 1, &VBO );
  glBindBuffer( GL_ARRAY_BUFFER, VBO );
  glBufferData( GL_ARRAY_BUFFER, 2*numVerts*sizeof(float), &landscapeVerts[0], GL_STATIC_DRAW );

  // define the position attribute

  glEnableVertexAttribArray( 0 );
  glVertexAttribPointer( 0, 2, GL_FLOAT, GL_FALSE, 0, 0 );
}

// Draw the landscape


void Landscape::draw(  mat4 &worldToViewTransform )

{
  GLCall(glBindVertexArray( VAO ));

  GLCall(glUniformMatrix4fv( glGetUniformLocation( myGPUProgram->id(), "MVP"), 1, GL_TRUE, &worldToViewTransform[0][0] ));

#ifndef MACOS  
  glLineWidth( 2.0 );
#endif

  GLCall(glDrawArrays( GL_LINE_STRIP, 0, numVerts ));
}


// Find the point on a segment ( 'P', 'Q' ) that is
// closest to 'X'.

vec3 Landscape::findClosestPoint( vec3 X, vec3 P, vec3 Q )

{
    // dot product to find closest point
    float dot = (X - P) * (Q - P) / ((Q - P) * (Q - P));

    if (dot <= 0)
        return P;

    else if (dot > 1)
        return Q;

    else
        return P + dot * (Q - P);

}


// Find the point on the landscape that is closest to 'position'.
//
// This is very inefficiently done.


vec3 Landscape::findClosestPoint( vec3 position )

{
  vec3  closestPoint;
  float minSquaredDistance = MAXFLOAT;

  for (int i=0; i<numVerts-1; i++) {

    vec3 thisClosestPoint = findClosestPoint( position,
					      vec3( landscapeVerts[2*i], landscapeVerts[2*i+1], 0 ),
					      vec3( landscapeVerts[2*(i+1)], landscapeVerts[2*(i+1)+1], 0 ) );

    float thisSquaredDistance = (thisClosestPoint - position) * (thisClosestPoint - position);

    if (thisSquaredDistance < minSquaredDistance) {
      closestPoint = thisClosestPoint;
      minSquaredDistance = thisSquaredDistance;
    }
  }

  return closestPoint;
}


float Landscape::findClosestPointf(vec3 position)
{
    // x coordinate of the lander
    float xPosition = position.x;
    // y coordinate of the lander 
    float yPosition = position.y;

    // find y coordinate where xcoordinate of terrain is x coordinate of lander

    float low = 10000;
    int n1, n2;

    for (int i = 0; i < numVerts - 1; i++)
    {
        float x1 = landscapeVerts[2 * i];
        float x2 = landscapeVerts[2 * (i + 1)];

        float xMid = (x1 + x2) / 2;

        if (abs(xPosition - xMid) < low)
        {
            low = abs(xPosition - xMid);
            n1 = 2 * i;
            n2 = 2 * (i + 1);
        }

        
    }

    float yPoint = (landscapeVerts[n1 + 1] + landscapeVerts[n2 + 1]) / 2;

    return yPosition - yPoint;
}

float Landscape::findYCoord(vec3 position) const
{

    
    float posX = position.x;
    float posY = position.y;

    float closestDistance = abs(position.x - landscapeVerts[0]);
    int closestIndex = 0;

    for (int i = 0; i < numVerts - 1; i++)
    {
        float landX = landscapeVerts[2 * i];

        if (abs(posX - landX) < closestDistance)
        {
            closestDistance = abs(landX - posX);
            closestIndex = 2 * i;
        }


    }

    int adjacentIndex;

    if ((closestIndex != 0) && (closestIndex != (2 * numVerts - 2)))
    {
        if (posX > landscapeVerts[closestIndex])
            adjacentIndex = closestIndex + 2;
        else
            adjacentIndex = closestIndex - 2;

        float x1 = landscapeVerts[closestIndex];
        float y1 = landscapeVerts[closestIndex + 1];
        float x2 = landscapeVerts[adjacentIndex];
        float y2 = landscapeVerts[adjacentIndex + 1];

        float alt = posY - ((y2 - y1) / (x2 - x1)) * (posX - x1) + y1;

        return alt;

    }

    else
    {
        std::cout << "edge case" << std::endl;
        return landscapeVerts[closestIndex + 1];

    }
  


}


// Landscape model consisting of a path of segments
//
// These are in a ARBITRARY coordinate system and get remapped to the
// world coordinate system when the VAO is set up.


float Landscape::landscapeVerts[] = {
  -463, 866,
  -449, 866,
  -445, 879,
  -433, 880,
  -431, 893,
  -423, 894,
  -422, 927,
  -408, 958,
  -409, 975,
  -402, 996,
  -384, 1010,
  -380, 1030,
  -364, 1050,
  -347, 1060,
  -336, 1040,
  -321, 1020,
  -312, 1010,
  -302, 998,
  -296, 987,
  -281, 976,
  -277, 965,
  -263, 958,
  -251, 942,
  -238, 941,
  -226, 932,
  -213, 932,
  -197, 934,
  -187, 945,
  -185, 956,
  -172, 968,
  -172, 980,
  -160, 992,
  -160, 998,
  -147, 1010,
  -135, 1010,
  -125, 990,
  -114, 985,
  -103, 992,
  -93, 1010,
  -87.3, 1030,
  -64.6, 1040,
  -62.3, 1080,
  -52.1, 1110,
  -55.5, 1120,
  -38.5, 1130,
  -11.3, 1130,
  15.9, 1110,
  21.5, 1100,
  35.1, 1090,
  43.1, 1080,
  57.8, 1070,
  63.5, 1040,
  72.5, 1020,
  82.8, 1010,
  99.8, 999,
  111, 983,
  122, 963,
  130, 934,
  141, 929,
  148, 916,
  152, 903,
  162, 890,
  178, 891,
  190, 881,
  203, 855,
  214, 846,
  220, 820,
  227, 784,
  224, 760,
  229, 733,
  239, 703,
  254, 700,
  258, 687,
  266, 675,
  280, 675,
  282, 686,
  294, 685,
  299, 699,
  306, 699,
  317, 705,
  328, 717,
  331, 743,
  354, 754,
  354, 768,
  366, 793,
  374, 809,
  388, 811,
  399, 823,
  400, 831,
  411, 845,
  414, 856,
  427, 869,
  440, 869,
  442, 906,
  452, 939,
  453, 950,
  464, 950,
  467, 963,
  478, 976,
  485, 995,
  495, 1010,
  506, 1020,
  508, 1050,
  521, 1090,
  520, 1100,
  525, 1110,
  535, 1120,
  546, 1120,
  554, 1150,
  561, 1160,
  575, 1170,
  589, 1180,
  696, 1180,
  702, 1140,
  713, 1120,
  720, 1100,
  728, 1100,
  736, 1070,
  747, 1070,
  759, 1050,
  774, 1050,
  784, 1040,
  804, 1040,
  805, 1050,
  829, 1090,
  829, 1100,
  845, 1110,
  855, 1120,
  864, 1130,
  871, 1150,
  894, 1170,
  951, 1170,
  977, 1180,
  1030, 1180,
  1040, 1150,
  1040, 1110,
  1040, 1090,
  1060, 1090,
  1060, 1070,
  1070, 1060,
  1080, 1050,
  1080, 1030,
  1090, 1020,
  1110, 1020,
  1110, 992,
  1120, 966,
  1130, 962,
  1140, 974,
  1170, 974,
  1180, 963,
  1180, 937,
  1190, 906,
  1210, 902,
  1210, 888,
  1220, 877,
  1230, 865,
  1250, 864,
  1250, 879,
  1270, 878,
  1270, 889,
  1280, 889,
  1280, 927,
  1290, 959,
  1290, 970,
  1300, 991,
  1320, 1000,
  1320, 1030,
  1330, 1040,
  -1
};
