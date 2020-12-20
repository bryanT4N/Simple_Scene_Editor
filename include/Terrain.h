#pragma once
#include<iostream>
#include<glad/include/glad/glad.h>
#include<GLFW/glfw3.h>
#include<glm/glm/glm.hpp>
#include<vector>
#include"Ray.h"

struct Plant {
	Plant(glm::vec3 po1, glm::vec3 po2, glm::vec3 po3, glm::vec3 norm)
	{
		this->p1 = po1;
		this->p2 = po2;
		this->p1 = po3;
		this->Plant_normal = norm;
	}
	glm::vec3 p1,p2,p3,Plant_normal;
};


class Terrain
{
public:
	Terrain(const char* path, float *HighMap,int map_size,	float step_size,glm::vec2 backleft)
	{
		MAP_SIZE = map_size;
		STEP_SIZE = step_size;
		BACK = backleft.x;
		LEFT = backleft.y;

		int all_size = MAP_SIZE * MAP_SIZE;
		FILE* fptr=NULL;
		cout << path << endl;
		fptr = fopen(path, "r");
		if (fptr == NULL)
			return;
		
		//fread(HighMap, 1, all_size, fptr);
		for (int i = 0; i < all_size; i++)//从matlab导出的灰度txt文件读入高度图
		{
			fscanf(fptr, "%f,", &HighMap[i]);
		}
		fclose(fptr);

		

	}

	float getHeight(float HighMap[], int px, int pz) {
		int x = px % MAP_SIZE;                                                                
		int z = pz % MAP_SIZE;                                                             
                                                       
		float y = 0;
		if (px >= 0 && pz >= 0)
			y = HighMap[x + (z * MAP_SIZE)];
		return y/5; ///                              
	}

	void getNormal(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec3 &result) {
		const GLfloat l1[] = { p2[0] - p1[0], p2[1] - p1[1], p2[2] - p1[2] };
		const GLfloat l2[] = { p3[0] - p1[0], p3[1] - p1[1], p3[2] - p1[2] };
		//叉乘求法向量
		GLfloat n[] = {
				l1[1] * l2[2] - l1[2] * l2[1],
				l1[2] * l2[0] - l1[0] * l2[2],
				l1[0] * l2[1] - l1[1] * l2[0]
		};
		//归一化
		GLfloat abs = sqrt(n[0] * n[0] + n[1] * n[1] + n[2] * n[2]);
		n[0] /= abs;
		n[1] /= abs;
		n[2] /= abs;
		//??/ 2
		result.x = n[0] / 2;
		result.y = n[1] / 2;
		result.z = n[2] / 2;
	}

	void putinVN(float HighMap[])//将顶点信息(和平面法向量)放入向量
	{
		int i, j,count=0;
		for (i = 0; i < (MAP_SIZE - 1); i++)
		{
			for (j = 0; j < (MAP_SIZE - 1); j++)
			{
				glm::vec3 p1;
				p1.x = BACK + j * STEP_SIZE;
				p1.z = LEFT + i * STEP_SIZE;
				p1.y = getHeight(HighMap, j, i);

				glm::vec3 p2;
				p2.x = BACK + (j+1) * STEP_SIZE;
				p2.z = LEFT + i * STEP_SIZE;
				p2.y = getHeight(HighMap, j+1, i);

				glm::vec3 p3;
				p3.x = BACK + j * STEP_SIZE;
				p3.z = LEFT + (i+1) * STEP_SIZE;
				p3.y = getHeight(HighMap, j, i+1);

				glm::vec3 nor;
				getNormal(p1, p2, p3, nor);
				Plant plant1(p1,p2,p3,nor);
				plants.push_back(plant1);

				glm::vec3 p4;
				p4.x = BACK + (j+1) * STEP_SIZE;
				p4.z = LEFT + (i+1) * STEP_SIZE;
				p4.y = getHeight(HighMap, j+1, i+1);

				//p5=p3;p6=p2
				getNormal(p4, p3, p2, nor);
				Plant plant2(p4, p3, p2, nor);
				plants.push_back(plant2);
				//注意这里得到的是平面法向量，我们需要的是顶点法向量
				vertics.push_back(p1);
				vertics.push_back(p2);
				vertics.push_back(p3);
				vertics.push_back(p4);
				vertics.push_back(p3);
				vertics.push_back(p2);
				count = i * MAP_SIZE + j;
				vertics_index.push_back(count);
				vertics_index.push_back(count+1);
				vertics_index.push_back(count+MAP_SIZE);
				vertics_index.push_back(count+MAP_SIZE+1);
				vertics_index.push_back(count + MAP_SIZE);
				vertics_index.push_back(count + 1);

			}
		}
		
	}

