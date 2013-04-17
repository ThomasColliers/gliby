#include <ShaderManager.h>
#include <iostream>
#include <fstream>
#include <stdarg.h>
#include <vector>
#include <boost/filesystem.hpp>

ShaderManager::ShaderManager(std::vector<const char*>* search_path){
    searchPath = search_path;
}

ShaderManager::~ShaderManager(void){
}

// TODO: Gracefully fail if file not found
GLuint ShaderManager::buildShader(const char* shader_file,GLenum type){
    // try and load compiled shader from cache 
    if(shaderCache[shader_file]){
        return shaderCache[shader_file];
    }
    // create shader
    GLuint shader = glCreateShader(type);
    // load file from search path
    for(std::vector<const char*>::iterator i = searchPath->begin(); i != searchPath->end(); ++i){
        boost::filesystem::path p(*i);
        p /= shader_file;
        if(boost::filesystem::exists(p)){
            // load file
            std::ifstream fp(p.string().c_str());
            if(fp.is_open()){
                fp.seekg(0,std::ios::end);
                int length = fp.tellg();
                fp.seekg(0,std::ios::beg);
                char* buffer = new char[length+1];
                fp.read(buffer,length);
                fp.close();
                buffer[length] = '\0';
                // load into shader
                GLchar *stringPtr[1];
                stringPtr[0] = (GLchar *)buffer;
                glShaderSource(shader,1,(const GLchar **)stringPtr,NULL);
                break;
            }
        }
    }
    // compile 
    glCompileShader(shader);
    // check errors
    GLint status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if(status == GL_FALSE){
        // show errors
        char log[1024];
        glGetShaderInfoLog(shader, 1024, NULL, log);
        std::cout << "Shader '" << shader_file << "' failed to compile with error: " << log;
        // delete shader and return
        glDeleteShader(shader);
        return 0;
    }
    // add compiled shader id to cache
    shaderCache[shader_file] = shader;
    // return
    return shader;
}

GLuint ShaderManager::buildShaderPair(const char* vertex_file, const char* fragment_file, const unsigned int nAttributes, const ShaderAttribute* attributes){
    // build/retrieve shaders
    GLuint vertexShader = buildShader(vertex_file,GL_VERTEX_SHADER);
    GLuint fragmentShader = buildShader(fragment_file,GL_FRAGMENT_SHADER);
    if(vertexShader == 0 || fragmentShader == 0) return 0;
    // link them
    GLuint program = glCreateProgram();
    glAttachShader(program,vertexShader);
    glAttachShader(program,fragmentShader);
    // bind attributes
    if(nAttributes > 0 && attributes){
        for(unsigned int i = 0; i < nAttributes; i++){
            glBindAttribLocation(program, attributes[i].index, attributes[i].name);
        }
    }
    // link
    glLinkProgram(program);
    // check errors
    GLint status;
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if(status == GL_FALSE){
        std::cout << "Shader linking of '" << vertex_file << "' and '" << fragment_file << "' failed." << std::endl;
        glDeleteProgram(program);
        return 0;
    }
    // all seems well, return shader program
    return program;
}
