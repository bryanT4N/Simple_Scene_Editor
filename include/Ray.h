#pragma once

#include<glm/glm/glm.hpp>
#include<glm/glm/gtc/matrix_transform.hpp>
#include<glm/glm/gtc/type_ptr.hpp>


class Ray {
public:
	Ray(glm::vec3 beg, glm::vec3 dir)
	{
		Begin = beg;
		Direction = normalize(dir);
		Position = Begin;
		t = 0;
	}

	glm::vec3 normalize(glm::vec3 dir)
	{
		if ((dir.x) * (dir.x) + (dir.y) * (dir.y) + (dir.z) * (dir.z) != 0)
			return dir / sqrt((dir.x) * (dir.x) + (dir.y) * (dir.y) + (dir.z) * (dir.z));
		else return glm::vec3(0, 0, 0);
	}


	glm::vec3 Begin;
	glm::vec3 Direction;
	glm::vec3 Position;
	float t;

};
