/**
 A set of variables that records mouse activity.
 You can update the variables yourself or use the functions.
*/

#ifndef _INPUTSTATE_H_
#define _INPUTSTATE_H_

struct InputState
{    
	InputState(): lMousePressed(false),
                  rMousePressed(false),
    	    	  prevX(0), prevY(0),
	    	      deltaX(0), deltaY(0) {
	}

    // Is the mouse button currently being held down?
	bool lMousePressed;
	bool rMousePressed;

    // Last known position of the cursor
	float prevX;
	float prevY;

    // Accumulated change in cursor position. 
	float deltaX;
	float deltaY;

	float accX;
	float accY;

    // Update cursor variables based on new position x,y
    void update(float x, float y)
    {
        float xDiff = x - prevX;
        float yDiff = y - prevY;
        deltaX += xDiff;
        deltaY += yDiff;
		accX += xDiff;
		accY += yDiff;
        prevX = x;
        prevY = y;
    };

    // Read off the accumulated motion and reset it
    void readDeltaAndReset(float *x, float *y)
    {
		*x = deltaX;
		*y = deltaY;
        deltaX = 0;
        deltaY = 0;
    };

	// Read off the accumulated motion but does't reset it.
	// Scales the result down to be used for the game camera
	void readDelta(float *x, float *y)
	{
		*x = deltaX * 0.005f;
		*y = deltaY * 0.005f;
	};

	void readAcc(float *x, float *y)
	{
		*x = accX * 0.005f;
		*y = accY * 0.005f;
		accX = 0;
		accY = 0;
	};
};

#endif // _INPUTSTATE_H_
