/*
	Mech.cpp and Mech.h are the files that make
	the Mech Engine what it is. The ability to
	click to move, select units, tell a story,
	etc... All those elements will be addressed
	here. 
*/
#ifndef MY_DIRECT_X_H
#define MY_DIRECT_X_H
#include "MyDirectX.h"
#endif

#ifndef MECH_H
#define MECH_H
#include "Mech.h"
#endif

// constants for functions
const double PI = 3.1415926535;
const double PI_under_180 = 180.0f / PI;
const double PI_over_180 = PI / 180.0f;

// math functions
double toRadians(double degrees)
{
	return degrees * PI_over_180;
}

double toDegrees(double radians)
{
	return radians * PI_under_180;
}

double wrap(double value, double bounds)
{
	double result = fmod(value, bounds);
	if (result < 0) result += bounds;
	return result;
}

double wrapAngleDegs(double degs)
{
	return wrap(degs, 360.0);
}

double LinearVelocityX(double angle)
{
	if (angle < 0) angle = 360 + angle;
	return cos(angle * PI_over_180);
}

double LinearVelocityY(double angle)
{
	if (angle < 0) angle = 360 + angle;
	return sin(angle * PI_over_180);
}

// set values for the unit
void UNIT::setUnit(std::string filename, std::string unitName, float speed, float radii)
{
	loadModel(filename);
	name = unitName;
	speedMult = speed;
	unitRadii = radii;
	endPosition = translate;
	tempEndPos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
}

// moves unit and rotates it based on direction moved
void UNIT::moveUnit(D3DXVECTOR3 endPos)
{
	D3DXVECTOR3 unitDir;
	double unitAngle = 0;

	// check if we are colliding
	if (!colLocations.empty() && tempEndPos == D3DXVECTOR3(0.0f, 0.0f, 0.0f))
	{
		// find new location to move to
		eightWayCheck();
	}

	if (tempEndPos == D3DXVECTOR3(0.0f, 0.0f, 0.0f))
		unitDir = endPos - translate; // direction unit is moving in
	else
		unitDir = tempEndPos - translate; // temporary direction

	if (std::abs(unitDir.x) < 0.05f && std::abs(unitDir.z) < 0.05f) // reached destination
	{
		unitAngle = (double)rotate.x;
		unitDir *= 0;
		if(tempEndPos == D3DXVECTOR3(0.0f, 0.0f, 0.0f))
			endPosition = translate;
		tempEndPos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	}
	else if (unitDir.x > 0.0f && unitDir.z > 0.0f) // Quadrant 1 
	{
		unitAngle = atan(unitDir.x / unitDir.z);
		unitAngle = std::abs(toDegrees(unitAngle) + 180);
	}
	else if (unitDir.x < 0.0f && unitDir.z > 0.0f) // Quadrant 2
	{
		unitAngle = atan(unitDir.z / unitDir.x);
		unitAngle = std::abs(toDegrees(unitAngle)) + 450;
	}
	else if (unitDir.x < 0.0f && unitDir.z < 0.0f) // Quadrant 3
	{
		unitAngle = atan(unitDir.x / unitDir.z);
		unitAngle = std::abs(toDegrees(unitAngle)) + 360;
	}
	else if (unitDir.x > 0.0f && unitDir.z < 0.0f) // Quadrant 4
	{
		unitAngle = atan(unitDir.z / unitDir.x);
		unitAngle = std::abs(toDegrees(unitAngle)) + 270;
	}

	D3DXVec3Normalize(&unitDir, &unitDir); // normalize vector

	translate += unitDir * speedMult; // move unit

	rotate.x = (float)unitAngle; // set rotation
}

// check for collisions amongst units
void UNIT::collisionCheck(UNIT &other)
{
	if (collisionCheckSphere(other.translate, other.unitRadii))
	{
		collisionState = true;
		other.collisionState = true;

		// record collisions in unit
		colLocations.push_back(other.translate);
		colRadii.push_back(other.unitRadii);

		// record collisions in other unit
		other.colLocations.push_back(translate);
		other.colRadii.push_back(unitRadii);

		// find nowDir of first unit and put on resDir for other
		bool reserved = false;
		eightWayCheck();
		if (nowDir != 0) 
		{
			if (other.resDir.empty())
				other.resDir.push_back(nowDir);
			else 
				for (int i = 0; i < other.resDir.size(); i++)
				{
					if (nowDir == other.resDir[i])
					{
						reserved = true;
						break;
					}
				}
			if (!reserved) { other.resDir.push_back(nowDir); }
		}
	}
}