	void normalize(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3, glm::vec3 v4,
		glm::vec3 v5, glm::vec3 v6, glm::vec3 &result) {
		GLfloat x = v1[0] + v2[0] + v3[0] + v4[0] + v5[0] + v6[0];
		GLfloat y = v1[1] + v2[1] + v3[1] + v4[1] + v5[1] + v6[1];
		GLfloat z = v1[2] + v2[2] + v3[2] + v4[2] + v5[2] + v6[2];
		GLfloat l = sqrt(x * x + y * y + z * z);
		result[0] = x / l;
		result[1] = y / l;
		result[2] = z / l;
	}

	void putinPointNormal(float HighMap[])//将顶点法向量弄进去
	{
		glm::vec3 p1,p2,p3,p4,p5,p6,p7;
		glm::vec3 nor1, nor2, nor3, nor4, nor5, nor6,nor_res;
		int i, j;
		for (i = 0; i < MAP_SIZE; i++)
		{
			for (j = 0; j < MAP_SIZE; j++)
			{
				
				p1.x = BACK + j * STEP_SIZE;
				p1.z = LEFT + i * STEP_SIZE;
				p1.y = getHeight(HighMap, j, i);

				//非最后一列才有
				if (j != (MAP_SIZE - 1))
				{				
					p2.x = BACK + (j + 1) * STEP_SIZE;
					p2.z = LEFT + i * STEP_SIZE;
					p2.y = getHeight(HighMap, j + 1, i);
				}
				
				//非最头行最后一列才有
				if (i != 0 && j != (MAP_SIZE - 1))
				{
					p3.x = BACK + (j + 1) * STEP_SIZE;
					p3.z = LEFT + (i - 1) * STEP_SIZE;
					p3.y = getHeight(HighMap, j + 1, i - 1);
				}
				

				//非最头行才有
				if (i != 0)
				{
					p4.x = BACK + j * STEP_SIZE;
					p4.z = LEFT + (i - 1) * STEP_SIZE;
					p4.y = getHeight(HighMap, j, i - 1);
				}
				
				//非第一列才有
				if (j != 0)
				{
					p5.x = BACK + (j - 1) * STEP_SIZE;
					p5.z = LEFT + i * STEP_SIZE;
					p5.y = getHeight(HighMap, j - 1, i);
				}
				

				//非第一列、最后一行 才有
				if (i != (MAP_SIZE - 1) && j != 0)
				{
					p6.x = BACK + (j - 1) * STEP_SIZE;
					p6.z = LEFT + (i + 1) * STEP_SIZE;
					p6.y = getHeight(HighMap, j - 1, i + 1);
				}
				

				//非最后一行才有
				if (i != (MAP_SIZE - 1))
				{
					p7.x = BACK + j * STEP_SIZE;
					p7.z = LEFT + (i + 1) * STEP_SIZE;
					p7.y = getHeight(HighMap, j, i + 1);
				}
				
				nor1 = nor2 = nor3 = nor4 = nor5 = nor6 = glm::vec3(0, 0, 0);
				//分类计算法向量
				if (i == 0)
				{
					if (j == 0)
					{
						getNormal(p1, p2, p7, nor6);
						normals.push_back(nor6);
					}
					else if(j==(MAP_SIZE-1))
					{
						getNormal(p1, p5, p6, nor4);
						getNormal(p1, p6, p7, nor5);
						normalize(nor1, nor2, nor3, nor4, nor5, nor6, nor_res);
						normals.push_back(nor_res);
					}
					else
					{
						getNormal(p1, p2, p7, nor6);
						getNormal(p1, p5, p6, nor4);
						getNormal(p1, p6, p7, nor5);
						normalize(nor1, nor2, nor3, nor4, nor5, nor6, nor_res);
						normals.push_back(nor_res);
					}
				}
				else if(i==(MAP_SIZE-1))
				{
					if (j == 0)
					{
						getNormal(p1, p2, p3, nor1);
						getNormal(p1, p3, p4, nor2);
						normalize(nor1, nor2, nor3, nor4, nor5, nor6, nor_res);
						normals.push_back(nor_res);
					}
					else if (j == (MAP_SIZE - 1))
					{
						getNormal(p1, p4, p5, nor3);
						normals.push_back(nor3);
					}
					else
					{
						getNormal(p1, p2, p3, nor1);
						getNormal(p1, p3, p4, nor2);
						getNormal(p1, p4, p5, nor3);
						normalize(nor1, nor2, nor3, nor4, nor5, nor6, nor_res);
						normals.push_back(nor_res);
					}
				}
				else
				{
					if (j == 0)
					{
						getNormal(p1, p2, p3, nor1);
						getNormal(p1, p3, p4, nor2);
						getNormal(p1, p2, p7, nor6);
						normalize(nor1, nor2, nor3, nor4, nor5, nor6, nor_res);
						normals.push_back(nor_res);
					}
					else if (j == (MAP_SIZE - 1))
					{
						getNormal(p1, p4, p5, nor3);
						getNormal(p1, p5, p6, nor4);
						getNormal(p1, p6, p7, nor5);
						normalize(nor1, nor2, nor3, nor4, nor5, nor6, nor_res);
						normals.push_back(nor_res);
					}
					else
					{
						getNormal(p1, p2, p3, nor1);
						getNormal(p1, p3, p4, nor2);
						getNormal(p1, p4, p5, nor3);
						getNormal(p1, p5, p6, nor4);
						getNormal(p1, p6, p7, nor5);
						getNormal(p1, p2, p7, nor6);
						normalize(nor1, nor2, nor3, nor4, nor5, nor6, nor_res);
						normals.push_back(nor_res);
					}
				}


			}
		}

		

	}
	
	
	float* mergeVN()
	{
		int i, j,tem;
		int maxsize = vertics.size();
		float* res = new float[6 * maxsize];
		for (i = 0; i < maxsize; i++)
		{
			tem = 6 * i;
			res[tem] = vertics[i].x;
			res[tem + 1] = vertics[i].y;
			res[tem + 2] = vertics[i].z;
			res[tem + 3] = normals[vertics_index[i]].x;
			res[tem + 4] = normals[vertics_index[i]].y;
			res[tem + 5] = normals[vertics_index[i]].z;
		}
		return res;

	}

