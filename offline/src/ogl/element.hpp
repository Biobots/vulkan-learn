#ifndef TIDE_ELEMENT_HPP
#define TIDE_ELEMENT_HPP

#include <external.hpp>

namespace tide
{
	struct Attr
	{
		GLint size;
		GLenum type;
		GLboolean norm;
		GLsizei stride;
		GLsizei offset;
	};

	class Shader
	{
	public:
		GLuint handle;
		Shader(std::string vertex_file,std::string fragment_file)
		{
			handle = LoadShaders(vertex_file, fragment_file);
		}
		Shader(std::string vertex_file,std::string geometry_file,std::string fragment_file)
		{
			handle = LoadShaders(vertex_file, geometry_file, fragment_file);
		}
		Shader(const char * vertex_file_path,const char * geometry_file_path,const char * fragment_file_path)
		{
			handle = LoadExternalShaders(vertex_file_path, geometry_file_path, fragment_file_path);
		}
		Shader(GLuint handle)
		{
			this->handle = handle;
		}
		~Shader()
		{
			glDeleteProgram(handle);
		}
		void use()
		{
			glUseProgram(handle);
		}
		void setBool(const std::string &name, bool value) const
		{
			glUniform1i(glGetUniformLocation(handle, name.c_str()), (int)value); 
		}
		void setInt(const std::string &name, int value) const
		{
			glUniform1i(glGetUniformLocation(handle, name.c_str()), value); 
		}
		void setFloat(const std::string &name, float value) const
		{
			glUniform1f(glGetUniformLocation(handle, name.c_str()), value); 
		}
		void setMat4(const std::string &name, GLsizei count, GLboolean transpose, GLfloat* value) const
		{
			glUniformMatrix4fv(glGetUniformLocation(handle, name.c_str()), count, transpose, value); 
		}
		void setVec3(const std::string &name, GLsizei count, GLfloat* value) const
		{
			glUniform3fv(glGetUniformLocation(handle, name.c_str()), count, value);
		}
	};
	class Texture
	{
	public:
		GLuint handle;
		Texture(std::vector<unsigned char> data, int width, int height)
		{
			glGenTextures(1, &handle);
			glBindTexture(GL_TEXTURE_2D, handle);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data.data());
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glBindTexture(GL_TEXTURE_2D, 0);
		}
		~Texture()
		{
			glDeleteTextures(1, &handle);
		}
	};
	class Light
	{

	};
	class Element
	{
	private:
		Shader* _shader;
		GLuint _buffer_handle;
		GLfloat* _buffer;
		GLint* _indices;
		GLsizei _buffer_size;
		GLsizei _indices_size;
		GLuint _vao;
		GLuint _ibo;
		std::vector<Attr> _attr;

		GLuint _diffuse_texture;
		GLuint _specular_texture;
		GLuint _normal_texture;

		GLint _vert_num;
		GLint _attr_num;

		GLbitfield _clear_flag;

		glm::vec3 _pos = glm::vec3(0.0f);
		glm::vec3 _scale = glm::vec3(1.0f);
		glm::vec3 _rot = glm::vec3(0.0f, 0.0f, 0.0f);

	public:

		std::map<std::string, glm::vec3> vec3dic;
		std::map<std::string, glm::mat4> mat4dic;
		std::map<std::string, GLuint> texdic;
		std::map<std::string, GLfloat> floatdic;
		std::map<std::string, GLint> intdic;

		GLboolean internal_model = true;

		Element(GLfloat* data, GLint* indices, GLsizei buffer_size, GLsizei indices_size, std::vector<Attr> attr)
		{
			_buffer = data;
			_indices = indices;
			_buffer_size = buffer_size;
			_indices_size = indices_size;
			_attr = attr;

			glGenBuffers(1, &_buffer_handle);
			glBindBuffer(GL_ARRAY_BUFFER, _buffer_handle);
			glBufferData(GL_ARRAY_BUFFER, _buffer_size, _buffer, GL_STATIC_DRAW);

			glGenBuffers(1, &_ibo);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ibo);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, _indices_size, _indices, GL_STATIC_DRAW);

			glGenVertexArrays(1, &_vao);
			initialzeVAO();
		}
		~Element()
		{
			glDeleteBuffers(1, &_buffer_handle);
			glDeleteBuffers(1, &_ibo);
			glDeleteVertexArrays(1, &_vao);
			//release texture in resource pool
			for(auto t : texdic)
			{
				glDeleteTextures(1, &t.second);
			}
		}
    
		void addVec3Uniform(const std::string &name, glm::vec3 value)
		{
			vec3dic.insert(std::pair<std::string, glm::vec3>(name, value));
		}
		void addMat4Uniform(const std::string &name, glm::mat4 value)
		{
			mat4dic.insert(std::pair<std::string, glm::mat4>(name, value));
		}
		void addFloatUniform(const std::string &name, GLfloat value)
		{
			floatdic.insert(std::pair<std::string, GLfloat>(name, value));
		}
		void addIntUniform(const std::string &name, GLint value)
		{
			intdic.insert(std::pair<std::string, GLint>(name, value));
		}

		void setClearFlag(GLbitfield flag)
		{
			_clear_flag = flag;
		}

		void initialzeVAO()
		{
			glBindVertexArray(_vao);
			_attr_num = 0;
			for (size_t i = 0; i < _attr.size(); i++)
			{
				Attr a = _attr[i];
				glEnableVertexAttribArray(i);
				glBindBuffer(GL_ARRAY_BUFFER, _buffer_handle);
				glVertexAttribPointer(
				    i, a.size, a.type, a.norm, a.stride * sizeof(_buffer[0]), (void*)(a.offset * sizeof(_buffer[0]))
				);
				_attr_num += a.size;
			}
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ibo);
			_vert_num = _buffer_size / (_attr_num * sizeof(_buffer[0]));
			glBindVertexArray(0);
		}
		void attachShader(Shader* shader)
		{
			_shader = shader;
		}
        void render(GLenum mode=GL_TRIANGLES)
        {
			if(_clear_flag) glClear(_clear_flag);

			_shader->use();

			int texcount = 0;
			for(auto t : texdic)
			{
				glActiveTexture(GL_TEXTURE0 + texcount);
				glBindTexture(GL_TEXTURE_2D, t.second);
				_shader->setInt(t.first, texcount);
				texcount++;
			}

			for(auto v : mat4dic)
			{
				_shader->setMat4(v.first, 1, GL_FALSE, &v.second[0][0]);
			}
			for(auto v : vec3dic)
			{
				_shader->setVec3(v.first, 1, &v.second[0]);
			}
			for(auto v : floatdic)
			{
				_shader->setFloat(v.first, v.second);
			}
			for(auto v : intdic)
			{
				_shader->setInt(v.first, v.second);
			}

			if(internal_model)
			{
				glm::mat4 model = glm::mat4(1.0f);
				glm::qua<float> q(glm::radians(_rot));
				model = glm::scale(glm::mat4(1.0f), _scale) * model;
				model = glm::mat4_cast(q) * model;
				model = glm::translate(glm::mat4(1.0f), _pos) * model;
				_shader->setMat4("Model", 1, GL_FALSE, &model[0][0]);
			}

			glBindVertexArray(_vao);
			glDrawElements(mode, _indices_size/sizeof(int), GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
		}
		//generate texture in resource pool
		void addTexture(std::string type, std::vector<unsigned char> data, int width, int height)
		{
			GLuint tex;
			glGenTextures(1, &tex);
			glBindTexture(GL_TEXTURE_2D, tex);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data.data());
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			texdic.insert(std::pair<std::string, GLuint>(type, tex));
		}
		void addTexture(std::string type, GLuint tex)
		{
			texdic.insert(std::pair<std::string, GLuint>(type, tex));
		}

		void setPosition(glm::vec3 pos)
		{
			_pos = pos;
		}
		void setRotation(glm::vec3 rot)
		{
			_rot = rot;
		}
		void setScale(glm::vec3 scale)
		{
			_scale = scale;
		}
	};
	
}

#endif