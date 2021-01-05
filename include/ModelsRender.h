#include <glad/include/glad/glad.h>
#include <glm/glm/glm.hpp>
#include<glm/glm/gtc/matrix_transform.hpp>
#include<glm/glm/gtc/type_ptr.hpp>
#include<mutex>
#include"Model.h"
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>


const GLuint WIDTH = 800, HEIGHT = 600;

struct ModelUnit
{
	Model model;
	glm::vec3 translate;
	glm::vec3 scale;
	float rot1;
	float rot2;

	ModelUnit()
	{
		rot1 = 0;
		rot2 = 0;
	}
};

class ModelsRender {
private:
	vector<ModelUnit> ModelsList;
	Shader *shader;
	int choose=-1;
	bool choFlag=false;

public:

	ModelsRender()
	{
	
	}
	ModelsRender(Shader* shader)
	{
		this->shader = shader;
	}
	void addShader(Shader* shader)
	{
		this->shader = shader;
	}
	void deletemodel()
	{
		this->ModelsList.clear();
	}

	void addModel(ModelUnit modelUnit)
	{
		ModelsList.push_back(modelUnit);
	}
	vector<ModelUnit> getList()
	{
		return ModelsList;
	}
	void render(Camera camera)
	{
		if (ModelsList.empty())
		{
			return;
		}
			
		for (int i = 0; i < ModelsList.size(); i++)
		{
			shader->use();   // <-- Don't forget this one!
		// Transformation matrices
			glm::mat4 projection = glm::perspective(camera.Zoom, (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);
			glm::mat4 view = camera.GetViewMatrix();
			
			glUniformMatrix4fv(glGetUniformLocation(shader->ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
			glUniformMatrix4fv(glGetUniformLocation(shader->ID, "view"), 1, GL_FALSE, glm::value_ptr(view));

			// Draw the loaded model
			glm::mat4 model;
			model = glm::translate(model, ModelsList[i].translate); // Translate it down a bit so it's at the center of the scene
			//model = glm::scale(model, glm::vec3(0.02f, 0.02f, 0.02f));	// It's a bit too big for our scene, so scale it down
			model = glm::scale(model, ModelsList[i].scale);	// It's a bit too big for our scene, so scale it down
			model = glm::rotate(model,ModelsList[i].rot1, glm::vec3(0, 1,0));
			model = glm::rotate(model,ModelsList[i].rot2, glm::vec3(-1,0,0));
			glUniformMatrix4fv(glGetUniformLocation(shader->ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
			if (i == choose)
			{
				glUniform1f(glGetUniformLocation(shader->ID, "f"), 1);
			}
			else
			{
				glUniform1f(glGetUniformLocation(shader->ID, "f"), -1);
			}
			ModelsList[i].model.Draw(*shader);
		}
		//m.unlock();
	}
	bool isCho()
	{
		return choFlag;
	}
	void increaseChoose()
	{
		if (ModelsList.size() == 0||choFlag==false)
		{
			return;
		}
		else
		{
			choose++;
			if (choose == ModelsList.size())
				choose = 0;
		}
	}

	void decreaseChoose()
	{
		if (ModelsList.size() == 0 || choFlag == false)
		{
			return;
		}
		else
		{
			choose--;
			if (choose == -1)
				choose = ModelsList.size()-1;
		}
	}

	void openChoose()
	{
		if (ModelsList.empty())
		{
			return;
		}
		if (choFlag == false)
		{
			choFlag = true;
			choose = 0;
		}
		else
		{
			choose = -1;
			choFlag = false;
		}
	//	cout << choFlag << " " << choose << endl;
	}
	void upRot()
	{
		if (choose < 0 || choose >= ModelsList.size())
			return;
		ModelsList[choose].rot2 += 1;
	}
	void downRot()
	{
		if (choose < 0 || choose >= ModelsList.size())
			return;
		ModelsList[choose].rot2 -= 1;
	}
	void rightRot()
	{
		if (choose < 0 || choose >= ModelsList.size())
			return;
		ModelsList[choose].rot1 += 1;
	}
	void leftRot()
	{
		if (choose < 0 || choose >= ModelsList.size())
			return;
		ModelsList[choose].rot1 -= 1;
	}
	void expand()
	{
		if (choose < 0 || choose >= ModelsList.size())
			return;
		ModelsList[choose].scale+= glm::vec3(0.05, 0.05, 0.05);
	}
	void narrow()
	{
		if (choose < 0 || choose >= ModelsList.size())
			return;
		ModelsList[choose].scale -= glm::vec3(0.05, 0.05, 0.05);
	}
	glm::vec3 getTra()
	{
		return ModelsList[choose].translate;
	}
	void translate(glm::vec3 tra)
	{
		if (choose < 0 || choose >= ModelsList.size())
			return;
		ModelsList[choose].translate = tra;
	}
	void deleteModel()
	{
		if (choose < 0 || choose >= ModelsList.size())
			return;
		ModelsList.erase(ModelsList.begin() + choose);
		choose = 1;
	}

	void printMenu()
	{
		cout << "------------------ModelList-------------------" << endl;
		for (int i = 0; i < ModelsList.size(); i++)
		{
			cout << i + 1 << "." << ModelsList[i].model.getModelName()<<endl;
		}
		cout << endl;
		cout << "P:print menu"<<endl;
		cout << "E:edit model"<<endl;
		cout << "J:left switch model"<<endl;
		cout << "K:right switch model"<<endl;
		cout << "L:delete model" << endl;
		cout << "+:expand model" << endl;
		cout << "-:narrow model" << endl;
		cout << "¡ú:Model move right" << endl;
		cout << "¡û:Model move left" << endl;
		cout << "¡ü:Model move up" << endl;
		cout << "¡ý:Model move down" << endl;
		cout << "Ctrl+¡ú\\¡û\\¡ü\\¡ý:model rotate" << endl;
		cout << "R:Change terrain's texture" << endl;
		cout << "T:Change terrain" << endl;
		cout << "Y:Change skybox" << endl;
		cout << "W:Camera move forward" << endl;
		cout << "S:Camera move backward" << endl;
		cout << "A:Camera move left" << endl;
		cout << "D:Camera move right" << endl;
	}

};