	//求地形图交点
	float getAnyPlaceHeight(float HighMap[],glm::vec3 in_po)
	{
		if (in_po.x <BACK || in_po.x>BACK + MAP_SIZE * STEP_SIZE || in_po.z<LEFT || in_po.z>LEFT + MAP_SIZE * STEP_SIZE)return -1;
		float in_x = in_po.x;
		float in_z = in_po.z;

		int cloest_x = floor((in_x - BACK) / STEP_SIZE);
		int cloest_z = floor((in_z - LEFT) / STEP_SIZE);

		float clo_x_coor = cloest_x * STEP_SIZE + BACK;
		float clo_z_coor = cloest_z * STEP_SIZE + LEFT;

		glm::vec2 zs(clo_x_coor, clo_z_coor);
		glm::vec2 ys(clo_x_coor+STEP_SIZE, clo_z_coor);
		glm::vec2 zx(clo_x_coor, clo_z_coor+ STEP_SIZE);
		glm::vec2 yx(clo_x_coor+ STEP_SIZE, clo_z_coor+ STEP_SIZE);

		float del_x = in_po.x - zs.x;
		float del_z = zx.y - in_po.z;
		float alpha, beta, gama,fa,fb,fc;
		fa = getHeight(HighMap, cloest_x, cloest_z+1);
		fc = getHeight(HighMap, cloest_x+1, cloest_z);
		if (del_x < del_z)//三角形重心坐标插值得到任意点高度
		{
			alpha = (-(in_po.x - zs.x) * (ys.y - zs.y) + (in_po.z - zs.y) * (ys.x - zs.x)) / (-(zx.x - zs.x) * (ys.y - zs.y) + (zx.y - zs.y) * (ys.x - zs.x));
			beta = (-(in_po.x - ys.x) * (zx.y - ys.y) + (in_po.z - ys.y) * (zx.x - ys.x)) / (-(zs.x - ys.x) * (zx.y - ys.y) + (zs.y - ys.y) * (zx.x - ys.x));
			gama = 1 - alpha - beta;
			fb = getHeight(HighMap, cloest_x, cloest_z);
			return (alpha * fa + beta * fb + gama * fc);
		}
		else
		{
			alpha = (-(in_po.x - yx.x) * (ys.y - yx.y) + (in_po.z - yx.y) * (ys.x - yx.x)) / (-(zx.x - yx.x) * (ys.y - yx.y) + (zx.y - yx.y) * (ys.x - yx.x));
			beta = (-(in_po.x - ys.x) * (zx.y - ys.y) + (in_po.z - ys.y) * (zx.x - ys.x)) / (-(yx.x - ys.x) * (zx.y - ys.y) + (yx.y - ys.y) * (zx.x - ys.x));
			gama = 1 - alpha - beta;
			fb = getHeight(HighMap, cloest_x+1, cloest_z+1);
			return (alpha * fa + beta * fb + gama * fc);
		}
		return -1;

	}