// returns true if unit collides with given sphere location + radius
bool UNIT::collisionCheckSphere(D3DXVECTOR3 loc, float rad)
{
	float finalDist, radiiSum;

	float xDist = translate.x - loc.x;
	xDist *= xDist;

	float yDist = translate.y - loc.y;
	yDist *= yDist;

	float zDist = translate.z - loc.z;
	zDist *= zDist;

	finalDist = xDist + yDist + zDist;
	radiiSum = unitRadii + rad;
	radiiSum *= radiiSum;

	if (radiiSum >= finalDist)
		return true;
	
	return false;
}

// finds new position to move to if unit is colliding
void UNIT::eightWayCheck()
{
	float spacing = unitRadii * 1.25f;	  // how far new location will be compared to current 
	std::vector<D3DXVECTOR3> possiblePos; // possible places to move to
	std::vector<short> dir;				  // directions can move to in format of a numpad like in nethack
	short dirArray[] = { 1, 2, 3, 4, 6, 7, 8, 9 }; // to quickly fill vector dir

	dir.assign(dirArray, dirArray + (sizeof(dirArray) / sizeof(short))); // fill dir

	// fill with all possible locations to move to
	possiblePos.push_back(D3DXVECTOR3(translate.x + spacing, translate.y, translate.z + spacing)); // (+,+)
	possiblePos.push_back(D3DXVECTOR3(translate.x - spacing, translate.y, translate.z - spacing)); // (-,-)
	possiblePos.push_back(D3DXVECTOR3(translate.x + spacing, translate.y, translate.z - spacing)); // (+,-)
	possiblePos.push_back(D3DXVECTOR3(translate.x - spacing, translate.y, translate.z + spacing)); // (-,+)
	possiblePos.push_back(D3DXVECTOR3(translate.x + spacing, translate.y, translate.z));		   // (+, )
	possiblePos.push_back(D3DXVECTOR3(translate.x, translate.y, translate.z + spacing));		   // ( ,+)
	possiblePos.push_back(D3DXVECTOR3(translate.x - spacing, translate.y, translate.z));		   // (-, )
	possiblePos.push_back(D3DXVECTOR3(translate.x, translate.y, translate.z - spacing));		   // ( ,-)

	// remove unnecessary entries
	if (!resDir.empty())
	{
		for (int i = 0; i < dir.size(); i++)
		{
			for (int j = 0; j < resDir.size(); j++)
			{
				if (dir[i] == resDir[j])
				{
					dir.erase(dir.begin() + i);
					possiblePos.erase(possiblePos.begin() + i);
					break;
				}
			}
		}
	}

	for (int i = 0; i < colLocations.size(); i++) // loop through units collided with
	{
		for (int j = 0; j < possiblePos.size(); j++) // loop through each possiblePos
		{
			if (collisionCheckSphere(colLocations[i], colRadii[i])) // can't move there!
			{
				// take those options out
				dir.erase(dir.begin() + j); 
				possiblePos.erase(possiblePos.begin() + j); 
			}
		}
		
		if (possiblePos.empty()) { break; } // no possible place to move, break
	}

	// find closest location to endPosition
	int tracker = 0;
	if (!possiblePos.empty())
	{
		D3DXVECTOR3 closest;
		float closestDist, tempDist;
		for (int i = 0; i < possiblePos.size(); i++)
		{
			// find distance between possiblePos[i] and endPosition on the x and z
			tempDist = distanceForm(possiblePos[i].x, possiblePos[i].z, endPosition.x, endPosition.z);

			if (i == 0)
			{
				closest = possiblePos[0];
				tracker = dir[0];
				closestDist = tempDist;
			}
			else if (tempDist < closestDist)
			{
				closest = possiblePos[i];
				tracker = dir[i];
				closestDist = tempDist;
			}
		}

		tempEndPos = D3DXVECTOR3(closest.x, translate.y, closest.z); // move to best position
		(tracker == 0) ? (nowDir = 0) : (nowDir = tracker);		     // mark which way unit is going
	}
}