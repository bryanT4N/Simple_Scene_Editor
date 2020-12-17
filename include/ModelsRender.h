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

	ModelUnit()
	{
		
	}
};

class ModelsRender {
private:
	vector<ModelUnit> ModelsList;
	Shader *shader;
	std::mutex  m;
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
		m.lock();
		this->shader = shader;
		m.unlock();
	}

	void addModel(ModelUnit modelUnit)
	{
		m.lock();
		ModelsList.push_back(modelUnit);
		m.unlock();
	}
	vector<ModelUnit> getList()
	{
		return ModelsList;
	}
	string getFilepath(string filepath)
	{
		for (int i = 0; i < filepath.length(); i++)
		{
			if (filepath[i] == '\\')
				filepath[i] = '/';
		}
		return filepath;
	}
	void getModelFromUser()
	{
		while (true)
		{ 
	/*	cout << "if you want to add model? press 'n' to skip.";
		string flag = "";
		cin >> flag;
		if (flag == "n")
		return;*/

			float a;
			float b;
			float c;
			string filepath;
			ModelUnit modelUnit;
			cout << endl;
			cout << "please input the obj file path:";
			cout << endl;
			cin >> filepath;
			filepath = getFilepath(filepath);
			Model model((GLchar*)filepath.c_str());
			modelUnit.model = model;
			cout << "please input the translate matrix:" << endl;
			cin >> a;
			cin >> b;
			cin >> c;
			modelUnit.translate = glm::vec3(a, b, c);
			cout << "please scale size:";
			cin >> a;
			modelUnit.scale = glm::vec3(a, a, a);
			addModel(modelUnit);
		}
	}

	void render(Camera camera)
	{
		m.lock();
		if (ModelsList.empty())
		{
			m.unlock();
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
			glUniformMatrix4fv(glGetUniformLocation(shader->ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
			ModelsList[i].model.Draw(*shader);
		}
		m.unlock();
	}


	void render1(Camera camera,vector<int>pos)
	{
		if (ModelsList.empty())
			return;
		for (int i = 0; i < pos.size(); i++)
		{
			int j = pos[i];
			shader->use();   // <-- Don't forget this one!
		// Transformation matrices
			glm::mat4 projection = glm::perspective(camera.Zoom, (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);
			glm::mat4 view = camera.GetViewMatrix();
			glUniformMatrix4fv(glGetUniformLocation(shader->ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
			glUniformMatrix4fv(glGetUniformLocation(shader->ID, "view"), 1, GL_FALSE, glm::value_ptr(view));

			// Draw the loaded model
			glm::mat4 model;
			model = glm::translate(model, ModelsList[j].translate); // Translate it down a bit so it's at the center of the scene
			//model = glm::scale(model, glm::vec3(0.02f, 0.02f, 0.02f));	// It's a bit too big for our scene, so scale it down
			model = glm::scale(model, ModelsList[j].scale);	// It's a bit too big for our scene, so scale it down
			glUniformMatrix4fv(glGetUniformLocation(shader->ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
			ModelsList[j].model.Draw(*shader);
		}
	}

};