	bool Intersects(float HighMap[],Ray ray, glm::vec3 &collisionPoint)
	{
		float blockScale=2;
		glm::vec3 rayStep = ray.Direction * blockScale;
		glm::vec3 rayStartPosition = ray.Position;


		glm::vec3 lastRayPosition = ray.Position;
		ray.Position += rayStep;
		float height = getAnyPlaceHeight(HighMap, ray.Position);
		while (ray.Position.y > height&& height >= 0)
		{
			lastRayPosition = ray.Position;
			ray.Position += rayStep;
			height = getAnyPlaceHeight(HighMap,ray.Position);
			
		}
		
		if (height >= 0)
		{
			glm::vec3 startPosition = lastRayPosition;
			glm::vec3 endPosition = ray.Position;

			// Binary search. Find the exact collision point 
			for (int i = 0; i < 32; i++)
			{
				// Binary search pass 
				glm::vec3 middlePoint = (startPosition + endPosition) * 0.5f;
				if (middlePoint.y < height)
					endPosition = middlePoint;
				else
					startPosition = middlePoint;
			}
			collisionPoint = (startPosition + endPosition) * 0.5f;
			return true;
		}
		return false;
	}


	int MAP_SIZE;
	float STEP_SIZE;
	float BACK, LEFT;
	std::vector<int> vertics_index;
	std::vector < glm::vec3 > vertics;//顶点信息
	std::vector < glm::vec3 > normals;//法向量
	std::vector < glm::vec2 > texCoords;//贴图
	std::vector < Plant > plants;//网格平面
};


